#ifndef VIZPARALLELTERMPROJECT_PIANOROLLVIEW_H
#define VIZPARALLELTERMPROJECT_PIANOROLLVIEW_H


#include <QGraphicsView>
#include <QGridLayout>
#include "PianoRollScene.h"
#include "PianoRollTimeAxis.h"
#include "PianoRollPitchAxis.h"

class PianoRollView : public QGraphicsView {
Q_OBJECT
protected:
    void scrollContentsBy(int dx, int dy) override;

public:
    explicit PianoRollView(PianoRollScene *scene, QWidget *parent = nullptr);

public slots:

    void zoomIn() {
        this->scale(1.05, 1.05);
        timeAxis->multScale(1.05);
        pitchAxis->multScale(1.05);
    }

    void zoomOut() {
        this->scale(0.95, 0.95);
        timeAxis->multScale(0.95);
        pitchAxis->multScale(0.95);
    }

    void zoomInX() {
        this->scale(1.05, 1);
        timeAxis->multScale(1.05);
    }

    void zoomOutX() {
        this->scale(0.95, 1);
        timeAxis->multScale(0.95);
    }

    void zoomInY() {
        this->scale(1, 1.05);
        pitchAxis->multScale(1.05);
        dynamic_cast<PianoRollScene*>(scene())->multYScale(1.05);
    }

    void zoomOutY() {
        this->scale(1, 0.95);
        pitchAxis->multScale(0.95);
        dynamic_cast<PianoRollScene*>(scene())->multYScale(0.95);
    }

private:
    void updateAxes();

    PianoRollTimeAxis *timeAxis;
    PianoRollPitchAxis *pitchAxis;
    QGridLayout *grid;
};


#endif //VIZPARALLELTERMPROJECT_PIANOROLLVIEW_H
