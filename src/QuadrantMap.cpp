#include "QuadrantMap.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <limits>
#include <algorithm>

QuadrantMap::QuadrantMap(double width, double height, double resolution, double centerX, double centerY)
{
    gridMap_.setGeometry(grid_map::Length(width, height),
                         resolution,
                         grid_map::Position(centerX, centerY));
    gridMap_.add(LAYER_NAME, 0.0f);
}

void QuadrantMap::addPoint(double x, double y, float value)
{
    grid_map::Position pos(x, y);
    if (gridMap_.isInside(pos)) {
        grid_map::Index index;
        if (gridMap_.getIndex(pos, index)) {
            gridMap_.at(LAYER_NAME, index) += value;
        }
    }
}

bool QuadrantMap::saveAsImage(const std::string &fileName) const
{
    if (!gridMap_.exists(LAYER_NAME)) {
        std::cerr << "[QuadrantMap] Слой \"" << LAYER_NAME << "\" не найден.\n";
        return false;
    }

    const grid_map::Size size = gridMap_.getSize();
    cv::Mat image(size(1), size(0), CV_8UC1);

    float minVal = std::numeric_limits<float>::max();
    float maxVal = std::numeric_limits<float>::lowest();
    const grid_map::Matrix matrix = gridMap_.get(LAYER_NAME);
    for (int i = 0; i < matrix.size(); i++) {
        float v = matrix(i);
        if (v < minVal) minVal = v;
        if (v > maxVal) maxVal = v;
    }

    for (int r = 0; r < image.rows; r++) {
        for (int c = 0; c < image.cols; c++) {
            grid_map::Index index(c, r);
            float value = gridMap_.at(LAYER_NAME, index);
            int pixelValue = 0;
            if (maxVal > minVal) {
                pixelValue = static_cast<int>(std::round((value - minVal) / (maxVal - minVal) * 255));
            }
            pixelValue = std::min(std::max(pixelValue, 0), 255);
            image.at<uchar>(r, c) = static_cast<uchar>(pixelValue);
        }
    }
    cv::Mat colorMap;
    cv::applyColorMap(image, colorMap, cv::COLORMAP_JET);
    if (!cv::imwrite(fileName, colorMap)) {
        std::cerr << "[QuadrantMap] Не удалось сохранить изображение: " << fileName << "\n";
        return false;
    }
    return true;
}
