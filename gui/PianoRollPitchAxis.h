#ifndef VIZPARALLELTERMPROJECT_PIANOROLLPITCHAXIS_H
#define VIZPARALLELTERMPROJECT_PIANOROLLPITCHAXIS_H


#include <QWidget>
#include <QScrollBar>
#include <QAbstractScrollArea>
#include <QPainter>
#include <iostream>
#include <QGraphicsView>

class PianoRollPitchAxis : public QWidget {
Q_OBJECT
public:
    explicit PianoRollPitchAxis(int low_note, int high_note, QGraphicsView *parent = nullptr);

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

private:
    int offset;
    float scale;
    int low_note;
    int high_note;

};


#endif //VIZPARALLELTERMPROJECT_PIANOROLLPITCHAXIS_H
