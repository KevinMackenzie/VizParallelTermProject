#ifndef VIZPARALLELTERMPROJECT_SCRUBBER_H
#define VIZPARALLELTERMPROJECT_SCRUBBER_H


#include <QGraphicsItem>

// May need to switch to inheriting QGraphicsObject if we want to have signals/slots for mouseover/playback
class Scrubber : public QGraphicsItem {
public:
    explicit Scrubber(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QColor color;
};


#endif //VIZPARALLELTERMPROJECT_SCRUBBER_H
