//
// Created by Linus Koepfer on 4/21/20.
//

#ifndef VIZPARALLELTERMPROJECT_PIANOROLLTIMEAXIS_H
#define VIZPARALLELTERMPROJECT_PIANOROLLTIMEAXIS_H


#include <QWidget>
#include <QScrollBar>
#include <QAbstractScrollArea>
#include <QPainter>
#include <iostream>
#include <QGraphicsView>

class PianoRollTimeAxis : public QWidget {
    Q_OBJECT
public:
    explicit PianoRollTimeAxis(QGraphicsView* parent = nullptr):
    QWidget(parent), offset(0), scale(1) {
        setFixedSize(parent->width(), 40);
//        move(40, 0);
        connect(parent->horizontalScrollBar(), &QScrollBar::valueChanged, this, &PianoRollTimeAxis::setOffset);
    }
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.translate(-offset, 0);
        painter.setFont(font());
        // Todo: Handle fractions on zoom-in
        double unit = width()/end_time_seconds*scale;

        for (int second = 0; second < end_time_seconds; ++second) {
            if (second != 0) {
                QString txt = QString::number(second);
                painter.drawText(second*unit-20, 0, 40, height()-20, Qt::AlignCenter | Qt::AlignBottom, txt);
            }
            if (second % 60 == 0) {
                painter.drawLine(second * unit, height()-20, second*unit, height());
            }
            else if (second % 10 == 0) {
                painter.drawLine(second * unit, height()-20, second*unit, height()-5);
            } else {
                painter.drawLine(second * unit, height()-20, second*unit, height()-10);
            }
            if (unit > 200) {
                painter.drawLine(second*unit + unit/2, height()-20, second*unit + unit/2, height()-15);
                painter.drawText(second*unit + unit/2 -20, 0, 40, height()-20, Qt::AlignCenter | Qt::AlignBottom, ".5");
            }
        }

    }

public slots:
    void setOffset(int value) {
        offset = value;
        update();
    }
    void multScale(float value) {
       scale *=  value;
       update();
    }
    void setEndTime(double value) {
        end_time_seconds = value;
        update();
    }

private:
    int offset;
    float scale;
    double end_time_seconds;

};


#endif //VIZPARALLELTERMPROJECT_PIANOROLLTIMEAXIS_H
