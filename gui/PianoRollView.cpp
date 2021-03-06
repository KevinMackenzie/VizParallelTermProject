//
// Created by Linus Koepfer on 4/10/20.
//

#include <iostream>
#include "PianoRollView.h"

void PianoRollView::scrollContentsBy(int dx, int dy) {
    QGraphicsView::scrollContentsBy(dx, dy);
//    std::cout << "Moved by " << dx << std::endl;
//     ((PianoRollScene*)scene())->scrubber.moveBy(0, -dy);
    updateAxes();
}

void PianoRollView::updateAxes() {
    if (!scene()) return;
    auto sc = (PianoRollScene*)scene();
    auto tl = mapToScene(rect().topLeft());
    auto br = mapToScene(rect().bottomRight());
    QSizeF sz;
    sz.setWidth((br.x() - tl.x()) / rect().width());
    sz.setHeight((br.y() - tl.y()) / rect().height());
    sc->updateScale(mapToScene(0, 0), sz);

    double inputEnd = 0;
    double refEnd = 0;
    if (sc->analysis.getInput().has_value()) {
        // This "/19200" is a hack to make the "seconds" line up correctly w/ 96 ticks/beat; 120bpm
        inputEnd = getMidiStringLength(sc->analysis.getInput().value()) / 19200.;
    }
    if (sc->analysis.getReference().has_value()) {
        refEnd = getMidiStringLength(sc->analysis.getReference().value()) / 19200.;
    }
    double endTime = std::max(inputEnd, refEnd);
    timeAxis->setEndTime(endTime);
    // sc->pitchAxis.setX(this->mapToScene(0, 0).x());
}

// void PianoRollView::drawForeground(QPainter *painter, const QRectF &rect) {
//
//     //TODO: Would be nice to keep X dimension of keys constant
//
//     // rect is the area (in scene coordinates) to update
//     painter->drawRect(rect.left(), 0, 30, 30);
//
//     PianoRollScene *scene = dynamic_cast<PianoRollScene *>(this->scene());
//     QPen pen(QColor(0, 0, 0));
//     QBrush blackBrush(QColor(0, 0, 0));
//     QBrush whiteBrush(QColor(255, 255, 255));
//
//     /* Due to the default ViewportUpdateMode of MinimalViewPortUpdate, sometimes we redraw only part of the viewport.
//      * Thus we cannot use rect.left() as it may be in the middle of the viewport
//      * Instead, we map the leftmost viewport coordinate (x=0) to a scene coordinate
//      * */
//     float left = this->mapToScene(0, 0).x();
//
//     //Draw white keys
//     painter->setPen(pen);
//     painter->setBrush((whiteBrush));
//     float top = this->sceneRect().top();
//     for (int i = scene->high_note; i >= scene->low_note; --i) {
//         if (!scene->isBlackKey(i)) {
//             painter->drawRect(left, top, scene->whiteKeySize.width(), scene->whiteKeySize.height());
//             top += scene->whiteKeySize.height();
//         }
//     }
//
//     // Draw black keys after (on top)
//     painter->setBrush(blackBrush);
//     top = this->sceneRect().top();
//     for (int i = scene->high_note; i >= scene->low_note; --i) {
//         if (scene->isBlackKey(i)) {
//             painter->drawRect(left, top - scene->blackKeySize.height() / 2, scene->blackKeySize.width(),
//                               scene->blackKeySize.height());
//         } else {
//             top += scene->whiteKeySize.height();
//         }
//     }
// }

