#include "include/Species.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(2 * WINDOW_DIMENSION, WINDOW_DIMENSION), "NEAT");
    Species mySpecies;

    for (unsigned generation = 0; true; ++generation)
    {
        mySpecies.play_one_generation(window, generation);

        mySpecies.record_result(generation);

        mySpecies.repopulate();
    }

    return 0;
}