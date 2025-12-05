module;
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <map>
#include <vector>
#include <set>
#include <functional>

export module train;

import networkelement;

sf::SoundBuffer& getTrainSoundBuffer() {
    static sf::SoundBuffer buffer;
    static bool loaded = false;
    if (!loaded) {
        if (buffer.loadFromFile("sound/tchoo.wav")) {
            loaded = true;
        }
    }
    return buffer;
}

export struct Train {
    sf::Vector2f position;
    float speed;
    float maxSpeed;
    Direction currentDir;
    sf::RectangleShape shape;
    sf::Vector2f targetPos;
    bool moving = false;

    sf::Sound sound;

    float stopTimer = 0.f;
    bool isStopped = false;

    Train(sf::Vector2f startPixel, Direction startDir)
        : position(startPixel), speed(0.f), maxSpeed(300.f), currentDir(startDir), sound(getTrainSoundBuffer())
    {
        sound.setRelativeToListener(true);
        shape.setSize({40.f, 20.f});
        shape.setFillColor(sf::Color::Blue);
        shape.setOrigin({20.f, 10.f});
        shape.setPosition(position);
        targetPos = position;
    }

    template<typename NetworkMap>
    void update(float dt, const NetworkMap& network, const std::vector<Train>& allTrains, float startX, float startY, float tileSize) {
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
            findNextTile(network, allTrains, startX, startY, tileSize);
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
    void findNextTile(const NetworkMap& network, const std::vector<Train>& allTrains, float startX, float startY, float tileSize) {
        int gx = static_cast<int>(std::round((position.x - startX - tileSize/2.f) / tileSize));
        int gy = static_cast<int>(std::round((position.y - startY - tileSize/2.f) / tileSize));

        if (network.count({gx, gy})) {
            const auto& currentTile = network.at({gx, gy});
            bool amIOnSwitch = (currentTile.type == ElementType::Switch);

            Direction from = Direction::None;
            if (currentDir == Direction::Right) from = Direction::Left;
            else if (currentDir == Direction::Left) from = Direction::Right;
            else if (currentDir == Direction::Up) from = Direction::Down;
            else if (currentDir == Direction::Down) from = Direction::Up;

            for (auto d : currentTile.connections) {
                if (d != from) {
                    int nextGx = gx;
                    int nextGy = gy;
                    if (d == Direction::Right) nextGx++;
                    else if (d == Direction::Left) nextGx--;
                    else if (d == Direction::Down) nextGy++;
                    else if (d == Direction::Up) nextGy--;

                    if (network.count({nextGx, nextGy})) {
                        const auto& nextTile = network.at({nextGx, nextGy});
                        
                        if (nextTile.type == ElementType::Switch && !nextTile.signalGreen) {
                            if (!amIOnSwitch) {
                                speed = 0;
                                return;
                            }
                        }

                        bool pathBlocked = false; 
                        bool blockedByPriority = false;

                        // 1. SECURITE PHYSIQUE
                        sf::FloatRect myNextBounds = shape.getGlobalBounds();
                        sf::Vector2f moveDir = {
                             (d == Direction::Right ? 1.f : (d == Direction::Left ? -1.f : 0.f)),
                             (d == Direction::Down ? 1.f : (d == Direction::Up ? -1.f : 0.f))
                        };
                        if (tileSize > 0) {
                             sf::Vector2f offset = moveDir * (tileSize * 0.9f); 
                             myNextBounds.position.x += offset.x;
                             myNextBounds.position.y += offset.y;
                        }
                        for(const auto& otherTrain : allTrains) {
                            if (&otherTrain == this) continue;
                            sf::FloatRect otherBounds = otherTrain.shape.getGlobalBounds();
                            bool collision = 
                                myNextBounds.position.x < otherBounds.position.x + otherBounds.size.x &&
                                myNextBounds.position.x + myNextBounds.size.x > otherBounds.position.x &&
                                myNextBounds.position.y < otherBounds.position.y + otherBounds.size.y &&
                                myNextBounds.position.y + myNextBounds.size.y > otherBounds.position.y;
                            if (collision) {
                                pathBlocked = true;
                                break;
                            }
                        }
                        if (pathBlocked) {
                            speed = 0;
                            shape.setFillColor(sf::Color::Magenta);
                            return;
                        }

                        // 2. SCAN SEGMENT
                        int simGx = nextGx;
                        int simGy = nextGy;
                        Direction simDir = d; 
                        
                        for(int k=0; k<1000; ++k) {
                            for(const auto& otherTrain : allTrains) {
                                if (&otherTrain == this) continue; 
                                int ogx = static_cast<int>(std::round((otherTrain.position.x - startX - tileSize/2.f) / tileSize));
                                int ogy = static_cast<int>(std::round((otherTrain.position.y - startY - tileSize/2.f) / tileSize));
                                
                                if (ogx == simGx && ogy == simGy) {
                                    const auto& obstacleTile = network.at({ogx, ogy});
                                    if (k == 0) pathBlocked = true; 
                                    else if (obstacleTile.type == ElementType::Rail) pathBlocked = true; 
                                    else if (obstacleTile.type == ElementType::Switch) {
                                        pathBlocked = true;
                                    }
                                    goto end_train_check;
                                }
                            }
                            
                            if (!network.count({simGx, simGy})) break; 
                            const auto& simTile = network.at({simGx, simGy});

                            if (simTile.type == ElementType::Switch && k > 0) {
                                bool threat = false;

                                for (auto convDir : simTile.connections) {

                                    bool isArrivalBranch = false;
                                    if (simDir == Direction::Right && convDir == Direction::Left) isArrivalBranch = true;
                                    else if (simDir == Direction::Left && convDir == Direction::Right) isArrivalBranch = true;
                                    else if (simDir == Direction::Up && convDir == Direction::Down) isArrivalBranch = true;
                                    else if (simDir == Direction::Down && convDir == Direction::Up) isArrivalBranch = true;
                                    
                                    if (isArrivalBranch) continue;

                                    for (int dist = 1; dist <= 5; ++dist) {
                                        int cx = simGx; 
                                        int cy = simGy;

                                        if (convDir == Direction::Right) cx += dist;
                                        else if (convDir == Direction::Left) cx -= dist;
                                        else if (convDir == Direction::Down) cy += dist;
                                        else if (convDir == Direction::Up) cy -= dist;

                                        if (!network.count({cx, cy})) break; 

                                        if (network.at({cx, cy}).type == ElementType::Switch) break;

                                        for(const auto& t : allTrains) {
                                            if (&t == this) continue;
                                            int tx = static_cast<int>(std::round((t.position.x - startX - tileSize/2.f) / tileSize));
                                            int ty = static_cast<int>(std::round((t.position.y - startY - tileSize/2.f) / tileSize));
                                            
                                            if (tx == cx && ty == cy) {
                                                bool comingTowards = false;
                                                if (convDir == Direction::Right && t.currentDir == Direction::Left) comingTowards = true;
                                                else if (convDir == Direction::Left && t.currentDir == Direction::Right) comingTowards = true;
                                                else if (convDir == Direction::Down && t.currentDir == Direction::Up) comingTowards = true;
                                                else if (convDir == Direction::Up && t.currentDir == Direction::Down) comingTowards = true;

                                                if (comingTowards || t.speed < 0.1f) {
                                                    if (&t < this) {
                                                        threat = true;
                                                        goto end_convergence_scan;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                end_convergence_scan:;

                                if (threat) {
                                    if (!amIOnSwitch) {
                                         if (k < 3) { 
                                             pathBlocked = true;
                                             blockedByPriority = true;
                                             goto end_train_check;
                                         }
                                    }
                                }
                                break;
                            }
                            
                            Direction arriveFrom = Direction::None;
                            if (simDir == Direction::Right) arriveFrom = Direction::Left;
                            else if (simDir == Direction::Left) arriveFrom = Direction::Right;
                            else if (simDir == Direction::Up) arriveFrom = Direction::Down;
                            else if (simDir == Direction::Down) arriveFrom = Direction::Up;

                            bool wayFound = false;
                            for(auto sd : simTile.connections) {
                                if (sd != arriveFrom) {
                                    simDir = sd; wayFound = true; break; 
                                }
                            }
                            if(!wayFound) break; 

                            if (simDir == Direction::Right) simGx++;
                            else if (simDir == Direction::Left) simGx--;
                            else if (simDir == Direction::Down) simGy++;
                            else if (simDir == Direction::Up) simGy--;
                        }
                        end_train_check:;

                        if (pathBlocked && amIOnSwitch) {
                            pathBlocked = false;
                            shape.setFillColor(sf::Color::Green); 
                        }

                        if (pathBlocked) {
                            speed = 0;
                            if (blockedByPriority) shape.setFillColor(sf::Color::Yellow);
                            else shape.setFillColor(sf::Color::Red);
                            return;
                        } else {
                            shape.setFillColor(sf::Color::Blue);
                        }

                        currentDir = d;
                        targetPos = {
                            startX + nextGx * tileSize + tileSize/2.f,
                            startY + nextGy * tileSize + tileSize/2.f
                        };
                        moving = true;
                        return;
                    }
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
                sound.play();
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};