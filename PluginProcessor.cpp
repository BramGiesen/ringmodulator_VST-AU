/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"


AudioProcessor* JUCE_CALLTYPE createPluginFilter();


//==============================================================================
RingModulatorAudioProcessor::RingModulatorAudioProcessor()
: AudioProcessor (getBusesProperties())
{
    lastPosInfo.resetToDefault();
    addParameter (inputVolumeParam = new AudioParameterFloat ("GLIDE",  "LFO-glide", 0.01f, 10.f, 0.01f));
    addParameter (frequencyParam  = new AudioParameterFloat ("Frequency",  "Frequency", 0.0f, 1200.0f, 0.9f));
    addParameter (amplitudeParam = new AudioParameterFloat ("Dry/Wet", "Dry/Wet", 0.0f, 1.0f, 0.5f));
    addParameter (LFOfrequencyParam  = new AudioParameterFloat ("LFO-Frequency",  "LFO-Frequency", 0.0f, 4.0f,0.9f));
    addParameter (LFOdepthParam = new AudioParameterFloat ("LFO-depth", "LFO-depth", 0.0f, 100.0f, 0.5f));
    addParameter (stereoParam   = new AudioParameterChoice ("Wave form", "LFO-wave form", { "saw wave", "noise generator" }, 1));
}

RingModulatorAudioProcessor::~RingModulatorAudioProcessor()
{
   
}


//==============================================================================
bool RingModulatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Only mono/stereo and input/output must have same layout
    const AudioChannelSet& mainOutput = layouts.getMainOutputChannelSet();
    const AudioChannelSet& mainInput  = layouts.getMainInputChannelSet();
    
    // input and output layout must either be the same or the input must be disabled altogether
    if (! mainInput.isDisabled() && mainInput != mainOutput)
        return false;
    
    // do not allow disabling the main buses
    if (mainOutput.isDisabled())
        return false;
    
    // only allow stereo and mono
    if (mainOutput.size() > 2)
        return false;
    
    return true;
}

AudioProcessor::BusesProperties RingModulatorAudioProcessor::getBusesProperties()
{
    return BusesProperties().withInput  ("Input",  AudioChannelSet::stereo(), true)
    .withOutput ("Output", AudioChannelSet::stereo(), true);
}

//==============================================================================
void RingModulatorAudioProcessor::prepareToPlay (double newSampleRate, int /*samplesPerBlock*/)
{
    oscillators = new Oscillator*[2];
    oscillators[0] = new SineWave(44100, 0, phase);
    oscillators[1] = new RandomGenerator(44100, 0, phase);
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void RingModulatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void RingModulatorAudioProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
}



template <typename FloatType>
void RingModulatorAudioProcessor::process (AudioBuffer<FloatType>& buffer,
                                            MidiBuffer& midiMessages,
                                            AudioBuffer<FloatType>& delayBuffer)
{
    const int numSamples = buffer.getNumSamples();
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    

    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, numSamples);
    
    
    
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
  
    for(int sample = 0; sample < buffer.getNumSamples(); ++sample){
        
        for (int channel = 0; channel< totalNumInputChannels; ++channel)
        {
            auto channelData = buffer.getWritePointer (channel);
            float  signal = buffer.getSample(channel, sample);

            LFO = lowPass->process(oscillators[1]->getSample()* *LFOdepthParam);
            sineWave = oscillators[0]->getSample();
            oscillators[0]->tick();
            oscillators[1]->tick();
            
            if(*frequencyParam <= 0)sineWave = 1;
            channelData[sample] = (amplitudeWet * signal * sineWave) + (amplitudeDry * signal);
        }
    setFrequency();
        
    setWaveForm();
        
    applyAmplitude();
    
    updateCurrentTimeInfoFromHost();
}
}


void RingModulatorAudioProcessor::updateCurrentTimeInfoFromHost()
{
    if (AudioPlayHead* ph = getPlayHead())
    {
        AudioPlayHead::CurrentPositionInfo newTime;
        
        if (ph->getCurrentPosition (newTime))
        {
            lastPosInfo = newTime;
            return;
        }
    }
    
    lastPosInfo.resetToDefault();
}

//==============================================================================
AudioProcessorEditor* RingModulatorAudioProcessor::createEditor()
{
    return new RingModulatorAudioProcessorEditor (*this);
}

void RingModulatorAudioProcessor::setFrequency()
{
//    lowpassParam = lowPass->process(*LFOfrequencyParam);
    lowPass->setFc(*inputVolumeParam/44100);
    oscillators[0]->setFrequency(*frequencyParam + LFO);
    oscillators[1]->setFrequency(*LFOfrequencyParam);
}

void RingModulatorAudioProcessor::setWaveForm()
{
    int i = stereoParam->getIndex() + 1;
    if(i != previousI){
        switch (i) {
            case 1: { oscillators[1] = new SawWave(44100, 0, phase);
                break; }
            case 2:{ oscillators[1] = new RandomGenerator(44100, 0, phase);   //execution starts at this case label
                break; }
        }
        previousI = i;
    }
    
}

void RingModulatorAudioProcessor::applyAmplitude()
{
    //crossfade
    amplitudeWet = *amplitudeParam;
    amplitudeDry = (amplitudeWet * -1.0f) + 1.0f;
}

//==============================================================================
void RingModulatorAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:
    
    // Create an outer XML element..
    XmlElement xml ("MYPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute ("uiWidth", lastUIWidth);
    xml.setAttribute ("uiHeight", lastUIHeight);
    
    // Store the values of all our parameters, using their param ID as the XML attribute
    for (auto* param : getParameters())
        if (auto* p = dynamic_cast<AudioProcessorParameterWithID*> (param))
            xml.setAttribute (p->paramID, p->getValue());
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void RingModulatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("MYPLUGINSETTINGS"))
        {
            // ok, now pull out our last window size..
            lastUIWidth  = jmax (xmlState->getIntAttribute ("uiWidth", lastUIWidth), 400);
            lastUIHeight = jmax (xmlState->getIntAttribute ("uiHeight", lastUIHeight), 200);
            
            // Now reload our parameters..
            for (auto* param : getParameters())
                if (auto* p = dynamic_cast<AudioProcessorParameterWithID*> (param))
                    p->setValue ((float) xmlState->getDoubleAttribute (p->paramID, p->getValue()));
        }
    }
}

void RingModulatorAudioProcessor::updateTrackProperties (const TrackProperties& properties)
{
    trackProperties = properties;
    
    if (auto* editor = dynamic_cast<RingModulatorAudioProcessorEditor*> (getActiveEditor()))
        editor->updateTrackProperties ();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RingModulatorAudioProcessor();
}

