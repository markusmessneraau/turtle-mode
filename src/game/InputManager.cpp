#include "InputManager.h"
#include <cmath>
#include <algorithm>

InputManager::InputManager() : isControllerConnected(false), currentThrottle(0.0f) {}

DroneControls InputManager::getControls() {
    isControllerConnected = sf::Joystick::isConnected(0);
    DroneControls controls;

    if (isControllerConnected) {
        float jX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X); 
        float jY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y); 
        float jZ = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Z); 
        float jU = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U); 


        // Stickdrift ausfiltern und in Steuerungswerte umwandeln
        if (std::abs(jU) > 10.0f) controls.yaw = (jU / 100.0f) * yawSpeed;
        controls.throttle = (jZ + 100.0f) / 200.0f;  // von -1..1 auf 0..1 mappen

        if (std::abs(jY) > 10.0f) controls.pitch = (jY / 100.0f) * pitchSpeed;
        if (std::abs(jX) > 10.0f) controls.roll = (jX / 100.0f) * rollSpeed;
    } else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) controls.yaw = -yawSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) controls.yaw = yawSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) currentThrottle += 0.008f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) currentThrottle -= 0.008f;
        
        currentThrottle = std::clamp(currentThrottle, 0.0f, 1.0f); // speed zwischen 0 und 1
        controls.throttle = currentThrottle;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) controls.pitch = pitchSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) controls.pitch = -pitchSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) controls.roll = -rollSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) controls.roll = rollSpeed;
    }

    controls.pitch = std::clamp(controls.pitch, -25.0f, 25.0f); // keine Loopings
    controls.roll = std::clamp(controls.roll, -25.0f, 25.0f);
    
    return controls;
}