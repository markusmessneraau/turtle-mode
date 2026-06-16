#include "KeyRenderer.h"
#include "../entities/Drone.h"

#include <cmath>
#include <algorithm>

KeyRenderer::KeyRenderer() {
}

bool KeyRenderer::loadTexture(const std::string& filename) {
    return keyTexture.loadFromFile(filename);
}

void KeyRenderer::reset() {
    rawYellowTiles.clear();
}

void KeyRenderer::collectKeyTile(float testX, float testY) {
    const float tileSize = 64.0f;

    int gridX = static_cast<int>(std::floor(testX / tileSize));
    int gridY = static_cast<int>(std::floor(testY / tileSize));

    sf::Vector2f tileCenter(
        gridX * tileSize + 32.0f,
        gridY * tileSize + 32.0f
    );

    for (const auto& existingTile : rawYellowTiles) {
        if (existingTile == tileCenter) {
            return;
        }
    }

    rawYellowTiles.push_back(tileCenter);
}

std::vector<sf::Vector2f> KeyRenderer::createClusteredItems() const {
    std::vector<sf::Vector2f> finalItems;
    std::vector<bool> merged(rawYellowTiles.size(), false);

    for (size_t i = 0; i < rawYellowTiles.size(); i++) {
        if (merged[i]) {
            continue;
        }

        float sumX = rawYellowTiles[i].x;
        float sumY = rawYellowTiles[i].y;
        int count = 1;

        for (size_t j = i + 1; j < rawYellowTiles.size(); j++) {
            if (!merged[j]) {
                float distX = std::abs(rawYellowTiles[i].x - rawYellowTiles[j].x);
                float distY = std::abs(rawYellowTiles[i].y - rawYellowTiles[j].y);

                if (distX <= 70.0f && distY <= 70.0f) {
                    sumX += rawYellowTiles[j].x;
                    sumY += rawYellowTiles[j].y;
                    count++;
                    merged[j] = true;
                }
            }
        }

        finalItems.push_back(sf::Vector2f(sumX / count, sumY / count));
    }

    return finalItems;
}

void KeyRenderer::render(
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
) {
    if (keyTexture.getSize().x == 0 || keyTexture.getSize().y == 0) {
        return;
    }

    const float PI = 3.14159265f;

    std::vector<sf::Vector2f> finalItems = createClusteredItems();

    for (const auto& itemPos : finalItems) {
        float vecX = itemPos.x - drone.getPosition().x;
        float vecY = itemPos.y - drone.getPosition().y;
        float distToItem = std::sqrt(vecX * vecX + vecY * vecY);

        float itemAngle = std::atan2(vecY, vecX) - droneYawRad;

        while (itemAngle < -PI) {
            itemAngle += 2.0f * PI;
        }

        while (itemAngle > PI) {
            itemAngle -= 2.0f * PI;
        }

        if (std::abs(itemAngle) < 0.6f && distToItem > 10.0f) {
            float perpendicularItemDist = distToItem * std::cos(itemAngle);

            if (perpendicularItemDist < 1.0f) {
                perpendicularItemDist = 1.0f;
            }

            float screenX = (screenWidth / 2.0f) +
                (
                    std::tan(itemAngle) *
                    (screenWidth / 2.0f) /
                    std::tan(fov * PI / 360.0f)
                );

            float wallSize = (screenHeight * 15.0f) / perpendicularItemDist;

            if (wallSize > screenHeight * 2.0f) {
                wallSize = screenHeight * 2.0f;
            }

            float itemHeight = wallSize * 0.4f;

            sf::Sprite keySprite(keyTexture);

            keySprite.setOrigin(sf::Vector2f(
                static_cast<float>(keyTexture.getSize().x) / 2.0f,
                static_cast<float>(keyTexture.getSize().y) / 2.0f
            ));

            keySprite.setPosition(sf::Vector2f(
                screenX,
                horizon + (wallSize * 0.2f)
            ));

            float scale = itemHeight / static_cast<float>(keyTexture.getSize().y);
            keySprite.setScale(sf::Vector2f(scale, scale));

            float itemShade = 255.0f * (1.0f - (distToItem / maxDepth));

            if (itemShade < 0.0f) {
                itemShade = 0.0f;
            }

            if (itemShade > 255.0f) {
                itemShade = 255.0f;
            }

            unsigned char shadeByte = static_cast<unsigned char>(itemShade);

            keySprite.setColor(sf::Color(
                shadeByte,
                shadeByte,
                shadeByte
            ));

            int checkIndex = static_cast<int>(screenX / columnWidth);

            if (checkIndex >= 0 && checkIndex < numRays) {
                if (depthBuffer[checkIndex] > perpendicularItemDist - 32.0f) {
                    window.draw(keySprite);
                }
            }
        }
    }
}