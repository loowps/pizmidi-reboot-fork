#ifndef PIZ_PIANO_ROLL_HEADER
#define PIZ_PIANO_ROLL_HEADER

#include "data/MidiLoop.h"
#include "data/PizNote.h"
#include "ui/PianoRollBackground.h"
#include "ui/Playbar.h"
#include "ui/Timeline.h"

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

class PianoRoll : public juce::Component,
                  public juce::ChangeBroadcaster
{
public:
    PianoRoll(juce::AudioProcessor* _plugin, juce::AudioProcessorEditor* _owner, Timeline* _timeline);
    ~PianoRoll() override;

    void setSequence(Loop* sequence_);

    Loop* getSequence()
    {
        return sequence;
    }

    void sequenceChanged();
    int getTimeInPixels();
    double pixelsToPpq(float pixels, bool snap, bool round = false);
    float ppqToPixels(double ppq);
    int ppqToPixelsWithOffset(double ppq);
    double snapPpqToGrid(double ppq, bool round = false);
    void setNoteLength(float beatDiv);
    double blankLength;
    float pixelsPerPpq;
    float getNoteHeight();

    void setDisplayLength(double ppq)
    {
        blankLength = ppq;
        sequenceChanged();
    }

    void setDisplayLength(int bars)
    {
        int pixelBarLength = (int) ppqToPixels(getPpqPerBar());
        blankLength        = getPpqPerBar() * bars;
        sequenceChanged();
        setSize((int) ppqToPixels(juce::jmax(blankLength, seqLengthInPpq)), getHeight());
    }

    void addBar()
    {
        int pixelBarLength = (int) ppqToPixels(getPpqPerBar());
        setDisplayLength(seqLengthInPpq + getPpqPerBar());
        setSize(getWidth() + pixelBarLength, getHeight());
    }

    void removeBar()
    {
        int pixelBarLength = (int) ppqToPixels(getPpqPerBar());
        setDisplayLength(juce::jmax(getPpqPerBar(), seqLengthInPpq - getPpqPerBar()));
        setSize(juce::jmax(pixelBarLength, getWidth() - pixelBarLength), getHeight());
    }

    int getDisplayLength()
    {
        return (int) (juce::jmax(blankLength, seqLengthInPpq) / getPpqPerBar());
    }

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    //void mouseMove (const MouseEvent& e);
    void mouseDoubleClick(const juce::MouseEvent& e) override;

    void paintOverChildren(juce::Graphics& g) override;
    void resized() override;

    bool getSnap()
    {
        return snapToGrid;
    }

    float getBeatDiv()
    {
        return 1.f / noteLength;
    }

    void setSnap(bool snap)
    {
        snapToGrid = snap;
    }

    void setPlayTime(double timeInPpq)
    {
        const int lastpixels = ppqToPixelsWithOffset(playTime);
        const int pixels     = ppqToPixelsWithOffset(timeInPpq);
        if (pixels != lastpixels)
        {
            playTime = timeInPpq;
            playline->repaint(lastpixels, 0, 1, getHeight());
            playline->repaint(pixels, 0, 1, getHeight());
        }
    }

    void setTimeSig(int n, int d);

    void setPlaying(bool isPlaying)
    {
        playing = isPlaying;
        playline->repaint();
    }

    void setRecording(bool isRecording)
    {
        recording = isRecording;
        playline->repaint();
    }

    double getPpqPerBar()
    {
        return timebase * quarterNotesPerBar;
    }

    Component* getPlayline()
    {
        return (Component*) playline;
    }

    int defaultChannel;
    int timeSigN, timeSigD;
    bool playing;
    bool recording;

    void repaintBG()
    {
        bg->repaint();
    }

    double seqLengthInPpq;
    double playTime;

    float gridSize; //quantization grid size in pixels
    float beatSize; //number of pixels per beat
    float barSize;  //number of pixels per measure

private:
    juce::Rectangle<int> lasso;
    juce::Array<PizMidiMessageSequence::mehPtr> selectedNotes;
    juce::Array<PizNote> selectedNoteLengths;

    void addToSelection(PizMidiMessageSequence::mehPtr note)
    {
        if (note->message.isNoteOn())
        {
            if (note->noteOffObject == nullptr)
            {
                sequence->updateMatchedPairs();
            }
            selectedNotes.addIfNotAlreadyThere(note);
            selectedNoteLengths.add(PizNote(note));
        }
    }

    void clearSelection()
    {
        selectedNotes.clear();
        selectedNoteLengths.clear();
    }

    class PianoRollNotes : public juce::Component
    {
    public:
        PianoRollNotes()
        {
            setBufferedToImage(true);
        }

        ~PianoRollNotes() override
        {
        }

        void paint(juce::Graphics& g) override
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
                            auto noteLength = (float) (juce::jmax(1.0, (roll->sequence->getEventTime(roll->sequence->getIndexOfMatchingKeyUp(i)) - roll->sequence->getEventTime(i))) * (double) getWidth() / roll->seqLengthInPpq);
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
    };

    Timeline* timeline;
    Playbar* playline;
    PianoRollBackground* bg;
    PianoRollNotes* noteLayer;
    juce::AudioProcessor* plugin;
    juce::AudioProcessorEditor* owner;
    Loop* sequence;
    PizMidiMessageSequence::mehPtr hoveringNote;
    int hoveringNoteIndex;
    double timebase;
    double stepLengthInPpq;

    float seqLength;

    int numEvents;
    float xinc;
    float yinc;
    double lastDragTime;
    juce::uint8 draggingNoteChannel;
    int draggingNoteNumber;
    int draggingNoteVelocity;
    int originalNoteVelocity;
    double draggingNoteLength;
    double draggingNoteStartTime;
    double draggingNoteEndOffset;

    int draggingNoteTransposition;
    double draggingNoteTimeDelta;

    bool wasResizing;
    bool snapToGrid;
    float noteLength;
    double quarterNotesPerBar;
};

#endif
