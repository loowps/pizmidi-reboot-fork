#ifndef PIZ_PIANO_ROLL_HEADER
#define PIZ_PIANO_ROLL_HEADER

#include "../data/MidiLoop.h"
#include "../data/PizNote.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_events/juce_events.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "PianoRollBackground.h"
#include "PianoRollNotes.h"
#include "Playbar.h"
#include "Timeline.h"

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

    Loop* sequence;

    float xinc;
    float yinc;

    int draggingNoteTransposition;
    double draggingNoteTimeDelta;

    juce::Array<PizMidiMessageSequence::mehPtr> selectedNotes;

private:
    juce::Rectangle<int> lasso;
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

    Timeline* timeline;
    Playbar* playline;
    PianoRollBackground* bg;
    PianoRollNotes* noteLayer;
    juce::AudioProcessor* plugin;
    juce::AudioProcessorEditor* owner;
    PizMidiMessageSequence::mehPtr hoveringNote;
    int hoveringNoteIndex;
    double timebase;
    double stepLengthInPpq;

    float seqLength;

    int numEvents;
    double lastDragTime;
    juce::uint8 draggingNoteChannel;
    int draggingNoteNumber;
    int draggingNoteVelocity;
    int originalNoteVelocity;
    double draggingNoteLength;
    double draggingNoteStartTime;
    double draggingNoteEndOffset;

    bool wasResizing;
    bool snapToGrid;
    float noteLength;
    double quarterNotesPerBar;
};

#endif
