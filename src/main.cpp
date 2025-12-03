#include <iostream>
#include <optional>
#include <SFML/Graphics.hpp>
#include <map>
#include <ranges>

import networkelement;
import train;

std::map<GridPos, NetworkElement> networkMap;
constexpr float TILE_SIZE = 64.f;
constexpr float START_X = 50.f;
constexpr float START_Y = 50.f;

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

    sf::RectangleShape stationShape;
    stationShape.setSize({128.f, 64.f}); 
    stationShape.setFillColor(sf::Color(200, 50, 50, 150)); 
    stationShape.setPosition({START_X + 1 * TILE_SIZE, START_Y - 64.f}); 

    if (networkMap.contains({1, 0})) networkMap[{1, 0}].hasPlatform = true;
    if (networkMap.contains({2, 0})) networkMap[{2, 0}].hasPlatform = true;

    sf::Vector2f startPos = { START_X + 1 * TILE_SIZE + TILE_SIZE/2.f, START_Y + TILE_SIZE/2.f };
    Train myTrain(startPos, Direction::Right);
    myTrain.maxSpeed = 300.f;
    myTrain.speed = 300.f;

    sf::View view = window.getDefaultView();
    view.setCenter({START_X + (WIDTH_TILES + 1) * TILE_SIZE, START_Y + (HEIGHT_TILES / 2.f) * TILE_SIZE});
    sf::Clock clock; 

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
            // Clic
            if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left) {
                    sf::Vector2i pixelPos = {mb->position.x, mb->position.y};
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);
                    
                    int gx = static_cast<int>(std::round((worldPos.x - START_X - TILE_SIZE/2.f) / TILE_SIZE));
                    int gy = static_cast<int>(std::round((worldPos.y - START_Y - TILE_SIZE/2.f) / TILE_SIZE));
                    
                    if (networkMap.contains({gx, gy})) {
                        NetworkElement& el = networkMap[{gx, gy}];
                        if (el.type == ElementType::Switch) {
                            el.rotateSwitch();
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

        myTrain.update(dtSeconds, networkMap, START_X, START_Y, TILE_SIZE);

        window.clear(sf::Color::Black);
        window.setView(view);
        
        if (hasBg) window.draw(backgroundSprite);
        
        for (auto &val: networkMap | std::views::values) val.draw(window);
        window.draw(stationShape);
        myTrain.draw(window);

        window.display();
    }
    return 0;
}