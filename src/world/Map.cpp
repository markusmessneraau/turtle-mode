#include "Map.h"
#include <fstream>  
#include <iostream>
#include <cstdlib>
#include <algorithm>

Map::Map() : width(0), height(0) {
    // wird erst beim Aufruf von loadFromText() initialisiert
}


// wandelt X/Y-Koordinate in 1D-Index um
int Map::getIndex(int x, int y) const {
    return y * width + x;
}


// Funktion zum Einlesen der Textdatei (Level)
bool Map::loadFromText(const std::string& filepath) {
    levelFinished = false;  // Reset von Level-Status beim Laden eines neuen Levels
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Fehler: Konnte " << filepath << " nicht oeffnen!\n";
        return false;
    }

    grid.clear(); // Vorherige Daten löschen, falls neues Level geladen wird
    std::string line;
    width = 0;
    height = 0;


    // Liest Datei Zeile für Zeile aus
    while (std::getline(file, line)) {
        if (width == 0) width = line.length(); // Breite anhand der ersten Zeile festlegen
        

        // Geht jeden char aus Zeile durch
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

    // Erstellt Rechteck-Vorlage für Kacheln
    sf::RectangleShape tile(sf::Vector2f(static_cast<float>(tileSize - 1), static_cast<float>(tileSize - 1)));

    // geht alle Kacheln durch (Position und Farbe)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Tile currentTile = grid[getIndex(x, y)]; // hol sich das Tile aus dem 1D-Vector
            
            // Farben zuweisen je nach Typ
            if (currentTile.type == TileType::Wall) tile.setFillColor(sf::Color(80, 80, 80));
            else if (currentTile.type == TileType::Door) tile.setFillColor(sf::Color(150, 50, 50)); // Rote Tür
            else if (currentTile.type == TileType::Key) tile.setFillColor(sf::Color(255, 215, 0));  // Goldener Schlüssel
            else if (currentTile.type == TileType::Exit) tile.setFillColor(sf::Color(50, 200, 50));
            else tile.setFillColor(sf::Color(25, 25, 25)); // Leer
            

            // Position der Kachel auf dem Fenster setzen
            tile.setPosition(sf::Vector2f(static_cast<float>(x * tileSize), static_cast<float>(y * tileSize)));
            window.draw(tile);
        }
    }
}


// Kollisionserkennung
bool Map::isWall(float pixelX, float pixelY) const {

    // wandelt Pixel-Koordinaten in Rasterfeld um
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



// prüft ob Drohne etwas berührt
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

            // wie viel Schlüssel liegen noch rum? Wenn 0, dann Türen öffnen
            int keysLeft = std::count_if(grid.begin(), grid.end(), [](const Tile& t) {
                return t.type == TileType::Key;
            });

            if (keysLeft == 0) {
                std::cout << "Alle Schluessel eingesammelt! Tueren oeffnen sich...\n";
                openDoors(); 
            } else {
                std::cout << "Schluessel gefunden! Noch " << keysLeft << " uebrig...\n";
            }
        }
        else if (grid[index].type == TileType::Exit) {
            std::cout << "Portal erreicht! Lade naechstes Level...\n";
            levelFinished = true;
        }
    }
}



// Türe löschen
void Map::openDoors() {
    for (Tile& t : grid) {
        if (t.type == TileType::Door) {
            t.type = TileType::Empty; // Tür verschwindet
        }
    }
}


// Gibt 3D-Engine Farbe d. Blocks als ID zurück, 
// damit Raycaster weiß, welche Textur er zeichnen muss.
sf::Color Map::getWallColor(float pixelX, float pixelY) const {
    int cellX = static_cast<int>(pixelX / tileSize);
    int cellY = static_cast<int>(pixelY / tileSize);

    if (cellX >= 0 && cellX < width && cellY >= 0 && cellY < height) {
        Tile currentTile = grid[getIndex(cellX, cellY)];
        
        // Gibt Raycaster Farbe für den 3D-Block zurück
        if (currentTile.type == TileType::Wall) return sf::Color(150, 150, 150); // Graue Steinmauer
        if (currentTile.type == TileType::Door) return sf::Color(200, 50, 50);   // Rote Tür
        if (currentTile.type == TileType::Key) return sf::Color(255, 215, 0);
        if (currentTile.type == TileType::Exit) return sf::Color(50, 200, 50);
    }
    // falls Strahl ins Leere außerhalb der Map trifft (dunkelgrau)
    return sf::Color(50, 50, 50); 
}




// was muss gezeichnet werden? Wand, Tür, Schlüssel, Portal oder nichts 
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



sf::Vector2f Map::getStartPosition() const {

    // sucht Startposition in Karte
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid[getIndex(x, y)].type == TileType::Start) {
                // Berechnet Pixel-Mitte des Startfelds
                return sf::Vector2f(x * tileSize + tileSize / 2.0f, y * tileSize + tileSize / 2.0f);
            }
        }
    }
    return sf::Vector2f(100.0f, 100.0f); // Fallback
}



int Map::getRemainingKeys() const {
    return std::count_if(grid.begin(), grid.end(), [](const Tile& t) {
        return t.type == TileType::Key;
    });
}