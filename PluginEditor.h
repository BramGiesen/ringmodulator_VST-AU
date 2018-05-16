/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <iostream>


//==============================================================================
class RingModulatorAudioProcessorEditor  : public AudioProcessorEditor,
private Timer, private ComboBox::Listener
{
public:
    RingModulatorAudioProcessorEditor (RingModulatorAudioProcessor&);
    ~RingModulatorAudioProcessorEditor();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void hostMIDIControllerIsAvailable (bool) override;
    void updateTrackProperties();
    
private:
    void comboBoxChanged (ComboBox*) override;
    void setBPM(AudioPlayHead::CurrentPositionInfo);
    class ParameterSlider;
    
    double beats = 0;
    
    RingModulatorAudioProcessor& processor;
    
    Label timecodeDisplayLabel, LFOglideLabel, frequencyLabel, amplitudeLabel, LFOfrequencyLabel, LFOdepthLabel, waveFormLabel;
    ScopedPointer<ParameterSlider> glideSlider, frequencySlider, amplitudeSlider, LFOfrequencySlider, LFOdepthSlider;
    ComboBox waveFormBox;
    Colour backgroundColour;
    std::string rateValues[11]{"32","32t","16","16t","8","8t","4","4t","2","2t","1"};
    std::string LFOf;
    //==============================================================================
    RingModulatorAudioProcessor& getProcessor() const
    {
        return static_cast<RingModulatorAudioProcessor&> (processor);
    }
    
};

