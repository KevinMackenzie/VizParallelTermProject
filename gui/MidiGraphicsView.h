#ifndef PROJECT_MIDIGRAPHICSVIEW_H
#define PROJECT_MIDIGRAPHICSVIEW_H


#include <QtWidgets/QGraphicsView>
#include "MidiNoteGraphicsItem.h"

class MidiGraphicsView : public QGraphicsView {
  QGraphicsTextItem *pitchLabel;
  MidiNoteGraphicsItem* selectedItem;
  QBrush oldBrush;
public:
  MidiGraphicsView(QGraphicsScene *scene, QWidget *parent);

  void mouseMoveEvent(QMouseEvent *event) override;
};


#endif //PROJECT_MIDIGRAPHICSVIEW_H
