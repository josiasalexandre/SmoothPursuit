#include <OpticalFlowCPU.hpp>

// basic constructor
OpticalFlowCPU::OpticalFlowCPU(OpticalFlow oft) : flow_type(oft),
    termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS,20,0.03),
    subPixWinSize(10, 10), winSize(31,31), MAX_COUNT(500), needToInit(true),
    displacement(0.0, 0.0) {}

// computes the optical flow
cv::Point2f OpticalFlowCPU::run(cv::Mat frame) {

    // the mean
    mean.x = 0.0;
    mean.y = 0.0;

    // convert to gray scale
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    if(!old_gray.empty()) {

        if (OPTICAL_FLOW_FARNEBACK_CPU == flow_type) {

            cv::calcOpticalFlowFarneback(old_gray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);

            uflow.copyTo(flow);

            if (0 != flow.rows && 0 != flow.cols) {

                for(int i = 0; i < flow.rows; i++) {

                    cv::Point2f *row = flow.ptr<cv::Point2f>(i);

                    for(int j = 0; j < flow.cols; j++) {

                        if (std::fabs(mean.x) < std::fabs(row[j].x)) {
                            mean.x = row[j].x;
                        }

                        if (std::fabs(mean.y) < std::fabs(row[j].y)) {
                            mean.x = row[j].y;
                        }

                    }

                }

            }

            std::cout << std::endl << "farnelback mean: " << mean << std::endl;

        } else if (OPTICAL_FLOW_LUKAS_KANADE_PYR_CPU == flow_type) {

            if (needToInit) {
                // automatic initialization
                cv::goodFeaturesToTrack(old_gray, points[0], 100, 0.001, 10, cv::Mat(), 3, 0, 0.04);
                if (0 != points[0].size()) {
                    cv::cornerSubPix(old_gray, points[0], subPixWinSize, cv::Size(-1,-1), termcrit);
                    needToInit = false;
                }

            }

            if (0 == points[0].size()) {

                needToInit == true;

            } else {

                // compúting the optical flow
                cv::calcOpticalFlowPyrLK(old_gray, gray, points[0], points[1], status, err, winSize, 3, termcrit, 0, 0.001);

                if (0 == points[1].size()) {

                    needToInit = true;

                } else {

                    // get the frame copy
                    frame.copyTo(image);

                    int k = 0;

                    displacement.x = 0;
                    displacement.y = 0;

                    for( int i = 0; i < points[1].size(); i++ ) {

                        if( !status[i]) {
                            continue;
                        }

                        if (std::fabs(displacement.x) < std::fabs(points[1][i].x - points[0][i].x)) {

                            displacement.x = points[1][i].x - points[0][i].x;
                            // mean.x = displacement.x;

                        }

                        if (std::fabs(displacement.y) < std::fabs(points[1][i].y - points[0][i].y)) {

                            displacement.y = points[1][i].y - points[0][i].y;
                            // mean.y = displacement.y;

                        }

                        mean += points[1][i] - points[0][i];

                        points[1][k++] = points[1][i];

                        // draw the circles
                        cv::circle(image, points[0][i], 3, cv::Scalar(0,255,0), -1, 8);

                    }

                    // computes the mean
                    mean /= k;

                    // for drawing purpose
                    cv::imshow("tracking", image);

                    // resize the points
                    points[1].resize(k);

                }

            }

            // swap the points
            std::swap(points[1], points[0]);

            if (0 == points[0].size()) {
                needToInit = true;
            }

            //std::cout << std::endl << "lkpyr mean: " << mean << std::endl;

        }

    }

    // swap the gray images
    std::swap(old_gray, gray);

    // return the mean
    return mean;

}

