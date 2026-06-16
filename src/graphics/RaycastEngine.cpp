#include "RaycastEngine.h"
#include <cmath>
#include <algorithm>
#include <iostream>

RaycastEngine::RaycastEngine(unsigned int width, unsigned int height) 
    : screenWidth(static_cast<float>(width)), screenHeight(static_cast<float>(height)) {
        if (!tileset.loadFromFile("sokoban_tilesheet.png")) {
            std::cerr << "Fehler: Konnte Tileset nicht laden!\n";
        }

        if (!droneTexture.loadFromFile("drone.png")) {
            std::cerr << "Fehler: Konnte drone.png nicht laden!\n";
        }
    }

void RaycastEngine::render3DFPV(sf::RenderWindow& window, const Drone& drone, const Map& gameMap) {

    // Kamera View holen
    sf::View defaultView = window.getView(); 
    sf::View fpvView = defaultView;
    
    // Drohnen-Neigung auf View anwenden
    fpvView.setRotation(sf::degrees(drone.getRoll())); 
    fpvView.setSize(sf::Vector2f(defaultView.getSize().x * 0.82f, defaultView.getSize().y * 0.82f));
    window.setView(fpvView); 


    // Raycasting-Parameter
    int numRays = 1000;    // Anzahl der Strahlen, die wir abfeuern         
    float fov = 60.0f;     // Sichtfeld in Grad  
    float maxDepth = 500.0f;       // Maximale Sichtweite
    float columnWidth = screenWidth / static_cast<float>(numRays); // Breite jedes Strahlen-Slices auf Bildschirm

    float droneYawRad = drone.getYaw() * 3.14159265f / 180.0f;

    float horizonOffset = (-drone.getPitch() * 4.0f) + (drone.getAltitude() * 0.8f);
    float horizon = (screenHeight / 2.0f) + horizonOffset;

    sf::RectangleShape sky(sf::Vector2f(screenWidth * 2.0f, screenHeight * 2.0f));
    sky.setOrigin(sf::Vector2f(screenWidth, screenHeight));
    sky.setPosition(sf::Vector2f(screenWidth / 2.0f, horizon));
    sky.setFillColor(sf::Color(25, 25, 25)); 
    window.draw(sky);

    sf::RectangleShape floor(sf::Vector2f(screenWidth * 2.0f, screenHeight * 2.0f));
    floor.setOrigin(sf::Vector2f(screenWidth, 0.0f));
    floor.setPosition(sf::Vector2f(screenWidth / 2.0f, horizon));
    floor.setFillColor(sf::Color(60, 20, 20));
    window.draw(floor);

    for (int i = 0; i < numRays; i++) {
        float rayAngleRad = droneYawRad + ((i - numRays / 2.0f) * (fov / numRays) * 3.14159265f / 180.0f);
        float distanceToWall = 0.0f;
        bool hitSolidWall = false;

        bool hitItem = false;
        float itemDistance = 0.0f;
        float itemHitX = 0.0f, itemHitY = 0.0f;

        float eyeX = std::cos(rayAngleRad);
        float eyeY = std::sin(rayAngleRad);

        while (!hitSolidWall && distanceToWall < maxDepth) {
            distanceToWall += 1.0f; 

            float testX = drone.getPosition().x + eyeX * distanceToWall;
            float testY = drone.getPosition().y + eyeY * distanceToWall;

            sf::Color currentColor = gameMap.getWallColor(testX, testY);

            // Schlüssel? -> Merken, aber weiterfliegen
            if (currentColor == sf::Color(255, 215, 0)) {
                if (!hitItem) { 
                    hitItem = true;
                    itemDistance = distanceToWall;
                    itemHitX = testX;
                    itemHitY = testY;
                }
            }

            else if (gameMap.isVisualWall(testX, testY)) {
                hitSolidWall = true;
            }

        }

        float correctedDistance = distanceToWall * std::cos(rayAngleRad - droneYawRad);
        if (correctedDistance < 1.0f) correctedDistance = 1.0f;

        float wallHeight = (screenHeight * 15.0f) / correctedDistance;
        if (wallHeight > screenHeight * 2.0f) wallHeight = screenHeight * 2.0f;

        float wallTop = horizon - (wallHeight / 2.0f);
    

        // Trefferpunkt berechnen
        float hitX = drone.getPosition().x + std::cos(rayAngleRad) * distanceToWall;
        float hitY = drone.getPosition().y + std::sin(rayAngleRad) * distanceToWall;

        //fragt Map nach ursprünglichen Farbe des Blocks
        sf::Color blockColor = gameMap.getWallColor(hitX, hitY);

        int kachelSpalte = 8; // Steinwand
        int kachelZeile = 6;  


       if (blockColor == sf::Color(50, 200, 50)) { 
            kachelSpalte = 4; 
            kachelZeile = 3; 
        }

        else if (blockColor == sf::Color(200, 50, 50)) {
            kachelSpalte = 6; 
            kachelZeile = 0;
        }

        int rawWallX = static_cast<int>(hitX + hitY);
        int wallX = (rawWallX % 64 + 64) % 64;

        
        // aus Tileset ausschneiden
        sf::IntRect textureRect(sf::Vector2i(kachelSpalte * 64 + wallX, kachelZeile * 64), sf::Vector2i(1, 64));
        // Sprite erstellen, positionieren und skalieren
        sf::Sprite wallSlice(tileset, textureRect);
        wallSlice.setPosition(sf::Vector2f(static_cast<float>(i) * columnWidth, wallTop));
        wallSlice.setScale(sf::Vector2f(columnWidth / 1.0f, wallHeight / 64.0f));

        float shade = 255.0f * (1.0f - (distanceToWall / maxDepth));
        if (shade < 0) shade = 0; if (shade > 255) shade = 255;

        // schatten-Effekt berechnen 
        float lightIntensity = shade / 255.0f;
        unsigned char intensityByte = static_cast<unsigned char>(255.0f * lightIntensity);
        wallSlice.setColor(sf::Color(intensityByte, intensityByte, intensityByte));

        // Zeichnen
        window.draw(wallSlice);


        if (hitItem) {
            // Entfernungs-Berechnung wie bei Wand, aber mit itemDistance
            float correctedItemDist = itemDistance * std::cos(rayAngleRad - droneYawRad);
            if (correctedItemDist < 1.0f) correctedItemDist = 1.0f;
            
            float itemHeight = (screenHeight * 15.0f) / correctedItemDist;
            if (itemHeight > screenHeight * 2.0f) itemHeight = screenHeight * 2.0f;
            float itemTop = horizon - (itemHeight / 2.0f);

            int rawItemX = static_cast<int>(itemHitX + itemHitY);
            int itemX = (rawItemX % 64 + 64) % 64; 

        
            sf::IntRect itemRect(sf::Vector2i(10 * 64 + itemX, 5 * 64), sf::Vector2i(1, 64));
            sf::Sprite itemSlice(tileset, itemRect);
            
            itemSlice.setPosition(sf::Vector2f(static_cast<float>(i) * columnWidth, itemTop));
            itemSlice.setScale(sf::Vector2f(columnWidth / 1.0f, itemHeight / 64.0f));

            // Schatten auch auf das Item anwenden
            float itemShade = 255.0f * (1.0f - (itemDistance / maxDepth));
            if (itemShade < 0) itemShade = 0; if (itemShade > 255) itemShade = 255;
            unsigned char itemIntensityByte = static_cast<unsigned char>(255.0f * (itemShade / 255.0f));
            itemSlice.setColor(sf::Color(itemIntensityByte, itemIntensityByte, itemIntensityByte));

            // Zeichnet Item über bereits gezeichnete Wand
            window.draw(itemSlice);
        }

        
    }

    window.setView(defaultView);
}

void RaycastEngine::render2DMap(sf::RenderWindow& window, const Drone& drone, const Map& gameMap) {
    const_cast<Map&>(gameMap).draw(window); 

    float scaleFactor = 1.0f + (drone.getAltitude() * 0.005f); 
    if (scaleFactor < 0.5f) scaleFactor = 0.5f;
    if (scaleFactor > 2.5f) scaleFactor = 2.5f;

    sf::Sprite droneSprite(droneTexture);

    sf::FloatRect bounds = droneSprite.getLocalBounds();
    droneSprite.setOrigin(sf::Vector2f(bounds.size.x / 2.0f, bounds.size.y / 2.0f));

    // Position, Rotation und Zoom
    droneSprite.setPosition(drone.getPosition());
    droneSprite.setRotation(sf::degrees(drone.getYaw()));

    float targetWidth = 30.0f;
    float baseScale = targetWidth / bounds.size.x;

    droneSprite.setScale(sf::Vector2f(baseScale * scaleFactor, baseScale * scaleFactor));

    window.draw(droneSprite);
}