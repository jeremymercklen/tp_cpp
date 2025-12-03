module;
#include <SFML/Graphics.hpp>
#include <cmath>
#include <map>
#include <vector>

export module train;

// Import du module contenant Direction et GridPos
import networkelement;

export struct Train {
    sf::Vector2f position;
    float speed;
    float maxSpeed; 
    Direction currentDir;
    sf::RectangleShape shape;
    sf::Vector2f targetPos;
    bool moving = false;
    
    float stopTimer = 0.f; 
    bool isStopped = false;

    Train(sf::Vector2f startPixel, Direction startDir) 
        : position(startPixel), speed(0.f), maxSpeed(300.f), currentDir(startDir) 
    {
        shape.setSize({40.f, 20.f});
        shape.setFillColor(sf::Color::Blue);
        shape.setOrigin({20.f, 10.f});
        shape.setPosition(position);
        targetPos = position;
    }

    // On passe les infos nécessaires pour naviguer : 
    // - map des tiles (abstraite ou template)
    // - constantes de grille
    template<typename NetworkMap>
    void update(float dt, const NetworkMap& network, float startX, float startY, float tileSize) {
        if (isStopped) {
            stopTimer -= dt;
            if (stopTimer <= 0.f) {
                isStopped = false;
                speed = maxSpeed;
            } else {
                return;
            }
        }

        if (speed <= 0 && !isStopped) return; 

        if (!moving) {
            findNextTile(network, startX, startY, tileSize);
        }

        if (moving) {
            sf::Vector2f diff = targetPos - position;
            float dist = std::sqrt(diff.x*diff.x + diff.y*diff.y);
            
            if (dist < speed * dt) {
                position = targetPos;
                moving = false;
                checkStationStop(network, startX, startY, tileSize);
            } else {
                sf::Vector2f dir = diff / dist;
                position += dir * speed * dt;
                float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;
                shape.setRotation(sf::degrees(angle));
            }
            shape.setPosition(position);
        }
    }

    template<typename NetworkMap>
    void findNextTile(const NetworkMap& network, float startX, float startY, float tileSize) {
        int gx = static_cast<int>(std::round((position.x - startX - tileSize/2.f) / tileSize));
        int gy = static_cast<int>(std::round((position.y - startY - tileSize/2.f) / tileSize));
        
        // On suppose que NetworkMap est std::map<pair<int,int>, NetworkElement>
        // et que NetworkElement a un membre 'connections' compatible avec Direction
        if (network.count({gx, gy})) {
            const auto& currentTile = network.at({gx, gy});
            
            Direction from = Direction::None;
            if (currentDir == Direction::Right) from = Direction::Left;
            else if (currentDir == Direction::Left) from = Direction::Right;
            else if (currentDir == Direction::Up) from = Direction::Down;
            else if (currentDir == Direction::Down) from = Direction::Up;

            for (auto d : currentTile.connections) {
                // Conversion si nécessaire, ici on suppose que l'enum est le même
                // ou que d est castable.
                // Comme Direction est définie ici, il faut que NetworkElement utilise CE Direction ou un compatible.
                if (d != from) {
                    currentDir = d;
                    int nextGx = gx;
                    int nextGy = gy;
                    if (d == Direction::Right) nextGx++;
                    else if (d == Direction::Left) nextGx--;
                    else if (d == Direction::Down) nextGy++;
                    else if (d == Direction::Up) nextGy--;

                    targetPos = {
                        startX + nextGx * tileSize + tileSize/2.f, 
                        startY + nextGy * tileSize + tileSize/2.f
                    };
                    moving = true;
                    return;
                }
            }
        }
        speed = 0; 
    }

    template<typename NetworkMap>
    void checkStationStop(const NetworkMap& network, float startX, float startY, float tileSize) {
        int gx = static_cast<int>(std::round((position.x - startX - tileSize/2.f) / tileSize));
        int gy = static_cast<int>(std::round((position.y - startY - tileSize/2.f) / tileSize));

        if (network.count({gx, gy})) {
            if (network.at({gx, gy}).hasPlatform) {
                isStopped = true;
                stopTimer = 2.0f; 
                speed = 0.f;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};