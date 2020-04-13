#ifndef VIZPARALLELTERMPROJECT_NOTEINFO_H
#define VIZPARALLELTERMPROJECT_NOTEINFO_H


#include <QGroupBox>
#include <QLabel>
#include "MidiAnalysis.h"

class NoteInfo : public QGroupBox {
Q_OBJECT

public:
    explicit NoteInfo(const MidiAnalysis& analysis, QWidget *parent = nullptr);

public slots:
    void displayNote(int slotNum, int noteIdx);

private:
    QLabel * pitchLabel;
    const MidiAnalysis& analysis;
};


#endif //VIZPARALLELTERMPROJECT_NOTEINFO_H
