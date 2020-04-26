#include "MidiNoteGraphicsItem.h"
#include "PianoRollScene.h"

MidiNoteGraphicsItem::MidiNoteGraphicsItem(const QRectF &rc, int slotNum, int noteIdx, QGraphicsItem *parent)
        : QGraphicsRectItem(rc, parent), slotNum(slotNum), noteIdx(noteIdx) {
    this->setAcceptHoverEvents(true);
}

void MidiNoteGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    emit mouseEnter(slotNum, noteIdx);
    update();
}

void MidiNoteGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    emit mouseExit(slotNum, noteIdx);
    update();
}

