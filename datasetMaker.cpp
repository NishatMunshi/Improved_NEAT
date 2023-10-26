#include <fstream>
#include "libraries/randomNumberGenerator.hpp"

int main()
{

    std::ofstream file("xor.txt");

    for (unsigned i = 0; i < 200; ++i)
    {
        int randomX = random_bool.generate();
        int randomY = random_bool.generate();
        int randomZ = random_bool.generate();

        auto result = ((randomX and randomY) xor randomZ) ? 1 : 0;
        randomX = randomX ? 1 : 0;
        randomY = randomY ? 1 : 0;
        randomZ = randomZ ? 1 : 0;
        file << randomX << ' ' << randomY << ' ' << randomZ << ' ' << result << '\n';
    }

    file.close();
    return 0;
}