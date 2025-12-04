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

    NetworkElement() : type(ElementType::Rail) {}

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
        if (type == ElementType::Station) window.draw(shape);
        else if (sprite.has_value()) window.draw(*sprite);
    }

    void draw(sf::RenderWindow& window) const {
        if (sprite.has_value()) {
            window.draw(*sprite);
        }
    }
};