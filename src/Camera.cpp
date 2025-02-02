#include "Camera.hpp"
#include <opencv2/calib3d.hpp>   // findHomography
#include <opencv2/imgproc.hpp>   // warpPerspective, perspectiveTransform
#include <stdexcept>
#include <iostream>

Camera::Camera() {
    homography_ = cv::Mat::eye(3, 3, CV_64F);
}

Camera::Camera(const cv::Mat& homography) {
    if (homography.empty() || homography.rows != 3 || homography.cols != 3) {
        throw std::invalid_argument("неправильный размер матрицы гомографии");
    }
    homography_ = homography.clone();
}

bool Camera::computeHomography(const std::vector<cv::Point2f>& imagePoints,
                           const std::vector<cv::Point2f>& worldPoints) {
    if (imagePoints.size() < 4 || worldPoints.size() < 4 || imagePoints.size() != worldPoints.size()) {
        std::cerr << "недостаточно данных для вычисления гомографии\n";
        return false;
    }

    double scale = 1.0;
    std::vector<cv::Point2f> dstPoints;
    for (const auto& pt : worldPoints) {
         cv::Point2f p(static_cast<float>(scale * pt.y),
                       static_cast<float>(scale * pt.x));
         dstPoints.push_back(p);
    }
    
    cv::Mat H = cv::findHomography(imagePoints, dstPoints, 0);
    if (H.empty() || H.rows != 3 || H.cols != 3) {
        std::cerr << "Не удалось вычислить гомографию.\n";
        return false;
    }
    homography_ = H.clone();
    return true;
}

cv::Point2f Camera::projectPoint(const cv::Point2f& imagePoint) const {
    std::vector<cv::Point2f> srcPts(1, imagePoint);
    std::vector<cv::Point2f> dstPts;
    cv::perspectiveTransform(srcPts, dstPts, homography_);
    if (!dstPts.empty())
        return dstPts[0];
    return cv::Point2f(0.f, 0.f);
}
