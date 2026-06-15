#pragma once
#include <SFML/Graphics.hpp>
#include "../world/Map.h"
#include "../entities/Drone.h"

class Game {
private:
    sf::RenderWindow window;
    Map gameMap;
    Drone myDrone;

    // Die drei Säulen von der Game-Loop
    void processEvents(); // Fenster schließen etc.
    void update();        // Physik & Steuerung berechnen
    void render();        // Alles neu zeichnen

public:
    Game();
    void run();           // Startet das Spiel
};