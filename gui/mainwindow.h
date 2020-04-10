#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGroupBox>
#include <QRectF>
#include <iostream>
#include "NoteInfo.h"
#include "MidiFile.h"


class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void resizeEvent(QResizeEvent *event) override;

private slots:
    void open();

private:
    void setupMenuBar();
    void drawMidiNotes(QGraphicsScene *sc, QRectF rect, smf::MidiFile f);

    QGraphicsScene *scene;
    QGraphicsView *gview;
    NoteInfo *noteInfo;
    smf::MidiFile file1;
};
#endif // MAINWINDOW_H
