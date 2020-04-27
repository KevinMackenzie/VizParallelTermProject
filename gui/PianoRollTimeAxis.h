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
    explicit PianoRollTimeAxis(QGraphicsView *parent = nullptr);

    void paintEvent(QPaintEvent *event) override;

public slots:

    void setOffset(int value) {
        offset = value;
        update();
    }

    void multScale(float value) {
        scale *= value;
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
