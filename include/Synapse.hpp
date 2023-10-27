#pragma once
#include <bits/stdc++.h>
#include "../libraries/randomNumberGenerator.hpp"

#define NUMBER_OF_INPUTS 24
#define NUMBER_OF_OUTPUTS 4
// #define NUMBER_OF_LINES  200
#define WINDOW_DIMENSION 900.f

#define BOARD_WIDTH  40
#define BOARD_HEIGHT  40

class Neuron;
using NeuronID = unsigned;
using SynapseID = std::pair<NeuronID, NeuronID>;

using Synapse = std::pair<Neuron *, float>;
template <>
struct std::hash<SynapseID>
{
    inline std::size_t operator()(const SynapseID &_id) const
    {
        return ((std::hash<unsigned>()(_id.first) xor (std::hash<unsigned>()(_id.second) << 1)) >> 1);
    }
};