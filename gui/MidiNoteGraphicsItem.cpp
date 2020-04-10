#include <iostream>
#include "MidiNoteGraphicsItem.h"

MidiNoteGraphicsItem::MidiNoteGraphicsItem(const QRectF &rc, int p, QGraphicsItem *parent)
        : QGraphicsRectItem(rc, parent), pitch(p) {
    this->setAcceptHoverEvents(true);
}

void MidiNoteGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    this->setBrush(red);
    emit moused(pitch);
    update();
}

void MidiNoteGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    this->setBrush(green);
    update();
}
