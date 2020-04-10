//Custom Qt Items
#include "mainwindow.h"
#include "MidiNoteGraphicsItem.h"

//Standard Qt Items
#include <QResizeEvent>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QLabel>

//Other Libraries
#include <iostream>

//Custom Libraries


bool isBlackKey(int midiIdx) {
    int pc = midiIdx % 12;
    return (pc < 4 && pc % 2 == 1) || (pc > 4 && pc % 2 == 0);
}

void drawPianoRoll(QGraphicsScene *sc, QRectF rect, int low_note, int high_note) {
    QSizeF whiteKeySize(150, 2.25);
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

    QPen *pen = new QPen(QColor(0, 0, 0));
    QBrush *blackBrush = new QBrush(QColor(0, 0, 0));

    float top = rect.y();
    float left = rect.x();
    for (int i = high_note; i >= low_note; --i) {
        if (!isBlackKey(i)) {
            sc->addRect(QRectF(QPointF(left, top), whiteKeySize));
            top += whiteKeySize.height();
        }
    }

    // Draw black keys after (on top)
    top = rect.y();
    for (int i = high_note; i >= low_note; --i) {
        if (isBlackKey(i)) {
            sc->addRect(QRectF(QPointF(left, top - blackKeySize.height() / 2), blackKeySize), *pen, *blackBrush);
        } else {
            top += whiteKeySize.height();
        }
    }
}

int getYFromPitch(int pitch, int low_note, int high_note, QRectF rect, QSizeF blackKeySize) {
    int numWhiteKeys = 0;
    for (int i = low_note; i <= high_note; ++i) {
        if (!isBlackKey(i)) {
            ++numWhiteKeys;
        }
    }

    int whiteKeyIdx = 0;
    for (int i = high_note; i > pitch; --i) {
        if (!isBlackKey(i)) {
            ++whiteKeyIdx;
        }
    }

    float whiteKeyY = (whiteKeyIdx / (float) numWhiteKeys) * rect.height() + rect.top();
    if (isBlackKey(pitch)) {
        return whiteKeyY + (blackKeySize.height() / 2);
    }
    return whiteKeyY;

}

void MainWindow::drawMidiNotes(QGraphicsScene *sc, QRectF rect, smf::MidiFile f) {
    QSizeF whiteKeySize(15, 2.25);
    QSizeF blackKeySize(10, 1);

    float h = 0;
    for (int i = 21; i <= 108; ++i) {
        if (!isBlackKey(i)) {
            h += whiteKeySize.height();
        }
    }
    whiteKeySize *= rect.height() / h;
    blackKeySize *= rect.height() / h;

    float scale_fac = (rect.width() - whiteKeySize.width()) / f.getFileDurationInSeconds();

    QPen *pen = new QPen(QColor(0, 0, 0));
    QBrush *greenBrush = new QBrush(QColor(0, 127, 0));
    int track = 0;
    for (int i = 0; i < f[track].size(); i++) {
        if (!f[track][i].isNoteOn()) {
            continue;
        }
        int pitch = f[track][i].getP1();
        float startsec = f[track][i].seconds;
        float duration = f[track][i].getDurationInSeconds();
        std::cout << "Pitch: " << pitch << " on: " << startsec << " duration: " << duration << std::endl;
        qreal top = getYFromPitch(pitch, 21, 108, rect, QSizeF(10, 1));
        qreal left = startsec * scale_fac + rect.left() + whiteKeySize.width();
        qreal w = duration * scale_fac;
        // sc.addRect(QRectF(left, top, w, isBlackKey(pitch) ? blackKeySize.height() : whiteKeySize.height()), *pen, *greenBrush);

        auto item = new MidiNoteGraphicsItem(
                QRectF(left, top, w, isBlackKey(pitch) ? blackKeySize.height() : whiteKeySize.height()),
                f[track][i].getKeyNumber());
        item->setBrush(*greenBrush);
        item->setPen(*pen);
        sc->addItem(item);
        connect(item, &MidiNoteGraphicsItem::moused, this->noteInfo, &NoteInfo::displayNote);
    }
}

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent) {
    setObjectName("MainWindow");
    setWindowTitle("MIDI Comparison");

    scene = new QGraphicsScene(QRectF(0, 0, 100, 100), this);
    gview = new QGraphicsView(scene, this);
    noteInfo = new NoteInfo(this);
    this->setCentralWidget(gview);

    setupMenuBar();

    QDockWidget *dockWidget = new QDockWidget;
    dockWidget->setWidget(noteInfo);
    this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    gview->move(10, 10);

    // Draw a standard 88 key keyboard
    drawPianoRoll(scene, QRectF(30, 20, 100, 500), 21, 108);
}

MainWindow::~MainWindow() {

}

void MainWindow::setupMenuBar() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    gview->resize(event->size() - 2 * QSize(10, 10));
    gview->setInteractive(true);

    auto sz = gview->contentsRect().size();
    gview->setSceneRect(0, 0, sz.width(), sz.height());
}

void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    file1.read(fileName.toStdString());
    file1.doTimeAnalysis();
    file1.linkNotePairs();
    drawMidiNotes(scene, QRectF(130, 20, 500, 500), file1);
}


