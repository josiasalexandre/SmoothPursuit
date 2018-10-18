#ifndef IMAGE_VIEWER_DEVICE_H
#define IMAGE_VIEWER_DEVICE_H

#include <thread>
#include <mutex>

#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QPixmap>
#include <QtGui/QImage>

#include <opencv2/opencv.hpp>

#include <OutputSignalDevice.hpp>
#include <DeviceOutput.hpp>

class ImageViewerDevice : virtual public OutputSignalDevice<cv::Mat>
{
    private:

        DeviceInputBuffer<cv::Mat> input;

        QGraphicsScene *scene;

        QGraphicsView *view;

        QGraphicsPixmapItem *item;

        bool opened;

        std::mutex img_view_mutex;

    public:

        ImageViewerDevice() : opened(false)
        {
            // creates an empty image
            cv::Mat empty_img;

            // set the null value
            input.first.set_null_value(empty_img);
            scene = new QGraphicsScene();
            view = new QGraphicsView(scene);
            item = new QGraphicsPixmapItem();
        }

        ~ImageViewerDevice ()
        {
            if (nullptr != input.second) { remove_signal_source(input.second); }

            view->close();

            delete item;
            delete scene;
            delete view;
        }

        // connect two devices
        virtual void connect(BaseDevice *dev)
        {
            // try to add to signal source
            add_signal_source(dev);
        }

        // disconnect two devices
        virtual void disconnect(BaseDevice *dev)
        {
            // try to remove the external device's output from the current device's input
            remove_signal_source(dev);
        }

        // the main device method
        virtual void run()
        {
            if(!opened)
            {
                scene->addItem(item);

                // lock the image viewer
                img_view_mutex.lock();

                // update the pixmap and unlock the image viewer
                show_image();

                // open a window to show the QGraphicsScene
                view->show();

                // set the flag to true
                opened = true;

            }
            else
            {
                if (img_view_mutex.try_lock())
                {
                    // a new thread!
                    std::thread show_image_thread(&ImageViewerDevice::show_image, this);
                    // leave it alone
                    show_image_thread.detach();
                }
            }
        }

        virtual void show_image()
        {
            // get the next element inside the CircularBuffer
            cv::Mat frame = input.first.pop();
            if (!frame.empty())
            {
                item->setPixmap(QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, QImage::Format_RGB888).rgbSwapped()));
            }
            img_view_mutex.unlock();
        }

        // add a signal source to the current input
        virtual void add_signal_source(BaseDevice *dev)
        {
            if (nullptr != dev)
            {
                // try to downcast the external device pointer to DeviceOutput
                DeviceOutput<cv::Mat> *out = dynamic_cast<DeviceOutput<cv::Mat> *>(dev);

                if (nullptr != out)
                {
                    // creates the buffer ref
                    DeviceInputBufferRef<cv::Mat> buffer(&input.first, this);

                    if (nullptr != input.second)
                    {
                        // remove the old publisher
                        input.second->remove_output(this);
                    }

                    // update the input
                    input.second = out;

                    // connect the current input to the external device output
                    out->add_output(buffer);
                }
            }
        }

        // remove a signal source from the current inputs
        virtual void remove_signal_source(BaseDevice *dev)
        {
            if (nullptr != dev)
            {
                // try to downcast the external device pointer to DeviceOutput
                DeviceOutput<cv::Mat> *out = dynamic_cast<DeviceOutput<cv::Mat> *>(dev);

                if(nullptr != out)
                {
                    if (out == input.second)
                    {
                        // remove the current device's input from the external device output
                        out->remove_output(this);

                        // remove the output from the current device input
                        input.second = nullptr;
                    }
                }
            }
        }


        // disconnect the a signal source from the current inputs
        virtual void disconnect_signal_source(BaseDevice *dev)
        {
            if (nullptr != dev)
            {
                // try to downcast the external device pointer to DeviceOutput
                DeviceOutput<cv::Mat> *out = dynamic_cast<DeviceOutput<cv::Mat> *>(dev);

                if(nullptr != out)
                {
                    if (out == input.second)
                    {
                        // remove the output from the current device input
                        input.second = nullptr;
                    }
                }
            }
        }

};

#endif
