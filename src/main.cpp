#include <iostream>
#include <optional>
#include <SFML/Graphics.hpp>
#include "SFML/Audio.hpp"
#include <map>
#include <ranges>
#include <vector> // Ajout nécessaire

import networkelement;
import train;

std::map<GridPos, NetworkElement> networkMap;
constexpr float TILE_SIZE = 64.f;
constexpr float START_X = 50.f;
constexpr float START_Y = 50.f;

enum class EditorMode {
    Play,
    BuildRail,
    BuildSwitch,
    BuildStation,
    Delete,
    PlaceTrain
};

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Train sim");
    window.setFramerateLimit(60);

    sf::Texture railTexture;
    if (!railTexture.loadFromFile("texture/rail.png")) std::cerr << "Error loading rail.png" << std::endl;

    sf::Texture switchTexture;
    if (!switchTexture.loadFromFile("texture/switch.png")) std::cerr << "Error loading switch.png" << std::endl;

    sf::Texture backgroundTexture;
    bool hasBg = backgroundTexture.loadFromFile("texture/grass.jpg");
    if (hasBg) backgroundTexture.setRepeated(true);
    sf::Sprite backgroundSprite(backgroundTexture);

    networkMap.clear();
    constexpr int WIDTH_TILES = 8;
    constexpr int HEIGHT_TILES = 5;

    EditorMode currentMode = EditorMode::Play;
    float buildRotation = 0.f;

    auto updateTitle = [&]() {
        std::string modeStr;
        switch(currentMode) {
            case EditorMode::Play: modeStr = "JEU (Clic: Aiguillage)"; break;
            case EditorMode::BuildRail: modeStr = "EDITEUR: Rail (R: Pivoter)"; break;
            case EditorMode::BuildSwitch: modeStr = "EDITEUR: Aiguillage (R: Pivoter)"; break;
            case EditorMode::BuildStation: modeStr = "EDITEUR: Gare (Clic sur rail)"; break;
            case EditorMode::Delete: modeStr = "EDITEUR: Supprimer"; break;
            case EditorMode::PlaceTrain: modeStr = "TRAIN: Clic G (Ajout) / Clic D (Suppr)"; break;
        }
        window.setTitle("Train Sim - " + modeStr + " [1-6: Modes]");
    };
    updateTitle();

    auto addElement = [&](const ElementType type, const sf::Texture& tex, const float x, const float y, const float angle) {
        int gx = static_cast<int>(std::round((x - START_X) / TILE_SIZE));
        int gy = static_cast<int>(std::round((y - START_Y) / TILE_SIZE));

        NetworkElement el;
        el.type = type;
        el.gridPos = {gx, gy};
        el.setupConnections(angle);

        sf::Sprite s(tex);
        float scale = 1.0f;
        if (tex.getSize().x > 0) scale = TILE_SIZE / static_cast<float>(tex.getSize().x);
        s.setScale({scale, scale});

        sf::FloatRect bounds = s.getLocalBounds();
        s.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
        s.setRotation(sf::degrees(angle));

        s.setPosition({x + TILE_SIZE / 2.f, y + TILE_SIZE / 2.f});
        el.sprite = s;
        networkMap[{gx, gy}] = el;
    };

    for (int i=0; i<WIDTH_TILES; ++i) addElement(ElementType::Rail, railTexture, START_X + (i+1)*TILE_SIZE, START_Y, 0.f);
    addElement(ElementType::Switch, switchTexture, START_X + (WIDTH_TILES+1)*TILE_SIZE, START_Y, 90.f);
    for (int i=0; i<HEIGHT_TILES; ++i) addElement(ElementType::Rail, railTexture, START_X + (WIDTH_TILES+1)*TILE_SIZE, START_Y + (i+1)*TILE_SIZE, 90.f);
    addElement(ElementType::Switch, switchTexture, START_X + (WIDTH_TILES+1)*TILE_SIZE, START_Y + (HEIGHT_TILES+1)*TILE_SIZE, 180.f);
    for (int i=0; i<WIDTH_TILES; ++i) addElement(ElementType::Rail, railTexture, START_X + (WIDTH_TILES-i)*TILE_SIZE, START_Y + (HEIGHT_TILES+1)*TILE_SIZE, 180.f);
    addElement(ElementType::Switch, switchTexture, START_X, START_Y + (HEIGHT_TILES+1)*TILE_SIZE, 270.f);
    for (int i=0; i<HEIGHT_TILES; ++i) addElement(ElementType::Rail, railTexture, START_X, START_Y + (HEIGHT_TILES-i)*TILE_SIZE, 270.f);
    addElement(ElementType::Switch, switchTexture, START_X, START_Y, 0.f);

    for (int i=0; i<WIDTH_TILES; ++i) addElement(ElementType::Rail, railTexture, START_X + (WIDTH_TILES + 1 + i + 1)*TILE_SIZE, START_Y, 0.f);
    addElement(ElementType::Switch, switchTexture, START_X + (WIDTH_TILES*2 + 2)*TILE_SIZE, START_Y, 90.f);
    for (int i=0; i<HEIGHT_TILES; ++i) addElement(ElementType::Rail, railTexture, START_X + (WIDTH_TILES*2 + 2)*TILE_SIZE, START_Y + (i+1)*TILE_SIZE, 90.f);
    addElement(ElementType::Switch, switchTexture, START_X + (WIDTH_TILES*2 + 2)*TILE_SIZE, START_Y + (HEIGHT_TILES+1)*TILE_SIZE, 180.f);
    for (int i=0; i<WIDTH_TILES; ++i) addElement(ElementType::Rail, railTexture, START_X + (WIDTH_TILES*2 + 2 - 1 - i)*TILE_SIZE, START_Y + (HEIGHT_TILES+1)*TILE_SIZE, 180.f);

    sf::RectangleShape trainStation;
    trainStation.setSize({TILE_SIZE, 20.f});
    trainStation.setFillColor(sf::Color(200, 50, 50, 255));
    trainStation.setOrigin({TILE_SIZE / 2.f, 10.f});

    if (networkMap.contains({1, 0})) networkMap[{1, 0}].hasPlatform = true;
    if (networkMap.contains({2, 0})) networkMap[{2, 0}].hasPlatform = true;

    std::vector<Train> trains;
    std::vector<sf::Vector2f> prevPositions;
    std::vector<sf::Vector2f> lastDirections;

    sf::Vector2f startPos = { START_X + 1 * TILE_SIZE + TILE_SIZE/2.f, START_Y + TILE_SIZE/2.f };
    
    trains.emplace_back(startPos, Direction::Right);
    trains.back().maxSpeed = 300.f;
    trains.back().speed = 300.f;
    prevPositions.push_back(startPos);
    lastDirections.push_back({1.f, 0.f});

    sf::View view = window.getDefaultView();
    view.setCenter({START_X + (WIDTH_TILES + 1) * TILE_SIZE, START_Y + (HEIGHT_TILES / 2.f) * TILE_SIZE});
    sf::Clock clock;

    sf::Music music{};
    if (!music.openFromFile("sound/music.ogg")) {
        std::cerr << "Erreur de chargement de fichier audio" << std::endl;
        return -1;
    }
    music.setLooping(true);
    music.play();

    while (window.isOpen())
    {
        float cameraSpeed = 500.f;
        sf::Time dt = clock.restart();
        float dtSeconds = dt.asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                view.setSize({static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
            }
            if (const auto* scrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {
                if (scrolled->wheel == sf::Mouse::Wheel::Vertical) {
                    if (scrolled->delta > 0) view.zoom(0.9f); else view.zoom(1.1f);
                }
            }

            if (const auto* kb = event->getIf<sf::Event::KeyPressed>()) {
                if (kb->code == sf::Keyboard::Key::Num1) { currentMode = EditorMode::Play; updateTitle(); }
                if (kb->code == sf::Keyboard::Key::Num2) { currentMode = EditorMode::BuildRail; updateTitle(); }
                if (kb->code == sf::Keyboard::Key::Num3) { currentMode = EditorMode::BuildSwitch; updateTitle(); }
                if (kb->code == sf::Keyboard::Key::Num4) { currentMode = EditorMode::BuildStation; updateTitle(); }
                if (kb->code == sf::Keyboard::Key::Num5) { currentMode = EditorMode::Delete; updateTitle(); }
                if (kb->code == sf::Keyboard::Key::Num6) { currentMode = EditorMode::PlaceTrain; updateTitle(); }
        
                if (kb->code == sf::Keyboard::Key::R) { 
                    buildRotation += 90.f; 
                    if (buildRotation >= 360.f) buildRotation = 0.f;
                }
            }

            if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left) {
                    sf::Vector2i pixelPos = {mb->position.x, mb->position.y};
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);
                
                    int gx = static_cast<int>(std::round((worldPos.x - START_X - TILE_SIZE/2.f) / TILE_SIZE));
                    int gy = static_cast<int>(std::round((worldPos.y - START_Y - TILE_SIZE/2.f) / TILE_SIZE));

                    float snapedX = START_X + gx * TILE_SIZE;
                    float snapedY = START_Y + gy * TILE_SIZE;

                    if (currentMode == EditorMode::Play) {
                        if (networkMap.contains({gx, gy})) {
                            NetworkElement& el = networkMap[{gx, gy}];
                            if (el.type == ElementType::Switch) {
                                el.rotateSwitch();
                            }
                        }
                    }
                    else if (currentMode == EditorMode::BuildRail) {
                        addElement(ElementType::Rail, railTexture, snapedX, snapedY, buildRotation);
                    }
                    else if (currentMode == EditorMode::BuildSwitch) {
                        addElement(ElementType::Switch, switchTexture, snapedX, snapedY, buildRotation);
                    }
                    else if (currentMode == EditorMode::BuildStation) {
                        if (networkMap.contains({gx, gy})) {
                            networkMap[{gx, gy}].hasPlatform = !networkMap[{gx, gy}].hasPlatform;
                        }
                    }
                    else if (currentMode == EditorMode::Delete) {
                        networkMap.erase({gx, gy});
                    }
                    else if (currentMode == EditorMode::PlaceTrain) {
                         if (networkMap.contains({gx, gy})) {
                             sf::Vector2f pos = { snapedX + TILE_SIZE/2.f, snapedY + TILE_SIZE/2.f };
                             trains.emplace_back(pos, Direction::Right);
                             trains.back().maxSpeed = 300.f;
                             trains.back().speed = 300.f;
                             prevPositions.push_back(pos);
                             lastDirections.push_back({1.f, 0.f});
                        }
                    }
                }
                else if (mb->button == sf::Mouse::Button::Right) {
                    if (currentMode == EditorMode::PlaceTrain) {
                        sf::Vector2i pixelPos = {mb->position.x, mb->position.y};
                        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);
                            
                        for (size_t i = 0; i < trains.size(); ) {
                            float d2 = (trains[i].position.x - worldPos.x)*(trains[i].position.x - worldPos.x) + 
                                       (trains[i].position.y - worldPos.y)*(trains[i].position.y - worldPos.y);
                            if (d2 < 50.f * 50.f) {
                                trains.erase(trains.begin() + i);
                                prevPositions.erase(prevPositions.begin() + i);
                                lastDirections.erase(lastDirections.begin() + i);
                            } else {
                                ++i;
                            }
                        }
                    }
                }
            }
        }

        sf::Vector2f moveAmount{0.f, 0.f};
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) moveAmount.y -= cameraSpeed * dtSeconds;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) moveAmount.y += cameraSpeed * dtSeconds;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) moveAmount.x -= cameraSpeed * dtSeconds;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) moveAmount.x += cameraSpeed * dtSeconds;
        view.move(moveAmount);

        if (hasBg) {
            sf::Vector2f vC = view.getCenter(); sf::Vector2f vS = view.getSize(); sf::Vector2f vTL = vC - (vS / 2.f);
            backgroundSprite.setPosition(vTL);
            backgroundSprite.setTextureRect({{static_cast<int>(vTL.x), static_cast<int>(vTL.y)}, {static_cast<int>(vS.x), static_cast<int>(vS.y)}});
        }

        for (size_t i = 0; i < trains.size(); ++i) {
            float acceleration = 200.f * dtSeconds;
            float brakingDecel = 800.f * dtSeconds; 

            float finalTargetSpeed = trains[i].maxSpeed;
            bool hardStopRequested = false;

            sf::Vector2f currentMove = trains[i].position - prevPositions[i];
            float moveLen = std::sqrt(currentMove.x*currentMove.x + currentMove.y*currentMove.y);
            if (moveLen > 0.001f) {
                lastDirections[i] = currentMove / moveLen;
            }
            sf::Vector2f myDir = lastDirections[i];

            for (size_t j = 0; j < trains.size(); ++j) {
                if (i == j) continue;

                sf::Vector2f diff = trains[j].position - trains[i].position;
                float distSq = diff.x*diff.x + diff.y*diff.y;
                float dist = std::sqrt(distSq);
                    
                float stopDist = 65.f;
                float slowDist = 150.f;

                if (dist < slowDist) {
                    float dot = diff.x * myDir.x + diff.y * myDir.y;

                    if (dot > 0) {
                        float limitSpeed = trains[i].maxSpeed;

                            if (dist < stopDist) {
                                limitSpeed = 0.f;
                                hardStopRequested = true;
                        } else {
                            float factor = (dist - stopDist) / (slowDist - stopDist);
                            limitSpeed = trains[i].maxSpeed * factor * 0.6f;
                        }

                        if (limitSpeed < finalTargetSpeed) {
                            finalTargetSpeed = limitSpeed;
                        }
                    }
                }
            }

            if (hardStopRequested) {
                trains[i].speed = 0.f;
            } else {
                if (trains[i].speed < finalTargetSpeed) {
                    trains[i].speed += acceleration;
                    if (trains[i].speed > finalTargetSpeed) trains[i].speed = finalTargetSpeed;
                } else if (trains[i].speed > finalTargetSpeed) {
                    trains[i].speed -= brakingDecel;
                    if (trains[i].speed < finalTargetSpeed) trains[i].speed = finalTargetSpeed;
                }
            }

            prevPositions[i] = trains[i].position;
            trains[i].update(dtSeconds, networkMap, START_X, START_Y, TILE_SIZE);
        }

        window.clear(sf::Color::Black);
        window.setView(view);

        if (hasBg) window.draw(backgroundSprite);
    
        for (auto &val: networkMap | std::views::values) {
            val.draw(window);
            if (val.hasPlatform && val.sprite.has_value()) {
                trainStation.setPosition(val.sprite->getPosition());
                trainStation.setRotation(val.sprite->getRotation());
                trainStation.move(sf::Transform().rotate(val.sprite->getRotation()).transformPoint({0.f, -32.f}));
                window.draw(trainStation);
            }
        }

        for (auto& train : trains) {
            train.draw(window);
        }

        window.display();
    }
    return 0;
}