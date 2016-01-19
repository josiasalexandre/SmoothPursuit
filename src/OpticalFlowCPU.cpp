#include <OpticalFlowCPU.hpp>

// computes the optical flow
cv::Point2f OpticalFlowCPU::run(cv::Mat frame) {

    // the mean
    mean.x = 0.0;
    mean.y = 0.0;

    // convert to gray scale
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    if(!old_gray.empty()) {

        cv::calcOpticalFlowFarneback(old_gray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);

        uflow.copyTo(flow);

        cv::Point2f greater(-1e12, -1e12);

        for(int i = 0; i < flow.rows; i++) {

            cv::Point2f *row = flow.ptr<cv::Point2f>(i);

            for(int j = 0; j < flow.cols; j++) {

                if (row[j].x > greater.x) {
                    greater = row[j];
                }
                mean += row[j];

            }

        }

    }

    std::swap(old_gray, gray);

    if (0 != flow.rows && 0 != flow.cols) {

        mean /= ((float) flow.rows*flow.cols);

    } else {

        mean.x = 0.0;
        mean.y = 0.0;

    }

    // return the mean
    return mean;

}

