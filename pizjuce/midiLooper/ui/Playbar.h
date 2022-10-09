#ifndef PIZMIDI_PLAYBAR_H
#define PIZMIDI_PLAYBAR_H

#include "juce_gui_basics/juce_gui_basics.h"

class PianoRoll;

class Playbar : public juce::Component
{
public:
    Playbar(PianoRoll* pianoroll);

    ~Playbar() override;

    void paint(juce::Graphics& g) override;

private:
    PianoRoll* roll;
};

#endif //PIZMIDI_PLAYBAR_H
