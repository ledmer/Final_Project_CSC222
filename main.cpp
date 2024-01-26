#include <SFML/Graphics.hpp>
#include <math.h>
#include <SFML/Window.hpp>
#include <vector>
#include <cstdlib> // for std::rand
#include <ctime>   // for std::time

class Bullet {
public:
    sf::RectangleShape shape;
    sf::Vector2f velocity;

    Bullet(float x, float y, float speed) {
        shape.setSize(sf::Vector2f(10, 10));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Yellow);
        velocity = sf::Vector2f(0.0f, 0.0f); // Bullets initially stationary
        speed_ = speed;
    }

    void update() {
        shape.move(velocity);
    }

    void setDirection(sf::Vector2f direction) {
        velocity = direction * speed_;
    }

private:
    float speed_;
};

class Player {
public:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    std::vector<Bullet> bullets;
    sf::Clock shootTimer;

    Player(float x, float y) {
        shape.setSize(sf::Vector2f(50, 50));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Green);
    }

    void update() {
        shape.move(velocity);

        // Update bullets
        for (auto& bullet : bullets) {
            bullet.update();
        }
    }

    void shoot(sf::Keyboard::Key directionKey) {
        if (shootTimer.getElapsedTime().asSeconds() >= 1.0) {
            sf::Vector2f direction(0.0f, 0.0f);

            switch (directionKey) {
                case sf::Keyboard::W:
                    direction.y = -1.0f;
                    break;
                case sf::Keyboard::S:
                    direction.y = 1.0f;
                    break;
                case sf::Keyboard::A:
                    direction.x = -1.0f;
                    break;
                case sf::Keyboard::D:
                    direction.x = 1.0f;
                    break;
                default:
                    break;
            }

            bullets.push_back(Bullet(shape.getPosition().x + shape.getSize().x / 2, shape.getPosition().y + shape.getSize().y / 2, 5.0f));
            bullets.back().setDirection(direction);

            shootTimer.restart();
        }
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

    bool checkCollision(const Player& player) {
        return shape.getGlobalBounds().intersects(player.shape.getGlobalBounds());
    }
};

class Game {
public:
    sf::RenderWindow window;
    Player player;
    std::vector<Enemy> enemies;
    bool gameRunning;
    bool startScreen;
    sf::Clock enemySpawnTimer;

    Game() : window(sf::VideoMode(800, 600), "SFML Game"), player(100, 100),
             gameRunning(false), startScreen(true) {
        // Seed the random number generator
        std::srand(std::time(0));
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();

            window.clear();
            draw();
            window.display();
        }
    }

private:
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                sf::Time t = sf::seconds(1);

                if (startScreen) {
                    startScreen = false;
                    gameRunning = true;
                } else if (!gameRunning) { // Loss screen
                    sf::sleep(t);
                    if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    } else {
                        // Only transition to start screen if any key is pressed
                        startScreen = true;
                        gameRunning = false;
                        player = Player(100, 100);
                        enemies.clear();
                        enemySpawnTimer.restart();
                    }
                    sf::sleep(t);
                } else if (gameRunning) {
                    // Check shooting keys
                    if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S ||
                        event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::D) {
                        player.shoot(event.key.code);
                    }
                }
            }
        }

        if (gameRunning) {
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

            // Check and spawn a new enemy every second
            if (enemySpawnTimer.getElapsedTime().asSeconds() >= 1.0) {
                spawnEnemy();
                enemySpawnTimer.restart();
            }

            // Update all enemies
            for (auto& enemy : enemies) {
                enemy.update(player.shape.getPosition());
                if (enemy.checkCollision(player)) {
                    // Perform action on collision (e.g., end the game)
                    gameRunning = false;
                }
            }

            // Check bullet-enemy collision
            for (auto& bullet : player.bullets) {
                for (auto it = enemies.begin(); it != enemies.end();) {
                    if (bullet.shape.getGlobalBounds().intersects(it->shape.getGlobalBounds())) {
                        it = enemies.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }
    }

    void update() {
        if (gameRunning) {
            // Additional game-specific updates can be added here
        }
    }

    void draw() {
        if (startScreen) {
            // Draw start screen with default font
            sf::Font font;
            if (!font.loadFromFile("universal-serif.ttf")) {
                // error...
            }
            sf::Text text("Press any key to start", font, 30);
            text.setFillColor(sf::Color::Red);

            text.setPosition(250, 300);
            window.draw(text);
        } else if (gameRunning) {
            // Draw game elements
            window.draw(player.shape);
            for (const auto& enemy : enemies) {
                window.draw(enemy.shape);
            }
            for (const auto& bullet : player.bullets) {
                window.draw(bullet.shape);
            }
        } else if (!gameRunning) {
            sf::Font font;
            if (!font.loadFromFile("universal-serif.ttf")) {
                // error...
            }
            sf::Text text("You lost press esc to exit, any key to try again", font, 30);
            text.setFillColor(sf::Color::Red);
            text.setPosition(250, 300);
            window.draw(text);
        }
    }

    void spawnEnemy() {
        constexpr int maxAttempts = 10; // Maximum attempts to find a suitable position

        for (int attempt = 0; attempt < maxAttempts; ++attempt) {
            // Generate random positions for enemies
            float x = static_cast<float>(std::rand() % 800); // Random x position between 0 and 800
            float y = static_cast<float>(std::rand() % 600); // Random y position between 0 and 600

            // Check if the position is far enough from the player
            if (std::abs(x - player.shape.getPosition().x) >= 100 && y >= player.shape.getPosition().y + 100) {
                enemies.push_back(Enemy(x, y, 0.5f));
                return; // Successful spawn, exit the loop
            }
        }

        // If no suitable position is found after maxAttempts, use a simpler strategy
        float x = static_cast<float>(std::rand() % 800);
        float y = static_cast<float>(std::rand() % 600);
        enemies.push_back(Enemy(x, y, 0.5f));
    }
};

int main() {
    Game game;
    game.run();

    return 0;
}
