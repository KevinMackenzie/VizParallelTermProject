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
    explicit PianoRollScene(QRectF rect, const MidiAnalysis& a, int low_note = 21, int high_note = 108, int subdiv = 1, QWidget *parent = nullptr);

    void drawMidiNotes(smf::MidiFile &f, NoteInfo *toConnect, int idx);
    void clearMidiNotes(int idx);

    void updateScale(QPointF origin, QSizeF scale);

private slots:
    void showConnectivity(int slotNum, int noteIdx);
    void hideConnectivity(int slotNum, int noteIdx);

private:
    qreal getYFromPitch(int pitch);

    void drawKeyboard();

    void drawStaff();

    int subdiv;
    PianoRollPitchAxis pitchAxis;
    std::vector<std::vector<MidiNoteGraphicsItem*> > midiItems;
    const MidiAnalysis& analysis;
};


#endif //VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H