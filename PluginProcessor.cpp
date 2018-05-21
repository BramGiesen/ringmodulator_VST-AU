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
    addParameter (glideParam = new AudioParameterFloat ("GLIDE",  "LFO_glide", 0.1f, 10.f, 0.1f));
    addParameter (frequencyParam  = new AudioParameterFloat ("Frequency",  "Frequency", 0.0f, 1200.0f, 0.9f));
    addParameter (LFOfrequencyParam  = new AudioParameterFloat ("LFO_Frequency",  "LFO_Frequency", -10.0f, 10.0f,0.0f));
    addParameter (LFOdepthParam = new AudioParameterFloat ("LFO_depth", "LFO_depth", 0.0f, 100.0f, 0.5f));
    addParameter (waveFormParam   = new AudioParameterChoice ("Wave_form", "LFO_wave form", { "rising saw", "falling saw", "sine", "square", "noise generator" }, 4));
    addParameter (amplitudeParam = new AudioParameterFloat ("Dry_Wet", "Dry_Wet", 0.0f, 1.0f, 0.5f));
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
    sampleRate = newSampleRate;
    
    oscillators = new Oscillator*[2];
    oscillators[0] = new SineWave(sampleRate, 0, phase);
    oscillators[1] = new RandomGenerator(sampleRate, 0, phase);
 
}

void RingModulatorAudioProcessor::releaseResources()
{
   
}

void RingModulatorAudioProcessor::reset()
{
  
}



template <typename FloatType>
void RingModulatorAudioProcessor::process (AudioBuffer<FloatType>& buffer,
                                            MidiBuffer& midiMessages)
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
            
            sineWave = oscillators[0]->getSample();
            oscillators[0]->tick();
            oscillators[1]->tick();
            
            if(*frequencyParam <= 0)sineWave = 1;
            
            channelData[sample] = (amplitudeWet * signal * sineWave) + (amplitudeDry * signal);
        }
    
    setBPM(lastPosInfo);
        
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
    glide = *glideParam * -1 + 10.1;
    (glide < 10) ? LFO = lowPass->process(oscillators[1]->getSample()* *LFOdepthParam) : LFO = oscillators[1]->getSample()* *LFOdepthParam;
    lowPass->setFc(glide/sampleRate);
    
    oscillators[0]->setFrequency(*frequencyParam + LFO);

    
        if(*LFOfrequencyParam < 0){
            LFOP = *LFOfrequencyParam - 1;
            LFOI = int(*LFOfrequencyParam);
            LFOf = rateValues[(LFOI * -1)];
            syncfreq = beats/LFOf;
            oscillators[1]->setFrequency(syncfreq);
            
            if(LFOI != previousLFOfreq){
                if(!setPhaseSwitch){
                    setPhaseSwitch = true;
                    setOSCphase(lastPosInfo);
                }
                else{

                    previousLFOfreq = LFOI;
                    setPhaseSwitch = false;
                }
            }
            else {
                previousLFOfreq = LFOI;
            }
        }else{
            if(*LFOfrequencyParam < 0 && *LFOfrequencyParam > -0.5){ oscillators[1]->setFrequency(0); }
            else{
                oscillators[1]->setFrequency(*LFOfrequencyParam);
                }
            }
}

void RingModulatorAudioProcessor::setWaveForm()
{
    int i = waveFormParam->getIndex() + 1;
    if(i != previousI || !initWaveform){
        initWaveform = true;
        switch (i) {
            case 1: { oscillators[1] = new SawWave(sampleRate, 0, phase);
                break; }
            case 2: { oscillators[1] = new invertedSawWave(sampleRate, 0, phase);
                break; }
            case 3: { oscillators[1] = new SineWave(sampleRate, 0, phase);
                break; }
            case 4: { oscillators[1] = new SquareWave(sampleRate, 0, phase);
                break; }
            case 5: { oscillators[1] = new RandomGenerator(sampleRate, 0, phase);   //execution starts at this case label
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


void RingModulatorAudioProcessor::setBPM(AudioPlayHead::CurrentPositionInfo bpm)
{
    beats = bpm.bpm;
    
    if(bpm.isLooping && (bpm.isPlaying || bpm.isRecording)){
        if(!startTimeSet){
            startLoop = bpm.timeInSeconds;
            startTimeSet = true;
        }
        if(bpm.timeInSeconds == startLoop)
            setOSCphase(bpm);
    }
    
    if (bpm.isPlaying){
        if(!setPhase){
            setPhase = true;
            setOSCphase(bpm);
        }
    }
    else if (!bpm.isPlaying && !bpm.isRecording){
        setPhase = false;
        startTimeSet = false;
    }
    
    
}

void RingModulatorAudioProcessor::setOSCphase(AudioPlayHead::CurrentPositionInfo bpm)
{
    double beatss = bpm.bpm;
    double phaseLenght = 60 / beatss;
    double timer = fmod(bpm.timeInSeconds,phaseLenght)/phaseLenght;
    oscillators[1]->setPhase(timer);
}


//==============================================================================
//store parameters in the memory block.
void RingModulatorAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    
    // Create an outer XML element..
    XmlElement xml ("RMSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute ("uiWidth", lastUIWidth);
    xml.setAttribute ("uiHeight", lastUIHeight);
    
    // Store the values of all our parameters, using their param ID as the XML attribute
    for (auto* param : getParameters()){
        if (auto* p = dynamic_cast<AudioProcessorParameterWithID*> (param)){
            xml.setAttribute (p->paramID, p->getValue());
        }
    }
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}


// restore the parameters from this memory block
void RingModulatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("RMSETTINGS"))
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

