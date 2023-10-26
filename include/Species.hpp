#pragma once
#include "Network.hpp"

#define MUTATION_PROBABILITY 0.1f

class Species
{
private:
    std::unordered_map<NeuronID, Neuron *> m_neuronPool;

    std::deque<Genome *> m_genePool;

    const unsigned m_numberOfInputs;
    const unsigned m_numberOfOutputs;
    const unsigned m_population;

public:
    Species(const unsigned _numberOfInputs, const unsigned _numberOfOutputs, const unsigned _population) : m_numberOfInputs(_numberOfInputs), m_numberOfOutputs(_numberOfOutputs), m_population(_population)
    {
        assert(_population % 4 == 0);

        // make the neurons we will use and save them in the neuronPool
        for (unsigned indexInLayer = 0; indexInLayer < _numberOfInputs; ++indexInLayer)
        {
            const auto id = indexInLayer;
            const auto newNeuron = new Neuron(id, 0, indexInLayer);
            m_neuronPool.insert_or_assign(id, newNeuron);
        }
        for (unsigned indexInLayer = 0; indexInLayer < _numberOfOutputs; ++indexInLayer)
        {
            const auto id = _numberOfInputs + indexInLayer;
            const auto newNeuron = new Neuron(id, 1, indexInLayer);
            m_neuronPool.insert_or_assign(id, newNeuron);
        }

        // push the starting population
        for (unsigned individualIndex = 0; individualIndex < _population; ++individualIndex)
        {
            // make individual genomes
            m_genePool.push_back(new Genome(_numberOfInputs, _numberOfOutputs, m_neuronPool));
        }

    }

public:
    void play_one_generation(void)
    {
        std::ifstream file("E:/Code/NEAT/xor.txt");

        std::array<float, NUMBER_OF_INPUTS> inputs;
        std::array<float, NUMBER_OF_OUTPUTS> label;

        for (const auto &genome : m_genePool)
        {
            genome->numberOfCorrectAnswers = 0;
            genome->score = NUMBER_OF_LINES;

            // clear every neuron's output synapses container
            for(const auto&[id, neuron] : m_neuronPool)
            {
                neuron->clear_output_synapses();
            }
            Network individualBrain(m_neuronPool, genome);

            for (unsigned line = 0; line < NUMBER_OF_LINES; ++line)
            {
                file >> inputs[0];
                file >> inputs[1];
                file >> inputs[2];

                float output;
                file >> output;

                label[0] = 0;
                label[1] = 0;

                if (output == 1)
                    label[1] = 1;
                else
                    label[0] = 1;

                const auto resultIndex = individualBrain.feed_forward(inputs);

                if ((resultIndex == 0 and label[0] == 1) or (resultIndex == 1 and label[1] == 1))
                {
                    genome->numberOfCorrectAnswers++;
                }
                const auto error = individualBrain.calculate_error(label);
                genome->score -= error;
            }
        }
    }

    void print_best_scorer(sf::RenderWindow&_window) const
    {
        auto less = [](const Genome *A, const Genome *B)
        {
            return A->score < B->score;
        };
        const auto &bestGenome = *std::max_element(m_genePool.begin(), m_genePool.end(), less);

        std::cout << "best genome score: " << bestGenome->score << '\n'
                  << "number of neurons used: " << bestGenome->usedNeurons.size() << '\n'
                  << "number of synapses used: " << bestGenome->genes.size() << '\n'
                  << "number of correct answers: " << bestGenome->numberOfCorrectAnswers << '\n'
                  << "total number of neurons in species: " << m_neuronPool.size() << '\n';

        // std::cout << "\ngenes: \n";
        // for (const auto &[id, synapse] : bestGenome->genes)
        // {
        //     std::cout << id.first << ' ' << id.second << ' ' << synapse.second << '\n';
        // }

        // ------------------------------------------------
        if (_window.hasFocus())
        {
            _window.clear();
            Network bestBrain(m_neuronPool, bestGenome);
            bestBrain.g_draw(_window);
            _window.display();
        }
        // ------------------------------------------------
    }

public:
    void mutate(Genome *_genome)
    {
        // decide which mutation to do
        const auto randomInt = random_U32.generate();

        if (randomInt < UINT32_MAX * 0.8f)
        {
            // change a random weight
            _genome->change_random_weight();
        }
        else if (randomInt >= UINT32_MAX * 0.8f and randomInt < UINT32_MAX * 0.9f)
        {
            // add a new synapse
            _genome->add_new_random_synapse(m_neuronPool);
        }
        else if (randomInt >= UINT32_MAX * 0.9f and randomInt < UINT32_MAX)
        {
            // evolve a synapse
            const auto whatNewNeuronsIDWouldBe = _genome->usedNeurons.size();

            // figure out if this neuron already exists in the genepool
            const auto alreadyExists = m_neuronPool.count(whatNewNeuronsIDWouldBe);
            if (alreadyExists)
            {
                // pass the pointer to this neuron for the genome to save
                _genome->evolve_random_synapse(m_neuronPool.at(whatNewNeuronsIDWouldBe), m_neuronPool);
            }
            else
            {
                // create a new neuron with that given id and save it in the pool
                // and pass its pointer to the genome to save
                // WE PASS 0 HERE REMMEBER TO UPDATE THEM LATER
                // Network's constructor takes care of that
                const auto newNeuronPtr = new Neuron(whatNewNeuronsIDWouldBe, 0, 0);
                // m_net->neuralNetwork.at(1).insert(std::make_pair(whatNewNeuronsIDWouldBe, newNeuronPtr));
                m_neuronPool.insert(std::make_pair(whatNewNeuronsIDWouldBe, newNeuronPtr));

                _genome->evolve_random_synapse(newNeuronPtr, m_neuronPool);
            }
        }
    }
    void repopulate(void)
    {
        // natural selection phase
        auto greater = [](const Genome *A, const Genome *B)
        {
            return A->score > B->score;
        };
        std::sort(m_genePool.begin(), m_genePool.end(), greater);

        for (unsigned index = m_population / 2; index < m_population; ++index)
        {
            delete m_genePool.at(index);
        }
        m_genePool.resize(m_population / 2);
        auto &selectedGenomes = m_genePool;

        std::set<NeuronID> neuronsUsedByNewGeneration; // need to keep track for extinction phase

        // reproduction phase
        for (long int index = m_population / 2 - 1; index >= 0; index -= 2)
        {
            const auto &father = selectedGenomes.at(index);
            const auto &mother = selectedGenomes.at(index - 1);

            // each couple must produce four offsprings
            for (unsigned childIndex = 0; childIndex < 4; ++childIndex)
            {
                auto child = father->cross(mother);

                // decide whether to mutate
                const auto randomInt = random_U32.generate();
                if (randomInt < MUTATION_PROBABILITY * UINT32_MAX)
                {
                    mutate(child);
                }

                // add child to the front of genepool
                m_genePool.push_front(child);

                // keep track of which neurons this child uses
                for (const auto &[id, layerIndex] : child->usedNeurons)
                    neuronsUsedByNewGeneration.insert(id);
            }

            // now that father and mother are done reproducing
            // they can be removed from the species (extinction)
            delete selectedGenomes.back();
            selectedGenomes.pop_back();
            delete selectedGenomes.back();
            selectedGenomes.pop_back();
        }

        // REMEMBER TO REMOVE UNUSED NEURONS FROM THE SPECIES AT THIS POINT IN EVOLUTION
        // neuron extinction phase
        std::set<NeuronID> neuronIDsToRemove;
        for (const auto &[id, neuron] : m_neuronPool)
        {
            if (not neuronsUsedByNewGeneration.count(id))
            {
                // free memory
                delete neuron;

                // record the id to remove
                neuronIDsToRemove.insert(id);
            }
        }
        // remove them from the species altogether
        for (const auto &id : neuronIDsToRemove)
        {
            m_neuronPool.erase(id);
        }
    }
};