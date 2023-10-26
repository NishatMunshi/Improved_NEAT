#pragma once
#include "Neuron.hpp"

class Genome
{
public:
    std::unordered_map<NeuronID, LayerIndex> usedNeurons; // cannot permit repetitions
    // std::set<SynapseID> usedSynapses; // cannot permit repetitions
    std::unordered_map<SynapseID, Synapse> genes; // cannot permit repetitions

    unsigned numberOfLayersUsed;

    unsigned numberOfCorrectAnswers;
    float score;

private:
    static std::set<NeuronID> m_inputNeuronIDs;  // need to keep track to not mess up the input layer
    static std::set<NeuronID> m_outputNeuronIDs; // need to keep track to not mess up the output layer

public:
    // this constructor is called only when starting the species
    Genome(const unsigned _numberOfInputs, const unsigned _numberOfOutputs, const std::unordered_map<NeuronID, Neuron *> &_neuronPool)
    {
        numberOfLayersUsed = 2;
        // fully connect input to output layer
        for (unsigned inputIndex = 0; inputIndex < _numberOfInputs; ++inputIndex)
        {
            const auto sensorNeuronID = inputIndex;
            usedNeurons.insert_or_assign(sensorNeuronID, 0);
            m_inputNeuronIDs.insert(sensorNeuronID);

            for (unsigned outputIndex = 0; outputIndex < _numberOfOutputs; ++outputIndex)
            {
                const auto motorNeuronID = _numberOfInputs + outputIndex;
                // since it is a map, duplicates are already taken care of
                usedNeurons.insert_or_assign(motorNeuronID, 1);
                m_outputNeuronIDs.insert(motorNeuronID);

                const auto id = SynapseID(sensorNeuronID, motorNeuronID);
                const auto synapse = Synapse(_neuronPool.at(motorNeuronID), random_float());

                genes.insert(std::make_pair(id, synapse));
            }
        }
    }

public:
    Genome *cross(Genome *_mother)
    {
        const auto &father = this;
        Genome *child;
        Genome *dominantParent;
        Genome *nonDominantParent;
        // select the dominant parent

        if (father->numberOfLayersUsed >= _mother->numberOfLayersUsed)
        {
            child = new Genome(*father);
            dominantParent = father;
            nonDominantParent = _mother;
        }
        else
        {
            child = new Genome(*_mother);
            dominantParent = _mother;
            nonDominantParent = father;
        }

        // child must use all neurons used by father and mother but their layer indeces may be different
        // carefully take care of input and output layer
        for (const auto &[id, layerIndex] : nonDominantParent->usedNeurons)
        {
            const auto matched = dominantParent->usedNeurons.count(id);
            if (matched)
            {
                // consider nondominant's choice only if this isnt an input or output neuron
                bool unchangable = dominantParent->m_outputNeuronIDs.count(id) or dominantParent->m_inputNeuronIDs.count(id);
                if (not unchangable)
                {
                    // decide whether to use nondominant's layer choice
                    auto decision = random_bool.generate();
                    if (decision)
                    {
                        child->usedNeurons.at(id) = layerIndex;
                    }
                }
            }
            // if it did not match, it must be a new one and we have to insert it
            else
            {
                child->usedNeurons.insert_or_assign(id, layerIndex);
            }
        }

        // at this point we need to check if child contains any empty layer
        // if yes we need to shift every layer
        for (unsigned layerIndex = 0; layerIndex < dominantParent->numberOfLayersUsed; ++layerIndex)
        {
            unsigned count = 0;
            for (const auto &[id, index] : child->usedNeurons)
            {
                if (index == layerIndex)
                    count++;
            }
            // at this point if count is zero we know layer with index = layerIndex is empty
            // then shift all layers after it by -1;
            if (count == 0)
            {
                bool removedSomething = false;
                for (auto &[id, index] : child->usedNeurons)
                {
                    if (index > layerIndex)
                    {
                        index--;
                        removedSomething = true;
                    }
                }
                // at this point record the fact that we removed a layer from child
                if (removedSomething)
                    child->numberOfLayersUsed--;
            }

            //
        }

        // child must use all synapseIndeces used by father and mother
        for (const auto &[id, synapse] : nonDominantParent->genes)
        {
            const auto matched = dominantParent->genes.count(id);
            if (matched)
            {
                // decide whether to use nondominant's gene
                auto decision = random_bool.generate();
                if (decision)
                {
                    // use nondominant's gene
                    child->genes.at(id) = synapse;
                }
            }
            // if it did not match, it must be a new one and we have to insert it
            else
            {
                child->genes.insert_or_assign(id, synapse);
            }
        }
        return child;
    }

public: // mutations
    void change_random_weight(void)
    {
        const auto &randomShift = random_U32.generate(0, genes.size() - 1);
        const auto &randomSynapseIterator = std::next(genes.begin(), randomShift);

        randomSynapseIterator->second.second = random_float();
    }
    void add_new_random_synapse(const std::unordered_map<NeuronID, Neuron *> &_neuronPool)
    {
        const auto &randomShift1 = random_U32.generate(0, usedNeurons.size() - 1);
        const auto &neuronIDIterator1 = std::next(usedNeurons.begin(), randomShift1);

        const auto &randomShift2 = random_U32.generate(0, usedNeurons.size() - 1);
        const auto &neuronIDIterator2 = std::next(usedNeurons.begin(), randomShift2);

        const auto newSynapseId = std::make_pair(neuronIDIterator1->first, neuronIDIterator2->first);
        const auto newSynapse = Synapse(_neuronPool.at(neuronIDIterator2->first), random_float());

        // attempt to insert it, if already there then change its weight
        genes.insert_or_assign(newSynapseId, newSynapse);
    }

    void evolve_random_synapse(Neuron *_newPtr, const std::unordered_map<NeuronID, Neuron *> &_neuronPool)
    {
        const auto whatNewNeuronsIDWouldBe = usedNeurons.size();

        const auto &randomShift = random_U32.generate(0, genes.size() - 1);
        const auto &randomSynapseIterator = std::next(genes.begin(), randomShift);

        // DISABLE IT (make weight 0)
        randomSynapseIterator->second.second = 0.f;

        // extract the information about where it comes from and whre it goes
        const auto startingNeuronID = randomSynapseIterator->first.first;
        const auto endingNeuronID = randomSynapseIterator->first.second;

        const auto startingNeuronPtr = _neuronPool.at(startingNeuronID);
        const auto endingNeuronPtr = _neuronPool.at(endingNeuronID);

        // make two new syanpseids and corresponding synapses with this info
        const auto newSynapseId1 = std::make_pair(startingNeuronID, whatNewNeuronsIDWouldBe);
        const auto newSynapse1 = Synapse(_newPtr, random_float());
        const auto newGene1 = std::make_pair(newSynapseId1, newSynapse1);

        const auto newSynapseId2 = std::make_pair(whatNewNeuronsIDWouldBe, endingNeuronID);
        const auto newSynapse2 = Synapse(endingNeuronPtr, random_float());
        const auto newGene2 = std::make_pair(newSynapseId2, newSynapse2);

        // compute what new neuron's LayerIndex will be
        auto newLayerIndex = (startingNeuronPtr->g_layerIndex + endingNeuronPtr->g_layerIndex + 1) / 2;
        //  if it wants layer 0, we cant allow that
        if (newLayerIndex == 0)
        {
            newLayerIndex++;
        }

        // decide whether we need to insert a layer
        // we insert  a layer if the starting neuron and eding neuron are only one layer apart
        // OR the layer it wants is the output layer
        const int startingLayerIndex = usedNeurons.at(startingNeuronID);
        const int endingLayerIndex = usedNeurons.at(endingNeuronID);

        auto needLayer = std::abs(startingLayerIndex - endingLayerIndex) == 1 or (newLayerIndex == numberOfLayersUsed - 1);
        if (needLayer)
        {
            // we need to shift all following layer neuron's layerindex
            for (auto &[id, layerIndex] : usedNeurons)
            {
                if (layerIndex >= newLayerIndex)
                    layerIndex++; // basically we shift the output layer by one unit to the right
            }
            numberOfLayersUsed++;
        }

        // add the new genes and the newly used neurons ids in their proper containers
        usedNeurons.insert_or_assign(whatNewNeuronsIDWouldBe, newLayerIndex);

        genes.insert(newGene1);
        genes.insert(newGene2);
    }
};

std::set<NeuronID> Genome::m_inputNeuronIDs = std::set<NeuronID>();
std::set<NeuronID> Genome::m_outputNeuronIDs = std::set<NeuronID>();