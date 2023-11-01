#include <SFML/Graphics.hpp>
#include "include/Board.hpp"

int main(int argc, char const *argv[])
{
    // init the window
    sf::RenderWindow window(sf::VideoMode(2 * WINDOW_DIMENSION, WINDOW_DIMENSION), "test");
    window.setFramerateLimit(4);

    Board myBoard(10, 10);
    
    while (window.isOpen())
    {
        // event check
        sf::Event _event;
        while (window.pollEvent(_event))
        {
            if (_event.type == sf::Event::Closed)
                window.close();
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        myBoard.play_one_move(0);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        myBoard.play_one_move(1);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        myBoard.play_one_move(2);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        myBoard.play_one_move(3);

        window.clear();
        myBoard.g_draw(window);
        window.display();
    }
    return 0;
}