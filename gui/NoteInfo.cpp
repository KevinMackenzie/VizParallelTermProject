#include <QVBoxLayout>
#include <QLabel>
#include <Analysis.h>
#include "NoteInfo.h"

NoteInfo::NoteInfo(QWidget *parent)
        : QGroupBox(parent) {

    QVBoxLayout *vbox = new QVBoxLayout;
    pitchLabel = new QLabel(tr("Hover over note to see pitch"));
    vbox->addWidget(pitchLabel);
    this->setLayout(vbox);

}

void NoteInfo::displayNote(int pitch) {
    pitchLabel->setText((QString("Pitch: %1").arg(pitchToNote(pitch).c_str())));
}

