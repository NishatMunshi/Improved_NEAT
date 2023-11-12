#include "include/Species.hpp"

int main()
{
    Species mySpecies;

    for (unsigned generation = 0; true; ++generation)
    {
        mySpecies.play_one_generation(generation);

        std::ofstream resultFile("evolutionResults.txt");
        resultFile << "Generation: " << generation << '\n';
        mySpecies.record_result(resultFile);
        resultFile.close();

        mySpecies.repopulate();
    }

    return 0;
}