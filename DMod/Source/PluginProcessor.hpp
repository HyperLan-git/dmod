#pragma once

class DModAudioProcessorEditor;

#include "JuceHeader.h"

#include "PluginEditor.hpp"

// 100ms so 3 oscillations at 30hz or 100 at 1khz
#define MAX_DELAY_SAMPLES 4410

class DModAudioProcessor : public juce::AudioProcessor {
   public:
    DModAudioProcessor();
    ~DModAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    inline juce::AudioParameterFloat* getModParam() { return modulation; }

   private:
    juce::AudioProcessor::BusesProperties createProperties();

    juce::AudioParameterFloat* modulation;

    float block_l[MAX_DELAY_SAMPLES] = {}, block_r[MAX_DELAY_SAMPLES] = {};
    float prev_l[MAX_DELAY_SAMPLES] = {}, prev_r[MAX_DELAY_SAMPLES] = {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DModAudioProcessor)
};
