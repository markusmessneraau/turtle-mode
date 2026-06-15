#pragma once
#include <SFML/Graphics.hpp>
#include <vector>


enum class TileType { Empty, Wall, Key, Door, Start, Exit };
struct Tile {
    TileType type;
};

class Map {
private:
    const int tileSize = 40; // Größe jeder Kachel in Pixeln

    std::vector<Tile> grid; 
    int width;
    int height;


    // wandelt X/Y in den 1D_- Index um
    int getIndex(int x, int y) const;

    // löscht Türen
    void openDoors();

public:
    Map();

    // Funktion zum Einlesen der Textdatei
    bool loadFromText(const std::string& filepath);
    void draw(sf::RenderWindow& window);
    bool isWall(float pixelX, float pixelY) const;
    bool isVisualWall(float pixelX, float pixelY) const;

    // prüft ob Drohne etwas berührt
    void collectItems(float pixelX, float pixelY);
    sf::Color getWallColor(float pixelX, float pixelY) const;

    // Für Level-System und Startposition der Drohne
    bool levelFinished = false;
    sf::Vector2f getStartPosition() const;
};