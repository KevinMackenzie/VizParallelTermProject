#ifndef PROJECT_MIDINOTEGRAPHICSITEM_H
#define PROJECT_MIDINOTEGRAPHICSITEM_H


#include <QtWidgets/QGraphicsItem>

class MidiNoteGraphicsItem : public QGraphicsRectItem {
public:
  const int Pitch;
  MidiNoteGraphicsItem(const QRectF &rc, int p, QGraphicsItem *parent = nullptr) : QGraphicsRectItem(rc, parent), Pitch(p) { }

};


#endif //PROJECT_MIDINOTEGRAPHICSITEM_H
