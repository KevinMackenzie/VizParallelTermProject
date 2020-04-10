#ifndef PROJECT_MIDINOTEGRAPHICSITEM_H
#define PROJECT_MIDINOTEGRAPHICSITEM_H


#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QBrush>

class MidiNoteGraphicsItem : public QObject, public QGraphicsRectItem {
Q_OBJECT
public:
    explicit MidiNoteGraphicsItem(const QRectF &rc, int p, QGraphicsItem *parent = nullptr);

    const QBrush green = QBrush(QColor(0, 127, 0));
    const QBrush red = QBrush(QColor(255, 0, 0));

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

signals:

    void moused(int pitch);

private:
    const int pitch;
};


#endif //PROJECT_MIDINOTEGRAPHICSITEM_H
