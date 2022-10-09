#include "PianoRollBackground.h"

#include "../PianoRoll.h"

PianoRollBackground::PianoRollBackground()
{
    setBufferedToImage(true);
}

PianoRollBackground::~PianoRollBackground()
{
}

void PianoRollBackground::paint(juce::Graphics& g)
{
    const PianoRoll* roll = (PianoRoll*) getParentComponent();
    int n                 = 0;
    float y               = (float) getHeight();
    float yinc            = (float) getHeight() / 128.f;

    while (y > 0)
    {
        if (getNoteNameWithoutOctave(n).contains("#"))
        {
            g.setColour(juce::Colours::lightgrey);
        }
        else if (n == 60)
        {
            g.setColour(juce::Colours::yellow);
        }
        else
        {
            g.setColour(juce::Colours::white);
        }

        g.fillRect(0.f, y - yinc, (float) getWidth(), yinc);

        if (getNoteNameWithoutOctave(n).contains("F") && ! getNoteNameWithoutOctave(n).contains("#"))
        {
            g.setColour(juce::Colours::grey);
            g.drawLine(0.f, y, (float) getWidth(), y, 1);
        }

        if (getNoteNameWithoutOctave(n).contains("C") && ! getNoteNameWithoutOctave(n).contains("#"))
        {
            g.setColour(juce::Colours::black);
            g.drawLine(0.f, y, (float) getWidth(), y, 1);
        }

        n++;
        y -= yinc;
    }

    float x = roll->gridSize;
    while (x < getWidth())
    {
        //draw grid
        if (! (fmod(x, roll->barSize) < 0.0001) && ! (fmod(x, roll->beatSize) < 0.0001))
        {
            g.setColour(juce::Colours::lightgrey);
            g.drawLine(x, 0.f, x, (float) getHeight());
        }
        x += roll->gridSize;
    }

    x = roll->beatSize;
    while (x < getWidth())
    {
        //draw beats
        if (! (fmod(x, roll->barSize) < 0.0001))
        {
            g.setColour(juce::Colours::grey);
            g.drawLine(x, 0.f, x, (float) getHeight());
        }
        x += roll->beatSize;
    }

    x = roll->barSize;
    while (x < getWidth())
    {
        //draw bars
        g.setColour(juce::Colours::black);
        g.drawLine(x, 0.f, x, (float) getHeight());
        x += roll->barSize;
    }
}
