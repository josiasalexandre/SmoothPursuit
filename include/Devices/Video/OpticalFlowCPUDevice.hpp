#ifndef OPTICAL_FLOW_DEVICE_H
#define OPTICAL_FLOW_DEVICE_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <SingleInputDevice.hpp>
#include <VideoSignalDevice.hpp>

enum OpticalFlowDeviceType {OPTICAL_FLOW_FARNEBACK, OPTICAL_FLOW_LUKAS_KANADE_PYR};


class OpticalFlowCPUDevice : virtual public SingleInputDevice<cv::Mat, cv::Point2f> {

    private:

        // the optical flow type
        OpticalFlowDeviceType flow_type;

        // the frame fps
        float fps, dt;

        // just to avoid a lot of allocs
        cv::Mat frame, flow, cflow;

        //
        cv::UMat gray, old_gray, uflow;

        // the base class buffer
        CircularBuffer<cv::Mat> *buffer;

        // the output
        cv::Point2f mean;

        // OPTICAL_FLOW_LUKAS_KANADE_PYR
        cv::TermCriteria termcrit;
        cv::Size subPixWinSize, winSize;

        const int MAX_COUNT;
        bool needToInit;

        // the good point to track
        std::vector<cv::Point2f> points[2];

        // the displacement between the points
        cv::Point2f displacement;

        std::vector<uchar> status;
        std::vector<float> err;

    public:

        // basic constructor
        OpticalFlowCPUDevice(cv::Mat v_null, OpticalFlowDeviceType opt_type) :
            SingleInputDevice<cv::Mat, cv::Point2f>::SingleInputDevice(v_null),
            dt(1.0/25.0),
            mean(0.0),
            flow_type(opt_type),
            termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS,20,0.03),
            subPixWinSize(10,10),
            winSize(31,31),
            MAX_COUNT(500),
            needToInit(true),
            displacement(0.0, 0.0)
        {
             buffer = SingleInputDevice<cv::Mat, cv::Point2f>::get_buffer();
        }

        // basic destructor
        virtual ~OpticalFlowCPUDevice() {}

        // @overriding the connect method
        // connect two devices
        virtual void connect(BaseDevice *dev)
        {
            if (nullptr != dev && this != dev)
            {
                // try to dowcast to DeviceOutput pointer
                DeviceOutput<cv::Mat> *out = dynamic_cast<DeviceOutput<cv::Mat> *>(dev);

                // try to dowcast to DeviceInput pointer
                DeviceInput<cv::Point2f> *in = dynamic_cast<DeviceInput<cv::Point2f> *>(dev);

                if (nullptr != in)
                {
                    // connect the current device's output to the external device's input
                    in->add_signal_source(this);

                }
                else if (nullptr != out)
                {
                    // connec the external device's output to this current device's input
                    // reverse case
                    add_signal_source(dev);

                    // try to update the fps
                    VideoSignalDevice *vsd = dynamic_cast<VideoSignalDevice *>(dev);

                    if(nullptr != vsd)
                    {
                        fps = vsd->get_fps();

                       dt = 1.0/fps;
                    }
                }
            }
        }

        // @overriding the main method
        virtual void run()
        {
            // the mean
            mean.x = 0.0;
            mean.y = 0.0;

            if (0 < buffer->get_size())
            {
                // get the images from the the input buffer
                frame = buffer->pop();

                // convert to gray scale
                cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

                if(!old_gray.empty())
                {
                    if (OPTICAL_FLOW_FARNEBACK == flow_type)
                    {

                        cv::calcOpticalFlowFarneback(old_gray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);

                        uflow.copyTo(flow);

                        for(int i = 0; i < flow.rows; i++)
                        {
                            cv::Point2f *row = flow.ptr<cv::Point2f>(i);

                            for(int j = 0; j < flow.cols; j++)
                            {
                                if (std::fabs(mean.x) < std::fabs(row[j].x)) { mean.x = row[j].x; }
                                if (std::fabs(mean.y) < std::fabs(row[j].y)){ mean.x = row[j].y; }
                            }

                        }


                        if (0 == flow.rows || 0 == flow.cols)
                        {
                            mean.x = 0.0;
                            mean.y = 0.0;
                        }
                    }
                    else if (OPTICAL_FLOW_LUKAS_KANADE_PYR == flow_type)
                    {
                        if (needToInit)
                        {
                            // automatic initialization
                            cv::goodFeaturesToTrack(old_gray, points[0], 1, 0.01, 10, cv::Mat(), 3, 0, 0.04);
                            cv::cornerSubPix(old_gray, points[0], subPixWinSize, cv::Size(-1,-1), termcrit);
                            needToInit = false;
                        }

                        // compúting the optical flow
                        cv::calcOpticalFlowPyrLK(old_gray, gray, points[0], points[1], status, err, winSize, 3, termcrit, 0, 0.001);

                        if (1 > points[1].size()) { needToInit = true; }

                        int k = 0;

                        for( int i = 0; i < points[1].size(); i++ )
                        {
                            if( !status[i]) { continue; }
                            if (std::fabs(mean.x) < std::fabs(points[1][i].x)) { mean.x = points[1][i].x; }
                            if (std::fabs(mean.y) > std::fabs(points[1][i].y)) { mean.y = points[1][i].y; }

                            points[1][k++] = points[1][i];
                        }

                        // resize the points
                        points[1].resize(k);

                        // swap the points
                        std::swap(points[1], points[0]);
                    }
                }

                // swap the gray images
                std::swap(old_gray, gray);
            }

            // push to outputs
            DeviceOutput<cv::Point2f>::send(mean);

        }

        // draw the optical flow
        virtual void drawOpticalFlow(cv::Mat &flow)
        {
            cv::Mat xy[2]; //X,Y
            cv::split(flow, xy);

            //calculate angle and magnitude
            cv::Mat magnitude, angle;
            cv::cartToPolar(xy[0], xy[1], magnitude, angle, true);

            //translate magnitude to range [0;1]
            double mag_max;
            cv::minMaxLoc(magnitude, nullptr, &mag_max);
            magnitude.convertTo(magnitude, -1, 1.0/mag_max);

            //build hsv image
            cv::Mat _hls[3], hsv;
            _hls[0] = angle;
            _hls[1] = cv::Mat::ones(angle.size(), CV_32F);
            _hls[2] = magnitude;
            cv::merge(_hls, 3, hsv);

            //convert to BGR and show
            cv::Mat bgr;
            cv::cvtColor(hsv, bgr, cv::COLOR_HSV2BGR);

            cv::imshow("frame", frame);
            cv::imshow("optical flow", bgr);

            cv::waitKey(15);
        }

        // add a signal source to the current input
        virtual void add_signal_source(BaseDevice *dev)
        {
            SingleInputDevice<cv::Mat, cv::Point2f>::add_signal_source(dev);

            // try to update the fps
            VideoSignalDevice *vsd = dynamic_cast<VideoSignalDevice *>(dev);

            if(nullptr != vsd)
            {
                fps = vsd->get_fps();
                dt = 1.0/fps;
            }
        }

        // disconnect the a signal source from the current inputs
        virtual void disconnect_signal_source(BaseDevice *dev)
        {
            SingleInputDevice<cv::Mat, cv::Point2f>::disconnect_signal_source(dev);
        }
};

#endif
