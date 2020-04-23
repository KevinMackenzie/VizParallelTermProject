#ifndef VIZPARALLELTERMPROJECT_SCRUBBER_H
#define VIZPARALLELTERMPROJECT_SCRUBBER_H


#include <QGraphicsItem>

class Scrubber : public QGraphicsObject {
    Q_OBJECT
public:
    explicit Scrubber(QGraphicsObject *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QColor color;
    QList<QGraphicsItem*> scrubbedItems;
};


#endif //VIZPARALLELTERMPROJECT_SCRUBBER_H
