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
    double yaw;  ///< Угол курса (градусы, уже "исправленный" под ось X)
};

/**
 * @brief Класс чтения траектории (например, из .ext1)
 */
class TrajectoryReader
{
public:
    /**
     * @param extFilePath Путь к файлу, в котором есть:
     *        time.s, local.x.m, local.y.m, local_yaw.grad (и т.п.)
     */
    explicit TrajectoryReader(const std::string& extFilePath);

    /**
     * @brief Считывает файл и заполняет вектор trajectory_
     *        Причём *сразу* добавляет +90° к local_yaw.grad,
     *        чтобы привести угол к "оси X" (против часовой стрелки).
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
     * @brief Проверка согласованности yaw (с учётом +90) с (dx, dy)
     *        Просто выводит в консоль отклонения для соседних точек.
     */
    void checkYawConsistency() const;

    /**
     * @return Константная ссылка на весь вектор точек
     */
    const std::vector<TrajectoryPoint>& getTrajectory() const { return trajectory_; }

private:
    std::string extFilePath_;
    std::vector<TrajectoryPoint> trajectory_;
};

#endif // TRAJECTORYREADER_HPP
