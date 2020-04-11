#include <Analysis.h>
#include "PianoRollScene.h"
#include "MidiNoteGraphicsItem.h"

PianoRollScene::PianoRollScene(QRectF rect, int low_note, int high_note, int subdiv, QWidget *parent)
        : QGraphicsScene(rect, parent), pitchAxis(rect, low_note, high_note), subdiv(subdiv), midiItems(subdiv) {

    addItem(&pitchAxis);
    this->drawStaff();
}

void PianoRollScene::drawMidiNotes(smf::MidiFile &f, NoteInfo *toConnect, int idx) {
    if (idx >= subdiv) return;

    QPen *pen = new QPen(QColor(0, 0, 0));
    QBrush *greenBrush = new QBrush(QColor(0, 127, 0));
    auto whiteKeySize = pitchAxis.getWhiteKeySize();
    auto blackKeySize = pitchAxis.getBlackKeySize();

    float width_scale = (this->width() - blackKeySize.width()) / f.getFileDurationInSeconds();

    int track = 0;
    for (int i = 0; i < f[track].size(); i++) {
        if (!f[track][i].isNoteOn()) {
            continue;
        }
         // Get note values
        int pitch = f[track][i].getP1();
        float startsec = f[track][i].seconds;
        float duration = f[track][i].getDurationInSeconds();
//        std::cout << "Pitch: " << pitch << " on: " << startsec << " duration: " << duration << std::endl;

        // Calculate Coordinates
        qreal full_height = isBlackKey(pitch) ? blackKeySize.height() : whiteKeySize.height();
        qreal top = getYFromPitch(pitch) + ((float)idx/subdiv) * full_height;
        qreal left = startsec * width_scale + this->sceneRect().left() + whiteKeySize.width();
        qreal w = duration * width_scale;

        // Draw items
        auto item = new MidiNoteGraphicsItem(
                QRectF(left, top, w, (1.f/subdiv) * full_height),
                f[track][i].getKeyNumber());
        item->setBrush(*greenBrush);
        item->setPen(*pen);
        this->addItem(item);
        this->midiItems[idx].push_back(item);

        // Connect mouseover signal to noteInfo
        connect(item, &MidiNoteGraphicsItem::moused, toConnect, &NoteInfo::displayNote);
    }
}

void PianoRollScene::clearMidiNotes(int idx) {
    if (idx >= subdiv) return;

    for (auto item : midiItems[idx]) {
        this->removeItem(item);
    }
    midiItems[idx].clear();
}

void PianoRollScene::drawStaff() {
    QPen pen = QPen(QColor(0, 0, 0));
    qreal top = this->sceneRect().top();
    qreal left = this->sceneRect().left();
    qreal right = this->sceneRect().right();
    for (unsigned int i = 0; i <= this->pitchAxis.getNumWhiteKeys(); ++i) {
        this->addLine(left, top, right, top, pen);
        top += pitchAxis.getWhiteKeySize().height();
    }
}

qreal PianoRollScene::getYFromPitch(int pitch) {
    int whiteKeyIdx = 0;
    for (int i = this->pitchAxis.getHighNote(); i > pitch; --i) {
        if (!isBlackKey(i)) {
            ++whiteKeyIdx;
        }
    }

    float whiteKeyY = whiteKeyIdx * pitchAxis.getWhiteKeySize().height();
    if (isBlackKey(pitch)) {
        return whiteKeyY - (pitchAxis.getBlackKeySize().height() / 2);
    }
    return whiteKeyY;
}
