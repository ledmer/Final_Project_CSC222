#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

class Weapon {
public:
    float bulletSpeed;
    Weapon(float speed) : bulletSpeed(speed) {}
};

class Bullet {
public:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    const Weapon& weapon;

    Bullet(float x, float y, const Weapon& w) : weapon(w) {
        shape.setSize(sf::Vector2f(10, 10));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Yellow);
        velocity = sf::Vector2f(0.0f, 0.0f);
    }

    void update() {
        shape.move(velocity);
    }

    void setDirection(sf::Vector2f direction) {
        velocity = direction * weapon.bulletSpeed;
    }
};

class Player {
public:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    std::vector<Bullet> bullets;
    sf::Clock shootTimer;

    std::vector<Weapon> weapons;
    int currentWeaponIndex;

    Player(float x, float y) : currentWeaponIndex(0) {
        shape.setSize(sf::Vector2f(50, 50));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Green);

        weapons.push_back(Weapon(5.0f * 1.5f));  // Weapon 1 with 1.5 times speed
    }

    void update() {
        shape.move(velocity * 1.5f);

        sf::Vector2f newPosition = shape.getPosition();
        if (newPosition.x < 0) newPosition.x = 0;
        if (newPosition.x > 800 - shape.getSize().x) newPosition.x = 800 - shape.getSize().x;
        if (newPosition.y < 0) newPosition.y = 0;
        if (newPosition.y > 600 - shape.getSize().y) newPosition.y = 600 - shape.getSize().y;

        shape.setPosition(newPosition);

        for (auto& bullet : bullets) {
            bullet.update();
        }
    }

    void shoot(sf::Keyboard::Key directionKey) {
        if (shootTimer.getElapsedTime().asSeconds() >= 1.0) {
            sf::Vector2f direction(0.0f, 0.0f);

            switch (directionKey) {
                case sf::Keyboard::W: direction.y = -1.0f; break;
                case sf::Keyboard::S: direction.y = 1.0f; break;
                case sf::Keyboard::A: direction.x = -1.0f; break;
                case sf::Keyboard::D: direction.x = 1.0f; break;
            }

            if (currentWeaponIndex == 0) {
                for (int i = 0; i < 3; ++i) {
                    float spreadAngle = ((std::rand() % 5) - 2) * 5.0f;
                    sf::Vector2f spreadDirection = rotateVector(direction, spreadAngle);

                    bullets.push_back(Bullet(shape.getPosition().x + shape.getSize().x / 2,
                                             shape.getPosition().y + shape.getSize().y / 2,
                                             weapons[currentWeaponIndex]));
                    bullets.back().setDirection(spreadDirection);
                }
            } else {
                bullets.push_back(Bullet(shape.getPosition().x + shape.getSize().x / 2,
                                         shape.getPosition().y + shape.getSize().y / 2,
                                         weapons[currentWeaponIndex]));
                bullets.back().setDirection(direction);
            }

            shootTimer.restart();
        }
    }

private:
    sf::Vector2f rotateVector(const sf::Vector2f& vector, float degrees) {
        float radians = degrees * (3.14159265f / 180.0f);
        float cosine = std::cos(radians);
        float sine = std::sin(radians);
        return sf::Vector2f(vector.x * cosine - vector.y * sine, vector.x * sine + vector.y * cosine);
    }
};

class Enemy {
public:
    sf::RectangleShape shape;
    float speed;

    Enemy(float x, float y, float spd) : speed(spd * 1.5f) {  // Enemy speed multiplied by 1.5
        shape.setSize(sf::Vector2f(50, 50));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Red);
    }

    void update(const sf::Vector2f& playerPos) {
        sf::Vector2f direction = playerPos - shape.getPosition();
        direction /= std::sqrt(direction.x * direction.x + direction.y * direction.y);
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
    int botsKilled = 0;
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                sf::Time t = sf::seconds(1);

                if (startScreen) {
                    if (event.key.code == sf::Keyboard::Num1) {
                        player.currentWeaponIndex = 0;
                        startScreen = false;
                        gameRunning = true;
                    }
                } else if (!gameRunning) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    } else if (event.key.code == sf::Keyboard::Enter) {
                        startScreen = true;
                        gameRunning = false;
                        player = Player(100, 100);
                        enemies.clear();
                        enemySpawnTimer.restart();
                    }
                } else if (gameRunning) {
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

            if (enemySpawnTimer.getElapsedTime().asSeconds() >= 1.0) {
                spawnEnemy();
                enemySpawnTimer.restart();
            }

            for (auto& enemy : enemies) {
                enemy.update(player.shape.getPosition());
                if (enemy.checkCollision(player)) {
                    gameRunning = false;
                }
            }

            for (auto& bullet : player.bullets) {
                for (auto it = enemies.begin(); it != enemies.end();) {
                    if (bullet.shape.getGlobalBounds().intersects(it->shape.getGlobalBounds())) {
                        it = enemies.erase(it);
                        botsKilled++;

                    } else {
                        ++it;
                    }
                }
            }
        }
    }

    void update() {
        // Additional game-specific updates can be added here
    }

    void draw() {
        if (startScreen) {
            sf::Font font;

            if (font.loadFromFile("universal-serif.ttf")) {
                sf::Text text("Press 1 to start", font, 30);
                text.setFillColor(sf::Color::Green);
                text.setPosition(250, 300);
                window.draw(text);
            }
        } else if (gameRunning) {
            window.draw(player.shape);
            for (const auto& enemy : enemies) {
                window.draw(enemy.shape);
            }
            for (const auto& bullet : player.bullets) {
                window.draw(bullet.shape);
            }
            sf::Font font;
            if (font.loadFromFile("universal-serif.ttf")) {
                sf::Text text("Bots Killed: " + std::to_string(botsKilled), font, 20);
                text.setFillColor(sf::Color::White);
                text.setPosition(window.getSize().x - text.getLocalBounds().width - 10, 10);
                window.draw(text);
            }
        } else if (!gameRunning) {
            sf::Font font;
            if (font.loadFromFile("universal-serif.ttf")) {
                sf::Text text("You lost press esc to exit", font, 30);
                sf::Text text2("Press enter to try again", font, 30);
                text2.setFillColor(sf::Color::Green);
                text2.setPosition(160, 300);
                text.setFillColor(sf::Color::Green);
                text.setPosition(150, 250);
                window.draw(text);
                window.draw(text2);
            }
        }
    }

    void spawnEnemy() {
        constexpr int maxAttempts = 10;
        int pos_x = player.shape.getPosition().x;
        int pos_y = player.shape.getPosition().y;
        for (int attempt = 0; attempt < maxAttempts; ++attempt) {
            float x = static_cast<float>(std::rand() % 800);
            float y = static_cast<float>(std::rand() % 600);

            if (std::abs(x - pos_x) >= 120 && y >= pos_y + 120) {
                enemies.push_back(Enemy(x, y, 0.4f));
                return;
            }
        }

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
