#ifndef QUADRANTMAP_HPP
#define QUADRANTMAP_HPP

#include <grid_map_core/GridMap.hpp>
#include <string>

/**
 * @brief Класс, представляющий один квадрант карты
 *
 * Оборачивает grid_map_core, позволяя добавлять точки
 * и сохранять квадрант как изображение (тепловая карта)
 */
class QuadrantMap {
public:
    /**
     * @brief Конструктор квадранта.
     * @param width Физическая ширина квадранта в метрах
     * @param height Физическая высота квадранта в метрах
     * @param resolution Размер ячейки м/пикс
     * @param centerX Координата X центра квадранта
     * @param centerY Координата Y центра квадранта
     */
    QuadrantMap(double width = 500.0, double height = 500.0, double resolution = 0.1,
                double centerX = 0.0, double centerY = 0.0);

    /**
     * @brief Добавляет точку (увеличивает значение) в квадранте
     * @param x Координата X в метрах
     * @param y Координата Y в метрах
     * @param value Приращение
     */
    void addPoint(double x, double y, float value = 1.0f);

    /**
     * @brief Сохраняет квадрант как изображение
     * @param fileName Путь к файлу для сохранения
     * @return true, если сохранение прошло успешно
     */
    bool saveAsImage(const std::string &fileName) const;

private:
    grid_map::GridMap gridMap_;
    static constexpr const char* LAYER_NAME = "heat";
};

#endif // QUADRANTMAP_HPP
