#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Map {
private:
    const int tileSize = 40; // Größe jeder Kachel in Pixeln
    std::vector<std::vector<int>> grid;

public:
    Map();
    void draw(sf::RenderWindow& window);
    bool isWall(float pixelX, float pixelY) const;
};