#ifndef GLOBALGRIDMAPHANDLER_HPP
#define GLOBALGRIDMAPHANDLER_HPP

#include "QuadrantMap.hpp"
#include <map>
#include <memory>
#include <string>
#include <utility>

// Ключ квадранта – пара (qx, qy)
using QuadrantKey = std::pair<int, int>;

/**
 * @brief Класс для управления глобальной картой, разбитой на квадранты.
 *
 * При добавлении точки GlobalGridMapHandler вычисляет, в какой квадрант (по размеру quadrantSize)
 * она попадает, и, если объект для этого квадранта ещё не создан, создаёт его.
 * Затем точка добавляется в соответствующий квадрант.
 */
class GlobalGridMapHandler {
public:
    /**
     * @brief Конструктор глобальной карты.
     * @param quadrantSize Размер одного квадранта в метрах (например, 500).
     * @param resolution Разрешение для каждого квадранта (м/пикс).
     */
    GlobalGridMapHandler(double quadrantSize = 500.0, double resolution = 0.1);

    /**
     * @brief Добавляет точку в глобальную карту.
     * @param x Координата X в метрах.
     * @param y Координата Y в метрах.
     * @param value Приращение (по умолчанию +1).
     */
    void addPoint(double x, double y, float value = 1.0f);

    /**
     * @brief Сохраняет квадрант с заданным ключом в виде изображения.
     * @param key Ключ квадранта (qx, qy).
     * @param fileName Путь к выходному файлу.
     * @return true, если успешно.
     */
    bool saveQuadrant(const QuadrantKey &key, const std::string &fileName) const;

    /**
     * @brief Сохраняет все существующие квадранты, используя заданный префикс для имен файлов.
     * Файлы будут иметь имена вида: prefix_qx_qy.png.
     */
    void saveAllQuadrants(const std::string &prefix) const;

private:
    // Вычисляет ключ квадранта для точки (x, y)
    QuadrantKey getQuadrantKey(double x, double y) const;

    double quadrantSize_; // размер одного квадранта (в метрах)
    double resolution_;   // разрешение для каждого квадранта (м/пикс)
    // Словарь квадрантов: ключ – пара (qx, qy), значение – объект QuadrantMap.
    std::map<QuadrantKey, std::unique_ptr<QuadrantMap>> quadrants_;
};

#endif // GLOBALGRIDMAPHANDLER_HPP
