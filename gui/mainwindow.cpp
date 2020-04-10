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


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent) {
    setObjectName("MainWindow");
    setWindowTitle("MIDI Comparison");

    scene = new PianoRollScene(QRectF(0, 0, 5000, 500), 21, 108, this);
    gview = new QGraphicsView(scene, this);
    noteInfo = new NoteInfo(this);
    this->setCentralWidget(gview);

    setupMenuBar();

    QDockWidget *dockWidget = new QDockWidget;
    dockWidget->setWidget(noteInfo);
    this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);

//    gview->move(10, 10);
}

void MainWindow::setupMenuBar() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
//    QWidget::resizeEvent(event);
//    gview->resize(event->size() - 2 * QSize(10, 10));
//    gview->setInteractive(true);
//
//    auto sz = gview->contentsRect().size();
//    gview->setSceneRect(0, 0, sz.width(), sz.height());
}

void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    file1.read(fileName.toStdString());
    file1.doTimeAnalysis();
    file1.linkNotePairs();
    scene->drawMidiNotes(file1, noteInfo);
}


