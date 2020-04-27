//
// Created by Linus Koepfer on 4/10/20.
//

#include <iostream>
#include <QGridLayout>
#include "PianoRollView.h"

PianoRollView::PianoRollView(PianoRollScene *scene, QWidget *parent) :
        QGraphicsView(scene, parent), timeAxis(new PianoRollTimeAxis(this)),
        pitchAxis(new PianoRollPitchAxis(scene->low_note, scene->high_note, this)) {

    setViewportMargins(pitchAxis->width(), timeAxis->height(), 0, 0);

    timeAxis->setFixedWidth(scene->width());
    pitchAxis->setFixedHeight(scene->height());
    timeAxis->move(pitchAxis->width(), 0);
    pitchAxis->move(0, timeAxis->height());
    updateAxes();

}

void PianoRollView::scrollContentsBy(int dx, int dy) {
    QGraphicsView::scrollContentsBy(dx, dy);
    updateAxes();
}

void PianoRollView::updateAxes() {
    if (!scene()) return;
    auto sc = (PianoRollScene *) scene();

    pitchAxis->setFixedHeight(sc->height());

    double inputEnd = 0;
    double refEnd = 0;
    if (sc->analysis.getInput().has_value()) {
        inputEnd = sc->analysis.getInput().value().getFileDurationInSeconds();
    }
    if (sc->analysis.getReference().has_value()) {
        refEnd = sc->analysis.getReference().value().getFileDurationInSeconds();
    }
    double endTime = std::max(inputEnd, refEnd);
    std::cout << "Got there" << std::endl;
    timeAxis->setEndTime(endTime);
}

