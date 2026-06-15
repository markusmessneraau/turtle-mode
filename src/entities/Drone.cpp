#include "Drone.h"
#include <cmath>
#include <algorithm> // Für std::clamp

Drone::Drone(float startX, float startY) {
    position = sf::Vector2f(startX, startY);
    velocity = sf::Vector2f(0.0f, 0.0f);
    yawAngle = 0.0f;
    pitchAngle = 0.0f;
    rollAngle = 0.0f;
    throttle = 0.0f;
    isControllerConnected = false;
}

void Drone::handleInput() {
    isControllerConnected = sf::Joystick::isConnected(0);

    if (isControllerConnected) {
        // --- RADIOMASTER POCKET ACHSEN ---
        float jX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X); // ROLL
        float jY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y); // PITCH
        float jZ = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Z); // THROTTLE
        float jU = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U); // YAW

        // LINKER STICK: Links/Rechts -> YAW (Drehen)
        if (std::abs(jU) > 10.0f) {
            yawAngle += (jU / 100.0f) * yawSpeed;
        }

        //LINKER STICK: Hoch/Runter -> THROTTLE (Gas)
       
        throttle = (100.0f + jZ) / 200.0f; 

        //RECHTER STICK: Hoch/Runter -> PITCH (Nase neigen)
        if (std::abs(jY) > 10.0f) {
            pitchAngle += (jY / 100.0f) * pitchSpeed;
        }
        
        //RECHTER STICK: Links/Rechts -> ROLL (Seitlich kippen)
        if (std::abs(jX) > 10.0f) {
            rollAngle += (jX / 100.0f) * rollSpeed;
        }

        // FPV-Winkelgrenzen setzen
        pitchAngle = std::clamp(pitchAngle, -50.0f, 50.0f);
        rollAngle = std::clamp(rollAngle, -25.0f, 25.0f);
        
    } else {
        // --- TASTATUR MODUS ---
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) yawAngle -= yawSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) yawAngle += yawSpeed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) throttle += 0.008f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) throttle -= 0.008f;
        throttle = std::clamp(throttle, 0.0f, 1.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) pitchAngle += pitchSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) pitchAngle -= pitchSpeed;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) rollAngle -= rollSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) rollAngle += rollSpeed;
        
        pitchAngle = std::clamp(pitchAngle, -50.0f, 50.0f);
        rollAngle = std::clamp(rollAngle, -25.0f, 25.0f); 
    }
}

void Drone::update(const Map& gameMap) {
    float yawRad = yawAngle * 3.14159265f / 180.0f;
    sf::Vector2f forwardDirection(std::cos(yawRad), std::sin(yawRad));
    sf::Vector2f rightDirection(-std::sin(yawRad), std::cos(yawRad));

    float totalThrust = throttle * thrustPower;
    
    // Kräfte anhand der echten Neigungswinkel der Sticks berechnen
    float pitchRad = pitchAngle * 3.14159265f / 180.0f;
    sf::Vector2f pitchForce = forwardDirection * (totalThrust * std::sin(pitchRad));

    float rollRad = rollAngle * 3.14159265f / 180.0f;
    sf::Vector2f rollForce = rightDirection * (totalThrust * std::sin(rollRad));

    velocity += pitchForce + rollForce;
    velocity *= drag;

    sf::Vector2f nextPos = position + velocity;
    if (!gameMap.isWall(nextPos.x, nextPos.y)) {
        position = nextPos;
    } else {
        velocity = sf::Vector2f(0.0f, 0.0f); // Crash!
    }
}

void Drone::draw(sf::RenderWindow& window) {
    sf::CircleShape body(radius);
    body.setFillColor(sf::Color(255, 50, 50));
    body.setOrigin(sf::Vector2f(radius, radius));
    body.setPosition(position);

    sf::RectangleShape frontPointer(sf::Vector2f(22.0f, 4.0f));
    frontPointer.setFillColor(sf::Color::White);
    frontPointer.setOrigin(sf::Vector2f(0.0f, 2.0f));
    frontPointer.setPosition(position);
    frontPointer.setRotation(sf::degrees(yawAngle));

    window.draw(body);
    window.draw(frontPointer);
}