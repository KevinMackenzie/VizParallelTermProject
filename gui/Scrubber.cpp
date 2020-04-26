#include "Scrubber.h"
#include "MidiNoteGraphicsItem.h"
#include <QCursor>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QApplication>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <iostream>

Scrubber::Scrubber(QGraphicsObject *parent)
        : QGraphicsObject(parent), color(QColor(255, 0, 0)), YScale(1) {
    setCursor(Qt::OpenHandCursor);
    setFlags(QGraphicsItem::ItemIsMovable
    | QGraphicsItem::ItemSendsGeometryChanges
    | QGraphicsItem::ItemIgnoresTransformations
             );
}

void Scrubber::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    auto transform = painter->viewport();
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::red);
    painter->drawRect(0, 0, 4, 500/YScale);
}

QRectF Scrubber::boundingRect() const {
    return QRectF(0, 0, 4, 500/YScale);
}

void Scrubber::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    setCursor(Qt::ClosedHandCursor);
    QGraphicsItem::mousePressEvent(event);

}

void Scrubber::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent(event);
}

void Scrubber::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    setCursor(Qt::OpenHandCursor);
    QGraphicsItem::mouseReleaseEvent(event);
}

QVariant Scrubber::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange) {
        for (auto item : scrubbedItems) {
            auto midi_item = dynamic_cast<MidiNoteGraphicsItem *>(item);
            if (midi_item) {
                midi_item->hoverLeaveEvent(nullptr);
            }
        }
        scrubbedItems.clear();
        auto colliding_items = this->collidingItems();
        for (auto item : colliding_items) {
            auto midi_item = dynamic_cast<MidiNoteGraphicsItem *>(item);
            if (midi_item) {
                scrubbedItems.push_back(midi_item);
                midi_item->hoverEnterEvent(nullptr);
            }
        }

        //Restrict movement to X-axis
        return QPointF(value.toPointF().x(), pos().y());
    }
    return QGraphicsItem::itemChange(change, value);
}


