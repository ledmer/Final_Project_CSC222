#include <SFML/Graphics.hpp>
#include <math.h>
#include <SFML/Window.hpp>

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
    bool checkCollision(const Player& player) {
        return shape.getGlobalBounds().intersects(player.shape.getGlobalBounds());
    }
};

class Game {
public:
    sf::RenderWindow window;
    Player player;
    Enemy enemy;
    bool gameRunning;
    bool startScreen;

    Game() : window(sf::VideoMode(800, 600), "SFML Game"), player(100, 100), enemy(700, 500, .5f),
             gameRunning(false), startScreen(true) {}

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
                        enemy = Enemy(700, 500, .5f);              
                    
                    }
                    sf::sleep(t);

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
            enemy.update(player.shape.getPosition());
            if (enemy.checkCollision(player)) {
                // Perform action on collision (e.g., end the game)
                gameRunning = false;
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
            if (!font.loadFromFile("universal-serif.ttf"))
            {
                // error...
            }
            sf::Text text("Press any key to start", font, 30);
            text.setFillColor(sf::Color::Red);

            text.setPosition(250, 300);
            window.draw(text);
            sf::Time t = sf::seconds(1);


        } else if (gameRunning) {
            // Draw game elements
            window.draw(player.shape);
            window.draw(enemy.shape);
        }
        else if(!(gameRunning) ){
            sf::Font font;
            if (!font.loadFromFile("universal-serif.ttf"))
            {
                // error...
            }
            sf::Text text("You lost press esc to exit, any key to try again", font, 30);
            text.setFillColor(sf::Color::Red);
            text.setPosition(250, 300);
            window.draw(text);
        }
    }
};

int main() {
    Game game;
    game.run();

    return 0;
}
