#ifndef GRIDMAPHANDLER_HPP
#define GRIDMAPHANDLER_HPP

#include <grid_map_core/GridMap.hpp>
#include <string>

/**
 * @brief Класс-обёртка над grid_map_core для создания и обновления "карты" (квадранта).
 *
 * По умолчанию:
 *   - Размер карты: 500×500 метров.
 *   - Разрешение: 0.1 м/пикс (то есть 5000×5000 ячеек).
 *   - Центр карты по умолчанию: (0, 0).
 *   - Основной слой называется "road".
 *
 * Параметры можно изменить при инициализации, в том числе задать центр карты.
 */
class GridMapHandler
{
public:
    /**
     * @brief Конструктор.
     * @param lengthX     Физический размер карты по оси X в метрах (по умолч. 500).
     * @param lengthY     Физический размер карты по оси Y в метрах (по умолч. 500).
     * @param resolution  Размер одной ячейки (м/пикс), по умолчанию 0.1.
     * @param layerName   Имя основного слоя, по умолчанию "road".
     * @param centerX     Координата X центра карты (по умолчанию 0).
     * @param centerY     Координата Y центра карты (по умолчанию 0).
     */
    explicit GridMapHandler(double lengthX = 500.0,
                              double lengthY = 500.0,
                              double resolution = 0.1,
                              const std::string& layerName = "road",
                              double centerX = 0.0,
                              double centerY = 0.0);

    /**
     * @brief Добавляет новый слой.
     * @param layerName  Имя слоя.
     * @param defaultVal Начальное значение ячеек.
     */
    void addLayer(const std::string& layerName, float defaultVal = 0.0f);

    /**
     * @brief Добавляет (увеличивает) значение в ячейке слоя по мировым координатам.
     * @param layerName Имя слоя (если отсутствует, создаётся).
     * @param x         Координата X в метрах.
     * @param y         Координата Y в метрах.
     * @param value     Приращение (по умолчанию +1).
     */
    void addPoint(const std::string& layerName, double x, double y, float value = 1.0f);

    /**
     * @brief Сохраняет указанный слой в виде изображения (PNG) с использованием тепловой цветовой карты.
     *        Реализация конвертирует данные из GridMap в cv::Mat вручную (без grid_map_cv).
     * @param layerName Имя слоя.
     * @param fileName  Путь к выходному файлу (например, "road_heatmap.png").
     * @return true, если успешно.
     */
    bool saveLayerAsImage(const std::string& layerName, const std::string& fileName) const;

    /**
     * @brief Возвращает константную ссылку на внутренний объект GridMap.
     */
    const grid_map::GridMap& getMap() const { return gridMap_; }

private:
    grid_map::GridMap gridMap_;
    std::string mainLayer_;
};

#endif // GRIDMAPHANDLER_HPP
