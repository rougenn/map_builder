#include "GlobalGridMapHandler.hpp"
#include <cmath>
#include <sstream>
#include <iostream>

QuadrantKey GlobalGridMapHandler::getQuadrantKey(double x, double y) const {
    int qx = static_cast<int>(std::floor(x / quadrantSize_));
    int qy = static_cast<int>(std::floor(y / quadrantSize_));
    return {qx, qy};
}

GlobalGridMapHandler::GlobalGridMapHandler(double quadrantSize, double resolution)
    : quadrantSize_(quadrantSize), resolution_(resolution)
{
}

void GlobalGridMapHandler::addPoint(double x, double y, float value)
{
    QuadrantKey key = getQuadrantKey(x, y);
    if (quadrants_.find(key) == quadrants_.end()) {
        // Центр квадранта: ((qx + 0.5) * quadrantSize, (qy + 0.5) * quadrantSize)
        double centerX = (key.first + 0.5) * quadrantSize_;
        double centerY = (key.second + 0.5) * quadrantSize_;
        quadrants_[key] = std::make_unique<QuadrantMap>(quadrantSize_, quadrantSize_, resolution_, centerX, centerY);
    }
    quadrants_[key]->addPoint(x, y, value);
}

bool GlobalGridMapHandler::saveQuadrant(const QuadrantKey &key, const std::string &fileName) const
{
    auto it = quadrants_.find(key);
    if (it == quadrants_.end()) {
        std::cerr << "[GlobalGridMapHandler] Квадрант (" << key.first << ", " << key.second << ") не существует.\n";
        return false;
    }
    return it->second->saveAsImage(fileName);
}

void GlobalGridMapHandler::saveAllQuadrants(const std::string &prefix) const
{
    for (const auto &item : quadrants_) {
        QuadrantKey key = item.first;
        std::ostringstream oss;
        oss << prefix << "_" << key.first << "_" << key.second << ".png";
        if (item.second->saveAsImage(oss.str()))
            std::cout << "Квадрант сохранён: " << oss.str() << "\n";
        else
            std::cerr << "Ошибка сохранения квадранта: " << oss.str() << "\n";
    }
}
