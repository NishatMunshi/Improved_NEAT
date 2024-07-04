#include "include\Species.hpp"
#include <fstream>
#include <string>

int main()
{
    Species mySpecies;
    std::ofstream resultFile{};
    resultFile.open("evolutionResults.txt");
    bool singleThreadMode = true;

#if ENABLE_GRAPHICS
    sf::RenderWindow window(sf::VideoMode(2 * WINDOW_DIMENSION, WINDOW_DIMENSION), "NEAT: Snake Game");
    window.setFramerateLimit(10);
#endif

    for (unsigned generation = 0; true; ++generation)
    {
        singleThreadMode = mySpecies.play_one_generation(generation
#if ENABLE_GRAPHICS
                                                         ,
                                                         window
#endif
                                                         ,
                                                         singleThreadMode);

        std::string str;
        str = singleThreadMode ? "single" : "multi";
        std::cout << "Generation " << generation << " done in " << str << "thread mode \n";

#if RECORD_RESULTS
        resultFile.seekp(std::ios_base::beg);
        mySpecies.record_result(resultFile);
#endif

        mySpecies.repopulate();
    }

    resultFile.close();
    return 0;
}