#include "KeySelector.h"

KeySelector::KeySelector(juce::MidiKeyboardState& state)
    : MidiKeyboardComponent(state, MidiKeyboardComponent::horizontalKeyboard)
{
    s = &state;
    this->setColour(MidiKeyboardComponent::textLabelColourId, juce::Colours::transparentBlack);
}

KeySelector::~KeySelector()
{
}

bool KeySelector::mouseDownOnKey(int midiNoteNumber, const juce::MouseEvent& e)
{
    if (s->isNoteOn(this->getMidiChannel(), midiNoteNumber))
    {
        s->noteOff(this->getMidiChannel(), midiNoteNumber, 1.f);
    }
    else
    {
        s->noteOn(this->getMidiChannel(), midiNoteNumber, 1.f);
    }
    return false;
}