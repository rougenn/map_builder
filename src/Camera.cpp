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
    
    // Преобразуем мировые точки (в метрах) в пиксельные координаты по схеме:
    // scale = 10, uCenter = 500, vBottom = 999.
    // Для инверсии проекции (чтобы начало дороги оказалось внизу) используем -X:
    double scale = 10.0;
    double uCenter = 500.0;
    double vBottom = 999.0;
    std::vector<cv::Point2f> dstPoints;
    for (const auto& pt : worldPoints) {
         // pt.x = X (в метрах, направление вперед) и pt.y = Y (в метрах, вправо).
         // Чтобы проекция была ориентирована так, что ближняя (меньшая X) будет внизу,
         // используем: u = uCenter + scale * Y, v = vBottom + scale * X.
         cv::Point2f p(uCenter + static_cast<float>(scale * pt.y),
                       vBottom + static_cast<float>(scale * pt.x));
         dstPoints.push_back(p);
    }
    
    cv::Mat H = cv::findHomography(imagePoints, dstPoints, 0);
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
    // Если размер не задан, используем 1000×1000, как в рабочем примере.
    cv::Size sizeForWarp = (outputSize.width > 0 && outputSize.height > 0) ? outputSize : cv::Size(1000, 1000);
    cv::warpPerspective(inputImage, output, homography_, sizeForWarp,
                        cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
    return output;
}
