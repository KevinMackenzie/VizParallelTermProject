#ifndef VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H
#define VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H


#include <QGraphicsScene>
#include <MidiFile.h>
#include "NoteInfo.h"
#include "PianoRollPitchAxis.h"

class PianoRollView;
class MidiNoteGraphicsItem;
class PianoRollScene : public QGraphicsScene {
Q_OBJECT
friend class PianoRollView;
public:
    explicit PianoRollScene(QRectF rect, int low_note = 21, int high_note = 108, QWidget *parent = nullptr);

    void drawMidiNotes(smf::MidiFile &f, NoteInfo *toConnect);
    void clearMidiNotes();

private:
    qreal getYFromPitch(int pitch);

    void drawKeyboard();

    void drawStaff();

    PianoRollPitchAxis pitchAxis;
    std::vector<MidiNoteGraphicsItem*> midiItems;
};


#endif //VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H
