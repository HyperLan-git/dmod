#include "PluginProcessor.hpp"

juce::AudioProcessor::BusesProperties DModAudioProcessor::createProperties() {
    return juce::AudioProcessor::BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withInput("Modulator", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true);
}

DModAudioProcessor::DModAudioProcessor()
    :
#ifndef JucePlugin_PreferredChannelConfigurations
      AudioProcessor(createProperties()),
#endif
      modulation(new juce::AudioParameterFloat(
          {"Modulation", 1}, "Modulation amount", 0.f, 1.f, 1.f)) {
    addParameter(modulation);
}

DModAudioProcessor::~DModAudioProcessor() {}

const juce::String DModAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool DModAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DModAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool DModAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double DModAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int DModAudioProcessor::getNumPrograms() { return 1; }

int DModAudioProcessor::getCurrentProgram() { return 0; }

void DModAudioProcessor::setCurrentProgram(int index) { (void)index; }

const juce::String DModAudioProcessor::getProgramName(int index) {
    (void)index;
    return {};
}

void DModAudioProcessor::changeProgramName(int index,
                                           const juce::String& newName) {
    (void)index;
    (void)newName;
}

void DModAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    (void)sampleRate;
    (void)samplesPerBlock;
}

void DModAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DModAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void DModAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                      juce::MidiBuffer& midiMessages) {
    (void)midiMessages;

    juce::ScopedNoDenormals noDenormals;
    const int inputs = getTotalNumInputChannels();
    const int outputs = getTotalNumOutputChannels();
    const int samples = buffer.getNumSamples();

    if (inputs == 0) return;
    if (outputs == 0) return;
    if (outputs >= inputs) return;

    if (inputs == 2) {
        return;
    }

    float *l = buffer.getWritePointer(0), *r = buffer.getWritePointer(1);
    const float *l2 = buffer.getReadPointer(2), *r2 = buffer.getReadPointer(3);

    const float mod = *modulation;

    if (samples >= MAX_DELAY_SAMPLES) {
        for (int b = 0; b < samples; b += MAX_DELAY_SAMPLES) {
            if (samples - b < MAX_DELAY_SAMPLES) {
                const int sz = samples - b;
                std::memmove(block_l, block_l + sz,
                             (MAX_DELAY_SAMPLES - sz) * sizeof(float));
                std::memcpy(block_l + (MAX_DELAY_SAMPLES - sz), l + b,
                            sz * sizeof(float));
                std::memmove(block_r, block_r + sz,
                             (MAX_DELAY_SAMPLES - sz) * sizeof(float));
                std::memcpy(block_r + (MAX_DELAY_SAMPLES - sz), r + b,
                            sz * sizeof(float));
            } else {
                std::memcpy(block_l, l + b, MAX_DELAY_SAMPLES * sizeof(float));
                std::memcpy(block_r, r + b, MAX_DELAY_SAMPLES * sizeof(float));
            }

            for (int i = 0; i < MAX_DELAY_SAMPLES; i++) {
                const float sampleChosen_l =
                                (float)i +
                                MAX_DELAY_SAMPLES * mod * l2[i] / 2.f -
                                MAX_DELAY_SAMPLES / 2,
                            sampleChosen_r =
                                (float)i +
                                MAX_DELAY_SAMPLES * mod * r2[i] / 2.f -
                                MAX_DELAY_SAMPLES / 2;

                const int fl = (int)std::floor(sampleChosen_l),  // FL studio real
                    fr = (int)std::floor(sampleChosen_r);        // French
                if (sampleChosen_l <= -MAX_DELAY_SAMPLES) {
                    l[i] = prev_l[0];
                } else if (sampleChosen_l >= MAX_DELAY_SAMPLES - 1) {
                    l[i] = block_l[MAX_DELAY_SAMPLES - 1];
                } else if (sampleChosen_l >= -1 && sampleChosen_l <= 0) {
                    l[i] = prev_l[MAX_DELAY_SAMPLES - 1] *
                               (1 - sampleChosen_l + fl) +
                           block_l[0] * (sampleChosen_l - fl);
                } else if (sampleChosen_l < -1) {
                    l[i] = prev_l[fl + MAX_DELAY_SAMPLES] *
                               (1 - sampleChosen_l + fl) +
                           prev_l[fl + 1 + MAX_DELAY_SAMPLES] *
                               (sampleChosen_l - fl);
                } else {
                    l[i] = block_l[fl] * (1 - sampleChosen_l + fl) +
                           block_l[fl + 1] * (sampleChosen_l - fl);
                }

                if (sampleChosen_r <= -MAX_DELAY_SAMPLES) {
                    r[i] = prev_r[0];
                } else if (sampleChosen_r >= MAX_DELAY_SAMPLES - 1) {
                    r[i] = block_r[MAX_DELAY_SAMPLES - 1];
                } else if (sampleChosen_r >= -1 && sampleChosen_r <= 0) {
                    r[i] = prev_r[MAX_DELAY_SAMPLES - 1] * (-sampleChosen_r) +
                           block_r[0] * (sampleChosen_r + 1);
                } else if (sampleChosen_r < -1) {
                    r[i] = prev_r[fr + MAX_DELAY_SAMPLES] *
                               (1 - sampleChosen_r + fr) +
                           prev_r[fr + 1 + MAX_DELAY_SAMPLES] *
                               (sampleChosen_r - fr);
                } else {
                    r[i] = block_r[fr] * (1 - sampleChosen_r + fr) +
                           block_r[fr + 1] * (sampleChosen_r - fr);
                }
            }

            std::memcpy(prev_l, block_l, MAX_DELAY_SAMPLES * sizeof(float));
            std::memcpy(prev_r, block_r, MAX_DELAY_SAMPLES * sizeof(float));
        }
        return;
    }
    const int sz = MAX_DELAY_SAMPLES - samples;
    std::memmove(block_l, block_l + samples, sz * sizeof(float));
    std::memcpy(block_l + sz, l, samples * sizeof(float));
    std::memmove(block_r, block_r + samples, sz * sizeof(float));
    std::memcpy(block_r + sz, r, samples * sizeof(float));

    for (int i = 0; i < samples; i++) {
        const float sampleChosen_l = (float)i +
                                     MAX_DELAY_SAMPLES * mod * l2[i] / 2.f -
                                     MAX_DELAY_SAMPLES / 2,
                    sampleChosen_r = (float)i +
                                     MAX_DELAY_SAMPLES * mod * r2[i] / 2.f -
                                     MAX_DELAY_SAMPLES / 2;
        const int fl = (int)std::floor(sampleChosen_l),
                  fr = (int)std::floor(sampleChosen_r);
        if (sampleChosen_l >= samples) {
            l[i] = block_l[MAX_DELAY_SAMPLES - 1];
        } else if (sampleChosen_l <= -MAX_DELAY_SAMPLES) {
            l[i] = prev_l[0];
        } else if (sampleChosen_l <= -sz) {
            l[i] = prev_l[fl + MAX_DELAY_SAMPLES] * (1 - sampleChosen_l + fl) +
                   prev_l[fl + MAX_DELAY_SAMPLES + 1] * (sampleChosen_l - fl);
        } else {
            l[i] = block_l[fl + sz] * (1 - sampleChosen_l + fl) +
                   block_l[fl + 1 + sz] * (sampleChosen_l - fl);
        }

        if (sampleChosen_r >= samples) {
            r[i] = block_r[MAX_DELAY_SAMPLES - 1];
        } else if (sampleChosen_r <= -MAX_DELAY_SAMPLES) {
            r[i] = prev_r[0];
        } else if (sampleChosen_r <= -sz) {
            r[i] = prev_r[fr + MAX_DELAY_SAMPLES] * (1 - sampleChosen_r + fr) +
                   prev_r[fr + MAX_DELAY_SAMPLES + 1] * (sampleChosen_r - fr);
        } else {
            r[i] = block_r[fr + sz] * (1 - sampleChosen_r + fr) +
                   block_r[fr + 1 + sz] * (sampleChosen_r - fr);
        }
    }
    std::memcpy(prev_l, block_l, MAX_DELAY_SAMPLES * sizeof(float));
    std::memcpy(prev_r, block_r, MAX_DELAY_SAMPLES * sizeof(float));
}

bool DModAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* DModAudioProcessor::createEditor() {
    return new DModAudioProcessorEditor(*this);
}

void DModAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(GET_PARAM_NORMALIZED(modulation));
}

void DModAudioProcessor::setStateInformation(const void* data,
                                             int sizeInBytes) {
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes),
        false);
    modulation->setValueNotifyingHost(stream.readFloat());
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new DModAudioProcessor();
}
