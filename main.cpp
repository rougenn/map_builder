#include <iostream>
#include <cmath>

#include "TrajectoryReader.hpp"

int main()
{
    std::string extFilePath = "/home/rougenn/projects/map_builder/data/get.356.trk.ext1";

    TrajectoryReader reader(extFilePath);
    if (!reader.readExtFile()) {
        std::cerr << "Не удалось прочитать EXT файл: " << extFilePath << std::endl;
        return -1;
    }

    // Проверим согласование yaw
    reader.checkYawConsistency();

    // Тестируем получение ближайшей точки
    double testTime = 16.572000;
    TrajectoryPoint tp;
    if (reader.getClosestTrajectoryPoint(testTime, tp)) {
        std::cout << "Время: " << testTime 
                  << " => Найдено (time=" << tp.time
                  << ", x=" << tp.x 
                  << ", y=" << tp.y
                  << ", yaw=" << tp.yaw << ")"
                  << std::endl;
    } else {
        std::cout << "Точка не найдена для времени " << testTime << std::endl;
    }

    return 0;
}
