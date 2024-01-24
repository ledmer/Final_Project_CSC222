#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
bool GAME = true;
sf::RenderWindow window(sf::VideoMode(400, 200), "SFML works!");

class Player{
    
    sf::Sprite p;
    sf::Texture player_texture;

    public:
        void player(){
            if((!player_texture.loadFromFile("img/player.png"))){
                throw std::invalid_argument("AddPositiveIntegers arguments must be positive");
            }
            p.setTexture(player_texture);
        }
        void player_movement(){
            int x_mov = 0;
            int y_mov = 0;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                x_mov -= 1;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                x_mov += 1;

            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
                y_mov -= 1;

            }
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
                y_mov += 1;

            }
            p.move(x_mov, y_mov);
        };
        void draw(){
            window.draw(p);
        };

};

int main(){  
    sf::Sprite Enemy;
    sf::Texture e_texture;
    Player p;
    p.draw();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {    
                window.close();
            }
        } 
        p.player_movement();

        window.clear();
        p.draw();
        window.display();
    }

    return 0;
}