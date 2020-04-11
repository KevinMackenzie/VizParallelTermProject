#ifndef VIZPARALLELTERMPROJECT_PIANOROLLVIEW_H
#define VIZPARALLELTERMPROJECT_PIANOROLLVIEW_H


#include <QGraphicsView>
#include "PianoRollScene.h"

class PianoRollView : public QGraphicsView {
Q_OBJECT
protected:
  void scrollContentsBy(int dx, int dy) override;

public:
    explicit PianoRollView(PianoRollScene *scene, QWidget *parent = nullptr) : QGraphicsView(scene, parent) {}

    // void drawForeground(QPainter *painter, const QRectF &rect) override;


public slots:

    void zoomIn() { this->scale(1.05, 1.05); updateAxes(); }

    void zoomOut() { this->scale(0.95, 0.95); updateAxes(); }

    void zoomInX() { this->scale(1.05, 1); updateAxes(); }

    void zoomOutX() { this->scale(0.95, 1); updateAxes(); }

    void zoomInY() { this->scale(1, 1.05); updateAxes(); }

    void zoomOutY() { this->scale(1, 0.95); updateAxes(); }

private:
    void updateAxes();
};


#endif //VIZPARALLELTERMPROJECT_PIANOROLLVIEW_H
