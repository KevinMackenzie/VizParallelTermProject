#ifndef PROJECT_MIDINOTEGRAPHICSITEM_H
#define PROJECT_MIDINOTEGRAPHICSITEM_H


#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QBrush>

class MidiNoteGraphicsItem : public QObject, public QGraphicsRectItem {
Q_OBJECT
public:
    explicit MidiNoteGraphicsItem(const QRectF &rc, int slotNum, int noteIdx, QGraphicsItem *parent = nullptr);

    const int slotNum;
    const int noteIdx;
    bool scrubbed = false;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

signals:

    void mouseEnter(int slotNum, int noteIdx);

    void mouseExit(int slotNum, int noteIdx);
};


#endif //PROJECT_MIDINOTEGRAPHICSITEM_H
