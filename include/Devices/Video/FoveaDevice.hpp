#ifndef RETINA_DEVICE_H
#define RETINA_DEVICE_H

#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <boost/concept_check.hpp>

#include <BaseDevice.hpp>
#include <DeviceInput.hpp>
#include <DeviceOutput.hpp>
#include <VideoSignalDevice.hpp>


class FoveaDevice :
                        virtual public DeviceOutput<cv::Mat>,
                        virtual public DeviceInput<cv::Mat>,
                        virtual public DeviceInput<cv::Point2f>,
                        virtual public VideoSignalDevice
{
    private:

        // the rectangle
        cv::Rect retina;

        // the translation
        cv::Point2f translation;

        // the first device input buffer
        DeviceInputBuffer<cv::Mat> frame_input;

        // the second device input buffer
        DeviceInputBuffer<cv::Point2f> velocity_input;

        // first time running
        bool first_time;

        // the last frame
        cv::Mat frame;

        // the frame rate
        float fps, dt, rate;

        bool selected;

        // mouse callback
        void mouse_click_callback(int event, int x, int y, int flags) {

            if (cv::EVENT_LBUTTONDBLCLK == event) {

                retina.x = x - 50;
                retina.y = y - 50;

                selected = true;

            }

        }

        static void mouse_click(int event, int x, int y, int flags, void* param) {

            FoveaDevice *self = static_cast<FoveaDevice *>(param);

            self->mouse_click_callback(event, x, y, flags);

        }

        // private methods
        void select_roi() {

            selected = false;

            int keyboard = 0;

            cv::Mat temp_window;

            cv::namedWindow("select", cv::WINDOW_AUTOSIZE);

            cv::setMouseCallback("select", mouse_click, this);

            while('q' != (char) keyboard || !selected) {

                temp_window = frame.clone();

                cv::rectangle(temp_window, retina, cv::Scalar(0, 255, 0));

                cv::imshow("select", temp_window);

                keyboard = cv::waitKey(fps);

                if (43 == keyboard) {

                    retina.width += 1;
                    retina.height += 1;

                } else if (45 == keyboard) {

                    retina.width -= 1;
                    retina.height -= 1;

                }

            }

            cv::destroyWindow("select");

        }

    public:

        // basic constructor
        FoveaDevice() : first_time(true), retina(300.0, 200.0, 100, 100), translation(0.0, 0.0), fps(30.0), dt(1.0/30.0), rate(1000.0/30.0) {}

        // basic destructor
        ~FoveaDevice() {

            if (nullptr != frame_input.second) {

                remove_signal_source(frame_input.second);

            }

            if (nullptr != velocity_input.second) {

                remove_signal_source(velocity_input.second);

            }

        }
        // connect two devices
        virtual void connect(BaseDevice *dev) {

            if (nullptr != dev) {

                // try to downcast to DeviceInput<cv::Mat> pointer
                DeviceInput<cv::Mat> *in = dynamic_cast<DeviceInput<cv::Mat> *>(dev);

                if (nullptr != in) {

                    in->add_signal_source(this);

                } else {

                    add_signal_source(dev);

                }

            }

        }

        // disconnect two devices
        virtual void disconnect(BaseDevice *dev) {


            if (nullptr != dev) {

                // try to remove from the input
                remove_signal_source(dev);

                // try to dowcast to DeviceInput pointer
                DeviceInput<cv::Mat> *out_mat = dynamic_cast<DeviceInput<cv::Mat> *>(dev);

                if (nullptr != out_mat) {

                    // try to disconnect this output device from the external device's input(s)
                    out_mat->remove_signal_source(this);

                } else {

                    // try to dowcast to DeviceInput pointer
                    DeviceInput<cv::Point2f> *out_point = dynamic_cast<DeviceInput<cv::Point2f> *>(dev);

                    if (nullptr != out_point) {

                        // try to disconnect this output device from the external device's input(s)
                        out_point->remove_signal_source(this);

                    }

                }

            }

        }

        // the main device method
        virtual void run() {

            cv::Mat cropped_image;

            if (!frame_input.first.empty()) {

                // the frame buffer
                frame = frame_input.first.pop().clone();

                if (first_time) {

                    // set the flag
                    first_time = false;

                    // get the frame ROI
                    select_roi();

                }

                if (!velocity_input.first.empty()) {

                    // get the translation command
                    translation += velocity_input.first.pop()*dt;

                    // move the retina
                    retina.x += (int) translation.x;
                    retina.y += (int) translation.y;

                    if (1 > retina.x) {

                        retina.x = 1;

                    } else if (retina.x > frame.cols - 101) {

                        retina.x = frame.cols - 101;

                    }

                    if (1 > retina.y) {

                        retina.y = 1;

                    } else if (retina.y > frame.rows - 101) {

                        retina.y = frame.rows - 101;

                    }

                }



                // crop
                cropped_image = frame(retina).clone();

                cv::Point l1, l2, l3, l4;

                l1.x = retina.x + 40;
                l1.y = retina.y + 50;

                l2.x = retina.x + 60;
                l2.y = retina.y + 50;

                l3.x = retina.x + 50;
                l3.y = retina.y + 40;

                l4.x = retina.x + 50;
                l4.y = retina.y + 60;

                cv::line(frame, l1, l2, cv::Scalar(0,255,0));
                cv::line(frame, l3, l4, cv::Scalar(0,255,0));

                cv::imshow("frame", frame);
                cv::imshow("crop", cropped_image);

                cv::waitKey(1);

                // lock the output pointers
                output_mutex.lock();

                // send the value to all external devices
                if (0 < output.size()) {


                    int out_size = output.size();

                    for (int i = 0; i < out_size; i++) {

                        // send the value
                        output[i].first->push(cropped_image);

                    }

                }
                // unlock the output pointers
                output_mutex.unlock();

            }

        }

         // add a signal source to the cur  rent input
        virtual void add_signal_source(BaseDevice *dev) {

            if(nullptr != dev) {

                // try to downcast to DeviceOutput<cv::Mat>
                DeviceOutput<cv::Mat> *out_mat = dynamic_cast<DeviceOutput<cv::Mat> *>(dev);

                if (nullptr != out_mat) {

                    // build the buffer ref
                    DeviceInputBufferRef<cv::Mat> frame_buffer(&frame_input.first, static_cast<DeviceInput<cv::Mat> *>(this));

                    if (nullptr != frame_input.second) {

                        frame_input.second->remove_output(frame_buffer);

                    }

                    out_mat->add_output(frame_buffer);

                    frame_input.second = out_mat;

                    // try to update the fps
                    VideoSignalDevice *vsd = dynamic_cast<VideoSignalDevice *>(dev);

                    if(nullptr != vsd) {

                        fps = vsd->get_fps();

                        dt = 1.0/fps;

                        rate = 1000/fps;

                    }

                } else {

                    // try to downcast to DeviceOutput<cv::Mat>
                    DeviceOutput<cv::Point2f> *out_point = dynamic_cast<DeviceOutput<cv::Point2f> *>(dev);

                    if (nullptr != out_point) {

                        // build the buffer ref
                        DeviceInputBufferRef<cv::Point2f> vel_buffer(&velocity_input.first, static_cast<DeviceInput<cv::Point2f> *>(this));

                        if (nullptr != velocity_input.second) {

                            velocity_input.second->remove_output(vel_buffer);

                        }

                        out_point->add_output(vel_buffer);

                        velocity_input.second = out_point;

                    }

                }

            }

        }

        // remove a signal source from the current inputs
        virtual void remove_signal_source(BaseDevice *dev) {

            if (nullptr != dev) {

                // try to downcast to DeviceOutput<cv::Mat>
                DeviceOutput<cv::Mat> *out_mat = dynamic_cast<DeviceOutput<cv::Mat> *>(dev);

                if (nullptr != out_mat && out_mat == frame_input.second) {

                    // build the buffer ref
                    DeviceInputBufferRef<cv::Mat> frame_buffer(&frame_input.first, static_cast<DeviceInput<cv::Mat> *>(this));

                    frame_input.second->remove_output(frame_buffer);

                    frame_input.second = nullptr;

                } else {

                    // try to downcast to DeviceOutput<cv::Point2f> pointer
                    DeviceOutput<cv::Point2f> *out_point = dynamic_cast<DeviceOutput<cv::Point2f> *>(dev);

                    if (nullptr != out_point && out_point == velocity_input.second) {

                        // build the buffer ref
                        DeviceInputBufferRef<cv::Point2f> vel_buffer(&velocity_input.first, static_cast<DeviceInput<cv::Point2f> *>(this));

                        velocity_input.second->remove_output(vel_buffer);

                        velocity_input.second = nullptr;

                    }

                }

            }

        }

        // disconnect the a signal source from the current inputs
        virtual void disconnect_signal_source(BaseDevice *dev) {

            if (nullptr != dev) {

                // try to downcast to DeviceOutput<cv::Mat>
                DeviceOutput<cv::Mat> *out_mat = dynamic_cast<DeviceOutput<cv::Mat> *>(dev);

                if (nullptr != out_mat && out_mat == frame_input.second) {

                    frame_input.second = nullptr;

                } else {

                    // try to downcast to DeviceOutput<cv::Point2f> pointer
                    DeviceOutput<cv::Point2f> *out_point = dynamic_cast<DeviceOutput<cv::Point2f> *>(dev);

                    if (nullptr != out_point && out_point == velocity_input.second) {

                        velocity_input.second = nullptr;

                    }

                }

            }

        }

        // get the frame rate
        virtual float get_fps() {

            return fps;

        }

        //
        virtual CircularBuffer<cv::Mat>* get_buffer() {

            return nullptr;

        }


};

#endif
