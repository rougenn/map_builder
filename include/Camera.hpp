#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <opencv2/core.hpp>
#include <vector>

/**
 * @brief Класс для работы с гомографией.
 *
 * Позволяет вычислять гомографию по соответствующим точкам,
 * проецировать отдельные точки и выполнять warpPerspective для получения вида сверху.
 */
class Camera
{
public:
    /**
     * @brief Конструктор по умолчанию. Создаёт единичную гомографию.
     */
    Camera();

    /**
     * @brief Конструктор с заданной гомографией.
     * @param homography Матрица 3x3 преобразования.
     */
    explicit Camera(const cv::Mat& homography);

    /**
     * @brief Вычисляет гомографию по 4+ парам точек.
     * @param imagePoints Вектор точек в системе изображения (пиксели).
     * @param worldPoints Вектор соответствующих точек в мировой системе (метры).
     * @return true, если вычисление прошло успешно, иначе false.
     */
    bool computeHomography(const std::vector<cv::Point2f>& imagePoints,
                           const std::vector<cv::Point2f>& worldPoints);

    /**
     * @brief Проецирует одну точку из системы изображения в мировую (z = 0)
     *        с помощью текущей гомографии.
     * @param imagePoint Точка в пикселях.
     * @return Точка в мировой системе (пиксели).
     */
    cv::Point2f projectPoint(const cv::Point2f& imagePoint) const;

    /**
     * @brief Возвращает копию текущей гомографии.
     */
    cv::Mat getHomography() const { return homography_.clone(); }

private:
    cv::Mat homography_; ///< 3x3 матрица гомографии (image -> world)
};

#endif // CAMERA_HPP
