#ifndef VIZPARALLELTERMPROJECT_NOTEINFO_H
#define VIZPARALLELTERMPROJECT_NOTEINFO_H


#include <QGroupBox>
#include <QLabel>

class NoteInfo : public QGroupBox {
Q_OBJECT

public:
    explicit NoteInfo(QWidget *parent = nullptr);

public slots:
    void displayNote(int pitch);

private:
    QLabel * pitchLabel;
};


#endif //VIZPARALLELTERMPROJECT_NOTEINFO_H
