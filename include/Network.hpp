#pragma once
#include "Genome.hpp"
#include "Board.hpp"

class Network
{
private:
    using Layer = std::vector<Neuron *>;
    std::vector<Layer> m_neuralNetwork;

    std::unordered_map<NeuronID, Neuron *> m_usedNeurons;

public:
    // runs for every individual once
    // THIS CONSTRUCTOR MUST TAKE CARE OF EACH NEURON'S INDEX IN LAYER
    explicit Network(const Genome &_genome) noexcept
    {
        for (unsigned layerIndex = 0; layerIndex < _genome.numberOfLayersUsed(); ++layerIndex)
        {
            m_neuralNetwork.push_back(Layer());
        }
        // push neurons in Layers
        for (const auto &[id, layerIndex] : _genome.usedNeurons)
        {
            const auto newNeuron = new Neuron(id, layerIndex, m_neuralNetwork.at(layerIndex).size());

            m_neuralNetwork.at(layerIndex).push_back(newNeuron);
            m_usedNeurons.insert_or_assign(id, newNeuron);
        }
        // add synapses to them
        for (const auto &[id, weight] : _genome.usedSynapses)
        {
            auto synapse = Synapse(m_usedNeurons.at(id.endingNeuronID), weight);
            m_usedNeurons.at(id.startingNeuronID)->add_output_synapse(std::move(synapse));
        }
    }
    ~Network(void)
    {
        for (const auto &[id, neuron] : m_usedNeurons)
        {
            delete neuron;
        }
    }

public:
    auto feed_forward(const std::array<float, NUMBER_OF_INPUTS> &_inputs) const
    {
        assert(m_neuralNetwork.front().size() == _inputs.size());

        for (const auto &neuron : m_neuralNetwork.front())
        {
            neuron->set_input(_inputs.at(neuron->indexInLayer));
        }
        for (const auto &layer : m_neuralNetwork)
        {
            for (const auto &neuron : layer)
            {
                neuron->feed_forward();
            }
        }
        auto iterator = std::max_element(m_neuralNetwork.back().cbegin(), m_neuralNetwork.back().cend(), [](const Neuron *const _a, const Neuron *const &_b)
                                         { return _a->get_output() < _b->get_output(); });
        return (*iterator)->indexInLayer;
    }

    void play(
#if ENABLE_GRAPHICS
        sf::RenderWindow &_window,
#endif
        const unsigned _generation, Genome &_genome)
    {
        Board board;
        const unsigned numberOfTotalAllowedMoves = 10 * BOARD_WIDTH + sqrt(_generation);

        for (unsigned gameIndex = 0; gameIndex < NUMBER_OF_GAMES; ++gameIndex)
        {
            board.reset_stats();

            for (unsigned movesLeft = numberOfTotalAllowedMoves; movesLeft > 0 and not board.game_over(); --movesLeft)
            {
                const auto inputs = board.get_input_for_NN();

                const auto move = feed_forward(inputs);

#if ENABLE_GRAPHICS
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
#endif

                const auto gameResult = board.play_one_move(move);
                if (gameResult == Board::GameResult::FOODEATEN)
                {
                    _genome.numberOfFoodsEaten++;
                }
            }
        }
    }

#if 0
    auto calculate_error(const std::array<float, NUMBER_OF_OUTPUTS> &_label)
    {
        assert(m_neuralNetwork.back().size() == _label.size());

        float error = 0;

        for (const auto &[id, neuron] : m_neuralNetwork.back())
        {
            auto del = neuron->get_output() - _label.at(neuron->indexInLayer);
            del *= del;
            error += del;
        }
        return error / m_neuralNetwork.back().size();
    }

    auto get_results(void) const
    {
        std::array<float, 2> result;
        for (const auto &[id, neuron] : m_neuralNetwork.back())
        {
            if (neuron->indexInLayer == 0)
            {
                result[0] = neuron->get_output();
            }
            if (neuron->indexInLayer == 1)
            {
                result[1] = neuron->get_output();
            }
        }
        return result;
    }
#endif
// GRAPHICS
#if ENABLE_GRAPHICS

public:
    void g_draw(sf::RenderWindow &_window) const
    {
        std::vector<float> horizontalDivisionWidths;
        const float verticalDivisionWidth = WINDOW_DIMENSION / (m_neuralNetwork.size() + 1);

        // CHECK IF THESE FOR LOOPS CAN BE MERGED INTO ONE
        for (const auto &layer : m_neuralNetwork)
        {
            horizontalDivisionWidths.push_back(WINDOW_DIMENSION / (layer.size() + 1));
        }

        for (const auto &layer : m_neuralNetwork)
        {
            for (const auto &[id, neuron] : layer)
            {
                neuron->g_draw(_window, verticalDivisionWidth, horizontalDivisionWidths);
            }
        }
    }
#endif
};
