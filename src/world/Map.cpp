#include "Map.h"

Map::Map() {
    // 1 = Wand, 0 = Freier Raum zum Fliegen
    grid = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
}

void Map::draw(sf::RenderWindow& window) {
    // Wir erstellen ein Quadrat, das minimal kleiner alstileSize ist, damit man ein Gitter sieht
    sf::RectangleShape tile(sf::Vector2f(static_cast<float>(tileSize - 1), static_cast<float>(tileSize - 1)));

    for (size_t y = 0; y < grid.size(); ++y) {
        for (size_t x = 0; x < grid[y].size(); ++x) {
            if (grid[y][x] == 1) {
                tile.setFillColor(sf::Color(80, 80, 80)); // Wände sind grau
            } else {
                tile.setFillColor(sf::Color(25, 25, 25)); // Flugraum ist fast schwarz
            }
            tile.setPosition(sf::Vector2f(static_cast<float>(x * tileSize), static_cast<float>(y * tileSize)));
            window.draw(tile);
        }
    }
}

bool Map::isWall(float pixelX, float pixelY) const {
    // Rechnet die Pixel-Koordinate der Drohne in die Gitter-Zelle (0-9) um
    int cellX = static_cast<int>(pixelX / tileSize);
    int cellY = static_cast<int>(pixelY / tileSize);

    // Prüfen, ob wir uns innerhalb des Gitters bewegen
    if (cellY >= 0 && cellY < static_cast<int>(grid.size()) && cellX >= 0 && cellX < static_cast<int>(grid[cellY].size())) {
        return grid[cellY][cellX] == 1;
    }
    return true; // Alles außerhalb des Spielfelds blockiert als Wand
}