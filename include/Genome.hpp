#pragma once
#include "Neuron.hpp"

class Genome
{
public:
    std::unordered_map<NeuronID, LayerIndex> usedNeurons; // cannot permit repetitions
    // std::set<SynapseID> usedSynapses; // cannot permit repetitions
    std::unordered_map<SynapseID, Synapse> genes; // cannot permit repetitions

    unsigned numberOfLayersUsed;

    unsigned numberOfFoodsEaten = 0;

    double score = 0.0;

private:
    // static std::set<NeuronID> m_inputNeuronIDs;  // need to keep track to not mess up the input layer
    // static std::set<NeuronID> m_outputNeuronIDs; // need to keep track to not mess up the output layer

public:
    Genome(void) {}
    // this constructor is called only when starting the species
   explicit  Genome(const std::unordered_map<NeuronID, Neuron *> &_neuronPool) noexcept
    {
        numberOfLayersUsed = 2;
        // fully connect input to output layer
        for (unsigned inputIndex = 0; inputIndex < NUMBER_OF_INPUTS; ++inputIndex)
        {
            const auto sensorNeuronID = inputIndex;
            usedNeurons.insert_or_assign(sensorNeuronID, 0);
            // m_inputNeuronIDs.insert(sensorNeuronID);

            for (unsigned outputIndex = 0; outputIndex < NUMBER_OF_OUTPUTS; ++outputIndex)
            {
                const auto motorNeuronID = NUMBER_OF_INPUTS + outputIndex;
                // since it is a map, duplicates are already taken care of
                usedNeurons.insert_or_assign(motorNeuronID, 1);
                // m_outputNeuronIDs.insert(motorNeuronID);

                const auto id = SynapseID(sensorNeuronID, motorNeuronID);
                const auto synapse = Synapse(_neuronPool.at(motorNeuronID), random_double());

                genes.insert(std::make_pair(id, synapse));
            }
        }
    }

public:
    Genome cross(const Genome &_other) const
    {
        const Genome *dominantParent;
        const Genome *nonDominantParent;

        // select the dominant parent
        if (this->numberOfLayersUsed >= _other.numberOfLayersUsed)
        {
            dominantParent = this;
            nonDominantParent = &_other;
        }
        else
        {
            dominantParent = &_other;
            nonDominantParent = this;
        }
        auto child = *dominantParent; // must be a copy operation

        // we keep the dominant parent's layer choice and add extra neurons nondominant parent may have
        for (const auto &[id, layerIndex] : nonDominantParent->usedNeurons)
        {
            if (not child.usedNeurons.count(id))
            {
                // this means child does not have this neuron and must use it
                child.usedNeurons.insert_or_assign(id, layerIndex);
            }
        }

        // child must use all synapseIndeces used by father and mother
        for (const auto &[id, synapse] : nonDominantParent->genes)
        {
            if (child.genes.count(id))
            {
                // decide whether to use nondominant's gene
                const auto decision = random_bool();
                if (decision)
                {
                    // use nondominant's gene
                    child.genes.at(id) = synapse;
                }
            }
            // if it did not match, it must be a new one and we have to insert it
            else
            {
                child.genes.insert_or_assign(id, synapse);
            }
        }
        return child;
    }

public: // mutations
    void change_random_weight(void)
    {
        const auto &randomShift = random_U32.generate(0, genes.size() - 1);
        const auto &randomSynapseIterator = std::next(genes.begin(), randomShift);

        randomSynapseIterator->second.second = random_double();
    }
    void add_new_random_synapse(const std::unordered_map<NeuronID, Neuron *> &_neuronPool)
    {
        const auto &randomShift1 = random_U32.generate(0, usedNeurons.size() - 1);
        const auto &neuronIDIterator1 = std::next(usedNeurons.begin(), randomShift1);

        const auto &randomShift2 = random_U32.generate(0, usedNeurons.size() - 1);
        const auto &neuronIDIterator2 = std::next(usedNeurons.begin(), randomShift2);

        const auto newSynapseId = std::make_pair(neuronIDIterator1->first, neuronIDIterator2->first);
        const auto newSynapse = Synapse(_neuronPool.at(neuronIDIterator2->first), random_double());

        // attempt to insert it, if already there then change its weight
        genes.insert_or_assign(newSynapseId, newSynapse);
    }

    void evolve_random_synapse(Neuron *_newPtr, const std::unordered_map<NeuronID, Neuron *> &_neuronPool)
    {
        const auto whatNewNeuronsIDWouldBe = usedNeurons.size();

        const auto &randomShift = random_U32.generate(0, genes.size() - 1);
        const auto &randomSynapseIterator = std::next(genes.begin(), randomShift);

        const auto oldWeight = randomSynapseIterator->second.second;

        // disable it
        randomSynapseIterator->second.second = 0;

        // extract the information about where it comes from and whre it goes
        const auto startingNeuronID = randomSynapseIterator->first.first;
        const auto endingNeuronID = randomSynapseIterator->first.second;

        // remove it altogether
        // genes.erase(randomSynapseIterator->first);

        // const auto startingNeuronPtr = _neuronPool.at(startingNeuronID);
        const auto endingNeuronPtr = _neuronPool.at(endingNeuronID);

        // make two new syanpse ids and corresponding synapses with this info
        const auto newSynapseId1 = std::make_pair(startingNeuronID, whatNewNeuronsIDWouldBe);
        const auto newSynapse1 = Synapse(_newPtr, 1.f);
        const auto newGene1 = std::make_pair(newSynapseId1, newSynapse1);

        const auto newSynapseId2 = std::make_pair(whatNewNeuronsIDWouldBe, endingNeuronID);
        const auto newSynapse2 = Synapse(endingNeuronPtr, oldWeight);
        const auto newGene2 = std::make_pair(newSynapseId2, newSynapse2);

        const auto startingLayerIndex = usedNeurons.at(startingNeuronID);
        const auto endingLayerIndex = usedNeurons.at(endingNeuronID);

        // compute what new neuron's LayerIndex will be
        auto newLayerIndex = (startingLayerIndex + endingLayerIndex + 1) / 2;
        //  if it wants layer 0, we cant allow that
        if (newLayerIndex == 0)
        {
            newLayerIndex++;
        }

        // decide whether we need to insert a layer
        // we insert  a layer if the starting neuron and eding neuron are only one layer apart
        // OR the layer it wants is the output layer

        auto needLayer = startingLayerIndex - endingLayerIndex == 1 or endingLayerIndex - startingLayerIndex == 1 or (newLayerIndex == numberOfLayersUsed - 1);
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

// std::set<NeuronID> Genome::m_inputNeuronIDs = std::set<NeuronID>();
// std::set<NeuronID> Genome::m_outputNeuronIDs = std::set<NeuronID>();