#pragma once
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>

struct DroneControls {
    float throttle = 0.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;
};

class InputManager {
public:
    InputManager();
    DroneControls getControls();

private:
    bool isControllerConnected;
    float currentThrottle;
    
    
    const float yawSpeed = 2.0f;
    const float pitchSpeed = 1.5f;
    const float rollSpeed = 1.5f;
};