#include <OpticalFlowCPU.hpp>

// basic constructor
OpticalFlowCPU::OpticalFlowCPU(OpticalFlow oft) : flow_type(oft),
    termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS,20,0.03),
    subPixWinSize(10, 10), winSize(31,31), MAX_COUNT(500), needToInit(true) {}

// computes the optical flow
cv::Point2f OpticalFlowCPU::run(cv::Mat frame) {

    // the mean
    mean.x = 0.0;
    mean.y = 0.0;

    if (!frame.empty()) {

        // convert to gray scale
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        if(!old_gray.empty()) {

            if (OPTICAL_FLOW_FARNEBACK_CPU == flow_type) {

                if (gaussian.empty()) {
                    build_gaussian_kernel(frame);
                }

                cv::calcOpticalFlowFarneback(old_gray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);

                uflow.copyTo(flow);

                if (0 != flow.rows && 0 != flow.cols) {

                    if (gaussian.rows > flow.rows) {

                        k_h = flow.rows;

                    }

                    if (gaussian.cols > flow.cols) {

                        k_w = flow.cols;

                    }

                    if (k_h < k_w) {

                        k_w = k_h;

                    } else if (k_w < k_h) {

                        k_h = k_w;

                    }

                    for (int i = 0; i < k_h; i++) {

                        cv::Point2f *row = flow.ptr<cv::Point2f>(i);

                        for (int j = 0; j < k_w; j++) {

                            mean.x += row[j].x*gaussian.at<float>(i, j);
                            mean.y += row[j].y*gaussian.at<float>(i, j);

                        }
                    }

                }

                std::cout << std::endl << "Farneback mean: " << mean << std::endl;
            } else if (OPTICAL_FLOW_LUKAS_KANADE_PYR_CPU == flow_type) {

                if (needToInit) {

                    // automatic initialization
                    cv::goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, cv::Mat(), 3, 0, 0.04);

                    if (0 != points[1].size()) {

                        cv::cornerSubPix(gray, points[1], subPixWinSize, cv::Size(-1,-1), termcrit);
                        needToInit = false;

                    } else {
                        needToInit = true;
                    }

                } else {

                    if (0 == points[0].size()) {

                        needToInit == true;

                    } else {

                        // compúting the optical flow
                        cv::calcOpticalFlowPyrLK(old_gray, gray, points[0], points[1], status, err, winSize, 3, termcrit, 0, 0.001);

                        if (0 == points[1].size()) {

                            needToInit = true;

                        } else {

                            int k = 0;

                            for( int i = 0; i < points[1].size(); i++ ) {

                                if( !status[i]) {
                                    continue;
                                }

                                mean += points[1][i] - points[0][i];

                                points[1][k++] = points[1][i];

                            }

                            // computes the mean
                            if (k != 0) {
                                mean /= k;
                            } else {
                                mean.x = 0.0;
                                mean.y = 0.0;
                            }

                            if (0.05 > std::fabs(mean.x)) {
                                mean.x = 0.0;
                            }

                            if (0.05 > std::fabs(mean.y)) {
                                mean.y = 0.0;
                            }

                            // resize the points
                            points[1].resize(k);

                        }

                    }

                }

                // swap the points
                std::swap(points[1], points[0]);

                if (0 == points[0].size()) {
                    needToInit = true;
                }

            }

        }

        // swap the gray images
        std::swap(old_gray, gray);
    }

    // return the mean
    return mean;

}

// building the kernel
void OpticalFlowCPU::build_gaussian_kernel(cv::Mat frame) {

    // the normalizer
    float normalizer = 0;

    unsigned int iterations;
    unsigned int size = frame.cols;


    if (size > frame.rows) {

        size = frame.rows;

    }

    if (size % 2 == 0) {

        size -= 1;

    }

    k_h = k_w = size;

    sigma = 0.05*size;

    if (sigma > 5) {
        sigma = 5;
    }

    // the left gaussian formula part
    left_g = 1.0/(2*M_PI*sigma*sigma);

    // the right gaussian formula part
    right_g = -0.5/(sigma*sigma);

    // the gaussian kernel
    gaussian = cv::Mat(size, size, CV_32FC1);

    // how many iterations?
    iterations = (unsigned int) size*0.5;

    cv::Point2i center;

    center.x = iterations;
    center.y = iterations;

    // compute the weights to the top left part
    for (int i = 0; i <= iterations; i++) {

        // get the corner weights
        gaussian.at<float>(i, i) = gaussian_weight(center, cv::Point2i(i, i));

        // get the row and collumn weigths
        for (int j = i + 1; j <= iterations; j++) {

            float w = gaussian_weight(center, cv::Point2i(i, j));

            // get the current row position weight
            gaussian.at<float>(i, j) = w;

            // get the current collumns position weight
            gaussian.at<float>(j, i) = w;

        }

    }

    // mirror the bottom
    for (int i = iterations + 1; i < size; i++) {

        for (int j = 0; j <= iterations; j++) {

            // copy the top values to the bottom
            gaussian.at<float>(i, j) = gaussian.at<float>(2*center.x - i, j);

        }

    }

    // mirror the right part
    for (int i = 0; i < size; i++) {

        for (int j = iterations + 1; j < size; j++) {

            // copy the left values to the right
            gaussian.at<float>(i, j) = gaussian.at<float>(i, 2*center.y - j);

        }

    }

    // get the sum
    //scaling the values
    for (int i = 0; i < size; i++) {

        for (int j = 0; j < size; j++) {

            normalizer += gaussian.at<float>(i, j);


        }

    }

    // invert the sum
    if (normalizer != 0) {
        normalizer = 1.0/normalizer;
    }

    // normalize the entire kernel
    for (int i = 0; i < size; i++) {

        for (int j = 0; j < size; j++) {

            gaussian.at<float>(i, j) = gaussian.at<float>(i, j)*normalizer;

        }

    }
}

// computes the gaussian weight
float OpticalFlowCPU::gaussian_weight(cv::Point2i a, cv::Point2i b) {

    int x = std::abs(b.x - a.x);
    int y = std::abs(b.y - a.y);

    return left_g*std::exp(right_g*(x*x + y*y));;

}


