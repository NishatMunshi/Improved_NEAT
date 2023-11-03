#pragma once
#include <bits/stdc++.h>
#include "../libraries/randomNumberGenerator.hpp"
#include "Defines.hpp"

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