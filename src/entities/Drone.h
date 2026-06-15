#pragma once
#include <SFML/System/Vector2.hpp>
#include "../world/Map.h"
#include "../game/InputManager.h"

class Drone {
public:
    Drone(float startX, float startY);
    void update(const DroneControls& controls, const Map& gameMap);

    // Getters für die Grafik-Engine 
    sf::Vector2f getPosition() const { return position; }
    float getAltitude() const { return altitude; }
    float getYaw() const { return yawAngle; }
    float getPitch() const { return pitchAngle; }
    float getRoll() const { return rollAngle; }
    float getRadius() const { return radius; }

private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float altitude;
    float verticalVelocity;
    float yawAngle;
    float pitchAngle;
    float rollAngle;
    float throttle;

    
    const float radius = 8.0f;
    const float thrustPower = 0.5f;
    const float gravity = 0.15f;
    const float drag = 0.98f;
    const float verticalDrag = 0.95f;
};