#include "include/Species.hpp"

int main()
{
    // init the window
    sf::RenderWindow window(sf::VideoMode(WINDOW_DIMENSION, WINDOW_DIMENSION), "NEAT");
    window.setFramerateLimit(60);

    const unsigned population = 2000;

    Species mySpecies(NUMBER_OF_INPUTS, NUMBER_OF_OUTPUTS, population);

    while (window.isOpen())
    {
        // event check
        sf::Event _event;
        while (window.pollEvent(_event))
        {
            if (_event.type == sf::Event::Closed)
                window.close();
        }

        for (unsigned generation = 0; generation < 20000; ++generation)
        {

            mySpecies.play_one_generation();

            std::cout << "\ngeneration: " << generation << '\n';
            mySpecies.print_best_scorer(window);

            mySpecies.repopulate();
        }
    }
    return 0;
}