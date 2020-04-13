#include <QVBoxLayout>
#include <QLabel>
#include <Analysis.h>
#include "NoteInfo.h"

NoteInfo::NoteInfo(const MidiAnalysis& a, QWidget *parent)
        : QGroupBox(parent), analysis(a) {

    QVBoxLayout *vbox = new QVBoxLayout;
    pitchLabel = new QLabel(tr("Hover over note to see pitch"));
    vbox->addWidget(pitchLabel);
    this->setLayout(vbox);

}

void NoteInfo::displayNote(int slotNum, int noteIdx) {
    int pitch = 0;
    if (analysis.getMapping()) {
        if (slotNum == 0) {
            pitch = analysis.getMapping()->GetL()[noteIdx].pitch;
        } else {
            pitch = analysis.getMapping()->GetR()[noteIdx].pitch;
        }
    }
    pitchLabel->setText((QString("Pitch: %1").arg(pitchToNote(pitch).c_str())));
}

