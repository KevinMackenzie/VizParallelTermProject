#ifndef VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H
#define VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H


#include <QGraphicsScene>
#include <MidiFile.h>
#include "NoteInfo.h"
class PianoRollView;
class PianoRollScene : public QGraphicsScene {
Q_OBJECT
friend class PianoRollView;
public:
    explicit PianoRollScene(QRectF rect, int low_note = 21, int high_note = 108, QWidget *parent = nullptr);

    void drawMidiNotes(smf::MidiFile &f, NoteInfo *toConnect);

private:
    qreal getYFromPitch(int pitch);
    bool isBlackKey(int midiIdx);

    void drawKeyboard();

    void drawStaff();

    int low_note;
    int high_note;
    int numWhiteKeys;
    QSizeF whiteKeySize;
    QSizeF blackKeySize;

};


#endif //VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H
