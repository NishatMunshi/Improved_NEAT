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
        assert(POPULATION % 8 == 0);

        // push the starting population
        for (unsigned individualIndex = 0; individualIndex < POPULATION; ++individualIndex)
        {
            // make individual genomes
            m_genePool.push_back(Genome());
        }
    }

public:
    void play_one_generation(const unsigned _generation)
    {
        m_brains.clear();
        m_threads.clear();

        for (auto &genome : m_genePool)
        {
            genome.numberOfFoodsEaten = 0;

            // make its brain
            m_brains.push_back(Network(genome));

#if PARALLELIZE
            m_threads.push_back(std::thread(&Network::play, &m_brains.back(), _generation, std::ref(genome)));
#else
            m_brains.back().play(_generation, genome);
#endif
        }
#if PARALLELIZE
        for (auto &thread : m_threads)
        {
            thread.join();
        }
#endif
    }

public:
    void record_result(std::ofstream &_resultFile) const
    {
        auto less = [](const Genome &_A, const Genome &_B)
        {
            return _A.numberOfFoodsEaten < _B.numberOfFoodsEaten;
        };
        const auto &bestGenome = std::max_element(m_genePool.begin(), m_genePool.end(), less);

        _resultFile << "Number of layers used: " << bestGenome->numberOfLayersUsed() << '\n'
                    << "Number of neurons used: " << bestGenome->usedNeurons.size() << '\n'
                    << "Number of synapses used: " << bestGenome->usedSynapses.size() << '\n'
                    << "Number of food eaten: " << bestGenome->numberOfFoodsEaten << '\n';

#if RECORD_NEURONS_AND_WEIGHTS
        _resultFile << "\nNeurons:\n";
        for (const auto &[id, layerIndex] : bestGenome->usedNeurons)
        {
            _resultFile << '(' << id << ',' << layerIndex << ')' << '\n';
        }

        _resultFile << "\nSynapses:\n";
        for (const auto &[id, weight] : bestGenome->usedSynapses)
        {
            _resultFile << '(' << id.startingNeuronID << ',' << id.endingNeuronID << ',' << weight << ")" << '\n';
        }
#endif
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
            const auto &father = selectedGenomes.at(index);
            const auto &mother = selectedGenomes.at(index + 1);

            // each couple must produce 2 offsprings
            for (unsigned childIndex = 0; childIndex < 2; ++childIndex)
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
            // they are kept in the generation to compare with new generation
        }
    }
};