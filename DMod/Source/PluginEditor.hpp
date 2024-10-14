#pragma once

class DModAudioProcessor;

#include "JuceHeader.h"

#include "PluginProcessor.hpp"
#include "KnobComponent.hpp"

// TODO indication of volume level and clipping

class DModAudioProcessorEditor : public juce::AudioProcessorEditor {
   public:
    DModAudioProcessorEditor(DModAudioProcessor& editor);
    ~DModAudioProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

   private:
    DModAudioProcessor& audioProcessor;

    KnobComponent mod;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DModAudioProcessorEditor)
};
