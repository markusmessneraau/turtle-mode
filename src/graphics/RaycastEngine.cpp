#include "RaycastEngine.h"
#include <cmath>
#include <algorithm>

RaycastEngine::RaycastEngine(unsigned int width, unsigned int height) 
    : screenWidth(static_cast<float>(width)), screenHeight(static_cast<float>(height)) {}

void RaycastEngine::render3DFPV(sf::RenderWindow& window, const Drone& drone, const Map& gameMap) {
    sf::View defaultView = window.getView(); 
    sf::View fpvView = defaultView;
    
    fpvView.setRotation(sf::degrees(drone.getRoll())); 
    fpvView.setSize(sf::Vector2f(defaultView.getSize().x * 0.82f, defaultView.getSize().y * 0.82f));
    window.setView(fpvView); 

    int numRays = 1000;             
    float fov = 60.0f;             
    float maxDepth = 500.0f;       
    float columnWidth = screenWidth / static_cast<float>(numRays);

    float droneYawRad = drone.getYaw() * 3.14159265f / 180.0f;

    float horizonOffset = (-drone.getPitch() * 4.0f) + (drone.getAltitude() * 0.8f);
    float horizon = (screenHeight / 2.0f) + horizonOffset;

    sf::RectangleShape sky(sf::Vector2f(screenWidth * 2.0f, screenHeight * 2.0f));
    sky.setOrigin(sf::Vector2f(screenWidth, screenHeight));
    sky.setPosition(sf::Vector2f(screenWidth / 2.0f, horizon));
    sky.setFillColor(sf::Color(20, 20, 40)); 
    window.draw(sky);

    sf::RectangleShape floor(sf::Vector2f(screenWidth * 2.0f, screenHeight * 2.0f));
    floor.setOrigin(sf::Vector2f(screenWidth, 0.0f));
    floor.setPosition(sf::Vector2f(screenWidth / 2.0f, horizon));
    floor.setFillColor(sf::Color(40, 70, 40)); 
    window.draw(floor);

    for (int i = 0; i < numRays; i++) {
        float rayAngleRad = droneYawRad + ((i - numRays / 2.0f) * (fov / numRays) * 3.14159265f / 180.0f);
        float distanceToWall = 0.0f;
        bool hitWall = false;

        float eyeX = std::cos(rayAngleRad);
        float eyeY = std::sin(rayAngleRad);

        while (!hitWall && distanceToWall < maxDepth) {
            distanceToWall += 6.0f; 

            float testX = drone.getPosition().x + eyeX * distanceToWall;
            float testY = drone.getPosition().y + eyeY * distanceToWall;

            if (gameMap.isVisualWall(testX, testY)) {
                hitWall = true;
            }
        }

        float correctedDistance = distanceToWall * std::cos(rayAngleRad - droneYawRad);
        if (correctedDistance < 1.0f) correctedDistance = 1.0f;

        float wallHeight = (screenHeight * 15.0f) / correctedDistance;
        if (wallHeight > screenHeight * 2.0f) wallHeight = screenHeight * 2.0f;

        float wallTop = horizon - (wallHeight / 2.0f);

        float shade = 255.0f * (1.0f - (distanceToWall / maxDepth));
        if (shade < 0) shade = 0;
        if (shade > 255) shade = 255;

        sf::RectangleShape wallColumn(sf::Vector2f(columnWidth + 1.0f, wallHeight));
        wallColumn.setPosition(sf::Vector2f(i * columnWidth, wallTop));

        float hitX = drone.getPosition().x + std::cos(rayAngleRad) * distanceToWall;
        float hitY = drone.getPosition().y + std::sin(rayAngleRad) * distanceToWall;
        sf::Color baseColor = gameMap.getWallColor(hitX, hitY);

        float lightIntensity = shade / 255.0f;
        wallColumn.setFillColor(sf::Color(
            baseColor.r * lightIntensity,
            baseColor.g * lightIntensity,
            baseColor.b * lightIntensity
        ));

        //wallColumn.setFillColor(sf::Color(shade, shade * 0.3f, shade * 0.3f)); 
        window.draw(wallColumn);
    }

    window.setView(defaultView);
}

void RaycastEngine::render2DMap(sf::RenderWindow& window, const Drone& drone, const Map& gameMap) {
    const_cast<Map&>(gameMap).draw(window); 

    float scaleFactor = 1.0f + (drone.getAltitude() * 0.005f); 
    if (scaleFactor < 0.5f) scaleFactor = 0.5f;
    if (scaleFactor > 2.5f) scaleFactor = 2.5f;

    sf::CircleShape body(drone.getRadius());
    body.setFillColor(sf::Color(255, 50, 50));
    body.setOrigin(sf::Vector2f(drone.getRadius(), drone.getRadius()));
    body.setPosition(drone.getPosition());
    body.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    sf::RectangleShape frontPointer(sf::Vector2f(22.0f, 4.0f));
    frontPointer.setFillColor(sf::Color::White);
    frontPointer.setOrigin(sf::Vector2f(0.0f, 2.0f));
    frontPointer.setPosition(drone.getPosition());
    frontPointer.setRotation(sf::degrees(drone.getYaw()));
    frontPointer.setScale(sf::Vector2f(scaleFactor, scaleFactor));

    window.draw(body);
    window.draw(frontPointer);
}