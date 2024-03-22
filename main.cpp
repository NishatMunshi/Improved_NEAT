#include "Species.hpp"
#include <fstream>

int main()
{
    Species mySpecies;
    std::ofstream resultFile {};
    resultFile.open("evolutionResults.txt");

    for (unsigned generation = 0; true; ++generation)
    {
        mySpecies.play_one_generation(generation);

        std::cout << "Generation " << generation << " done.\n";
        resultFile.seekp(std::ios_base::beg);

        #if RECORD_RESULTS
        mySpecies.record_result(resultFile);
        #endif

        mySpecies.repopulate();
    }

    resultFile.close();
    return 0;
}