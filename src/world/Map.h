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

    std::vector<Tile> grid; // 1D-Array für die Kartenkacheln

    // Breite und Höhe der Karte in Kacheln
    int width;
    int height;


    // wandelt X/Y Koordinate in 1D-Index um
    int getIndex(int x, int y) const;

    // löscht Türen
    void openDoors();

public:
    Map();

    // Funktion zum Einlesen der Textdatei & füllt grid-Array
    bool loadFromText(const std::string& filepath);

    // malt 2d-Ansicht
    void draw(sf::RenderWindow& window); 

    bool isWall(float pixelX, float pixelY) const;

    // prüft ob Drohne etwas berührt
    void collectItems(float pixelX, float pixelY);


    // für 3D-Ansicht
    sf::Color getWallColor(float pixelX, float pixelY) const;
    bool isVisualWall(float pixelX, float pixelY) const;

    // Für Level-System und Startposition der Drohne
    bool levelFinished = false;
    sf::Vector2f getStartPosition() const;

    int getRemainingKeys() const;
};