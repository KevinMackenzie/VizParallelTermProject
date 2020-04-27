#ifndef VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H
#define VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H


#include <QGraphicsScene>
#include <MidiFile.h>
#include "NoteInfo.h"
#include "PianoRollPitchAxis.h"
#include "Scrubber.h"

class PianoRollView;
class MidiNoteGraphicsItem;
class PianoRollScene : public QGraphicsScene {
Q_OBJECT
friend class PianoRollView;
public:
    constexpr static QColor trackOne = QColor(31, 120, 180);
    constexpr static QColor trackOneHighlight = QColor(166, 206, 227);
    constexpr static QColor trackTwo = QColor(51, 160, 44);
    constexpr static QColor trackTwoHighlight = QColor(178, 223, 138);

    explicit PianoRollScene(QRectF rect, MidiAnalysis& a, int low_note = 21, int high_note = 108, int subdiv = 1, QWidget *parent = nullptr);

    void drawMidiNotes(const MidiString &f, NoteInfo *toConnect, int idx);
    void clearMidiNotes(int idx);

    void updateScale(QPointF origin, QSizeF scale);

    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

private slots:
    void showConnectivity(int slotNum, int noteIdx);
    void hideConnectivity(int slotNum, int noteIdx);

private:
    qreal getYFromPitch(int pitch);

    void drawKeyboard();

    void drawStaff();

    int subdiv;
    float width_scale;
    PianoRollPitchAxis pitchAxis;
    std::vector<std::vector<MidiNoteGraphicsItem*> > midiItems;
    MidiAnalysis& analysis;
    Scrubber scrubber;
};


#endif //VIZPARALLELTERMPROJECT_PIANOROLLSCENE_H
