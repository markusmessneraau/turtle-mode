#include "Game.h"
#include <iostream>

Game::Game() 
    : window(sf::VideoMode(sf::Vector2u(800, 600)), "Turtle-Mode Simulator (Clean Code)"),
      myDrone(100.0f, 100.0f),
      renderer(800, 600),
      is3DMode(true),
      hudText(font)
{
    window.setFramerateLimit(60);
    if (!gameMap.loadFromText("level1.txt")) {
        std::cout << "Map konnte nicht geladen werden!" << std::endl;
    }

    if (!font.openFromFile("arial.ttf")) {
        std::cout << "Warnung: Schriftart arial.ttf nicht gefunden!\n";
    }
    
    hudText.setCharacterSize(24); // Schriftgröße
    hudText.setFillColor(sf::Color::Yellow);
    hudText.setPosition(sf::Vector2f(20.0f, 20.0f)); // Oben links


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

    if (gameWon) {
        return; 
    }

    DroneControls controls = inputManager.getControls();
    myDrone.update(controls, gameMap);

    // Map die aktuelle Position der Drohne übergeben
    gameMap.collectItems(myDrone.getPosition().x, myDrone.getPosition().y);

    if (gameMap.levelFinished) {
        static int currentLevel = 1;
        currentLevel++;
        
        // Baut Dateinamen zusammen
        std::string nextLevelPath = "level" + std::to_string(currentLevel) + ".txt";
        
        // Versucht nächste Datei zu laden
        if (gameMap.loadFromText(nextLevelPath)) {
            std::cout << "Starte Level " << currentLevel << "!\n";
            
            // Setzt Drohne auf start im neuen Level
            myDrone.setPosition(gameMap.getStartPosition()); 
        } else {
            std::cout << "\n====================================\n";
            std::cout << "          ALLE LEVEL GESCHAFFT!       \n";
            std::cout << "====================================\n\n";
            gameWon = true;
        }
    }
}

void Game::render() {
    window.clear();

    if (gameWon) {
        
        window.clear(sf::Color(50, 30, 0)); 

        sf::Text endText(font);
        endText.setString(" MISSION ACCOMPLISHED!\n\n       You escaped!!");
        endText.setCharacterSize(28);
        endText.setFillColor(sf::Color::Yellow);
        
        // grob in Mitte
        endText.setPosition(sf::Vector2f(60.0f, 250.0f)); 

        window.draw(endText);
    } else{
        if (is3DMode) {
        renderer.render3DFPV(window, myDrone, gameMap);
    } else {
        renderer.render2DMap(window, myDrone, gameMap);
    }

    int keysLeft = gameMap.getRemainingKeys();
    if (keysLeft > 0) {
        hudText.setString("Keys left: " + std::to_string(keysLeft));
        hudText.setFillColor(sf::Color::Yellow);
    } else {
        hudText.setString("Doors open! Find the portal!");
        hudText.setFillColor(sf::Color::Green);
    }

    window.draw(hudText);
    }
    


    window.display();
}