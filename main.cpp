#include "include/Species.hpp"

int main()
{
    // sf::RenderWindow window(sf::VideoMode(2 * WINDOW_DIMENSION, WINDOW_DIMENSION), "NEAT");
    // window.setFramerateLimit(60);

    const unsigned population = 80;

    Species mySpecies(NUMBER_OF_INPUTS, NUMBER_OF_OUTPUTS, population);

    // while (window.isOpen())
    {
        for (unsigned generation = 0; true; ++generation)
        {

            // event check
            // sf::Event _event;
            // while (window.pollEvent(_event))
            // {
            //     if (_event.type == sf::Event::Closed)
            //         window.close();
            // }

            mySpecies.play_one_generation( generation);

            mySpecies.record_result(generation);

            std::cout <<"Generation "<<generation<<" complete\n";

            mySpecies.repopulate();
        }
    }
    return 0;
}