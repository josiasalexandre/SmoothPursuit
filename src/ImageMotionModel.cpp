#include <ImageMotionModel.hpp>
#include <stdexcept>

// basic constructor with video filename and frame rate
ImageMotionModel::ImageMotionModel(std::string filename, float frame_rate) :
    video(filename),
    fps(frame_rate),
    wait_time(1000/frame_rate),
    dt(0.001),
    fs(1000),
    fovea(300.0, 200.0, 100, 100),
    system(),
    interpolated(0),
    current_flow(0.0, 0.0),
    last_flow(0.0, 0.0),
    displacement(0.0, 0.0),
    testing(false),
    translation(0.0, 0.0),
    old_translation(0.0, 0.0),
    stupid(150),
    optical_flow(OPTICAL_FLOW_LUKAS_KANADE_PYR_CPU)
    //optical_flow(OPTICAL_FLOW_FARNEBACK_CPU)
{

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
    wait_time(1000/frame_rate),
    dt(0.001),
    fovea(300.0, 200.0, 100, 100),
    system(),
    interpolated(0),
    current_flow(0.0, 0.0),
    last_flow(0.0, 0.0),
    displacement(0.0, 0.0),
    testing(false),
    translation(0.0, 0.0),
    old_translation(0.0, 0.0),
    stupid(150),
    //optical_flow(OPTICAL_FLOW_LUKAS_KANADE_PYR_CPU)
    optical_flow(OPTICAL_FLOW_FARNEBACK_CPU)
{

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

// just centering the fovea at some inside red object
// double click in the geometric object
// press f to call this function
void ImageMotionModel::find_roi(cv::Mat temp_window) {

    cv::Point2i center(0.0, 0.0);

    int k = 1;
    unsigned int red, green, blue;

    if (!temp_window.empty()) {

        for (int i = 0; i < temp_window.rows; i++) {

            for (int j = 0; j < temp_window.cols; j++) {

                blue = temp_window.at<cv::Vec3b>(i, j).val[0];
                green = temp_window.at<cv::Vec3b>(i, j).val[1];
                red = temp_window.at<cv::Vec3b>(i, j).val[2];

                if (150 > green && 150 < red && 150 < blue) {

                    center.x += j;
                    center.y += i;
                    k++;

                }

            }

        }

        center /= k;

        fovea.x -= (fovea.width*0.5 - center.x);
        fovea.y -= (fovea.height*0.5 - center.y);

    }

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

        keyboard = cv::waitKey(wait_time);

        if (43 == keyboard) {

            fovea.width += 1;
            fovea.height += 1;

        } else if (45 == keyboard) {

            fovea.width -= 1;
            fovea.height -= 1;

        } else if ('f' == (char) keyboard) {

            // center the fovea at some inside red object
            find_roi(frame(fovea).clone());

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

    // some output files to save octave plots
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

    // vectors to store the optical flow and the system output
    std::vector<cv::Point2f> input_signal(0);
    std::vector<cv::Point2f> output_signal(0);

    // image inside the fovea
    cv::Mat cropped_frame;

    // the fovea's center
    cv::Point2i center(fovea.width*0.5, fovea.height*0.5);

    // the fovea threshold
    float f_x_min_dist  = fovea.width*0.05;
    float f_y_min_dist  = fovea.height*0.05;

    // the fovea limit
    float f_x_max_dist = fovea.width*0.4;
    float f_y_max_dist = fovea.height*0.4;

    // the main loop
    while(!frame.empty() && 'q' != keyboard) {

        // fovea position
        cv::cvtColor(frame(fovea).clone(), cropped_frame, cv::COLOR_BGR2GRAY);

        // computes the displacement
        displacement = stupid.displacement(center, cropped_frame);
        // displacement = stupid.stupid_displacement(center, frame(fovea).clone());

        // optical flow
        // current_flow = optical_flow.run(cropped_frame);
        // current_flow = optical_flow.run_farneback(cropped_frame, old_translation);
        // current_flow = optical_flow.run_farneback(cropped_frame);
        // current_flow = optical_flow.run_lkpyr(cropped_frame);
        // current_flow = optical_flow.run_lkpyr(cropped_frame, old_translation);
        current_flow = optical_flow.run_lkpyr2(cropped_frame, old_translation);

        // verify th NaN and inf cases
        if (current_flow != current_flow || std::isinf(current_flow.x) || std::isinf(current_flow.y)) {
            current_flow = last_flow;
            std::cout << std::endl << NAN << std::endl;
        }

        // is the object escaping from the fovea's center? (x direction) and
        // the displacement and the optical flow has the same direction?
        if (f_x_min_dist < std::fabs(displacement.x) && 0 < displacement.x*current_flow.x) {

            current_flow.x *= 1.75;

        }

        // is the object escaping from the fovea's center? (y direction)
        // the displacement and the optical flow has the same direction?
        if (f_y_min_dist < std::fabs(displacement.y) && 0 < displacement.y*current_flow.y) {

            current_flow.y *= 1.75;

        }

        // save the current flow to the input signal vector
        input_signal.push_back(current_flow);

        // save the current mean
        last_flow = current_flow;

        // the first time? Used to run the step and sinusoidal input
        if (testing) {

            // file pointer
            std::ofstream plot;

            // open the file
            plot.open("plot.mat");

            float time = 15.0/50.0;;

            // build a step signal at signal.x and a sin at signal.y
            std::vector<cv::Point2f> signal;

            for (int i = 0; i < 50; i++) {
                signal.push_back(cv::Point2f(time*i, 15*std::sin(i*dt*2*M_PI)));

            }

            for (int i = 50; i < 6000; i++) {
                signal.push_back(cv::Point2f(15.0, 15*std::sin(i*dt*2*M_PI)));
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

        // saccade
        if (f_x_max_dist < std::fabs(displacement.x) || f_y_max_dist < std::fabs(displacement.y)) {

            std::cout << std::endl << "Sacade" << current_flow << std::endl;

            fovea.x += displacement.x;
            fovea.y += displacement.y;

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

            // reset the system
            system.reset();

            // interpolate the signal between frames
            linear_interpolation();

            // just to insert somo direction to the dsp system
            output = system.run(interpolated);

            translation.x = 0;
            translation.y = 0;

            old_translation = translation;

            // show the image
            cv::imshow("frame", frame);

            keyboard = cv::waitKey(wait_time);

            // get the next frame
            video >> frame;

            cv::cvtColor(frame(fovea).clone(), cropped_frame, cv::COLOR_BGR2GRAY);

            // reset the optical flow
            optical_flow.init(cropped_frame);

            continue;

        }

        // considering displacement
        current_flow.x = current_flow.x*fps;
        current_flow.y = current_flow.y*fps;

        // interpolate
        linear_interpolation();

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

            translation.x += (output[i].x)*dt;
            translation.y += (output[i].y)*dt;

        }

        // save the translation info
        old_translation.x = (int) translation.x;
        old_translation.y = (int) translation.y;

        // computes the error
        error = current_flow - translation;

        // save the error to the plot file
        error_file << " " << error;

        if (1 < std::abs(translation.x)) {

            fovea.x += (int) translation.x;

            translation.x -= (int) translation.x;

        }

        if (1 < std::abs(translation.y)) {

            fovea.y += (int) translation.y;

            translation.y -= (int) translation.y;

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

        cv::Point l1, l2, l3, l4;

        l1.x = fovea.x + center.x - 10;
        l1.y = fovea.y + center.y;

        l2.x = fovea.x + center.x + 10;
        l2.y = fovea.y + center.y;

        l3.x = fovea.x + center.x;
        l3.y = fovea.y + center.y - 10;

        l4.x = fovea.x + center.x;
        l4.y = fovea.y + center.y + 10;

        // draw the lines
        cv::line(frame, l1, l2, cv::Scalar(0,255,0));
        cv::line(frame, l3, l4, cv::Scalar(0,255,0));

        // draw the fovea rectangle
        cv::rectangle(frame, fovea, cv::Scalar(0, 255, 0));

        // show the image
        cv::imshow("frame", frame);

        keyboard = cv::waitKey(wait_time);

        // get the next frame
        video >> frame;

    }

    // savin some text file to run octave simulations and plots
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

    interpolated.push_back(current_flow);

    unsigned int interp = (fs/fps) - 1;

    for (int i = 0; i < interp; i++) {

        interpolated.push_back(current_flow);

    }

}
