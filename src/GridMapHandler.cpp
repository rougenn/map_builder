#include "GridMapHandler.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <limits>
#include <algorithm>

GridMapHandler::GridMapHandler(double lengthX,
                               double lengthY,
                               double resolution,
                               const std::string& layerName,
                               double centerX,
                               double centerY)
    : mainLayer_(layerName)
{
    // Инициализируем геометрию карты:
    // Карта будет иметь физические размеры (lengthX, lengthY) в метрах,
    // разрешение (размер ячейки) в метрах,
    // а центр карты задается параметрами (centerX, centerY).
    gridMap_.setGeometry(grid_map::Length(lengthX, lengthY),
                         resolution,
                         grid_map::Position(centerX, centerY));
    gridMap_.add(mainLayer_, 0.0f);
}

void GridMapHandler::addLayer(const std::string& layerName, float defaultVal)
{
    if (!gridMap_.exists(layerName))
        gridMap_.add(layerName, defaultVal);
}

void GridMapHandler::addPoint(const std::string& layerName, double x, double y, float value)
{
    if (!gridMap_.exists(layerName))
        gridMap_.add(layerName, 0.0f);

    grid_map::Position pos(x, y);
    if (gridMap_.isInside(pos)) {
        grid_map::Index index;
        if (gridMap_.getIndex(pos, index)) {
            gridMap_.at(layerName, index) += value;
        }
    }
}

bool GridMapHandler::saveLayerAsImage(const std::string& layerName, const std::string& fileName) const
{
    if (!gridMap_.exists(layerName)) {
        std::cerr << "[GridMapHandler] Слой \"" << layerName << "\" не найден.\n";
        return false;
    }

    // Получаем размеры карты (количество ячеек)
    const grid_map::Size size = gridMap_.getSize();
    // Создаём OpenCV матрицу: строки соответствуют оси Y, столбцы — оси X.
    cv::Mat image(size(1), size(0), CV_8UC1);

    // Определяем минимальное и максимальное значение для нормализации.
    float minVal = std::numeric_limits<float>::max();
    float maxVal = std::numeric_limits<float>::lowest();
    const grid_map::Matrix matrix = gridMap_.get(mainLayer_);
    for (int i = 0; i < matrix.size(); i++) {
        float v = matrix(i);
        if (v < minVal) minVal = v;
        if (v > maxVal) maxVal = v;
    }

    // Заполняем матрицу image нормализованными значениями [0,255].
    for (int r = 0; r < image.rows; r++) {
        for (int c = 0; c < image.cols; c++) {
            grid_map::Index index(c, r);
            float value = gridMap_.at(mainLayer_, index);
            int pixelValue = 0;
            if (maxVal > minVal) {
                pixelValue = static_cast<int>(std::round((value - minVal) / (maxVal - minVal) * 255));
            }
            pixelValue = std::min(std::max(pixelValue, 0), 255);
            image.at<uchar>(r, c) = static_cast<uchar>(pixelValue);
        }
    }

    // Применяем цветовую карту для эффекта "тепловой карты".
    cv::Mat colorMap;
    cv::applyColorMap(image, colorMap, cv::COLORMAP_JET);

    if (!cv::imwrite(fileName, colorMap)) {
        std::cerr << "[GridMapHandler] Не удалось сохранить изображение: " << fileName << "\n";
        return false;
    }
    return true;
}
