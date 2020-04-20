#include "Scrubber.h"
#include <QCursor>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QApplication>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <iostream>

Scrubber::Scrubber(QGraphicsItem *parent)
        : QGraphicsItem(parent), color(QColor(255, 0, 0)) {
    setCursor(Qt::OpenHandCursor);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
}

void Scrubber::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::red);
    painter->drawRect(-10, -10, 20, 20);
    painter->drawRect(-2, 0, 4, 200);
}

QRectF Scrubber::boundingRect() const {
    return QRectF(-10, -10, 20, 210);
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

QVariant Scrubber::itemChange(GraphicsItemChange change, const QVariant &value)
{
    std::cout << change << std::endl;
    if (change == ItemPositionChange)
        return QPointF(value.toPointF().x(), pos().y());
    return QGraphicsItem::itemChange(change, value);
}

