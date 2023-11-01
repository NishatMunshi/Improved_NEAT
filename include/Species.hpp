#pragma once
#include "Network.hpp"
#include "Board.hpp"

#define MUTATION_PROBABILITY 0.2f

class Species
{
private:
    std::unordered_map<NeuronID, Neuron *> m_neuronPool;

    std::deque<Genome *> m_genePool;

    const unsigned m_numberOfInputs;
    const unsigned m_numberOfOutputs;
    const unsigned m_population;

    Board m_board{BOARD_WIDTH, BOARD_HEIGHT};

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
    void play_one_generation(sf::RenderWindow &_window, const unsigned _generation)
    {
        const unsigned numberOfTotalAllowedMoves = 5 * BOARD_WIDTH + sqrt(_generation);
        const unsigned numberOfGamesPerIndividual = 25;

        unsigned individualIndex = 0;
        for (const auto &genome : m_genePool)
        {
            genome->numberOfFoodsEaten = 0;
            genome->score = 1000;

            // clear every neuron's output synapses container
            for (const auto &[id, neuron] : m_neuronPool)
            {
                neuron->clear_output_synapses();
            }

            // make its brain
            Network individualBrain(m_neuronPool, genome);

            for (unsigned gameIndex = 0; gameIndex < numberOfGamesPerIndividual; ++gameIndex)
            {
                m_board.reset_stats();

                for (unsigned movesLeft = numberOfTotalAllowedMoves; movesLeft > 0 and not m_board.game_over(); --movesLeft)
                {
                    const auto inputs = m_board.get_input_for_NN();

                    const auto move = individualBrain.feed_forward(inputs);

                    // -----------------------------------------------------------
                    if (_window.hasFocus()) // otherwise perform calculations internally
                    {
                        std::stringstream ss;
                        ss << "Generation: " << _generation << ' ' << "Individual: " << individualIndex;
                        _window.setTitle(ss.str());

                        sf::Event _event;
                        while (_window.pollEvent(_event))
                        {
                            if (_event.type == sf::Event::Closed)
                            {
                                _window.close();
                                abort();
                            }
                        }
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp))
                            _window.setFramerateLimit(UINT32_MAX);
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown))
                            _window.setFramerateLimit(5);
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Home))
                            while (true)
                            {
                                if (sf::Keyboard::isKeyPressed(sf::Keyboard::End))
                                    break;
                            }

                        _window.clear();
                        individualBrain.g_draw(_window);
                        m_board.g_draw(_window);
                        _window.display();
                    }
                    // -------------------------------------

                    const auto gameResult = m_board.play_one_move(move);
                    if (gameResult == 1)
                    {
                        genome->numberOfFoodsEaten++;
                    }
                    genome->score += fitness_function(movesLeft, gameResult, numberOfTotalAllowedMoves, numberOfGamesPerIndividual);
                }
            }
            individualIndex++;
        }
    }

private:
    static double fitness_function(const unsigned _movesLeft, const int _gameResult, const unsigned _numberOfAllowedMoves, const unsigned _numberOfGamesPerIndividual)
    {
        const double foodEatingReward = 70.0;
        const double moveUsingPunishment = 1.0;
        const double collisionPunishment = 55.0;
        const double highScoreReward = 5.0;
        const double runAroundPenalty = 2000.0;

        static unsigned numberOfFoodEaten = 0;
        static int stepsBetweenFood = _numberOfAllowedMoves + 1;

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
        }
        // if (nothinghappened)
        // {
        //     // calculate steps
        //     stepsBetweenFood = std::abs(stepsBetweenFood) - _movesLeft;

        //     // running around penalty
        //     if (std::abs(stepsBetweenFood) == _numberOfAllowedMoves / 4)
        //     {
        //         // std::cout <<"here";
        //         fitness -= runAroundPenalty;
        //     }
        // }
        if (gameEndedWithCollision)
        {
            // collision punishment
            // punish for one death
            fitness -= collisionPunishment;
        }
        if (gameEnded)
        {
            // reward for number of foods eaten
            fitness -= numberOfFoodEaten * highScoreReward;
            // punish for avg moves used between foods
            // fitness -= (stepsBetweenFood / _numberOfAllowedMoves) * moveUsingPunishment;

            //
            // reinit static variables
            stepsBetweenFood = _numberOfAllowedMoves + 1;
            numberOfFoodEaten = 0;
        }
        return fitness;
    }

public:
    void record_result(const unsigned _generation) const
    {
        auto less = [](const Genome *A, const Genome *B)
        {
            return A->score < B->score;
        };
        const auto &bestGenome = *std::max_element(m_genePool.begin(), m_genePool.end(), less);

        std::ofstream resultFile;
        resultFile.open("evolutionResults.txt");

        resultFile << "Generation: " << _generation << '\n'
                   << "Best genome score: " << bestGenome->score << '\n'
                   << "Number of neurons used: " << bestGenome->usedNeurons.size() << '\n'
                   << "Number of synapses used: " << bestGenome->genes.size() << '\n'
                   << "Number of food eaten: " << bestGenome->numberOfFoodsEaten << '\n'
                   << "Total number of neurons in species: " << m_neuronPool.size() << '\n';

        resultFile << "\nNeurons:\n";
        for (const auto &[id, layerIndex] : bestGenome->usedNeurons)
        {
            resultFile << '(' << id << ',' << layerIndex << ')' << '\n';
        }

        resultFile << "\nSynapses:\n";
        for (const auto &[id, synapse] : bestGenome->genes)
        {
            resultFile << '(' << id.first << ',' << id.second << ',' << synapse.second << ")" << '\n';
        }
        resultFile << '\n';

        resultFile.close();
    }

private:
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

public:
    void repopulate(void)
    {
        // natural selection phase
        auto greater = [](const Genome *A, const Genome *B)
        {
            return A->score > B->score;
        };
        std::sort(m_genePool.begin(), m_genePool.end(), greater);

        //  best quarter of them
        for (unsigned index = m_population / 2; index < m_population; ++index)
        {
            // std::cout << "broke here"<<index;
            delete m_genePool.at(index);
        }
        m_genePool.resize(m_population / 2);
        auto &selectedGenomes = m_genePool;

        std::set<NeuronID> neuronsUsedByNewGeneration; // need to keep track for extinction phase

        // these parents will be in the next gen
        for (const auto &genome : selectedGenomes)
            for (const auto &[id, layerIndex] : genome->usedNeurons)
                neuronsUsedByNewGeneration.insert(id);

        // reproduction phase
        for (long int index = m_population / 2 - 1; index >= 0; index -= 2)
        {
            const auto &father = selectedGenomes.at(index);
            const auto &mother = selectedGenomes.at(index - 1);

            // each couple must produce 6 offsprings
            for (unsigned childIndex = 0; childIndex < 2; ++childIndex)
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
            // they are kept in the species to compare
            // with the new generation
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