#include "Camera.hpp"
#include "TrajectoryReader.hpp"
#include "GlobalGridMapHandler.hpp" 
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>

int main()
{
    // 1. Создаём камеру и вычисляем гомографию из 4 пар точек
    Camera camera;
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

    // 2. Тест: warpImage (с сохранением результата для проверки)
    cv::Mat testImg = cv::imread("/home/rougenn/projects/map_builder/data/segmentation/get.356/16363.segm.png");
    if (testImg.empty()) {
        std::cerr << "Не удалось загрузить тестовое изображение.\n";
    } else {
        cv::Mat warped = camera.warpImage(testImg);
        cv::imwrite("warped_test.png", warped);
    }

    // 3. Создаём GlobalGridMapHandler с центром, чтобы покрыть проецированную область.
    // Предположим, что проецированные точки находятся около (500, 930)
    GlobalGridMapHandler gridMap(500.0, 0.1);

    // 4. Проходимся по ярким пикселям тестового изображения и добавляем их в карту
    int cnt = 0;
    if (!testImg.empty()) {
        for (int r = 0; r < testImg.rows; r++) {
            for (int c = 0; c < testImg.cols; c++) {
                cv::Vec3b color = testImg.at<cv::Vec3b>(r, c);
                int sum = color[0] + color[1] + color[2];
                // Если среднее значение пикселя равно 1 (как в вашем условии)
                if ((sum / 3) == 1) {
                    cnt++;
                    cv::Point2f worldPt = camera.projectPoint({(float)c, (float)r});
                    gridMap.addPoint(worldPt.x, worldPt.y, 10.0f);
                }
            }
        }
    }
    std::cout << "Количество ярких пикселей: " << cnt << std::endl;

    // 5. Сохраняем результат (тепловая карта)
    gridMap.saveAllQuadrants("quadrant");

    return 0;
}
