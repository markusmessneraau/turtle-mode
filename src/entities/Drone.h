#pragma once
#include <SFML/Graphics.hpp>
#include "../world/Map.h"

class Drone {
private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    
    // Die 3 Acro-Achsen + Gas (Mode 2)
    float yawAngle;           // Nase drehen (WASD: A/D)
    float pitchAngle;         // Nase kippen (Pfeiltasten: Hoch/Runter)
    float rollAngle;          // Seitlich kippen (Pfeiltasten: Links/Rechts)
    float throttle;           // Motorschub (WASD: W/S)
    bool isControllerConnected;

    // Physikalische Koeffizienten für echtes FPV-Feeling
    const float thrustPower = 0.25f;    
    const float drag = 0.985f;          
    const float yawSpeed = 3.5f;        
    const float pitchSpeed = 2.0f;      
    const float rollSpeed = 2.0f;       // Wie schnell sich die Drohne zur Seite rollt
    const float radius = 12.0f;         

public:
    Drone(float startX, float startY);
    
    void handleInput();
    void update(const Map& gameMap);
    void draw(sf::RenderWindow& window);
};