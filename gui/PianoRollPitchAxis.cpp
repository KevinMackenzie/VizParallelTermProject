#include <Analysis.h>
#include "PianoRollPitchAxis.h"

PianoRollPitchAxis::PianoRollPitchAxis(int low_note, int high_note, QGraphicsView *parent) :
        QWidget(parent), offset(0), scale(1), low_note(low_note), high_note(high_note) {

    setFixedWidth(60);
    connect(parent->verticalScrollBar(), &QScrollBar::valueChanged, this, &PianoRollPitchAxis::setOffset);

}

void PianoRollPitchAxis::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.translate(0, -offset);
    painter.setFont(font());
    double unit = (float)height() / (high_note - low_note+1) * scale;
    qreal top = 0;
    painter.setPen(QColor(0, 0, 0));
    for (int note = high_note; note >= low_note; --note) {
        painter.setBrush(isBlackKey(note) ? QColor(0, 0, 0) : QColor(255, 255, 255));
        painter.drawRect(0, top, width(), unit);
        top += unit;
    }
}
