#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGroupBox>
#include <QRectF>
#include <iostream>
#include <QToolBar>
#include "NoteInfo.h"
#include "MidiFile.h"
#include "PianoRollScene.h"
#include "MidiAnalysis.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event) override;

private slots:
    void open1() { open(0); }
    void open2() { open(1); }

private:
    void setupMenuBar();
    void setupToolBar();

    void open(int idx);
    void openFile(std::string filename, int idx);

    PianoRollScene *scene;
    PianoRollView *gview;
    QDockWidget *noteInfoDock;
    NoteInfo *noteInfo;
    MidiAnalysis analysis;

};
#endif // MAINWINDOW_H
