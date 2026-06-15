#include "Game.h"
#include <iostream>
#include <algorithm> 

Game::Game() 
    : window(sf::VideoMode(sf::Vector2u(800, 600)), "Turtle-Mode Simulator"),
      myDrone(100.0f, 100.0f),
      renderer(800, 600),
      is3DMode(true),
      soundStartup(bufferStartup), 
      soundIdle(bufferIdle),     
      soundGas2(bufferGas2),     
      soundGas3(bufferGas3),     
      soundGas4(bufferGas4),     
      soundMax(bufferMax),
      hudText(font)
{
    window.setFramerateLimit(60);
    
    
    if (!gameMap.loadFromText("level1.txt")) {
        std::cout << "Map konnte nicht geladen werden!" << std::endl;
    }

    
    if (!font.openFromFile("arial.ttf")) {
        std::cout << "Warnung: Schriftart arial.ttf nicht gefunden!\n";
    }
    
    hudText.setCharacterSize(24);
    hudText.setFillColor(sf::Color::Yellow);
    hudText.setPosition(sf::Vector2f(20.0f, 20.0f));

    
    if (!bufferStartup.loadFromFile("assets/sound/startup.wav") ||
        !bufferIdle.loadFromFile("assets/sound/idle.wav") ||
        !bufferGas2.loadFromFile("assets/sound/gas2.wav") ||
        !bufferGas3.loadFromFile("assets/sound/gas3.wav") ||
        !bufferGas4.loadFromFile("assets/sound/gas4.wav") ||
        !bufferMax.loadFromFile("assets/sound/max.wav")) 
    {
        std::cout << "[ERROR] Sound-Dateien konnten nicht geladen werden! Pfade pruefen!" << std::endl;
    }

    // Sounds initialisieren und starten
    soundStartup.setVolume(70.0f);
    soundStartup.play();

    soundIdle.setLooping(true);
    soundGas2.setLooping(true);
    soundGas3.setLooping(true);
    soundGas4.setLooping(true);
    soundMax.setLooping(true);

    soundIdle.setVolume(0.0f);
    soundGas2.setVolume(0.0f);
    soundGas3.setVolume(0.0f);
    soundGas4.setVolume(0.0f);
    soundMax.setVolume(0.0f);

    soundIdle.play();
    soundGas2.play();
    soundGas3.play();
    soundGas4.play();
    soundMax.play();
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

    
    updateSounds(controls.throttle);

    // Gegenstände sammeln
    gameMap.collectItems(myDrone.getPosition().x, myDrone.getPosition().y);

    
    if (gameMap.levelFinished) {
        static int currentLevel = 1;
        currentLevel++;
        
        std::string nextLevelPath = "level" + std::to_string(currentLevel) + ".txt";
        
        if (gameMap.loadFromText(nextLevelPath)) {
            std::cout << "Starte Level " << currentLevel << "!\n";
            myDrone.setPosition(gameMap.getStartPosition()); 
        } else {
            std::cout << "\n====================================\n";
            std::cout << "          ALLE LEVEL GESCHAFFT!       \n";
            std::cout << "====================================\n\n";
            gameWon = true;
        }
    }
}

void Game::updateSounds(float throttle) {
    float vIdle = 0.0f, vGas2 = 0.0f, vGas3 = 0.0f, vGas4 = 0.0f, vMax = 0.0f;
    float pIdle = 1.0f, pGas2 = 1.0f, pGas3 = 1.0f, pGas4 = 1.0f, pMax = 1.0f;

    if (throttle < 0.20f) {
        float p = throttle / 0.20f;
        vIdle = (1.0f - p) * 50.0f;
        vGas2 = p * 50.0f;

        pIdle = 0.90f + (p * 0.15f);
        pGas2 = 0.95f + (p * 0.10f);
    } 
    else if (throttle >= 0.20f && throttle < 0.45f) {
        float p = (throttle - 0.20f) / (0.45f - 0.20f);
        vGas2 = (1.0f - p) * 50.0f;
        vGas3 = p * 50.0f;

        pGas2 = 1.05f + (p * 0.15f);
        pGas3 = 0.95f + (p * 0.15f);
    } 
    else if (throttle >= 0.45f && throttle < 0.75f) {
        float p = (throttle - 0.45f) / (0.75f - 0.45f);
        vGas3 = (1.0f - p) * 50.0f;
        vGas4 = p * 50.0f;

        pGas3 = 1.10f + (p * 0.15f);
        pGas4 = 1.00f + (p * 0.15f);
    } 
    else if (throttle >= 0.75f && throttle < 0.92f) {
        float p = (throttle - 0.75f) / (0.92f - 0.75f);
        vGas4 = (1.0f - p) * 50.0f;
        vMax = p * 50.0f;

        pGas4 = 1.15f + (p * 0.15f);
        pMax = 1.00f + (p * 0.20f);
    } 
    else {
        float p = (throttle - 0.92f) / (1.0f - 0.92f);
        vMax = 50.0f;
        pMax = 1.20f + (p * 0.20f);  
    }

    soundIdle.setVolume(vIdle);
    soundGas2.setVolume(vGas2);
    soundGas3.setVolume(vGas3);
    soundGas4.setVolume(vGas4);
    soundMax.setVolume(vMax);

    soundIdle.setPitch(pIdle);
    soundGas2.setPitch(pGas2);
    soundGas3.setPitch(pGas3);
    soundGas4.setPitch(pGas4);
    soundMax.setPitch(pMax);
}

void Game::render() {
    window.clear();

    if (gameWon) {
        window.clear(sf::Color(50, 30, 0)); 

        sf::Text endText(font);
        endText.setString(" MISSION ACCOMPLISHED!\n\n       You escaped!!");
        endText.setCharacterSize(28);
        endText.setFillColor(sf::Color::Yellow);
        endText.setPosition(sf::Vector2f(60.0f, 250.0f)); 

        window.draw(endText);
    } else {
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