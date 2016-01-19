#include <ImageMotionModel.hpp>
#include <stdexcept>

// basic constructor with video filename and frame rate
ImageMotionModel::ImageMotionModel(std::string filename, float frame_rate) :
    video(filename), fps(frame_rate), fs(1000), fovea(300.0, 200.0, 100, 100), system(), interpolated(0),
    current_mean(0.0, 0.0), old_mean(0.0, 0.0), interp(false), translation(0.0, 0.0)
{

    inverse_fs = 1.0/fs;

    // verify video capture object
    if (!video.isOpened()) {

        // throw an exception
        throw std::invalid_argument("invalid filename");

    }

    // get the first frame
    video >> frame;

    select_roi();

}

// basic constructor with camera and frame rate
ImageMotionModel::ImageMotionModel(float frame_rate) :
    video(0), fps(frame_rate), fs(1000), fovea(300.0, 200.0, 100, 100), translation(0.0, 0.0), system(), interpolated(0),
    current_mean(0.0, 0.0), old_mean(0, 0), interp(false)
{

    inverse_fs = 1.0/fs;

    // verify video capture object
    if (!video.isOpened()) {

        // throw an exception
        throw std::invalid_argument("invalid device");

    }

    // get the first frame
    video >> frame;

    // get the ROI
    select_roi();

}

// mouse callback
void ImageMotionModel::mouse_click_callback(int event, int x, int y, int flags) {

    if (cv::EVENT_LBUTTONDBLCLK == event) {

        fovea.x = x - 50;
        fovea.y = y - 50;

    }

}

// the mouse click event
void ImageMotionModel::mouse_click(int event, int x, int y, int flags, void* param) {

    ImageMotionModel *self = static_cast<ImageMotionModel *>(param);

    self->mouse_click_callback(event, x, y, flags);

}

// get the REGION OF INTEREST
void ImageMotionModel::select_roi() {

    int keyboard = 0;

    cv::Mat temp_window;

    cv::namedWindow("select", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("select", mouse_click, this);

    while('q' != (char) keyboard) {

        temp_window = frame.clone();

        cv::rectangle(temp_window, fovea, cv::Scalar(0, 255, 0));

        cv::imshow("select", temp_window);

        keyboard = cv::waitKey(fps);

        if (43 == keyboard) {

            fovea.width += 1;
            fovea.height += 1;

        } else if (45 == keyboard) {

            fovea.width -= 1;
            fovea.height -= 1;

        }

    }

    cv::destroyWindow("select");

}

// the main method
void ImageMotionModel::run() {

    std::ofstream plot;

    plot.open("plot.mat");

    cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);

    int keyboard = 0;

    int output_size;

    while(!frame.empty() && 'q' != keyboard) {

        // fovea position

        // optical flow
        // displacement?
        current_mean = optical_flow.run(frame(fovea).clone());

        if (0.0001 > std::fabs(current_mean.x)) {
            current_mean.x = 0;
        }

        if (0.0001 > std::fabs(current_mean.y)) {
            current_mean.y = 0;
        }

        // considering displacement
        current_mean = current_mean/0.04;

        if (!interp) {

            // update the old mean value
            old_mean = current_mean;

            interp = true;

            float time = 15.0/50.0;;
            std::vector<cv::Point2f> step;
            for (int i = 0; i < 50; i++) {
                step.push_back(cv::Point2f(time*i, 15*std::sin(i*0.001*2*M_PI)));
//                 step.push_back(cv::Point2f(time*i, std::sin(i*0.001*2.3873241491*2*M_PI)));

            }

            for (int i = 50; i < 6000; i++) {
                step.push_back(cv::Point2f(15.0, 15*std::sin(i*0.001*2*M_PI)));
//                 step.push_back(cv::Point2f(1.0, std::sin(i*0.001*2.3873241491*2*M_PI)));
            }

            std::vector<cv::Point2f> result = system.run(step);

            plot << "step = [";
            for (int i = 0; i < step.size(); i++) {
                plot << " " << step[i].x;
            }

            plot << " ]" << std::endl;

            plot << "sinusoidal = [";
            for (int i = 0; i < step.size(); i++) {
                plot << " " << step[i].y;
            }

            plot << " ]" << std::endl;

            plot << std::endl << "step_result = [";

            for (int i = 0; i < result.size(); i++) {
                plot << " " << result[i].x;
            }

            plot << " ]" << std::endl;


            plot << std::endl << "sinusoidal_result = [";

            for (int i = 0; i < result.size(); i++) {
                plot << " " << result[i].y;
            }

            plot << " ]" << std::endl;

            plot << std::endl << "graphics_toolkit('gnuplot')" << std::endl;

            plot << std::endl << "plot(1:length(step), step, 1:length(step_result), step_result, '.')" << std::endl;

            plot << std::endl << "figure" << std::endl;

            plot << std::endl << "plot(1:length(sinusoidal), sinusoidal, 1:length(sinusoidal), sinusoidal_result, '.')" << std::endl;

            plot << "pause" << std::endl;;

            return;

        }

        // interpolate
        linear_interpolation();

        // update the old mean value
        old_mean = current_mean;

        // process the interpolated signal

        std::vector<cv::Point2f> out = system.run(interpolated);
        //std::cout << std::endl << "Output: " << out << std::endl;

        // get the output size
        output_size = out.size();

        // move the fovea
        for (int i = 0; i < output_size; i++) {

            translation += out[i]*0.001;

        }



        if (1 < translation.x || -1 > translation.x) {
            fovea.x += (int) std::floor(translation.x + 0.5);
            // std::cout << std::endl << "Fovea x: " << fovea.x << std::endl;
            translation.x = 0;
        }

        if (1 < translation.y || -1 > translation.y) {
            fovea.y += (int) std::floor(translation.y + 0.5);
            // std::cout << std::endl << "Fovea y: " << fovea.y << std::endl;
            translation.y = 0;
        }

        if (1 > fovea.x) {

            fovea.x = 1;

        } else if (fovea.x > frame.cols - 101) {

            fovea.x = frame.cols - 101;

        }

        if (1 > fovea.y) {

            fovea.y = 1;

        } else if (fovea.y > frame.rows - 101) {

            fovea.y = frame.rows - 101;

        }

        // crop
        cv::Mat cropped_image = frame(fovea).clone();

        cv::Point l1, l2, l3, l4;

        l1.x = fovea.x + 40;
        l1.y = fovea.y + 50;

        l2.x = fovea.x + 60;
        l2.y = fovea.y + 50;

        l3.x = fovea.x + 50;
        l3.y = fovea.y + 40;

        l4.x = fovea.x + 50;
        l4.y = fovea.y + 60;

        cv::line(frame, l1, l2, cv::Scalar(0,255,0));
        cv::line(frame, l3, l4, cv::Scalar(0,255,0));

        // show the image
        cv::imshow("frame", frame);
        cv::imshow("fovea", cropped_image);

        keyboard = cv::waitKey(fps);

        // get the next frame
        video >> frame;


        // optical flow - plant velocity command

        // interpolation


        // delay

        // process first pathway
        // linear gain
        // second order LOW PASS filter

        // optical flow derivative

        // process second pathway
        // impulse gain
        // second order LOW PASS filter

        // process third pathway
        // smooth gain
        // second order LOW PASS filter

        // sum all paths

        // signal integration

        // low pass filtering

        // feedback to retina and summing

    }

    //
    plot.close();
}

// the linear interpolation method
void ImageMotionModel::linear_interpolation() {

    unsigned int interp = (fs/fps) - 1;
    float a1, a2;
    cv::Point2f new_point;

    // clear the the interpolation
    interpolated.clear();

    // x interpolation
    // save the first value
    interpolated.push_back(old_mean);

    // get the inclination delta y / delta x
    a1 = (current_mean.x - old_mean.x);
    a2 = (current_mean.y - old_mean.y);

    for (int i = 1; i <= interp; i++) {

        new_point.x = a1*(i*0.001) + old_mean.x;
        new_point.y = a2*(i*0.001) + old_mean.y;

        interpolated.push_back(new_point);

    }

    // update the mean

}
