module;
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>
#include <cmath>

export module networkelement;

export using GridPos = std::pair<int, int>;

export enum class ElementType { Rail, Switch, Station };
export enum class Direction { None, Right, Down, Left, Up };

export struct NetworkElement {
    ElementType type;
    std::optional<sf::Sprite> sprite;
    sf::RectangleShape shape;
    GridPos gridPos;
    std::vector<Direction> connections;
    float currentAngle = 0.f;
    bool hasPlatform = false;

    bool signalGreen = true;
    bool autoBlocked = false;

    NetworkElement() : type(ElementType::Rail) {}
    NetworkElement(std::vector<Direction> dirs, bool platform = false)
        : connections(dirs), hasPlatform(platform) {}

    void setupConnections(float angleDeg) {
        connections.clear();
        int angle = static_cast<int>(std::round(angleDeg));
        while(angle < 0) angle += 360;
        angle %= 360;

        currentAngle = static_cast<float>(angle);

        if (type == ElementType::Rail || type == ElementType::Station) {
            if (angle == 0 || angle == 180) connections = {Direction::Right, Direction::Left};
            else connections = {Direction::Down, Direction::Up};
        } else if (type == ElementType::Switch) {
            if (angle == 0) connections = {Direction::Right, Direction::Down};
            else if (angle == 90) connections = {Direction::Left, Direction::Down};
            else if (angle == 180) connections = {Direction::Left, Direction::Up};
            else if (angle == 270) connections = {Direction::Right, Direction::Up};
        }
    }

    void rotateSwitch() {
        currentAngle += 90.f;
        setupConnections(currentAngle);
        if (sprite) {
            sprite->setRotation(sf::degrees(currentAngle));
        }
    }

    void draw(sf::RenderWindow& window) {
        if (sprite) {
            window.draw(*sprite);

            if (type == ElementType::Switch) {
                sf::Vector2f center = sprite->getPosition();
                float radius = 32.f;

                for (int i = 0; i < 4; ++i) {
                    int out = static_cast<int>(connections[i]);

                    if (out >= 0 && out < 4 && i < out) {

                        sf::Vector2f p1, p2;

                        auto getOffset = [&](int dir) -> sf::Vector2f {
                            if (dir == 0) return {0.f, -radius};
                            if (dir == 1) return {radius, 0.f};
                            if (dir == 2) return {0.f, radius};
                            if (dir == 3) return {-radius, 0.f};
                            return {0.f, 0.f};
                        };

                        p1 = center + getOffset(i);
                        p2 = center + getOffset(out);
                        sf::Color pathColor = sf::Color::Yellow;

                        sf::Vertex line[] = {
                            sf::Vertex(p1, pathColor),
                            sf::Vertex(p2, pathColor)
                        };
                        window.draw(line, 2, sf::PrimitiveType::Lines);

                        sf::CircleShape pivot(6.f);
                        pivot.setOrigin({6.f, 6.f});
                        pivot.setPosition((p1 + p2) / 2.f); // Au milieu du segment
                        pivot.setFillColor(pathColor);
                        window.draw(pivot);
                    }
                }

                sf::CircleShape signalLight(5.f);
                signalLight.setOrigin({5.f, 5.f});
                signalLight.setPosition(center + sf::Vector2f(-15.f, -15.f));
                bool finalGreen = signalGreen && !autoBlocked;
                signalLight.setFillColor(finalGreen ? sf::Color::Green : sf::Color::Red);
                
                signalLight.setOutlineThickness(1.f);
                signalLight.setOutlineColor(sf::Color::Black);
                window.draw(signalLight);
            }
        }
    }
};