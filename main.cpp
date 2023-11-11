#include "include/Species.hpp"

int main()
{
#if ENABLE_GRAPHICS
    sf::RenderWindow window(sf::VideoMode(2 * WINDOW_DIMENSION, WINDOW_DIMENSION), "NEAT");
#endif
    Species mySpecies;

    for (unsigned generation = 0; true; ++generation)
    {
        mySpecies.play_one_generation(
#if ENABLE_GRAPHICS
            window,
#endif
            generation);

        mySpecies.record_result(generation);

        mySpecies.repopulate();
    }

    return 0;
}