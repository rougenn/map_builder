#include "Camera.hpp"
#include "TrajectoryReader.hpp"
#include "GlobalGridMapHandler.hpp" 
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>

cv::Point2f rotatePoint(const cv::Point2f &pt, float angleDegrees) {
    float rad = angleDegrees * CV_PI / 180.0f;
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);
    return cv::Point2f(pt.x * cosA - pt.y * sinA,
                       pt.x * sinA + pt.y * cosA);
}



double extractTimestamp(const std::string &filename) {
    std::filesystem::path p(filename);
    std::string stem = p.stem().string(); // например, "16363.segm"
    std::istringstream iss(stem);
    double ts;
    iss >> ts;
    return ts / 1000;
}

int main() {
    // 1. Создаём камеру и вычисляем гомографию из 4 пар точек
    Camera camera;
    std::vector<cv::Point2f> imgPts = {
        {414.f, 540.f},
        {617.f, 540.f},
        {443.f, 408.f},
        {557.f, 408.f}
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


    // Загружаем данные траектории
    TrajectoryReader trajReader("/home/rougenn/projects/map_builder/data/get.356.trk.ext1");
    if (!trajReader.readExtFile()) {
        std::cerr << "Ошибка чтения данных траектории!\n";
        return -1;
    }

    // 3. Создаём GlobalGridMapHandler с центром, чтобы покрыть проецированную область.
    GlobalGridMapHandler globalMap(500.0, 0.1);

    std::string segFolder = "/home/rougenn/projects/map_builder/data/segmentation/get.356/";
    int totalBrightPixels = 0;

    std::vector<std::filesystem::directory_entry> entries;

    // Собираем все файлы из папки
    for (const auto &entry : std::filesystem::directory_iterator(segFolder)) {
        if (entry.is_regular_file()) {
            entries.push_back(entry);
        }
    }

    // Сортируем по имени файла
    std::sort(entries.begin(), entries.end(),
              [](const std::filesystem::directory_entry &a, const std::filesystem::directory_entry &b) {
                  return extractTimestamp(a.path().string()) < extractTimestamp(b.path().string());
              });

    int cnt = 0;


    for (const auto &entry : entries) {
        std::string filePath = entry.path().string();
        cv::Mat segImg = cv::imread(filePath, cv::IMREAD_COLOR);
        if (segImg.empty()) {
            std::cerr << "Не удалось загрузить изображение: " << filePath << "\n";
            continue;
        }

        // Устанавливаем лимит для тестирования кода
        ++cnt;
        if (cnt > 2000) break;

        // Извлекаем временную метку (если используется для смещения; здесь можно использовать pose)
        double timestamp = extractTimestamp(filePath);
        std::clog << filePath << std::endl;
        // Получаем ближайшую точку траектории для данного timestamp (если требуется)
        TrajectoryPoint pose;
        std::clog << timestamp << ' ';
        if (!trajReader.getClosestTrajectoryPoint(timestamp, pose)) {
            std::cerr << "Нет данных траектории для timestamp " << timestamp << "\n";
            continue;
        }
        std::clog << pose.x << ' ' << pose.y << ", " << pose.yaw << std::endl;


        // Обходим все пиксели изображения и выбираем яркие (условие: средняя яркость равна 1)
        for (int r = segImg.rows / 5 * 2; r < segImg.rows; r++) {
            for (int c = 0; c < segImg.cols; c++) {
                cv::Vec3b color = segImg.at<cv::Vec3b>(r, c);
                
                int avg = (color[0] + color[1] + color[2]) / 3;
                if (avg == 1) { // если яркий пиксель
                    totalBrightPixels++;

                    // Проецируем точку
                    cv::Point2f projPt = camera.projectPoint({static_cast<float>(c), static_cast<float>(r)});

                    // Поворот точки
                    cv::Point2f rotatedPt = rotatePoint(projPt, pose.yaw);

                    // Если требуется добавить смещение по траектории (pose)
                    cv::Point2f worldPt = rotatedPt;
                    worldPt.x += static_cast<float>(pose.x);
                    worldPt.y += static_cast<float>(pose.y);

                    // Добавляем точку в глобальную карту (приращение, например, 10)
                    globalMap.addPoint(worldPt.x, worldPt.y, 5.0f);
                }
            }
        }
    }

    // 6. Сохраняем все квадранты (файлы будут иметь имена вида "quadrant_qx_qy.png")
    globalMap.saveAllQuadrants("quadrant");


    return 0;
}
