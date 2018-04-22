/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>

//==============================================================================
// This is a handy slider subclass that controls an AudioProcessorParameter
// (may move this class into the library itself at some point in the future..)
class RingModulatorAudioProcessorEditor::ParameterSlider   : public Slider,
private Timer
{
public:
    ParameterSlider (AudioProcessorParameter& p)
    : Slider (p.getName (256)), param (p)
    {
//        setRange (0.0, 1.0, 0.0);
        startTimerHz (30);
        updateSliderPos();
    }
    
    void valueChanged() override        { param.setValueNotifyingHost ((float) Slider::getValue()); }
    
    void timerCallback() override       { updateSliderPos(); }
    
    void startedDragging() override     { param.beginChangeGesture(); }
    void stoppedDragging() override     { param.endChangeGesture();   }
    
    double getValueFromText (const String& text) override   { return param.getValueForText (text); }
    String getTextFromValue (double value) override         { return param.getText ((float) value, 1024); }
    
    void updateSliderPos()
    {
        const float newValue = param.getValue();
        
        if (newValue != (float) Slider::getValue() && ! isMouseButtonDown())
            Slider::setValue (newValue, NotificationType::dontSendNotification);
    }
    
    AudioProcessorParameter& param;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterSlider)
};

//==============================================================================
RingModulatorAudioProcessorEditor::RingModulatorAudioProcessorEditor (RingModulatorAudioProcessor& owner)
: AudioProcessorEditor (owner), processor (owner),
//midiKeyboard (owner.keyboardState, MidiKeyboardComponent::horizontalKeyboard),
timecodeDisplayLabel (String()),
frequencyLabel (String(), "Frequency :"),
amplitudeLabel (String(), "Dry/Wet :"),
LFOfrequencyLabel (String(), "LFO Frequency :"),
LFOdepthLabel (String(), "LFO depth :"),
stereoLabel (String(), "LFO Waveform :")
{
    // add some sliders..
    addAndMakeVisible (frequencySlider = new ParameterSlider (*owner.frequencyParam));
    frequencySlider->setSliderStyle (Slider::Rotary);
    frequencySlider-> setRange (0.0, 1.0, 0.0);
    
    addAndMakeVisible (LFOfrequencySlider = new ParameterSlider (*owner.LFOfrequencyParam));
    LFOfrequencySlider->setSliderStyle (Slider::Rotary);
    LFOfrequencySlider-> setRange (0.0, 1.0, 0.0);
    
    addAndMakeVisible (LFOdepthSlider = new ParameterSlider (*owner.LFOdepthParam));
    LFOdepthSlider->setSliderStyle (Slider::Rotary);
    LFOdepthSlider-> setRange (0.0, 1.0, 0.0);
    
    addAndMakeVisible (delaySlider = new ParameterSlider (*owner.amplitudeParam));
    delaySlider->setSliderStyle (Slider::Rotary);
    delaySlider-> setRange (0.0, 1.0, 0.0);
    
    addAndMakeVisible (stereoBox);
    
    auto i = 1;
    for (auto choice : processor.stereoParam->choices)
        stereoBox.addItem (choice, i++);
    
    stereoBox.addListener (this);
    stereoBox.setSelectedId (processor.stereoParam->getIndex() + 1);
   
    addAndMakeVisible (stereoLabel);
    stereoLabel.setJustificationType (Justification::centredLeft);
    stereoLabel.attachToComponent (&stereoBox, true);
    
    // add some labels for the sliders..
    frequencyLabel.attachToComponent (frequencySlider, false);
    frequencyLabel.setFont (Font (11.0f));
    
    LFOfrequencyLabel.attachToComponent (LFOfrequencySlider, false);
    LFOfrequencyLabel.setFont (Font (11.0f));
    
    LFOdepthLabel.attachToComponent (LFOdepthSlider, false);
    LFOdepthLabel.setFont (Font (11.0f));
    
    amplitudeLabel.attachToComponent (delaySlider, false);
    amplitudeLabel.setFont (Font (11.0f));
    
//     add a label that will display the current timecode and status..
//    addAndMakeVisible (timecodeDisplayLabel);
    timecodeDisplayLabel.setFont (Font (Font::getDefaultMonospacedFontName(), 15.0f, Font::plain));
    
    // set resize limits for this plug-in
    setResizeLimits (700, 150, 1024, 700);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize (owner.lastUIWidth,
             owner.lastUIHeight);
    
    updateTrackProperties();
    
    // start a timer which will keep our timecode display updated
    startTimerHz (30);
}

RingModulatorAudioProcessorEditor::~RingModulatorAudioProcessorEditor()
{
}

//==============================================================================
void RingModulatorAudioProcessorEditor::paint (Graphics& g)
{
    g.setColour (backgroundColour);
    g.fillAll();
}

void RingModulatorAudioProcessorEditor::resized()
{
    // This lays out our child components...
    
    Rectangle<int> r (getLocalBounds().reduced (8));
    
    timecodeDisplayLabel.setBounds (r.removeFromTop (26));
    
    r.removeFromTop (20);
    Rectangle<int> sliderArea (r.removeFromTop (60));
    frequencySlider->setBounds (sliderArea.removeFromLeft (jmin (180, sliderArea.getWidth() / 2)));
    LFOfrequencySlider->setBounds (sliderArea.removeFromLeft (jmin (180, sliderArea.getWidth() / 2)));
    LFOdepthSlider->setBounds (sliderArea.removeFromLeft (jmin (180, sliderArea.getWidth() / 2)));
    delaySlider->setBounds (sliderArea.removeFromLeft (jmin (180, sliderArea.getWidth())));
    
    stereoBox.setBounds (80, 1, 200, 18);
   
   
    getProcessor().lastUIWidth = getWidth();
    getProcessor().lastUIHeight = getHeight();
}

//==============================================================================
void RingModulatorAudioProcessorEditor::timerCallback()
{
    updateTimecodeDisplay (getProcessor().lastPosInfo);
}

void RingModulatorAudioProcessorEditor::hostMIDIControllerIsAvailable (bool controllerIsAvailable)
{
//    midiKeyboard.setVisible (! controllerIsAvailable);
}

void RingModulatorAudioProcessorEditor::updateTrackProperties ()
{
    auto trackColour = getProcessor().trackProperties.colour;
    auto& lf = getLookAndFeel();
    
    backgroundColour = (trackColour == Colour() ? lf.findColour (ResizableWindow::backgroundColourId)
                        : trackColour.withAlpha (1.0f).withBrightness (0.266f));
    repaint();
}

//==============================================================================
// quick-and-dirty function to format a timecode string
void RingModulatorAudioProcessorEditor::comboBoxChanged (ComboBox* box)
{
    auto index = box->getSelectedItemIndex();
    std::cout << "index @ editor = " << index << std::endl;
    
    if (box == &stereoBox)
    {
        processor.stereoParam->operator= (index);
    }
}


static String timeToTimecodeString (double seconds)
{
    const int millisecs = roundToInt (seconds * 1000.0);
    const int absMillisecs = std::abs (millisecs);

    return String::formatted ("%02d:%02d:%02d.%03d",
                              millisecs / 3600000,
                              (absMillisecs / 60000) % 60,
                              (absMillisecs / 1000) % 60,
                              absMillisecs % 1000);
}

// quick-and-dirty function to format a bars/beats string
static String quarterNotePositionToBarsBeatsString (double quarterNotes, int numerator, int denominator)
{
    if (numerator == 0 || denominator == 0)
        return "1|1|000";

    const int quarterNotesPerBar = (numerator * 4 / denominator);
    const double beats  = (fmod (quarterNotes, quarterNotesPerBar) / quarterNotesPerBar) * numerator;

    const int bar    = ((int) quarterNotes) / quarterNotesPerBar + 1;
    const int beat   = ((int) beats) + 1;
    const int ticks  = ((int) (fmod (beats, 1.0) * 960.0 + 0.5));

    return String::formatted ("%d|%d|%03d", bar, beat, ticks);
}

// Updates the text in our position label.
void RingModulatorAudioProcessorEditor::updateTimecodeDisplay (AudioPlayHead::CurrentPositionInfo pos)
{
    MemoryOutputStream displayText;

    displayText << "[" << SystemStats::getJUCEVersion() << "]   "
    << String (pos.bpm, 2) << " bpm, "
    << pos.timeSigNumerator << '/' << pos.timeSigDenominator
    << "  -  " << timeToTimecodeString (pos.timeInSeconds)
    << "  -  " << quarterNotePositionToBarsBeatsString (pos.ppqPosition,
                                                        pos.timeSigNumerator,
                                                        pos.timeSigDenominator);

    if (pos.isRecording)
        displayText << "  (recording)";
    else if (pos.isPlaying)
        displayText << "  (playing)";

    timecodeDisplayLabel.setText (displayText.toString(), dontSendNotification);
}

