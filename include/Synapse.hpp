#pragma once
#include <bits/stdc++.h>
#include "E:/C++/Tools/Custom_Libraries/randomNumberGenerator.hpp"
#include "Defines.hpp"

class Neuron;
using NeuronID = unsigned;
using Weight = double;
struct SynapseID
{
    NeuronID startingNeuronID, endingNeuronID;

    inline explicit SynapseID(const NeuronID &_startingNeuronID, const NeuronID &_endingNeuronID) noexcept
        : startingNeuronID(_startingNeuronID), endingNeuronID(_endingNeuronID) {}
    [[nodiscard]] inline bool operator==(const SynapseID &_other) const noexcept { return startingNeuronID == _other.startingNeuronID and endingNeuronID == _other.endingNeuronID; }
};
struct Synapse
{
    Neuron *const endingNeuron;
    Weight weight;
    inline explicit Synapse(Neuron *const &_endingNeuron, const Weight &_weight) noexcept
        : endingNeuron(_endingNeuron), weight(_weight) {}
    [[nodiscard]] inline bool operator<(const Synapse &_other) const noexcept { return endingNeuron < _other.endingNeuron; }
};
template <>
struct std::hash<SynapseID>
{
    inline std::size_t operator()(const SynapseID &_id) const
    {
        return ((std::hash<unsigned>()(_id.startingNeuronID) xor (std::hash<unsigned>()(_id.endingNeuronID) << 1)) >> 1);
    }
};