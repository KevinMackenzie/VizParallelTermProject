//Custom Qt Items
#include "mainwindow.h"
#include "MidiNoteGraphicsItem.h"
#include "PianoRollView.h"

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
    gview = new PianoRollView(scene, this);
    this->setCentralWidget(gview);
    gview->centerOn(0, 250);

    noteInfo = new NoteInfo(this);
    noteInfoDock = new QDockWidget(tr("Note Information"), this);
    noteInfoDock->setWidget(noteInfo);
    this->addDockWidget(Qt::TopDockWidgetArea, noteInfoDock);

    setupMenuBar();
    setupToolBar();
    openFile("midi_files/test1.mid"); //TODO: Remove after testing
}

void MainWindow::setupMenuBar() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);

    QMenu *viewMenu = menuBar()->addMenu(tr("View"));
    viewMenu->addAction(noteInfoDock->toggleViewAction());
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
    openFile(fileName.toStdString());
}

void MainWindow::setupToolBar() {
    QToolBar * zoomToolBar = addToolBar(tr("Zoom"));
    removeToolBar(zoomToolBar);
    addToolBar(Qt::BottomToolBarArea, zoomToolBar);
    zoomToolBar->show();
    zoomToolBar->setAllowedAreas(Qt::BottomToolBarArea);
    zoomToolBar->setMovable(false);

    QAction *zoomIn = new QAction(tr("+"), this);
    connect(zoomIn, &QAction::triggered, gview, &PianoRollView::zoomIn);
    zoomToolBar->addAction(zoomIn);
    
    QAction *zoomOut = new QAction(tr("-"), this);
    connect(zoomOut, &QAction::triggered, gview, &PianoRollView::zoomOut);
    zoomToolBar->addAction(zoomOut);
    
    zoomToolBar->addSeparator();
    
    QAction *zoomInX = new QAction(tr("+X"), this);
    connect(zoomInX, &QAction::triggered, gview, &PianoRollView::zoomInX);
    zoomToolBar->addAction(zoomInX);

    QAction *zoomOutX = new QAction(tr("-X"), this);
    connect(zoomOutX, &QAction::triggered, gview, &PianoRollView::zoomOutX);
    zoomToolBar->addAction(zoomOutX);

    zoomToolBar->addSeparator();
    
    QAction *zoomInY = new QAction(tr("+Y"), this);
    connect(zoomInY, &QAction::triggered, gview, &PianoRollView::zoomInY);
    zoomToolBar->addAction(zoomInY);

    QAction *zoomOutY = new QAction(tr("-Y"), this);
    connect(zoomOutY, &QAction::triggered, gview, &PianoRollView::zoomOutY);
    zoomToolBar->addAction(zoomOutY);

}

void MainWindow::openFile(std::string fileName) {
    file1.read(fileName);
    file1.doTimeAnalysis();
    file1.linkNotePairs();
    scene->clearMidiNotes();
    scene->drawMidiNotes(file1, noteInfo);
}


