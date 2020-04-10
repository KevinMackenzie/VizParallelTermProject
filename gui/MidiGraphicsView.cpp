#include <iostream>
#include "MidiGraphicsView.h"
#include "MidiNoteGraphicsItem.h"
#include <QMouseEvent>
#include <Analysis.h>

const char *empty_message = "Hover Over Note to See Pitch";

MidiGraphicsView::MidiGraphicsView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent),
                                                                             selectedItem(nullptr) {
  pitchLabel = scene->addText(empty_message);
}

void MidiGraphicsView::mouseMoveEvent(QMouseEvent *event) {
  QGraphicsView::mouseMoveEvent(event);
  auto item = itemAt(event->pos());
  auto midiItem = dynamic_cast<MidiNoteGraphicsItem *>(item);
  if (midiItem) {
    if (selectedItem != midiItem) {
      pitchLabel->setPlainText(pitchToNote(midiItem->Pitch).c_str());
      if (selectedItem) {
        selectedItem->setBrush(oldBrush);
      }
      oldBrush = midiItem->brush();
      selectedItem = midiItem;
      midiItem->setBrush(QBrush(QColor(255, 0, 0)));
    }
  } else {
    pitchLabel->setPlainText(empty_message);
    if (selectedItem) {
      selectedItem->setBrush(oldBrush);
      selectedItem = nullptr;
    }
  }
}
