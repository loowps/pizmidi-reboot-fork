
#include "PianoRollNotes.h"

#include "PianoRoll.h"

using juce::jmax;

PianoRollNotes::PianoRollNotes()
{
    setBufferedToImage(true);
}

PianoRollNotes::~PianoRollNotes() = default;

void PianoRollNotes::paint(juce::Graphics& g)
{
    const PianoRoll* roll = (PianoRoll*) getParentComponent();
    if (roll->barSize > 0)
    {
        if (roll->sequence)
        {
            for (int i = 0; i < roll->sequence->getNumEvents(); i++)
            {
                if (roll->sequence->getEventPointer(i)->message.isNoteOn() /* && (i-9999!=hoveringNote)*/)
                {
                    auto noteLength = (float) (jmax(1.0, (roll->sequence->getEventTime(roll->sequence->getIndexOfMatchingKeyUp(i)) - roll->sequence->getEventTime(i))) * (double) getWidth() / roll->seqLengthInPpq);
                    //if (i==sequence->indexOfLastNoteOn
                    //	|| abs(sequence->getEventTime(i)-sequence->getEventTime(sequence->indexOfLastNoteOn))<sequence->chordTolerance)
                    //	g.setColour(Colours::blue);
                    //else

                    if (roll->selectedNotes.contains(roll->sequence->getEventPointer(i)))
                    {
                        //outline of original note position
                        g.setColour(juce::Colours::blue);
                        g.drawRect((float) getWidth() * (float) (roll->sequence->getEventTime(i) / roll->seqLengthInPpq),
                                   (float) getHeight() - (float) (roll->sequence->getEventPointer(i)->message.getNoteNumber()) * roll->yinc - roll->yinc,
                                   noteLength,
                                   roll->yinc);

                        //dragging note position
                        const double newTime = (float) ((roll->sequence->getEventTime(i) + roll->draggingNoteTimeDelta));
                        const float newNote  = (float) (roll->sequence->getEventPointer(i)->message.getNoteNumber() + roll->draggingNoteTransposition);
                        g.setColour(juce::Colours::darkgoldenrod.withAlpha(roll->sequence->getEventPointer(i)->message.getFloatVelocity()));
                        g.fillRect((float) getWidth() * (float) (newTime / roll->seqLengthInPpq),
                                   (float) getHeight() - newNote * roll->yinc - roll->yinc,
                                   noteLength,
                                   roll->yinc);
                        g.setColour(juce::Colours::red);
                        g.drawRect((float) getWidth() * (float) (newTime / roll->seqLengthInPpq),
                                   (float) getHeight() - newNote * roll->yinc - roll->yinc,
                                   noteLength,
                                   roll->yinc);
                    }
                    else
                    {
                        g.setColour(juce::Colours::darkgoldenrod.withAlpha(roll->sequence->getEventPointer(i)->message.getFloatVelocity()));
                        g.fillRect((float) getWidth() * (float) (roll->sequence->getEventTime(i) / roll->seqLengthInPpq),
                                   (float) getHeight() - (float) (roll->sequence->getEventPointer(i)->message.getNoteNumber()) * roll->yinc - roll->yinc,
                                   noteLength,
                                   roll->yinc);
                        g.setColour(juce::Colours::black);
                        g.drawRect((float) getWidth() * (float) (roll->sequence->getEventTime(i) / roll->seqLengthInPpq),
                                   (float) getHeight() - (float) (roll->sequence->getEventPointer(i)->message.getNoteNumber()) * roll->yinc - roll->yinc,
                                   noteLength,
                                   roll->yinc);
                    }
                }
            }
        }
    }
}
