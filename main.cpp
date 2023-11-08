#include "include/Species.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(2 * WINDOW_DIMENSION, WINDOW_DIMENSION), "NEAT");
    Species mySpecies;

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

    return 0;
}