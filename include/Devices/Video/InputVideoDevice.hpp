#ifndef INPUT_VIDEO_DEVICE_H
#define INPUT_VIDEO_DEVICE_H

#include <unistd.h>

#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>

#include <InputSignalDevice.hpp>

class InputVideoDevice : public virtual InputSignalDevice<cv::Mat> {

    private:

        // the current output buffer refference
        DeviceInputBufferRef<cv::Mat> output;

        // the video
        cv::VideoCapture video;

        // a frame
        cv:: Mat frame;

        // the update rate
        unsigned int rate;

    public:

        // basic constructor - camera id
        InputVideoDevice(int id) : video(id), rate(1000*1000/30) {

            if (!video.isOpened()) {

                // throw an exception
                throw std::invalid_argument("invalid device id");

            }

        }

        // basic constructor, now with fps
        InputVideoDevice(int id, float fps) : video(id), rate(1000*1000/fps) {

            if (!video.isOpened()) {

                // throw an exception
                throw std::invalid_argument("invalid device id");

            }

        }

        // basic constructor- filename
        InputVideoDevice(const std::string filename, float fps) : video(filename), rate(1000*1000/fps) {

            if (!video.isOpened()) {

                // throw an exception
                throw std::invalid_argument("unable to open the file");

            }

        }

        // basid destructor
        ~InputVideoDevice () {

            if (video.isOpened()) {

                video.release();

            }

        }

        // the main method
        virtual void run() {

            // get the first frame
            video >> frame;

            while(!frame.empty()) {

                // lock the output vector
                DeviceOutput<cv::Mat>::output_mutex.lock();

                // send the current image to the output
                unsigned int out_size = DeviceOutput<cv::Mat>::output.size();

                if (0 < out_size) {

                    for (int i = 0; i < out_size; i++) {

                        // send the image frames
                        DeviceOutput<cv::Mat>::output[i].first->push(frame);

                        //
                        DeviceOutput<cv::Mat>::output[i].second->run();

                    }

                }

                // based on the fps
                usleep(rate);

                // unlock the output vector
                DeviceOutput<cv::Mat>::output_mutex.unlock();

                // get the next frame
                video >> frame;

            }

        }

};

#endif
