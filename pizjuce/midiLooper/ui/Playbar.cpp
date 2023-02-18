#include "Playbar.h"

#include "PianoRoll.h"

Playbar::Playbar(PianoRoll* pianoroll)
    : roll(pianoroll)
{
}

Playbar::~Playbar() = default;

void Playbar::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0x0));
    //if (sequence->isRecording) {
    //	g.setColour(Colours::red);
    //	g.drawVerticalLine((int)((float)sequence->recTime*(float)getWidth()/seqLengthInPpq),0.f,(float)getHeight());
    //}
    //else {
    g.setColour(juce::Colours::green);
    g.drawVerticalLine((int) roll->ppqToPixelsWithOffset(roll->playTime), 0.f, (float) getHeight());
    //}
}
