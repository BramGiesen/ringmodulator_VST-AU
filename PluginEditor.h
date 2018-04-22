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
/** This is the editor component that our filter will display.
 */
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
    class ParameterSlider;
    
    RingModulatorAudioProcessor& processor;
    
    Label timecodeDisplayLabel, frequencyLabel, amplitudeLabel, LFOfrequencyLabel, LFOdepthLabel, stereoLabel;
    ScopedPointer<ParameterSlider> frequencySlider, delaySlider, LFOfrequencySlider, LFOdepthSlider;
    ComboBox stereoBox;
    Colour backgroundColour;
    
    //==============================================================================
    RingModulatorAudioProcessor& getProcessor() const
    {
        return static_cast<RingModulatorAudioProcessor&> (processor);
    }
    
    void updateTimecodeDisplay (AudioPlayHead::CurrentPositionInfo);
};

