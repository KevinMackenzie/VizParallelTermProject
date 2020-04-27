#include "PianoRollTimeAxis.h"

PianoRollTimeAxis::PianoRollTimeAxis(QGraphicsView *parent) :
        QWidget(parent), offset(0), scale(1) {

    setFixedHeight(40);
    connect(parent->horizontalScrollBar(), &QScrollBar::valueChanged, this, &PianoRollTimeAxis::setOffset);

}

void PianoRollTimeAxis::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.translate(-offset, 0);
    painter.setFont(font());

    double unit = width() / end_time_seconds * scale;

    for (int second = 0; second < end_time_seconds; ++second) {
        if (second != 0) {
            QString txt = QString::number(second);
            painter.drawText(second * unit - 20, 0, 40, height() - 20, Qt::AlignCenter | Qt::AlignBottom, txt);
        }
        if (second % 60 == 0) {
            painter.drawLine(second * unit, height() - 20, second * unit, height());
        } else if (second % 10 == 0) {
            painter.drawLine(second * unit, height() - 20, second * unit, height() - 5);
        } else {
            painter.drawLine(second * unit, height() - 20, second * unit, height() - 10);
        }
        if (unit > 200) {
            painter.drawLine(second * unit + unit / 2, height() - 20, second * unit + unit / 2, height() - 15);
            painter.drawText(second * unit + unit / 2 - 20, 0, 40, height() - 20, Qt::AlignCenter | Qt::AlignBottom, ".5");
        }
    }

}
