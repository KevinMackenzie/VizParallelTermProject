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


class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event) override;

private slots:
    void open();

private:
    void setupMenuBar();
    void setupToolBar();

    void openFile(std::string filename);

    PianoRollScene *scene;
    PianoRollView *gview;
    QDockWidget *noteInfoDock;
    NoteInfo *noteInfo;
    smf::MidiFile file1;

};
#endif // MAINWINDOW_H
