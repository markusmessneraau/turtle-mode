#pragma once
#include <SFML/System/Vector2.hpp>
#include "../world/Map.h"
#include "../game/InputManager.h"

class Drone {
public:
    Drone(float startX, float startY);

    // Aktualisiert Drohne basierend auf Steuerungswerten und Karte
    void update(const DroneControls& controls, const Map& gameMap);

    // Getters für die Grafik-Engine (Raycaster & 2D-Ansicht)
    sf::Vector2f getPosition() const { return position; }
    float getAltitude() const { return altitude; }
    float getYaw() const { return yawAngle; } // Blickrichtung
    float getPitch() const { return pitchAngle; } // Neigung nach vorne/hinten
    float getRoll() const { return rollAngle; } // Neigung nach links/rechts
    float getRadius() const { return radius; } // Größe der Drohne für Kollision
    void setPosition(sf::Vector2f newPos);

private:
    sf::Vector2f position; // X & Y in Pixeln
    sf::Vector2f velocity;

    float altitude;
    float verticalVelocity;

    float yawAngle;
    float pitchAngle;
    float rollAngle;
    float throttle; // Schubkraft

    
    // Konstanten für Drohnenphysik
    const float radius = 8.0f;
    const float thrustPower = 0.5f;
    const float gravity = 0.15f;
    const float drag = 0.98f; // Luftwiderstand
    const float verticalDrag = 0.95f;
};