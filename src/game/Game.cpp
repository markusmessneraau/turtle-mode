#include "Game.h"
#include <iostream>

Game::Game() 
    : window(sf::VideoMode(sf::Vector2u(800, 600)), "Turtle-Mode Simulator (Clean Code)"),
      myDrone(100.0f, 100.0f),
      renderer(800, 600),
      is3DMode(true) 
{
    window.setFramerateLimit(60);
    if (!gameMap.loadFromText("level1.txt")) {
        std::cout << "Map konnte nicht geladen werden!" << std::endl;
    }
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Tab) {
                is3DMode = !is3DMode;
            }
        }
    }
}

void Game::update() {
    DroneControls controls = inputManager.getControls();
    myDrone.update(controls, gameMap);
    
    // Map die aktuelle Position der Drohne übergeben
    gameMap.collectItems(myDrone.getPosition().x, myDrone.getPosition().y);
}

void Game::render() {
    window.clear();
    if (is3DMode) {
        renderer.render3DFPV(window, myDrone, gameMap);
    } else {
        renderer.render2DMap(window, myDrone, gameMap);
    }
    window.display();
}