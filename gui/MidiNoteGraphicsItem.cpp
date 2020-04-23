#include <iostream>
#include "MidiNoteGraphicsItem.h"

const QBrush MidiNoteGraphicsItem::green = QBrush(QColor(0, 127, 0));
const QBrush MidiNoteGraphicsItem::yellow = QBrush(QColor(255, 255, 0));
const QBrush MidiNoteGraphicsItem::red = QBrush(QColor(255, 0, 0));

MidiNoteGraphicsItem::MidiNoteGraphicsItem(const QRectF &rc, int slotNum, int noteIdx, QGraphicsItem *parent)
        : QGraphicsRectItem(rc, parent), slotNum(slotNum), noteIdx(noteIdx) {
    this->setAcceptHoverEvents(true);
}

void MidiNoteGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    this->setBrush(red);
    emit mouseEnter(slotNum, noteIdx);
    update();
}

void MidiNoteGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    this->setBrush(green);
    emit mouseExit(slotNum, noteIdx);
    update();
}

