#include "Map.h"
#include <fstream>  
#include <iostream>
#include <cstdlib>

Map::Map() : width(0), height(0) {
    // wird erst beim Aufruf von loadFromText() initialisiert
}


// wandelt X/Y in den 1D_- Index um
int Map::getIndex(int x, int y) const {
    return y * width + x;
}


// Funktion zum Einlesen der Textdatei
bool Map::loadFromText(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Fehler: Konnte " << filepath << " nicht oeffnen!\n";
        return false;
    }

    grid.clear(); // Vorherige Daten löschen, falls neues Level geladen wird
    std::string line;
    width = 0;
    height = 0;
    
    while (std::getline(file, line)) {
        if (width == 0) width = line.length(); // Breite anhand der ersten Zeile festlegen
        
        for (char c : line) {
            Tile t;
            if (c == '#') t.type = TileType::Wall;
            else if (c == 'K') t.type = TileType::Key;
            else if (c == 'T') t.type = TileType::Door;
            else if (c == 'S') t.type = TileType::Start;
            else if (c == 'E') t.type = TileType::Exit; 
            else t.type = TileType::Empty;

            grid.push_back(t);
        }
        height++; // nächste Zeile
    }
    return true;
}

void Map::draw(sf::RenderWindow& window) {
    sf::RectangleShape tile(sf::Vector2f(static_cast<float>(tileSize - 1), static_cast<float>(tileSize - 1)));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Tile currentTile = grid[getIndex(x, y)]; // hol sich das Tile aus dem 1D-Vector
            
            // Farben zuweisen je nach Typ
            if (currentTile.type == TileType::Wall) tile.setFillColor(sf::Color(80, 80, 80));
            else if (currentTile.type == TileType::Door) tile.setFillColor(sf::Color(150, 50, 50)); // Rote Tür
            else if (currentTile.type == TileType::Key) tile.setFillColor(sf::Color(255, 215, 0));  // Goldener Schlüssel
            else if (currentTile.type == TileType::Exit) tile.setFillColor(sf::Color(50, 200, 50));
            else tile.setFillColor(sf::Color(25, 25, 25)); // Leer
            
            tile.setPosition(sf::Vector2f(static_cast<float>(x * tileSize), static_cast<float>(y * tileSize)));
            window.draw(tile);
        }
    }
}


bool Map::isWall(float pixelX, float pixelY) const {
    int cellX = static_cast<int>(pixelX / tileSize);
    int cellY = static_cast<int>(pixelY / tileSize);


    // Prüfen, ob wir überhaupt noch in der Map sind
    if (cellX >= 0 && cellX < width && cellY >= 0 && cellY < height) {
        Tile currentTile = grid[getIndex(cellX, cellY)];
        // Drohne knallt gegen Wände UND (vorerst) gegen verschlossene Türen
        return (currentTile.type == TileType::Wall || currentTile.type == TileType::Door);
    }
    return true;
}

void Map::collectItems(float pixelX, float pixelY) {
    // Rechnet aus, in welchem Raster-Feld Drohne gerade steht
    int cellX = static_cast<int>(pixelX / tileSize);
    int cellY = static_cast<int>(pixelY / tileSize);

    // ist  Drohne noch auf Map?
    if (cellX >= 0 && cellX < width && cellY >= 0 && cellY < height) {
        int index = getIndex(cellX, cellY);
        
        // Liegt auf diesem Feld ein Schlüssel?
        if (grid[index].type == TileType::Key) {
            grid[index].type = TileType::Empty; // Schlüssel verschwindet
            std::cout << "Schluessel eingesammelt! Tueren oeffnen sich...\n";
            openDoors(); // Türen öffnen
        }
        else if (grid[index].type == TileType::Exit) {
            std::cout << "\n====================================\n";
            std::cout << "           LEVEL GESCHAFFT!            \n";
            std::cout << "====================================\n\n";
            std::exit(0); 
        }
    }
}

void Map::openDoors() {
    for (Tile& t : grid) {
        if (t.type == TileType::Door) {
            t.type = TileType::Empty; // Tür verschwindet
        }
    }
}

sf::Color Map::getWallColor(float pixelX, float pixelY) const {
    int cellX = static_cast<int>(pixelX / tileSize);
    int cellY = static_cast<int>(pixelY / tileSize);

    if (cellX >= 0 && cellX < width && cellY >= 0 && cellY < height) {
        Tile currentTile = grid[getIndex(cellX, cellY)];
        
        if (currentTile.type == TileType::Wall) return sf::Color(150, 150, 150); // Graue Steinmauer
        if (currentTile.type == TileType::Door) return sf::Color(200, 50, 50);   // Rote Tür
        if (currentTile.type == TileType::Key) return sf::Color(255, 215, 0);
        if (currentTile.type == TileType::Exit) return sf::Color(50, 200, 50);
    }
    // falls Strahl ins Leere außerhalb der Map trifft
    return sf::Color(50, 50, 50); 
}

bool Map::isVisualWall(float pixelX, float pixelY) const {
    int cellX = static_cast<int>(pixelX / tileSize);
    int cellY = static_cast<int>(pixelY / tileSize);

    if (cellX >= 0 && cellX < width && cellY >= 0 && cellY < height) {
        Tile currentTile = grid[getIndex(cellX, cellY)];
        
        return (currentTile.type == TileType::Wall || 
                currentTile.type == TileType::Door || 
                currentTile.type == TileType::Key  ||
                currentTile.type == TileType::Exit);
    }
    return true; 
}
