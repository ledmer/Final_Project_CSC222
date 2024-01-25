#include <SFML/Graphics.hpp>
#include <math.h>
class Player {
public:
    sf::RectangleShape shape;
    sf::Vector2f velocity;

    Player(float x, float y) {
        shape.setSize(sf::Vector2f(50, 50));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Green);
    }

    void update() {
        shape.move(velocity);
    }
};

class Enemy {
public:
    sf::RectangleShape shape;
    float speed;

    Enemy(float x, float y, float spd) : speed(spd) {
        shape.setSize(sf::Vector2f(50, 50));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Red);
    }

    void update(const sf::Vector2f& playerPos) {
        // Move towards the player
        sf::Vector2f direction = playerPos - shape.getPosition();
        direction = sf::Vector2f(direction.x / std::sqrt(direction.x * direction.x + direction.y * direction.y),
                                 direction.y / std::sqrt(direction.x * direction.x + direction.y * direction.y));
        shape.move(direction * speed);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Game");

    Player player(100, 100);
    Enemy enemy(700, 500, .5f);

    Enemy enemy2(700, 500, 0.7f);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
            player.velocity.x = -1.0f;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
            player.velocity.x = 1.0f;
        } else {
            player.velocity.x = 0.0f;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
            player.velocity.y = -1.0f;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
            player.velocity.y = 1.0f;
        } else {
            player.velocity.y = 0.0f;
        }

        player.update();
        enemy.update(player.shape.getPosition());
        enemy2.update(player.shape.getPosition());
        window.clear();
        window.draw(player.shape);
        window.draw(enemy.shape);
        window.draw(enemy2.shape);

        window.display();
    }

    return 0;
}
