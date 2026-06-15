#include "Game.h"

// Fenster mit 800x600 Pixeln
// und spawnen die Drohne bei X: 100, Y: 100
Game::Game() 
    : window(sf::VideoMode(sf::Vector2u(800, 600)), "Turtle-Mode Simulator"),
      myDrone(100.0f, 100.0f) 
{
    window.setFramerateLimit(60); // Wichtig: Begrenzt das Spiel auf 60 FPS!!!!!
}

void Game::run() {
    // Diese Schleife läuft unendlich, bis das Fenster geschlossen wird
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    // wenn das spiel geschlossen wird
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
    }
}

void Game::update() {
    //  Eingabe von Tastatur oder Radiomaster abfragen
    myDrone.handleInput();
    
    //  Position und Kollisionen der Drohne updaten
    myDrone.update(gameMap);
}

void Game::render() {
    window.clear(sf::Color(15, 15, 15)); // Hintergrund dunkelgrau färben
    
    // Zuerst die Map (Wände) zeichnen
    gameMap.draw(window);
    
    // Die Drohne darüber zeichnen
    myDrone.draw(window);
    
    window.display(); //Bild auf dem Monitor anzeigen
}