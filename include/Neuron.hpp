#pragma once
#include "Synapse.hpp"
#if ENABLE_GRAPHICS
#include "E:/programming_tools/SFML/SFML_Sources/include/SFML/Graphics.hpp"
#endif
#include <set>

using LayerIndex = unsigned;
class Neuron
{
    double m_input;
    double m_output;

    std::set<Synapse> m_outputSynapses;

private:
    inline static double activation_function(const double _x)
    {
        return _x < 0 ? -1 : 9 * exp(-_x / 10);
    }

public:
    const NeuronID id;
    unsigned indexInLayer;
    Neuron(const NeuronID &_id, const unsigned _layerIndex, const unsigned _indexInLayer) : id(_id), indexInLayer(_indexInLayer), g_layerIndex(_layerIndex)
    {
        m_input = 0;
        m_output = 1;
    }
    inline void add_output_synapse(Synapse &&synapse) { m_outputSynapses.insert(synapse); }

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
    inline void set_input(const double _input) { m_input = _input; }
    inline auto get_output(void) const { return m_output; }

public:
    inline void clear_output_synapses(void) { m_outputSynapses.clear(); }

    // graphics
public:
    LayerIndex g_layerIndex;

public:
#if ENABLE_GRAPHICS
    void g_draw(sf::RenderWindow &_window, const double _verticalDivisionWidth, const std::vector<double> &_horizontalDivisionWidths) const
    {
        const uint8_t opacity = (this->m_output + 1) * 255 / 10.f;
        sf::Vector2f startingCoords, endingCoords;
        sf::Vector2f offset = {10.f, 10.f};
        sf::Vertex line[2];
        line[0].color = sf::Color(255, 255, 255, opacity);

        startingCoords.x = _verticalDivisionWidth * (1 + g_layerIndex);
        startingCoords.y = _horizontalDivisionWidths[g_layerIndex] * (1 + indexInLayer);

        // draw synapses
        for (const auto &[endingNeuronPtr, weight] : m_outputSynapses)
        {
            endingCoords.x = _verticalDivisionWidth * (1 + endingNeuronPtr->g_layerIndex);
            endingCoords.y = _horizontalDivisionWidths[endingNeuronPtr->g_layerIndex] * (1 + endingNeuronPtr->indexInLayer);

            if (weight < 0)
                line[1].color = sf::Color(25, 189, 255, opacity);
            else
                line[1].color = sf::Color(255, 71, 76, opacity);

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
        circle.setFillColor(sf::Color(255, 255, 0, opacity));

        _window.draw(circle);
    }
#endif
};
