#include <ImageMotionModel.hpp>

#include <stdexcept>

// basic constructor with video filename and frame rate
ImageMotionModel::ImageMotionModel(const std::string input_video_filename, float frame_rate, const std::string gt, std::string output_video_filename) :
    input_video(input_video_filename),
    output_video(),
    fps(frame_rate),
    wait_time(1000/frame_rate),
    dt(0.001),
    groundtruth(gt),
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
    if (!input_video.isOpened()) {

        // throw an exception
        throw std::invalid_argument("invalid input video filename");

    }

    // get the codec
    int ex = static_cast<int>(input_video.get(CV_CAP_PROP_FOURCC));
    cv::Size S = cv::Size((int) input_video.get(CV_CAP_PROP_FRAME_WIDTH),(int) input_video.get(CV_CAP_PROP_FRAME_HEIGHT));

    // open the output video
    output_video.open(output_video_filename, ex, fps, S, true);

    if (!output_video.isOpened()) {

        // throw an exception
        throw std::invalid_argument("invalid output video filename");

    }

    // set the groundtruth flag
    hasgroundtruth = !groundtruth.empty();

    // get the first frame
    input_video >> frame;

    if (!hasgroundtruth) {

        SelectRoi();

    }

}

// basic constructor with camera and frame rate
ImageMotionModel::ImageMotionModel(float frame_rate) :
    input_video(0),
    fps(frame_rate),
    fs(1000),
    wait_time(1000/frame_rate),
    dt(0.001),
    groundtruth(),
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
    if (!input_video.isOpened()) {

        // throw an exception
        throw std::invalid_argument("invalid device");

    }

    // set the groundtruth flag
    hasgroundtruth = false;

    // get the first frame
    input_video >> frame;

    // cv::flip(frame, frame, 1);

    if (!hasgroundtruth) {

        // get the ROI
        SelectRoi();

    }

}

// mouse callback
void ImageMotionModel::MouseClickCallback(int event, int x, int y, int flags) {

    if (cv::EVENT_LBUTTONDBLCLK == event) {

        fovea.x = x - (fovea.width*0.5);
        fovea.y = y - (fovea.width*0.5);

    }

}

// the mouse click event
void ImageMotionModel::MouseClick(int event, int x, int y, int flags, void* param) {

    ImageMotionModel *self = static_cast<ImageMotionModel *>(param);

    self->MouseClickCallback(event, x, y, flags);

}

// just centering the fovea at some inside red object
// double click in the geometric object
// press f to call this function
void ImageMotionModel::FindRoi(cv::Mat temp_window) {

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
void ImageMotionModel::SelectRoi() {

    int keyboard = 0;

    cv::Mat temp_window;

    cv::namedWindow("select", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("select", MouseClick, this);

    while('q' != (char) keyboard) {

        temp_window = frame.clone();

        cv::rectangle(temp_window, fovea, cv::Scalar(0, 255, 0));

        cv::imshow("select", temp_window);

        keyboard = cv::waitKey(wait_time);

        if (1114027 == keyboard) {

            fovea.width += 1;
            fovea.height += 1;

        } else if (1114029 == keyboard) {

            fovea.width -= 1;
            fovea.height -= 1;

        } else if ('f' == (char) keyboard) {

            // center the fovea at some inside red object
            FindRoi(frame(fovea).clone());

        }

    }

    cv::destroyWindow("select");

}

// verify if two bounding box overlaps with each other
bool ImageMotionModel::Overlap(const cv::Rect &a, const cv::Rect &b) {

    cv::Rect intersection(a & b);

    return 0 < intersection.area();

}

// the main method
void ImageMotionModel::Run(const std::string &statistics_filename) {

    cv::namedWindow("frame", cv::WINDOW_AUTOSIZE);

    int keyboard = 0;

    unsigned int output_size;

    cv::Rect groundtruth_rect;

    // the groundtruth file
    std::ifstream reference;

    // set the fovea value

    if (hasgroundtruth) {

        reference.open(groundtruth);

        if (!reference.is_open()) {

            std::cout << "Could not open the groundtruth.txt file";
            return;

        }

        // get the first reference
        reference >> groundtruth_rect.x;
        reference >> groundtruth_rect.y;
        reference >> groundtruth_rect.width;
        reference >> groundtruth_rect.height;

        // Get the fovea dimensions
        GetFoveaDimension(groundtruth_rect);

        if (!stupid.initialize(frame, groundtruth_rect)) {

            // excepction
            throw std::exception();

        }

    } else {

        // start the tld tracker
        if (!stupid.initialize(frame, fovea)) {

            // excepction
            throw std::exception();

        }

    }

    // image inside the fovea
    cv::Mat cropped_frame;


    cv::Rect2i bounding_box;

    unsigned int saccades = 0, pursuits = 0;
    unsigned int lost_frames = 0;
    unsigned int success_frames = 0;

    /// the main loop
    while(!frame.empty() && 'q' != keyboard) {

        // the fovea threshold
        float f_x_min_dist  = fovea.width*0.05;
        float f_y_min_dist  = fovea.height*0.05;

        // the fovea limit
        float f_x_max_dist = fovea.width*0.75;
        float f_y_max_dist = fovea.height*0.75;

        // the fovea's center
        cv::Point2i center(fovea.width*0.5, fovea.height*0.5);

        // fovea position
        cv::cvtColor(frame(fovea).clone(), cropped_frame, cv::COLOR_BGR2GRAY);

        // computes the displacement
        //displacement = stupid.displacement(center, cropped_frame);
        bounding_box = stupid.tld_displacement(frame);

        // get the displacement
        displacement.x = (bounding_box.x + bounding_box.width * 0.5) - (fovea.x + fovea.width * 0.5);
        displacement.y = (bounding_box.y + bounding_box.height * 0.5) - (fovea.y + fovea.height * 0.5);

        // optical flow
        // current_flow = optical_flow.run(cropped_frame);
        // current_flow = optical_flow.run_farneback(cropped_frame, old_translation);
        // current_flow = optical_flow.run_farneback(cropped_frame);
        // current_flow = optical_flow.run_lkpyr(cropped_frame);
        // current_flow = optical_flow.run_lkpyr(cropped_frame, old_translation);
        current_flow = optical_flow.run_lkpyr2(cropped_frame, old_translation);
        // std::cout << "Current flow: " << current_flow << "\n";

        // verify th NaN and inf cases
        if (current_flow != current_flow || std::isinf(current_flow.x) || std::isinf(current_flow.y)) {

            current_flow = last_flow;

        }

        // is the object escaping from the fovea's center? (x direction) and
        // Does the displacement and the optical flow has the same direction?
        if (f_x_min_dist < std::fabs(displacement.x) && 0 < displacement.x * current_flow.x) {

            current_flow.x *= 1.75;

        }

        // is the object escaping from the fovea's center? (y direction)
        // Does the displacement and the optical flow have the same direction?
        if (f_y_min_dist < std::fabs(displacement.y) && 0 < displacement.y * current_flow.y) {

            current_flow.y *= 1.75;

        }

        // save the current flow
        last_flow = current_flow;

        // the first time? Used to run the step and sinusoidal input
        if (testing) {

            // file pointer
            std::ofstream plot;

            // open the file
            plot.open("test.m");

            float time = 15.0/50.0;;

            // build a step signal at signal.x and a sin at signal.y
            std::vector<cv::Point2f> signal;

            for (int i = 0; i < 50; i++) {
                signal.push_back(cv::Point2f(-15*std::sin(i*dt*2*M_PI), -time*i));

            }

            for (int i = 50; i < 6000; i++) {
                signal.push_back(cv::Point2f(-15*std::sin(i*dt*2*M_PI), -15.0));
            }

            std::vector<cv::Point2f> result = system.run(signal);

            plot << "function test()\n";
            plot << "    step = [";
            for (int i = 0; i < signal.size(); i++) {
                plot << " " << signal[i].x;

                if ((i+1) % 50 == 0)
                    plot << " ...\n";
            }

            plot << " ];\n";;

            plot << "    sinusoidal = [";
            for (int i = 0; i < signal.size(); i++) {
                plot << " " << signal[i].y;

                if ((i + 1) % 50 == 0)
                    plot << " ...\n";
            }

            plot << " ];\n";;

            plot << "    step_result = [";

            for (int i = 0; i < result.size(); i++) {
                plot << " " << result[i].x;

                if ((i + 1) % 50 == 0)
                    plot << " ...\n";
            }

            plot << "];\n";;


            plot << "    sinusoidal_result = [";

            for (int i = 0; i < result.size(); i++) {
                plot << " " << result[i].y;

                if ((i + 1) % 50 == 0)
                    plot << " ...\n";
            }

            plot << "];\n";;

            plot << "    plot(1:length(step), step, 1:length(step_result), step_result, '.');\n";

            plot << "    figure;\n";

            plot << "    plot(1:length(sinusoidal), sinusoidal, 1:length(sinusoidal), sinusoidal_result, '.')\n";

            plot << "end\n";

            // close the output file
            plot.close();

            return;

        }

        // saccade?
        if (f_x_max_dist < std::fabs(displacement.x) || f_y_max_dist < std::fabs(displacement.y)) {

            if (hasgroundtruth) {

                // get the groundtruth_rect center
                int gt_center_x = groundtruth_rect.x + groundtruth_rect.width / 2;
                int gt_center_y = groundtruth_rect.y + groundtruth_rect.height / 2;

                // get the fovea center
                int fovea_center_x = fovea.x + fovea.width / 2;
                int fovea_center_y = fovea.y + fovea.height / 2;

                // get the bounding box center
                int bbox_center_x = bounding_box.x + bounding_box.width / 2;
                int bbox_center_y = bounding_box.y + bounding_box.height / 2;

                // the fovea squared distance to the groundtruth
                int fovea_distance = std::pow(gt_center_x - fovea_center_x, 2) + std::pow(gt_center_y - fovea_center_y, 2);

                // the tracker distance to the fovea
                int tracker_distance = std::pow(gt_center_x - bbox_center_x, 2) + std::pow(gt_center_y - bbox_center_y, 2);

                if (fovea_distance < tracker_distance) {

                    // the smooth pursuit model is closer to the ground truth
                    cv::Rect internal_frame;

                    // restart the tracker
                    stupid.initialize(frame, groundtruth_rect);

                    // set the pursuit helper
                    pursuits += 1;

                    old_translation = translation;

                    // show the image
                    cv::imshow("frame", frame);

                    if (Overlap(groundtruth_rect, fovea)) {

                        success_frames += 1;

                    } else {

                        lost_frames += 1;
                    }

                    keyboard = cv::waitKey(wait_time);

                    // get the next frame
                    input_video >> frame;

                    if (!frame.empty()) {

                        cv::cvtColor(frame(fovea).clone(), cropped_frame, cv::COLOR_BGR2GRAY);

                        // reset the optical flow
                        optical_flow.init(cropped_frame);

                        // update the reference rectangle
                        reference >> groundtruth_rect.x;
                        reference >> groundtruth_rect.y;
                        reference >> groundtruth_rect.width;
                        reference >> groundtruth_rect.height;

                    }

                    continue;

                }

            }

            saccades += 1;

            std::cout << std::endl << "Sacade" << current_flow << "\n";

            // reset the fovea size
            GetFoveaDimension(groundtruth_rect);

            // reset the system
            system.reset();

            // interpolate the signal between frames
            LinearInterpolation();

            // just to insert somo direction to the dsp system
            output = system.run(interpolated);

            translation.x = 0;
            translation.y = 0;

            old_translation = translation;

            // show the image
            cv::imshow("frame", frame);

            if (Overlap(groundtruth_rect, fovea)) {

                success_frames += 1;

            } else {

                lost_frames += 1;
            }

            keyboard = cv::waitKey(wait_time);

            // get the next frame
            input_video >> frame;

            if (!frame.empty()) {

                // cv::flip(frame, frame, 1);
                cv::cvtColor(frame(fovea).clone(), cropped_frame, cv::COLOR_BGR2GRAY);

                // reset the optical flow
                optical_flow.init(cropped_frame);

                // update the reference rectangle
                reference >> groundtruth_rect.x;
                reference >> groundtruth_rect.y;
                reference >> groundtruth_rect.width;
                reference >> groundtruth_rect.height;

            }

            continue;

        }

        // considering displacement
        current_flow.x = current_flow.x*fps;
        current_flow.y = current_flow.y*fps;

        // interpolate
        LinearInterpolation();

        // clear the output
        output.clear();

        // process the interpolated signal
        output = system.run(interpolated);

        // get the output size
        output_size = output.size();

        // move the fovea
        for (unsigned int i = 0; i < output_size; ++i) {

            translation.x += (output[i].x) * dt;
            translation.y += (output[i].y) * dt;

        }

        // save the translation info
        old_translation.x = (int) translation.x;
        old_translation.y = (int) translation.y;

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

        // draw the reference frame
        cv::rectangle(frame, groundtruth_rect, cv::Scalar(0, 0,255));

        cv::putText(frame, std::to_string(saccades) + std::string("/") + std::to_string(pursuits), cv::Point(20,20), CV_FONT_NORMAL, 0.5, cv::Scalar(250,250,0), 1, cv::LINE_AA);

        // show the image
        cv::imshow("frame", frame);

        if (Overlap(groundtruth_rect, fovea)) {

            success_frames += 1;

        } else {

            lost_frames += 1;
        }

        keyboard = cv::waitKey(wait_time);

        // save the current frame to the output video
        output_video << frame;

        // get the next frame
        input_video >> frame;

        if (!frame.empty()) {

            // update the reference rectangle
            reference >> groundtruth_rect.x;
            reference >> groundtruth_rect.y;
            reference >> groundtruth_rect.width;
            reference >> groundtruth_rect.height;

        }

    }

    // save the statistics

    // open the file
    std::ofstream statistics;

    if (!statistics_filename.empty()) {

        statistics.open(statistics_filename.c_str());

    } else {

        statistics.open("statistics.csv");

    }

    if (!statistics.is_open()) {

        std::cout << "Could not open the statistics file!\n";

        return;

    }


    statistics << success_frames << ";" << lost_frames << "\n";
    statistics << saccades << ";" << pursuits << "\n";

    // close the statistics file
    statistics.close();

}

// the linear interpolation method
void ImageMotionModel::LinearInterpolation() {

    interpolated.clear();

    interpolated.push_back(current_flow);

    unsigned int interp = (fs/fps) - 1;

    for (unsigned int i = 0; i < interp; ++i) {

        interpolated.push_back(current_flow);

    }

}

// get the external frame
cv::Rect ImageMotionModel::GetExternalFrame(cv::Rect fovea) {

    // the returning external frame
    cv::Rect external_frame;

    // get the x coordinate
    external_frame.x = fovea.x - fovea.width / 2;

    int x = external_frame.x + 2 * fovea.width;

    // verify the limits
    if (x >= frame.cols) {

        external_frame.x -= (x - frame.cols);

    }

    if (1 > external_frame.x) {

        external_frame.x = 1;

    }

    // get the y coordinate
    external_frame.y = fovea.y - fovea.height / 2;

    int y = external_frame.y + 2 * fovea.height;

    // verify the limits
    if (y >= frame.rows) {

        external_frame.y -= (y - frame.rows);

    }

    if (1 > external_frame.y) {

        external_frame.y = 1;

    }

    // set the external frame size
    external_frame.width = std::min(2*fovea.width, frame.cols);
    external_frame.height = std::min(2*fovea.height, frame.rows);

    // return the resulting external frame
    return external_frame;

}

// get the external frame
cv::Rect ImageMotionModel::GetInternalFrame(cv::Rect external_frame, cv::Rect fovea) {

    // the resulting internal frame
    return cv::Rect(fovea.x - external_frame.x, fovea.y - external_frame.y, fovea.width, fovea.height);

}

//
void ImageMotionModel::GetFoveaDimension(cv::Rect &groundtruth_rect) {

    if (groundtruth_rect.width < groundtruth_rect.height) {

        fovea.width = fovea.height = groundtruth_rect.height;

        if (30 > fovea.width) {

            fovea.width = fovea.height = 30;

        } else if (100 < fovea.height) {

            fovea.height = fovea.width = 100;

        }

        // set the fovea position
        fovea.x = (groundtruth_rect.x + groundtruth_rect.width / 2) - fovea.width / 2;
        fovea.y = groundtruth_rect.y;

    } else {

        fovea.width = fovea.height = groundtruth_rect.width;

        if (30 > fovea.width) {

            fovea.width = fovea.height = 30;

        } else if (100 > fovea.height) {

            fovea.width = fovea.height = 100;

        }

        // set the fovea position
        fovea.x = groundtruth_rect.x;
        fovea.y = (groundtruth_rect.y + groundtruth_rect.height) - fovea.height / 2;

    }

    int dx = fovea.x + fovea.width;

    if (frame.cols <= dx) {

        fovea.x -= (dx - frame.cols);

    }

    if (1 > fovea.x) {

        fovea.x = 1;

    }

    // set the fovea position
    fovea.y = groundtruth_rect.y;

    int dy = fovea.y + fovea.height;

    if (frame.rows <= dy) {

        fovea.y -= (dy - frame.rows);

    }

    if (1 > fovea.y) {

        fovea.y = 1;

    }

    // start the tld tracker
    if (!stupid.initialize(frame, fovea)) {

        // excepction
        throw std::exception();

    }

}

