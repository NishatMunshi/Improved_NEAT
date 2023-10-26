#pragma once
#include "Synapse.hpp"

#include "SFML/Graphics.hpp"

using LayerIndex = unsigned;
class Neuron
{
    float m_input;
    float m_output;

    std::unordered_map<Neuron *, float> m_outputSynapses;

private:
    inline static float activation_function(const float _x) { return 1 / (1 + exp(-_x)); }

public:
    const NeuronID id;
    unsigned indexInLayer;
    Neuron(const NeuronID &_id, const unsigned _layerIndex, const unsigned _indexInLayer) : id(_id), indexInLayer(_indexInLayer), g_layerIndex(_layerIndex)
    {
        m_input = 0;
        m_output = 1;
    }
    inline void add_output_synapse(const Synapse &synapse) { m_outputSynapses.insert(synapse); }

public:
    void feed_forward(void)
    {
        m_output = activation_function(m_input);
        m_input = 0;

        for (const auto &[endingNeuron, weight] : m_outputSynapses)
        {
            endingNeuron->m_input += weight * this->m_output;
        }
    }

public: // member access
    inline void set_input(const float _input) { m_input = _input; }
    inline auto get_output(void) const { return m_output; }

public:
    inline void clear_output_synapses(void) { m_outputSynapses.clear(); }

    // graphics
public:
    LayerIndex g_layerIndex;

public:
    void g_draw(sf::RenderWindow &_window, const float _verticalDivisionWidth, const std::vector<float> &_horizontalDivisionWidths) const
    {
        sf::Vector2f startingCoords, endingCoords;
        sf::Vector2f offset = {10.f, 10.f};
        sf::Vertex line[2];

        startingCoords.x = _verticalDivisionWidth * (1 + g_layerIndex);
        startingCoords.y = _horizontalDivisionWidths[g_layerIndex] * (1 + indexInLayer);

        // draw synapses
        for (const auto &[endingNeuronPtr, weight] : m_outputSynapses)
        {
            endingCoords.x = _verticalDivisionWidth * (1 + endingNeuronPtr->g_layerIndex);
            endingCoords.y = _horizontalDivisionWidths[endingNeuronPtr->g_layerIndex] * (1 + endingNeuronPtr->indexInLayer);

            if (weight < 0)
                line[1]
                    .color = sf::Color(25, 189, 255, this->m_output * 255);
            else
                line[1].color = sf::Color(255, 71, 76, this->m_output * 255);

            line[0].position = startingCoords + offset;
            line[1].position = endingCoords + offset;

            _window.draw(line, 2, sf::Lines);
        }

        // draw itself
        sf::CircleShape circle;
        circle.setRadius(10.f);
        circle.setOutlineThickness(4.f);
        circle.setOutlineColor(sf::Color::White);

        circle.setPosition(startingCoords);
        circle.setFillColor(sf::Color(255, 255, 0, m_output * 255));

        _window.draw(circle);
    }
};
