#include "Camera.hpp"
#include "GridMapHandler.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    // 1. Создаём камеру и вычисляем гомографию из 4 пар точек
    Camera camera;
    // Пример: 4 пары (image -> world)
    std::vector<cv::Point2f> imgPts = {
        {414.f, 540.f},
        {617.f, 540.f},
        {440.f, 408.f},
        {560.f, 408.f}
    };
    std::vector<cv::Point2f> worldPts = {
        {3.5f, -0.5f},
        {3.5f, 1.f},
        {6.1f, -0.5f},
        {6.1f, 1.f}
    };
    if (!camera.computeHomography(imgPts, worldPts)) {
        std::cerr << "Не смогли вычислить гомографию!\n";
        return -1;
    }
    std::cout << "Гомография:\n" << camera.getHomography() << std::endl;

    // 2. Тест: warpImage
    cv::Mat testImg = cv::imread("data/test.jpg");
    if (testImg.empty()) {
        std::cerr << "Не открылось test.jpg\n";
    } else {
        cv::Mat warped = camera.warpImage(testImg);
        cv::imwrite("warped_test.png", warped);
    }

    // 3. GridMapHandler по умолчанию (500×500м, 0.1 м/пикс)
    GridMapHandler gridMap;

    // 4. Пример: проекция точек из изображения и добавление их в карту
    // (допустим, пройдёмся по ярким пикселям)
    if (!testImg.empty()) {
        for (int r = 0; r < testImg.rows; r++) {
            for (int c = 0; c < testImg.cols; c++) {
                cv::Vec3b color = testImg.at<cv::Vec3b>(r, c);
                int sum = color[0] + color[1] + color[2];
                // Если яркий пиксель
                if (sum > 600) {
                    cv::Point2f worldPt = camera.projectPoint({(float)c, (float)r});
                    // Добавим +1 в слой "road"
                    gridMap.addPoint("road", worldPt.x, worldPt.y, 1.0f);
                }
            }
        }
    }

    // 5. Сохраним результат
    if (gridMap.saveLayerAsImage("road", "road_heatmap.png")) {
        std::cout << "Карта сохранена как road_heatmap.png\n";
    }

    return 0;
}
