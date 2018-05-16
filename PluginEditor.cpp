/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <iomanip>

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
timecodeDisplayLabel (String()),
LFOglideLabel(String(), "LFO GLIDE :"),
frequencyLabel (String(), "FREQUENCY :"),
amplitudeLabel (String(), "DRY/WET :"),
LFOfrequencyLabel (String(), "LFO FREQUENCY :"),
LFOdepthLabel (String(), "LFO DEPTH :"),
waveFormLabel (String(), "LFO WAVEFORM :")
{
    // add some sliders..
    
    
    addAndMakeVisible (waveFormBox);
    
    auto i = 1;
    for (auto choice : processor.waveFormParam->choices)
        waveFormBox.addItem (choice, i++);
    
    waveFormBox.addListener (this);
    waveFormBox.setSelectedId (processor.waveFormParam->getIndex() + 1);
   
    addAndMakeVisible (glideSlider = new ParameterSlider (*owner.glideParam));
    glideSlider-> setRange (0.0, 1.0, 0.0);
    glideSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    
    addAndMakeVisible (frequencySlider = new ParameterSlider (*owner.frequencyParam));
    frequencySlider->setSliderStyle (Slider::Rotary);
    frequencySlider-> setRange (0.0, 1.0, 0.0);
    frequencySlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

    addAndMakeVisible (LFOfrequencySlider = new ParameterSlider (*owner.LFOfrequencyParam));
    LFOfrequencySlider->setSliderStyle (Slider::Rotary);
    LFOfrequencySlider-> setRange (0.0, 1.0, 0.0);
    LFOfrequencySlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
   
    addAndMakeVisible (LFOdepthSlider = new ParameterSlider (*owner.LFOdepthParam));
    LFOdepthSlider->setSliderStyle (Slider::Rotary);
    LFOdepthSlider-> setRange (0.0, 1.0, 0.0);
    LFOdepthSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    
    addAndMakeVisible (amplitudeSlider = new ParameterSlider (*owner.amplitudeParam));
    amplitudeSlider->setSliderStyle (Slider::Rotary);
    amplitudeSlider-> setRange (0.0, 1.0, 0.0);
    amplitudeSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

    
    addAndMakeVisible (LFOglideLabel);
    LFOglideLabel.setJustificationType (Justification::centredLeft);
    LFOglideLabel.attachToComponent (glideSlider, true);
    
    addAndMakeVisible (waveFormLabel);
    waveFormLabel.setJustificationType (Justification::centredLeft);
    waveFormLabel.attachToComponent (&waveFormBox, true);
    
    frequencyLabel.attachToComponent (frequencySlider, false);
    frequencyLabel.setFont (Font (11.0f));

    LFOfrequencyLabel.attachToComponent (LFOfrequencySlider, false);
    LFOfrequencyLabel.setFont (Font (11.0f));

    LFOdepthLabel.attachToComponent (LFOdepthSlider, false);
    LFOdepthLabel.setFont (Font (11.0f));

    amplitudeLabel.attachToComponent (amplitudeSlider, false);
    amplitudeLabel.setFont (Font (11.0f));
    
//     add a label that will display the current timecode and status..
//    addAndMakeVisible (timecodeDisplayLabel);
//    timecodeDisplayLabel.setFont (Font (Font::getDefaultMonospacedFontName(), 15.0f, Font::plain));
    
    // set resize limits for this plug-in
    setResizeLimits (700, 200, 700, 200);
    
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

#include <sstream>
#include <iomanip>

template <typename T>
std::string to_string_with_precision(const T a_value, const int n)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(n) << a_value;
    return stream.str();
}

//==============================================================================
void RingModulatorAudioProcessorEditor::paint (Graphics& g)
{

    g.setColour(Colours::darkgrey);
    g.fillAll();
    
    g.setColour(Colours::white);
    g.drawLine(0, 54, 700, 54);
    
    getLookAndFeel().setColour (Slider::thumbColourId, Colours::rosybrown);

    float freq = processor.frequencyParam->get();

    std::string f = to_string_with_precision(freq,1);

    g.drawFittedText(f, 65, 135, 70, 10, Justification::centred, 1);
    g.drawFittedText("Hz", 91, 175, 70, 10, Justification::left, 1);
    
    float LFOfreq = processor.LFOfrequencyParam->get();
    
    if(LFOfreq < 0){
        LFOfreq -= 1;
        int LFOI = int(LFOfreq);
        std::string LFOf = rateValues[(LFOI * -1)-1];
        g.drawFittedText(LFOf, 245, 135, 70, 10, Justification::centred, 1);
        g.drawFittedText("Beat", 222, 175, 70, 10, Justification::right, 1);
        
    }else{
        if(LFOfreq < 0 && LFOfreq > -0.5){LFOfreq = 0; }
        else{
        std::string LFOf = to_string_with_precision(LFOfreq,1);
        g.drawFittedText(LFOf, 245, 135, 70, 10, Justification::centred, 1);
        g.drawFittedText("Hz", 216, 175, 70, 10, Justification::right, 1);
        }
    }
    
    float LFOdep = processor.LFOdepthParam->get();
    std::string LFOd = to_string_with_precision(LFOdep,0);
    g.drawFittedText(LFOd, 420, 135, 70, 10, Justification::centred, 1);
    g.drawFittedText("%", 388, 175, 70, 10, Justification::right, 1);
    
    float drywet = processor.amplitudeParam->get() * 100;
    std::string dw = to_string_with_precision(drywet,0);
    g.drawFittedText(dw, 583, 135, 70, 10, Justification::centred, 1);
    g.drawFittedText("%", 555, 175, 70, 10, Justification::right, 1);

    std::string beatsString = to_string_with_precision(beats,0);

    
    g.setFont(11.0f);
    g.setColour(Colours::white);

}

void RingModulatorAudioProcessorEditor::resized()
{
    // This lays out our child components...
    
    auto r = getLocalBounds();
    r.removeFromLeft(10);
    
    auto topSection = r.removeFromTop(30);

    waveFormBox.setBounds(100,15,200,20);
    glideSlider->setBounds(450, 0, 200, 50);

    
  
    
    r.removeFromTop (20);
    auto sliderArea = (r.removeFromTop (35));
        frequencySlider->setBounds (r.removeFromLeft (jmin (180, r.getWidth() / 2)));
        LFOfrequencySlider->setBounds (r.removeFromLeft (jmin (180, r.getWidth() / 2)));
        LFOdepthSlider->setBounds (r.removeFromLeft (jmin (180, r.getWidth() / 2)));
        amplitudeSlider->setBounds (r.removeFromLeft (jmin (180, r.getWidth())));

    
//
//    auto bottom = r.removeFromBottom(100);
//    waveFormBox.setBounds(bottom);
   
   
    getProcessor().lastUIWidth = getWidth();
    getProcessor().lastUIHeight = getHeight();
}

//==============================================================================
void RingModulatorAudioProcessorEditor::timerCallback()
{
    
}

void RingModulatorAudioProcessorEditor::hostMIDIControllerIsAvailable (bool controllerIsAvailable)
{
 
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

void RingModulatorAudioProcessorEditor::comboBoxChanged (ComboBox* box)
{
    auto index = box->getSelectedItemIndex();
    
    if (box == &waveFormBox)
    {
        processor.waveFormParam->operator= (index);
    }
}

