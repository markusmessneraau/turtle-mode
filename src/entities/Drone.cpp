#include "Drone.h"
#include <cmath>
#include <algorithm>

Drone::Drone(float startX, float startY) {
    position = sf::Vector2f(startX, startY);
    velocity = sf::Vector2f(0.0f, 0.0f);
    altitude = 0.0f;             
    verticalVelocity = 0.0f;
    yawAngle = 0.0f;
    pitchAngle = 0.0f;
    rollAngle = 0.0f;
    throttle = 0.0f;
}

void Drone::update(const DroneControls& controls, const Map& gameMap) {
    
    yawAngle   += controls.yaw;
    throttle   = controls.throttle;
    
  
    pitchAngle += controls.pitch * 0.8f;
    rollAngle  += controls.roll * 0.8f;

    
   
    pitchAngle = std::clamp(pitchAngle, -50.0f, 50.0f);
    rollAngle  = std::clamp(rollAngle, -50.0f, 50.0f);

    
    float yawRad = yawAngle * 3.14159265f / 180.0f;
    float pitchRad = pitchAngle * 3.14159265f / 180.0f;
    float rollRad = rollAngle * 3.14159265f / 180.0f;

    sf::Vector2f forwardDirection(std::cos(yawRad), std::sin(yawRad));
    sf::Vector2f rightDirection(-std::sin(yawRad), std::cos(yawRad));

    float finalThrottle = throttle;
    if (finalThrottle < 0.05f) {
        finalThrottle = 0.0f;
    }

    float totalThrust = finalThrottle * thrustPower;
    
    sf::Vector2f pitchForce = forwardDirection * (totalThrust * std::sin(pitchRad));
    sf::Vector2f rollForce = rightDirection * (totalThrust * std::sin(rollRad));
    velocity += pitchForce + rollForce;
    velocity *= drag;

    float upwardThrust = totalThrust * std::cos(pitchRad) * std::cos(rollRad);
    float verticalForce = upwardThrust - gravity;
    
    verticalVelocity += verticalForce; pitchAngle = std::clamp(pitchAngle, -50.0f, 50.0f);
    verticalVelocity *= verticalDrag; 
    altitude += verticalVelocity;

    if (altitude < 0.0f) {
        altitude = 0.0f;
        verticalVelocity = 0.0f;
    }

    // --- COLLISION CHECK---
    sf::Vector2f nextPosX = position + sf::Vector2f(velocity.x, 0.0f);
    bool wallX = gameMap.isWall(nextPosX.x, nextPosX.y) ||
                 gameMap.isWall(nextPosX.x + radius, nextPosX.y) ||
                 gameMap.isWall(nextPosX.x - radius, nextPosX.y) ||
                 gameMap.isWall(nextPosX.x, nextPosX.y + radius) ||
                 gameMap.isWall(nextPosX.x, nextPosX.y - radius);

    if (!wallX) position.x = nextPosX.x;
    else velocity.x = 0.0f;

    sf::Vector2f nextPosY = position + sf::Vector2f(0.0f, velocity.y);
    bool wallY = gameMap.isWall(nextPosY.x, nextPosY.y) ||
                 gameMap.isWall(nextPosY.x + radius, nextPosY.y) ||
                 gameMap.isWall(nextPosY.x - radius, nextPosY.y) ||
                 gameMap.isWall(nextPosY.x, nextPosY.y + radius) ||
                 gameMap.isWall(nextPosY.x, nextPosY.y - radius);

    if (!wallY) position.y = nextPosY.y;
    else velocity.y = 0.0f;
}

void Drone::setPosition(sf::Vector2f newPos) {
    position = newPos;
}