#include "include/Species.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(2 * WINDOW_DIMENSION, WINDOW_DIMENSION), "NEAT");
    // window.close();
    // window.setFramerateLimit(60);

    const unsigned population = 200;

    Species mySpecies(NUMBER_OF_INPUTS, NUMBER_OF_OUTPUTS, population);

    // while (true)
    {
        for (unsigned generation = 0; true; ++generation)
        {
            sf::Event _event;
            while (window.pollEvent(_event))
            {
                if (_event.type == sf::Event::Closed)
                    window.close();
            }

            mySpecies.play_one_generation(window, generation);

            mySpecies.record_result(generation);

            std::cout << "Generation " << generation << " complete\n";

            mySpecies.repopulate();
        }
    }
    return 0;
}

// #include "include/Board.hpp"

// int main()
// {
//     // init the window
//     sf::RenderWindow window(sf::VideoMode(WINDOW_DIMENSION, WINDOW_DIMENSION), "test");
//     window.setFramerateLimit(4);

//     Board myBoard(10, 10);
    
//     while (window.isOpen() and not myBoard.game_over())
//     {
//         // event check
//         sf::Event _event;
//         while (window.pollEvent(_event))
//         {
//             if (_event.type == sf::Event::Closed)
//                 window.close();
//         }
//         if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
//         myBoard.play_one_move(0);
//         if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
//         myBoard.play_one_move(1);
//         if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
//         myBoard.play_one_move(2);
//         if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
//         myBoard.play_one_move(3);

//         window.clear();
//         myBoard.g_draw(window);
//         window.display();
//     }
//     return 0;
// }