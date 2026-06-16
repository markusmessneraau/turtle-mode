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
    
    // Eingaben aus InputManager übernehmen
    yawAngle   += controls.yaw;
    throttle   = controls.throttle;
    
    // Neigung dämpfenn, um zu starkes Kippen zu verhindern
    pitchAngle += controls.pitch * 0.8f;
    rollAngle  += controls.roll * 0.8f;

    // Begrenzung der Neigungswinkel
    pitchAngle = std::clamp(pitchAngle, -50.0f, 50.0f);
    rollAngle  = std::clamp(rollAngle, -50.0f, 50.0f);

    
    // Radiant = Grad * PI / 180 wegen C++
    float yawRad = yawAngle * 3.14159265f / 180.0f;
    float pitchRad = pitchAngle * 3.14159265f / 180.0f;
    float rollRad = rollAngle * 3.14159265f / 180.0f;


    // Richtungsvektoren berechnen
    sf::Vector2f forwardDirection(std::cos(yawRad), std::sin(yawRad));
    sf::Vector2f rightDirection(-std::sin(yawRad), std::cos(yawRad));

    // Motor-Start-Schwelle: Ignoriert minimale Schub-Werte
    float finalThrottle = throttle;
    if (finalThrottle < 0.05f) {
        finalThrottle = 0.0f;
    }


    // Berechnet finalen Schub: Gaspedal * Maximale Motorleistung
    float totalThrust = finalThrottle * thrustPower;
    
    // Bewegungsvektoren: vorwärts und seitwärts Kraft basierend auf Neigung und Schub
    sf::Vector2f pitchForce = forwardDirection * (totalThrust * std::sin(pitchRad));
    sf::Vector2f rollForce = rightDirection * (totalThrust * std::sin(rollRad));

    // Addiert Kräfte und wendet Luftwiderstand an
    velocity += pitchForce + rollForce;
    velocity *= drag;

    // berechnet vertikale Bewegung: Schub nach oben minus Schwerkraft
    float upwardThrust = totalThrust * std::cos(pitchRad) * std::cos(rollRad);
    float verticalForce = upwardThrust - gravity;
    
    // Vertikale Kraft auf die Steig-/Sinkgeschwindigkeit addieren
    verticalVelocity += verticalForce; pitchAngle = std::clamp(pitchAngle, -50.0f, 50.0f);
    // Luftwiderstand
    verticalVelocity *= verticalDrag; 
    // Update der Flughöhe
    altitude += verticalVelocity;

    // Verhindert, dass Drohne unter Boden sinkt
    if (altitude < 0.0f) {
        altitude = 0.0f;
        verticalVelocity = 0.0f;
    }

    // --- COLLISION CHECK---
    sf::Vector2f nextPosX = position + sf::Vector2f(velocity.x, 0.0f); // zukünftige X-Position

    bool wallX = gameMap.isWall(nextPosX.x, nextPosX.y) || // mittelpunkt
                 gameMap.isWall(nextPosX.x + radius, nextPosX.y) || // rechts
                 gameMap.isWall(nextPosX.x - radius, nextPosX.y) || // links
                 gameMap.isWall(nextPosX.x, nextPosX.y + radius) || // unten
                 gameMap.isWall(nextPosX.x, nextPosX.y - radius); // oben

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