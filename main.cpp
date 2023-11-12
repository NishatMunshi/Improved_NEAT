#include "include/Species.hpp"

int main()
{
    Species mySpecies;
    std::ofstream resultFile("evolutionResults.txt");

    for (unsigned generation = 0; true; ++generation)
    {
        mySpecies.play_one_generation(generation);

        resultFile.seekp(std::ios_base::beg);
        resultFile << "Generation: " << generation << '\n';
        mySpecies.record_result(resultFile);

        mySpecies.repopulate();
    }

    resultFile.close();
    return 0;
}