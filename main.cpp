#include "include/Species.hpp"


int main()
{
    // init the window
    sf::RenderWindow window(sf::VideoMode(2 * WINDOW_DIMENSION, WINDOW_DIMENSION), "NEAT");
    // window.setFramerateLimit(60);

    const unsigned population = 200;

    Species mySpecies(NUMBER_OF_INPUTS, NUMBER_OF_OUTPUTS, population);

    // Board myBoard(20, 20);
    // window.clear();
    // myBoard.g_draw(window);
    // window.display();

    while (window.isOpen())
    {
        for (unsigned generation = 0; generation < 20000; ++generation)
        {

            // event check
            sf::Event _event;
            while (window.pollEvent(_event))
            {
                if (_event.type == sf::Event::Closed)
                    window.close();
            }
            //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            //         myBoard.play_one_move(0);
            //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            //         myBoard.play_one_move(1);
            //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            //         myBoard.play_one_move(2);
            //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            //         myBoard.play_one_move(3);

            // window.clear();
            // myBoard.g_draw(window);
            // window.display();

            mySpecies.play_one_generation(window, generation);

            // std::cout << "\ngeneration: " << generation << '\n';

            // window.setTitle();
            // mySpecies.print_best_scorer();

            mySpecies.repopulate();
        }
    }
    return 0;
}