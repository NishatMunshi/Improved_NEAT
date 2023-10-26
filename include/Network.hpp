#pragma once
#include "Genome.hpp"

class Network
{
private:
    using Layer = std::unordered_map<NeuronID, Neuron *>;
    std::vector<Layer> m_neuralNetWork;

public:
    // runs for every individual once
    // THIS CONSTRUCTOR MUST TAKE CARE OF EACH NEURON'S INDEX IN LAYER
    Network(const std::unordered_map<NeuronID, Neuron *> &_neuronPool, const Genome *_genome)
    {
        for (unsigned layerIndex = 0; layerIndex < _genome->numberOfLayersUsed; ++layerIndex)
        {
            m_neuralNetWork.push_back(Layer());
        }
        // push neurons in Layers
        for (const auto [id, layerIndex] : _genome->usedNeurons)
        {
            _neuronPool.at(id)->g_layerIndex = layerIndex;
            _neuronPool.at(id)->indexInLayer = m_neuralNetWork.at(layerIndex).size();

            // std::cout << "here";

            m_neuralNetWork.at(layerIndex).insert_or_assign(id, _neuronPool.at(id));
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
        assert(m_neuralNetWork.front().size() == _inputs.size());

        for (const auto &[id, neuron] : m_neuralNetWork.front())
        {
            neuron->set_input(_inputs.at(neuron->indexInLayer));
        }
        for (const auto &layer : m_neuralNetWork)
        {
            for (const auto &[id, neuron] : layer)
            {
                neuron->feed_forward();
            }
        }

        using NeuronGene = std::pair<NeuronID, Neuron *>;
        auto iterator = std::max_element(m_neuralNetWork.back().begin(), m_neuralNetWork.back().end(), [](const NeuronGene &_a, const NeuronGene &_b)
                                         { return _a.second->get_output() < _b.second->get_output(); });
        return iterator->second->indexInLayer;
    }

    auto calculate_error(const std::array<float, NUMBER_OF_OUTPUTS> &_label)
    {
        assert(m_neuralNetWork.back().size() == _label.size());

        float error = 0;

        for (const auto &[id, neuron] : m_neuralNetWork.back())
        {
            auto del = neuron->get_output() - _label.at(neuron->indexInLayer);
            del *= del;
            error += del;
        }
        return error / m_neuralNetWork.back().size();
    }

    // GRAPHICS
public:
    void g_draw(sf::RenderWindow &_window) const
    {
        std::vector<float> horizontalDivisionWidths;
        const float verticalDivisionWidth = WINDOW_DIMENSION / (m_neuralNetWork.size() + 1);

        for (unsigned layerIndex = 0; layerIndex < m_neuralNetWork.size(); ++layerIndex)
        {
            horizontalDivisionWidths.push_back(WINDOW_DIMENSION / (m_neuralNetWork.at(layerIndex).size() + 1));
        }

        for (const auto &layer : m_neuralNetWork)
        {
            for (const auto &[id, neuron] : layer)
            {
                neuron->g_draw(_window, verticalDivisionWidth, horizontalDivisionWidths);
            }
        }
    }
};
