#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QResizeEvent>

bool isBlackKey(int midiIdx) {
  int pc = midiIdx % 12;
  return (pc < 4 && pc % 2 == 1) || (pc > 4 && pc % 2 == 0);
}

void drawPianoRoll(QGraphicsScene& sc, QRectF rect, int low_note, int high_note) {
  QSizeF whiteKeySize(15, 2.25);
  QSizeF blackKeySize(10, 1);

  float h = 0;
  for (int i = low_note; i <= high_note; ++i) {
    if (!isBlackKey(i)) {
      h += whiteKeySize.height();
    }
  }
  whiteKeySize *= rect.height() / h;
  blackKeySize *= rect.height() / h;
  // whiteKeySize.scale(QSizeF(rect.width() / whiteKeySize.width(), rect.height() / h), Qt::IgnoreAspectRatio);
  // blackKeySize.scale(QSizeF(rect.width() / whiteKeySize.width(), rect.height() / h), Qt::IgnoreAspectRatio);

  QPen* pen = new QPen(QColor(0, 0, 0));
  QBrush* blackBrush = new QBrush(QColor(0, 0, 0));

  float top = rect.y();
  float left = rect.x();
  for (int i = high_note; i >= low_note; --i) {
    if (!isBlackKey(i)) {
      sc.addRect(QRectF(QPointF(left, top), whiteKeySize));
      top += whiteKeySize.height();
    }
  }

  // Draw black keys after (on top)
  top = rect.y();
  for (int i = high_note; i >= low_note; --i) {
    if (isBlackKey(i)) {
      sc.addRect(QRectF(QPointF(left, top - blackKeySize.height() / 2), blackKeySize), *pen, *blackBrush);
    } else {
      top += whiteKeySize.height();
    }
  }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), scene(QRectF(0, 0, 100, 100), this) {
  ui->setupUi(this);
  ui->mainGraphicsView->setScene(&scene);

  // Draw a standard 88 key keyboard
  drawPianoRoll(scene, QRectF(30, 20, 100, 500), 21, 108);
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  ui->mainGraphicsView->resize(event->size() - 2 * QSize(10, 10));

  auto sz = ui->mainGraphicsView->contentsRect().size();
  ui->mainGraphicsView->setSceneRect(0, 0, sz.width(), sz.height());
}

