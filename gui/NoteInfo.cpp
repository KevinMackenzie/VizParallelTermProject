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
    if (analysis.getAnalysisResults()) {
        if (slotNum == 0) {
            pitch = analysis.getAnalysisResults()->inpList.events[noteIdx].pitch;
        } else {
            pitch = analysis.getAnalysisResults()->refList.events[noteIdx].pitch;
        }
    }
    pitchLabel->setText((QString("Pitch: %1").arg(pitchToNote(pitch).c_str())));
}

