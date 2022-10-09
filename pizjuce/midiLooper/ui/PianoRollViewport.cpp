#include "PianoRollViewport.h"

PianoRollViewport::PianoRollViewport(const juce::String& name)
    : Viewport(name),
      timeline(nullptr),
      playline(nullptr),
      keyboard(nullptr)
{
}

PianoRollViewport::~PianoRollViewport()
{
    dispatchPendingMessages();
}

void PianoRollViewport::setTimeline(Timeline* t)
{
    timeline = t;
}

void PianoRollViewport::setPlayline(juce::Component* p)
{
    playline = p;
}

void PianoRollViewport::setKeyboard(juce::Viewport* kb)
{
    keyboard = kb;
}

void PianoRollViewport::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    this->getParentComponent()->mouseWheelMove(e, wheel);
}

void PianoRollViewport::visibleAreaChanged(const juce::Rectangle<int>& newVisibleArea)
{
    timeline->scrollOffset = newVisibleArea.getX();
    timeline->repaint();
    playline->setBounds(newVisibleArea);
    keyboard->setViewPosition(0, newVisibleArea.getY());
    sendChangeMessage();
}