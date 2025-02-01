#include "TrajectoryReader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm> // для std::sort

TrajectoryReader::TrajectoryReader(const std::string& extFilePath)
    : extFilePath_(extFilePath)
{
}

bool TrajectoryReader::readExtFile()
{
    std::ifstream ifs(extFilePath_);
    if (!ifs.is_open()) {
        std::cerr << "Не удалось открыть файл: " << extFilePath_ << std::endl;
        return false;
    }

    // Считываем заголовок (первая строка)
    std::string header;
    if (!std::getline(ifs, header)) {
        std::cerr << "Файл пуст или не содержит заголовка: " << extFilePath_ << std::endl;
        return false;
    }

    // Разбираем заголовок, чтобы найти индексы столбцов
    std::vector<std::string> colNames;
    {
        std::stringstream ss(header);
        std::string col;
        while (std::getline(ss, col, ',')) {
            while (!col.empty() && (col.front() == ' ' || col.front() == '\t')) col.erase(col.begin());
            while (!col.empty() && (col.back()  == ' ' || col.back()  == '\t')) col.pop_back();
            colNames.push_back(col);
        }
    }

    int idx_time     = -1;
    int idx_localX   = -1;
    int idx_localY   = -1;
    int idx_localYaw = -1;

    for (int i = 0; i < (int)colNames.size(); i++) {
        if (colNames[i] == "time.s")         idx_time = i;
        if (colNames[i] == "local.x.m")      idx_localX = i;
        if (colNames[i] == "local.y.m")      idx_localY = i;
        if (colNames[i] == "local_yaw.grad") idx_localYaw = i;
    }

    if (idx_time < 0)     std::cerr << "Не найден столбец time.s!\n";
    if (idx_localX < 0)   std::cerr << "Не найден столбец local.x.m!\n";
    if (idx_localY < 0)   std::cerr << "Не найден столбец local.y.m!\n";
    if (idx_localYaw < 0) std::cerr << "Не найден столбец local_yaw.grad!\n";

    // Читаем остальные строки
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::vector<std::string> cols;
        {
            std::string c;
            while (std::getline(ss, c, ',')) {
                while (!c.empty() && (c.front()==' ' || c.front()=='\t')) c.erase(c.begin());
                while (!c.empty() && (c.back()==' '  || c.back()=='\t')) c.pop_back();
                cols.push_back(c);
            }
        }

        // Определяем, можно ли считать нужные индексы
        int requiredIndex = std::max(std::max(idx_time, idx_localX),
                                     std::max(idx_localY, idx_localYaw));
        if ((int)cols.size() <= requiredIndex) {
            // Строка короче, чем нужно
            continue;
        }

        TrajectoryPoint tp;
        tp.time = 0.0;
        tp.x    = 0.0;
        tp.y    = 0.0;
        tp.yaw  = 0.0; // в градусах (ориентация от оси X)

        if (idx_time >= 0 && idx_time < (int)cols.size()) {
            tp.time = std::stod(cols[idx_time]);
        }
        if (idx_localX >= 0 && idx_localX < (int)cols.size()) {
            tp.x    = std::stod(cols[idx_localX]);
        }
        if (idx_localY >= 0 && idx_localY < (int)cols.size()) {
            tp.y    = std::stod(cols[idx_localY]);
        }
        if (idx_localYaw >= 0 && idx_localYaw < (int)cols.size()) {
            double localYaw = std::stod(cols[idx_localYaw]);
            // Добавляем +90°, чтобы привести к системе, где 0° значит "вправо"
            tp.yaw = localYaw + 90.0;
            // при желании можно нормализовать в диапазоне [-180..180] или [0..360]
            while (tp.yaw > 180.0)  tp.yaw -= 360.0;
            while (tp.yaw < -180.0) tp.yaw += 360.0;
        }

        // Добавляем в вектор, если время > 0
        if (tp.time > 0.0) {
            trajectory_.push_back(tp);
        }
    }

    ifs.close();

    // Сортируем по времени
    std::sort(trajectory_.begin(), trajectory_.end(),
              [](const TrajectoryPoint &a, const TrajectoryPoint &b){
                  return a.time < b.time;
              });

    if (trajectory_.empty()) {
        std::cerr << "Не найдено валидных точек в файле " << extFilePath_ << std::endl;
        return false;
    }

    return true;
}

bool TrajectoryReader::getClosestTrajectoryPoint(double imageTime, TrajectoryPoint& outPoint) const
{
    if (trajectory_.empty()) {
        return false;
    }

    double minDiff = std::numeric_limits<double>::max();
    bool found = false;

    for (auto & tp : trajectory_) {
        double d = std::fabs(tp.time - imageTime);
        if (d < minDiff) {
            minDiff = d;
            outPoint = tp;
            found = true;
        }
    }
    return found;
}

void TrajectoryReader::checkYawConsistency() const
{
    if (trajectory_.size() < 2) {
        std::cout << "Недостаточно точек для проверки yaw.\n";
        return;
    }

    // Сравниваем угол, полученный из (dx, dy), с tp1.yaw (уже "исправленным").
    for (size_t i = 0; i < trajectory_.size() - 1; i++) {
        const auto& tp1 = trajectory_[i];
        const auto& tp2 = trajectory_[i+1];

        double dx = tp2.x - tp1.x;
        double dy = tp2.y - tp1.y;
        double angleDeg = std::atan2(dy, dx) * 180.0 / M_PI;

        double diff = angleDeg - tp1.yaw;
        // нормализуем diff в [-180..180]
        while (diff > 180.0)  diff -= 360.0;
        while (diff < -180.0) diff += 360.0;

        std::cout << "i=" << i << " time=" << tp1.time
                  << " -> time=" << tp2.time
                  << " dx=" << dx << " dy=" << dy
                  << " angleDeg=" << angleDeg
                  << " yaw=" << tp1.yaw
                  << " diff=" << diff
                  << " (град)\n";
    }
}
