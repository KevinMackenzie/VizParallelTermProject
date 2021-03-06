#ifndef PROJECT_PIANOROLLPITCHAXIS_H
#define PROJECT_PIANOROLLPITCHAXIS_H


#include <QtWidgets/QGraphicsRectItem>

class PianoRollPitchAxis : public QGraphicsRectItem {
public:
    explicit PianoRollPitchAxis(QRectF rect, int lowNote, int highNote, QGraphicsItem *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void updateXScale(qreal xScale) { this->xScale = xScale; }

    QSizeF getBlackKeySize() const { return blackKeySize; }
    QSizeF getWhiteKeySize() const { return whiteKeySize; }
    int getNumWhiteKeys() const {return numWhiteKeys;}
    int getHighNote() const {return highNote; }
    int getLowNote() const {return lowNote; }
private:
    int lowNote;
    int highNote;
    QSizeF whiteKeySize;
    QSizeF blackKeySize;
    qreal xScale;
    int numWhiteKeys;
};


#endif //PROJECT_PIANOROLLPITCHAXIS_H
