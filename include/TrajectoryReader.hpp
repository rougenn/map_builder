#ifndef TRAJECTORYREADER_HPP
#define TRAJECTORYREADER_HPP

#include <string>
#include <vector>

/**
 * @brief Структура точки траектории
 */
struct TrajectoryPoint
{
    double time; ///< Время (секунды)
    double x;    ///< Координата X (метры)
    double y;    ///< Координата Y (метры)
    double yaw;  ///< Угол
};

/**
 * @brief Класс чтения траектории (из трк.эксти1)
 */
class TrajectoryReader
{
public:
    /**
     * @param extFilePath Путь к файлу, в котором есть как минимум
     *        time.s, local.x.m, local.y.m, local_yaw.grad
     */
    explicit TrajectoryReader(const std::string& extFilePath);

    /**
     * @brief Считывает файл и заполняет вектор trajectory_
     * @return true, если чтение прошло успешно
     */
    bool readExtFile();

    /**
     * @brief Находит ближайшую точку траектории по заданному времени
     * @param[in]  imageTime  время (сек)
     * @param[out] outPoint   ближайшая точка
     * @return true, если точка найдена
     */
    bool getClosestTrajectoryPoint(double imageTime, TrajectoryPoint& outPoint) const;

    /**
     * @return Константная ссылка на весь вектор точек
     */
    const std::vector<TrajectoryPoint>& getTrajectory() const { return trajectory_; }

private:
    std::string extFilePath_;
    std::vector<TrajectoryPoint> trajectory_;
};

#endif // TRAJECTORYREADER_HPP
