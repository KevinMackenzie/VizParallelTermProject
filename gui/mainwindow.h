#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include "MidiGraphicsView.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

  QGraphicsScene scene;
  MidiGraphicsView gview;
public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;

  void resizeEvent(QResizeEvent *event) override;

private:
  Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
