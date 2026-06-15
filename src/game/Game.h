#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../world/Map.h"
#include "../entities/Drone.h"
#include "../game/InputManager.h"
#include "../graphics/RaycastEngine.h"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();
    void updateSounds(float throttle);

    sf::RenderWindow window;
    Map gameMap;
    Drone myDrone;
    InputManager inputManager;
    RaycastEngine renderer;
    bool is3DMode;

   sf::SoundBuffer bufferStartup;
    sf::SoundBuffer bufferIdle;
    sf::SoundBuffer bufferGas2;
    sf::SoundBuffer bufferGas3;
    sf::SoundBuffer bufferGas4;
    sf::SoundBuffer bufferMax;

   
    sf::Sound soundStartup;
    sf::Sound soundIdle;
    sf::Sound soundGas2;
    sf::Sound soundGas3;
    sf::Sound soundGas4;
    sf::Sound soundMax;
};