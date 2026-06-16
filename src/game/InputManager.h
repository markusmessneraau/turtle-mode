#pragma once
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>

struct DroneControls {
    float throttle = 0.0f; // Hoch/Runter-Bewegung
    float yaw = 0.0f; // Drehung um vertikale Achse
    float pitch = 0.0f; // Neigung um Querachse
    float roll = 0.0f; //
};

class InputManager {

public:
    InputManager();
    DroneControls getControls(); // liest Controller/Tastatur

private:
    bool isControllerConnected;
    float currentThrottle; // aktueller Schub
    
    
    // feste Steuerungswerte für Drohne
    const float yawSpeed = 2.0f;
    const float pitchSpeed = 1.5f;
    const float rollSpeed = 1.5f;
};