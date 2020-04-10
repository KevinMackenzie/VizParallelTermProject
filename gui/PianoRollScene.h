#ifndef VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H
#define VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H


#include <QGraphicsScene>
#include <MidiFile.h>
#include "NoteInfo.h"

class PianoRollScene : public QGraphicsScene {
Q_OBJECT

public:
    explicit PianoRollScene(QRectF rect, int low_note = 21, int high_note = 108, QWidget *parent = nullptr);

    void drawMidiNotes(smf::MidiFile &f, NoteInfo *toConnect);

private:
    bool isBlackKey(int midiIdx);

    qreal getYFromPitch(int pitch);

    void drawKeyboard();

    void drawStaff();

    int low_note;
    int high_note;
    int numWhiteKeys;
    QSizeF whiteKeySize;
    QSizeF blackKeySize;

};


#endif //VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H
