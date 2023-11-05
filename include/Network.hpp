#pragma once
#include "Genome.hpp"

class Network
{
private:
    using Layer = std::unordered_map<NeuronID, Neuron *>;
    std::vector<Layer> m_neuralNetwork;

public:
    // runs for every individual once
    // THIS CONSTRUCTOR MUST TAKE CARE OF EACH NEURON'S INDEX IN LAYER
    Network(const std::unordered_map<NeuronID, Neuron *> &_neuronPool, const Genome *_genome)
    {
        for (unsigned layerIndex = 0; layerIndex < _genome->numberOfLayersUsed; ++layerIndex)
        {
            m_neuralNetwork.push_back(Layer());
        }
        // push neurons in Layers
        for (const auto &[id, layerIndex] : _genome->usedNeurons)
        {
            _neuronPool.at(id)->g_layerIndex = layerIndex;
            _neuronPool.at(id)->indexInLayer = m_neuralNetwork.at(layerIndex).size();

            // std::cout << "here";

            m_neuralNetwork.at(layerIndex).insert_or_assign(id, _neuronPool.at(id));
        }
        // add synapses to them
        for (const auto &[id, synapse] : _genome->genes)
        {
            _neuronPool.at(id.first)->add_output_synapse(synapse);
        }
    }

public:
    auto feed_forward(const std::array<float, NUMBER_OF_INPUTS> &_inputs)
    {
        assert(m_neuralNetwork.front().size() == _inputs.size());

        for (const auto &[id, neuron] : m_neuralNetwork.front())
        {
            neuron->set_input(_inputs.at(neuron->indexInLayer));
        }
        for (const auto &layer : m_neuralNetwork)
        {
            for (const auto &[id, neuron] : layer)
            {
                neuron->feed_forward();
            }
        }

        using NeuronGene = std::pair<NeuronID, Neuron *>;
        auto iterator = std::max_element(m_neuralNetwork.back().begin(), m_neuralNetwork.back().end(), [](const NeuronGene &_a, const NeuronGene &_b)
                                         { return _a.second->get_output() < _b.second->get_output(); });
        return iterator->second->indexInLayer;
    }

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

    auto get_results(void ) const{
        std::array<float, 2 > result;
        for(const auto &[id, neuron] : m_neuralNetwork.back())
        {
            if(neuron->indexInLayer == 0)
            {
                result[0] = neuron->get_output();
            }
            if(neuron->indexInLayer ==  1)
            {
                result[1] = neuron->get_output();
            }
        }
        return result;
    }

    // GRAPHICS
public:
    void g_draw(sf::RenderWindow &_window) const
    {
        std::vector<float> horizontalDivisionWidths;
        const float verticalDivisionWidth = WINDOW_DIMENSION / (m_neuralNetwork.size() + 1);


        // CHECK IF THESE FOR LOOPS CAN BE MERGED INTO ONE
        for (const auto &layer: m_neuralNetwork)
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
};
