#include <ImageMotionModel.hpp>
#include <stdexcept>

// basic constructor with video filename and frame rate
ImageMotionModel::ImageMotionModel(std::string filename, float frame_rate) :
    video(filename),
    fps(frame_rate),
    fs(1000),
    fovea(300.0, 200.0, 100, 100),
    system(),
    interpolated(0),
    current_mean(0.0, 0.0),
    mean_z1(0.0, 0.0),
    mean_z2(0.0, 0.0),
    interp(false),
    translation(0.0, 0.0),
    stupid(150),
    optical_flow(OPTICAL_FLOW_LUKAS_KANADE_PYR_CPU)
    //optical_flow(OPTICAL_FLOW_FARNEBACK_CPU)
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
    video(0),
    fps(frame_rate),
    fs(1000),
    fovea(300.0, 200.0, 100, 100),
    system(),
    interpolated(0),
    current_mean(0.0, 0.0),
    mean_z1(0.0, 0.0),
    mean_z2(0.0, 0.0),
    interp(false),
    translation(0.0, 0.0),
    stupid(150),
    optical_flow(OPTICAL_FLOW_LUKAS_KANADE_PYR_CPU)
    // optical_flow(OPTICAL_FLOW_FARNEBACK_CPU)
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

        fovea.x = x - (fovea.width*0.5);
        fovea.y = y - (fovea.width*0.5);

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

    cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);

    int keyboard = 0;

    int output_size;

    cv::Point2f error(0.0, 0.0);

    std::ofstream error_file, signal_file;

    error_file.open("error.mat");
    if (!error_file.is_open()) {
        std::cout << std::endl << "Could not open the file error.mat" << std::endl;
        return;
    }
    signal_file.open("signal.mat");
    if (!signal_file.is_open()) {
        std::cout << std::endl << "Could not open the file signal.mat" << std::endl;
        return;
    }

    std::vector<cv::Point2f> input_signal(0);
    std::vector<cv::Point2f> output_signal(0);

    error_file << "error = [";

    while(!frame.empty() && 'q' != keyboard) {

        // fovea position

        // optical flow
        // displacement?
        current_mean = stupid.displacement(cv::Point2i(fovea.width*0.5, fovea.height*0.5), frame(fovea).clone());
        //current_mean = optical_flow.run(frame(fovea).clone());
        current_mean.y = 0;

        if (current_mean != current_mean) {
            current_mean = mean_z1;
            std::cout << std::endl << NAN << std::endl;
        }

        //save the input signal
        input_signal.push_back(current_mean);

        if (0.15 > std::fabs(current_mean.x)) {

            current_mean.x = 0.0;

        } /* else if (0 > current_mean.x*mean_z1.x) {


            //current_mean.x = -current_mean.x*;
            current_mean.x *= 0.1;

        }
        */

        /*
        if (0.15 > std::fabs(current_mean.y)) {

            current_mean.y = 0.0;

        } else if (0 > current_mean.y*mean_z1.y) {

            current_mean.y *= 0.1;
            //current_mean.y = -current_mean.y;

        }
        */


        // considering displacement
        current_mean.x = current_mean.x/0.04;
        current_mean.y = current_mean.y/0.04;

        if (!interp) {

            // update the old mean value
            mean_z1 = current_mean;

            interp = true;

            // continue
            continue;

            // file pointer
            std::ofstream plot;

            // open the file
            plot.open("plot.mat");

            float time = 15.0/50.0;;

            // build a step signal at signal.x and a sin at signal.y
            std::vector<cv::Point2f> signal;

            for (int i = 0; i < 50; i++) {
                signal.push_back(cv::Point2f(time*i, 15*std::sin(i*0.001*2*M_PI)));

            }

            for (int i = 50; i < 6000; i++) {
                signal.push_back(cv::Point2f(15.0, 15*std::sin(i*0.001*2*M_PI)));
            }

            std::vector<cv::Point2f> result = system.run(signal);

            plot << "step = [";
            for (int i = 0; i < signal.size(); i++) {
                plot << " " << signal[i].x;
            }

            plot << " ]" << std::endl;

            plot << "sinusoidal = [";
            for (int i = 0; i < signal.size(); i++) {
                plot << " " << signal[i].y;
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

            // close the output file
            plot.close();

            return;

        }

        // interpolate
        linear_interpolation();

        // update the old mean value
        mean_z1 = current_mean;

        // clear the output
        output.clear();

        // process the interpolated signal
        output = system.run(interpolated);

        //save the input signal
        output_signal.insert(output_signal.end(), output.begin(), output.end());

        // get the output size
        output_size = output.size();

        // move the fovea
        for (int i = 0; i < output_size; i++) {

            translation += output[i]*0.001;

        }

        if (current_mean.x != 0 || current_mean.y != 0) {

            // computes the error
            error = current_mean - translation;

            error_file << " " << error;

        }

        if (1 < translation.x || -1 > translation.x) {

            int x = (int) std::floor(translation.x + 0.5);

            fovea.x += x;

            translation.x -= x;

        }

        if (1 < translation.y || -1 > translation.y) {

            int y = (int) std::floor(translation.y + 0.5);

            fovea.y += y;

            translation.y -= y;

        }

        if (1 > fovea.x) {

            fovea.x = 1;

        } else if (fovea.x > frame.cols - (fovea.width+1)) {

            fovea.x = frame.cols - (fovea.width + 1);

        }

        if (1 > fovea.y) {

            fovea.y = 1;

        } else if (fovea.y > frame.rows - (fovea.width + 1)) {

            fovea.y = frame.rows - (fovea.width + 1);

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

    }

    error_file << " ]" << std::endl;

    error_file << "graphics_toolkit('gnuplot')" << std::endl;

    error_file << "plot(1:length(error), error)" << std::endl;

    error_file << "pause" << std::endl;

    error_file.close();

    signal_file << "inputx = [";
    // save the input and output signal

    for (int i = 0; i < input_signal.size(); i++) {

        signal_file << " " << input_signal[i].x;

    }

    signal_file << " ]" << std::endl;

    signal_file << "inputy = [";
    // save the input and output signal

    for (int i = 0; i < input_signal.size(); i++) {

        signal_file << " " << input_signal[i].y;

    }

    signal_file << " ]" << std::endl;

    signal_file << "outputx = [";
    for (int i = 0; i < output_signal.size(); i++) {

        signal_file << " " << output_signal[i].x;

    }

    signal_file << " ]" << std::endl;

    signal_file << "outputy = [";
    for (int i = 0; i < output_signal.size(); i++) {

        signal_file << " " << output_signal[i].y;

    }

    signal_file << " ]" << std::endl;

    signal_file << std::endl << "graphics_toolkit('gnuplot')" << std::endl;

    signal_file << "filt = fir1(80, 0.1)" << std::endl;

    signal_file << "filtered = filter(filt, 1, inputx)" << std::endl;

    signal_file << "plot(1:length(inputx), inputx)" << std::endl;

    signal_file << "figure" << std::endl;

    signal_file << "plot(1:length(filtered), filtered)" << std::endl;

    signal_file << "figure" << std::endl;

    signal_file << "plot(1:length(outputx), outputx)" << std::endl;

    signal_file << "pause" << std::endl;

    signal_file.close();

}

// the linear interpolation method
void ImageMotionModel::linear_interpolation() {

    interpolated.clear();

    interpolated.push_back(current_mean);

    unsigned int interp = (fs/fps) - 1;

    for (int i = 0; i < interp; i++) {

        interpolated.push_back(current_mean);

    }

}
