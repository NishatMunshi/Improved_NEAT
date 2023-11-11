#pragma once
#include "Network.hpp"
#include "Board.hpp"
#include "Defines.hpp"

#include <thread>

class Species
{
private:
    std::deque<Genome> m_genePool;
    std::list<Network> m_brains;
    std::list<std::thread> m_threads;

public:
    Species(void) noexcept
    {
        assert(POPULATION % 4 == 0);

        // push the starting population
        for (unsigned individualIndex = 0; individualIndex < POPULATION; ++individualIndex)
        {
            // make individual genomes
            m_genePool.push_back(Genome());
        }
    }

public:
    void play_one_generation(
#if ENABLE_GRAPHICS
        sf::RenderWindow &_window,
#endif
        const unsigned _generation)

    {
        for(const auto&network:m_brains){network.free();}
        m_brains.clear();
        m_threads.clear();

        for (auto &genome : m_genePool)
        {
            genome.numberOfFoodsEaten = 0;

            // make its brain
            m_brains.push_back(Network(genome));

            m_brains.back().play(
#if ENABLE_GRAPHICS
                _window,
#endif
                _generation, genome);
        }
    }

#if 0
private:

    static double fitness_function(const unsigned _movesLeft, const int _gameResult, const unsigned _numberOfAllowedMoves)
    {
        const double foodEatingReward = 50.0;
        const double collisionPunishment = 0.0;
        const double runAroundPenalty = 0.0;

        const double moveUsingPunishment = 0.0;
        const double highScoreReward = 0.0;

        static unsigned numberOfFoodEaten = 0;
        static unsigned stepsToEatFood = 0;
        static unsigned avgStepsBetweenFood = 0;

        double fitness = 0.0;

        const bool foodEaten = _gameResult == 1;
        const bool gameEndedWithOutCollision = _gameResult == 0 and _movesLeft == 1;
        const bool gameEndedWithCollision = _gameResult == -1;
        const bool nothinghappened = _gameResult == 0 and _movesLeft > 1;
        const bool gameEnded = gameEndedWithCollision or gameEndedWithOutCollision;
        if (foodEaten)
        {
            // give reward
            fitness += foodEatingReward;

            // update number of foods
            numberOfFoodEaten++;

            // increment avg steps
            avgStepsBetweenFood += stepsToEatFood;

            // reset steps
            stepsToEatFood = 0;
        }
        if (nothinghappened)
        {
            // calculate steps
            stepsToEatFood++;

            // running around penalty
            if (stepsToEatFood >= _numberOfAllowedMoves / 3)
            {
                // std::cout <<"here";
                fitness -= runAroundPenalty;
            }
        }
        if (gameEndedWithCollision)
        {
            // collision punishment
            // punish for one death
            fitness -= collisionPunishment;
        }
        if (gameEnded)
        {
            // reward for number of foods eaten
            fitness += numberOfFoodEaten * highScoreReward;

            // // punish for avg moves used between foods
            fitness -= (avgStepsBetweenFood) / (numberOfFoodEaten + 1) * moveUsingPunishment;

            //
            // reinit static variables
            numberOfFoodEaten = 0;
            stepsToEatFood = 0;
            avgStepsBetweenFood = 0;
        }
        return fitness;
    }
#endif
public:
    void record_result(const unsigned _generation) const
    {
        auto less = [](const Genome &_A, const Genome &_B)
        {
            return _A.numberOfFoodsEaten < _B.numberOfFoodsEaten;
        };
        const auto &bestGenome = std::max_element(m_genePool.begin(), m_genePool.end(), less);

        std::ofstream resultFile;
        resultFile.open("evolutionResults.txt");

        resultFile << "Generation: " << _generation << '\n'
                   << "Number of layers used: " << bestGenome->numberOfLayersUsed() << '\n'
                   << "Number of neurons used: " << bestGenome->usedNeurons.size() << '\n'
                   << "Number of synapses used: " << bestGenome->usedSynapses.size() << '\n'
                   << "Number of food eaten: " << bestGenome->numberOfFoodsEaten << '\n';

#if RECORD_NEURONS_AND_WEIGHTS
        resultFile << "\nNeurons:\n";
        for (const auto &[id, layerIndex] : bestGenome.usedNeurons)
        {
            resultFile << '(' << id << ',' << layerIndex << ')' << '\n';
        }

        resultFile << "\nSynapses:\n";
        for (const auto &[id, synapse] : bestGenome.genes)
        {
            resultFile << '(' << id.first << ',' << id.second << ',' << synapse.second << ")" << '\n';
        }
        resultFile << '\n';
#endif

        resultFile.close();
    }

private:
    void mutate(Genome &_genome)
    {
        // decide which mutation to do
        const auto randomInt = random_U32.generate();

        if (randomInt < UINT32_MAX * 0.8f)
        {
            // change a random weight
            _genome.change_random_weight();
        }
        else if (randomInt >= UINT32_MAX * 0.8f and randomInt < UINT32_MAX * 0.9f)
        {
            // add a new synapse
            _genome.add_new_random_synapse();
        }
        else if (randomInt >= UINT32_MAX * 0.9f and randomInt < UINT32_MAX)
        {
            // evolve a random synapse (add a neuron in the middle of it)
            _genome.evolve_random_synapse();
        }
    }

public:
    void repopulate(void)
    {
        // natural selection phase
        auto greater = [](const Genome &_A, const Genome &_B)
        {
            return _A.numberOfFoodsEaten > _B.numberOfFoodsEaten;
        };
        std::sort(m_genePool.begin(), m_genePool.end(), greater);

        //  best half of them
        m_genePool.resize(POPULATION / 2);
        auto &selectedGenomes = m_genePool;

        // reproduction phase
        for (unsigned index = 0; index < POPULATION / 2; index += 2)
        {
            const auto &father = selectedGenomes.front();
            const auto &mother = selectedGenomes.at(1);

            // each couple must produce 4 offsprings
            for (unsigned childIndex = 0; childIndex < 4; ++childIndex)
            {
                // std::cout  << "here";
                auto child(father.cross(mother));

                // decide whether to mutate
                const auto randomInt = random_U32.generate();
                if (randomInt < MUTATION_PROBABILITY * UINT32_MAX)
                {
                    mutate(child);
                }

                // add child to the back of genepool
                m_genePool.push_back(std::move(child));
            }

            // now that father and mother are done reproducing
            // they can be removed safely
            m_genePool.pop_front();
            m_genePool.pop_front();
        }
    }
};