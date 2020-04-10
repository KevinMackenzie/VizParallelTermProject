#ifndef VIZPARALLELTERMPROJECT_PIANOROLLVIEW_H
#define VIZPARALLELTERMPROJECT_PIANOROLLVIEW_H


#include <QGraphicsView>
#include "PianoRollScene.h"

class PianoRollView : public QGraphicsView {
Q_OBJECT

public:
    explicit PianoRollView(PianoRollScene *scene, QWidget *parent = nullptr) : QGraphicsView(scene, parent) {}

    void drawForeground(QPainter *painter, const QRectF &rect) override;

public slots:

    void zoomIn() { this->scale(1.05, 1.05); }

    void zoomOut() { this->scale(0.95, 0.95); }

    void zoomInX() { this->scale(1.05, 1); }

    void zoomOutX() { this->scale(0.95, 1); }

    void zoomInY() { this->scale(1, 1.05); }

    void zoomOutY() { this->scale(1, 0.95); }

private:

};


#endif //VIZPARALLELTERMPROJECT_PIANOROLLVIEW_H
