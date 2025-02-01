#include "Camera.hpp"
#include <opencv2/calib3d.hpp>   // findHomography
#include <opencv2/imgproc.hpp>   // warpPerspective, perspectiveTransform
#include <stdexcept>
#include <iostream>

Camera::Camera()
{
    // Единичная гомография (матрица идентичности)
    homography_ = cv::Mat::eye(3, 3, CV_64F);
}

Camera::Camera(const cv::Mat& homography)
{
    if (homography.empty() || homography.rows != 3 || homography.cols != 3)
    {
        throw std::invalid_argument("Camera: homography must be a non-empty 3x3 matrix!");
    }
    homography_ = homography.clone();
}

bool Camera::computeHomography(const std::vector<cv::Point2f>& imagePoints,
                               const std::vector<cv::Point2f>& worldPoints)
{
    if (imagePoints.size() < 4 || worldPoints.size() < 4 || imagePoints.size() != worldPoints.size())
    {
        std::cerr << "[Camera::computeHomography] Требуется не менее 4 пар точек с равным количеством!\n";
        return false;
    }
    // Вычисляем гомографию с использованием RANSAC для устойчивости к шумам
    cv::Mat H = cv::findHomography(imagePoints, worldPoints, cv::RANSAC);
    if (H.empty() || H.rows != 3 || H.cols != 3)
    {
        std::cerr << "[Camera::computeHomography] Не удалось вычислить гомографию.\n";
        return false;
    }
    homography_ = H.clone();
    return true;
}

cv::Point2f Camera::projectPoint(const cv::Point2f& imagePoint) const
{
    std::vector<cv::Point2f> srcPts(1, imagePoint);
    std::vector<cv::Point2f> dstPts;
    cv::perspectiveTransform(srcPts, dstPts, homography_);
    if (!dstPts.empty())
        return dstPts[0];
    return cv::Point2f(0.f, 0.f);
}

cv::Mat Camera::warpImage(const cv::Mat& inputImage, const cv::Size& outputSize) const
{
    if (inputImage.empty())
        throw std::runtime_error("Camera::warpImage: input image is empty!");
    
    cv::Mat output;
    cv::Size sizeForWarp = (outputSize.width > 0 && outputSize.height > 0) ? outputSize : inputImage.size();
    cv::warpPerspective(inputImage, output, homography_, sizeForWarp, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
    return output;
}
