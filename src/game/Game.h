#pragma once
#include <SFML/Graphics.hpp>
#include "../world/Map.h"
#include "../entities/Drone.h"
#include "../game/InputManager.h"
#include "../graphics/RaycastEngine.h"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();

    sf::RenderWindow window;
    Map gameMap;
    Drone myDrone;
    InputManager inputManager;
    RaycastEngine renderer;
    bool is3DMode;
};