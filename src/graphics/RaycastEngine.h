#pragma once
#include <SFML/Graphics.hpp>
#include "../entities/Drone.h"
#include "../world/Map.h"

class RaycastEngine {
public:
    RaycastEngine(unsigned int width, unsigned int height);
    
    void render3DFPV(sf::RenderWindow& window, const Drone& drone, const Map& gameMap);
    void render2DMap(sf::RenderWindow& window, const Drone& drone, const Map& gameMap);

private:
    float screenWidth;
    float screenHeight;
    sf::Texture tileset;
};