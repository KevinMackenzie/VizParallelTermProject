//
// Created by donald on 4/11/20.
//

#include <QtGui/QPen>
#include <QtGui/QPainter>
#include "PianoRollPitchAxis.h"
#include <Analysis.h>

PianoRollPitchAxis::PianoRollPitchAxis(QRectF rect, int lowNote, int highNote, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent), lowNote(lowNote), highNote(highNote) {
  numWhiteKeys = 0;

  for (unsigned int i = lowNote; i <= highNote; ++i) {
    if (!isBlackKey(i)) {
      ++numWhiteKeys;
    }
  }
  qreal unit = rect.height() / numWhiteKeys;
  whiteKeySize = QSizeF(6 * unit, unit);
  blackKeySize = QSizeF(4 * unit, unit / 2);
}

void PianoRollPitchAxis::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

  QPen pen(QColor(0, 0, 0));
  QBrush blackBrush(QColor(0, 0, 0));
  QBrush whiteBrush(QColor(255, 255, 255));

  /* Due to the default ViewportUpdateMode of MinimalViewPortUpdate, sometimes we redraw only part of the viewport.
   * Thus we cannot use rect.left() as it may be in the middle of the viewport
   * Instead, we map the leftmost viewport coordinate (x=0) to a scene coordinate
   * */
  float left = 0;

  //Draw white keys
  painter->setPen(pen);
  painter->setBrush((whiteBrush));
  float top = this->rect().top();
  for (int i = highNote; i >= lowNote; --i) {
    if (!isBlackKey(i)) {
      painter->drawRect(left, top, whiteKeySize.width(), whiteKeySize.height());
      top += whiteKeySize.height();
    }
  }

  // Draw black keys after (on top)
  painter->setBrush(blackBrush);
  top = this->rect().top();
  for (int i = highNote; i >= lowNote; --i) {
    if (isBlackKey(i)) {
      painter->drawRect(left, top - blackKeySize.height() / 2, blackKeySize.width(), blackKeySize.height());
    } else {
      top += whiteKeySize.height();
    }
  }
}
