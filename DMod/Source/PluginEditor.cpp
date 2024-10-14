#include "PluginEditor.hpp"

DModAudioProcessorEditor::DModAudioProcessorEditor(DModAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), mod(p.getModParam()) {
    setSize(400, 300);

    this->addAndMakeVisible(mod);
}

DModAudioProcessorEditor::~DModAudioProcessorEditor() {}

void DModAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(15.0f));
}

void DModAudioProcessorEditor::resized() { mod.setBounds(0, 0, 100, 100); }
