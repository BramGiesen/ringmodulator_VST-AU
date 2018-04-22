/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "sineWave.h"
#include "sawWave.h"
#include "randomGen.h"

//==============================================================================

class RingModulatorAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    RingModulatorAudioProcessor();
    ~RingModulatorAudioProcessor();
    
    //==============================================================================
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;
    
    //==============================================================================
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override
    {
        jassert (! isUsingDoublePrecision());
        process (buffer, midiMessages, delayBufferFloat);
    }
    
    void processBlock (AudioBuffer<double>& buffer, MidiBuffer& midiMessages) override
    {
        jassert (isUsingDoublePrecision());
        process (buffer, midiMessages, delayBufferDouble);
    }
    
    //==============================================================================
    bool hasEditor() const override                                             { return true; }
    AudioProcessorEditor* createEditor() override;
    
    //==============================================================================
    const String getName() const override                                       { return JucePlugin_Name; }
    
    bool acceptsMidi() const override                                           { return true; }
    bool producesMidi() const override                                          { return true; }
    
    double getTailLengthSeconds() const override                                { return 0.0; }
    
    //==============================================================================
    int getNumPrograms() override                                               { return 0; }
    int getCurrentProgram() override                                            { return 0; }
    void setCurrentProgram (int /*index*/) override                             {}
    const String getProgramName (int /*index*/) override                        { return String(); }
    void changeProgramName (int /*index*/, const String& /*name*/) override     {}
    
    //==============================================================================
    void getStateInformation (MemoryBlock&) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    void updateTrackProperties (const TrackProperties& properties) override;
    
    void setFrequency();
    void setWaveForm();
    
    void applyAmplitude();
    
    
    //==============================================================================
 
    AudioPlayHead::CurrentPositionInfo lastPosInfo;
    

    int lastUIWidth = 400, lastUIHeight = 200;
    
    // Our parameters
    AudioParameterFloat* frequencyParam = nullptr;
    AudioParameterFloat* amplitudeParam = nullptr;
    AudioParameterFloat* LFOfrequencyParam = nullptr;
    AudioParameterFloat* LFOdepthParam = nullptr;
    AudioParameterChoice* stereoParam;
    
    // Current track colour and name
    TrackProperties trackProperties;
    
private:
    //==============================================================================
    template <typename FloatType>
    void process (AudioBuffer<FloatType>& buffer, MidiBuffer& midiMessages, AudioBuffer<FloatType>& delayBuffer);

    
    AudioBuffer<float> delayBufferFloat;
    AudioBuffer<double> delayBufferDouble;
    
    double phase = 0;
    double sineWave = 0;
    double LFO = 0;
    float amplitudeWet = 0;
    float amplitudeDry = 0;
    int delayPosition = 0;
    int previousI = 1;

    // #TODO: make sampleRate variable
    Oscillator **oscillators;
    
    void updateCurrentTimeInfoFromHost();
    static BusesProperties getBusesProperties();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RingModulatorAudioProcessor)
};

