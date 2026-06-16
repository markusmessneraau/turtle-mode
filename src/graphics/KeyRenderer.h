#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Drone;

class KeyRenderer {
public:
    KeyRenderer();

    bool loadTexture(const std::string& filename);

    void reset();

    void collectKeyTile(float testX, float testY);

    void render(
        sf::RenderWindow& window,
        const Drone& drone,
        const std::vector<float>& depthBuffer,
        float screenWidth,
        float screenHeight,
        float horizon,
        float fov,
        float maxDepth,
        float droneYawRad,
        int numRays,
        float columnWidth
    );

private:
    sf::Texture keyTexture;
    std::vector<sf::Vector2f> rawYellowTiles;

    std::vector<sf::Vector2f> createClusteredItems() const;
};