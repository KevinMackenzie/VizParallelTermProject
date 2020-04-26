#include <Analysis.h>
#include <QGraphicsSceneMouseEvent>
#include "PianoRollScene.h"
#include "MidiNoteGraphicsItem.h"

PianoRollScene::PianoRollScene(QRectF rect, MidiAnalysis &a, int low_note, int high_note, int subdiv,
                               QWidget *parent)
        : QGraphicsScene(rect, parent), pitchAxis(rect, low_note, high_note), subdiv(subdiv), midiItems(subdiv),
          analysis(a), scrubber(nullptr){

    addItem(&pitchAxis);
    addItem(&scrubber);
    scrubber.setPos(pitchAxis.getWhiteKeySize().width(), 0);
    this->drawStaff();
}

void PianoRollScene::drawMidiNotes(smf::MidiFile &f, NoteInfo *toConnect, int idx) {
    if (idx >= subdiv) return;

    QPen *pen = new QPen(QColor(0, 0, 0));
    auto whiteKeySize = pitchAxis.getWhiteKeySize();
    auto blackKeySize = pitchAxis.getBlackKeySize();

    float width_scale = (this->width() - blackKeySize.width()) / f.getFileDurationInSeconds();

    int track = 0;
    int aaa = 0; // TODO: connect this with the actual indexing instead of loading from file twice
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
        qreal top = getYFromPitch(pitch) + ((float) idx / subdiv) * full_height;
        qreal left = startsec * width_scale + this->sceneRect().left() + whiteKeySize.width();
        qreal w = duration * width_scale;

        // Draw items
        auto item = new MidiNoteGraphicsItem(
                QRectF(left, top, w, (1.f / subdiv) * full_height), idx, aaa++);
        item->setBrush(QBrush(idx == 0 ? PianoRollScene::trackOne : PianoRollScene::trackTwo));
        item->setPen(*pen);
        this->addItem(item);
        this->midiItems[idx].push_back(item);

        // Connect mouseover signal to noteInfo
        connect(item, &MidiNoteGraphicsItem::mouseEnter, toConnect, &NoteInfo::displayNote);

        // TODO: This probably isn't the best place to handle this signal
        connect(item, &MidiNoteGraphicsItem::mouseEnter, this, &PianoRollScene::showConnectivity);
        connect(item, &MidiNoteGraphicsItem::mouseExit, this, &PianoRollScene::hideConnectivity);

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

void PianoRollScene::updateScale(QPointF origin, QSizeF scale) {
    pitchAxis.setX(origin.x());
    pitchAxis.updateXScale(scale.width());
    scrubber.updateYScale(scale.height());
}

void PianoRollScene::showConnectivity(int slotNum, int noteIdx) {
    if (!analysis.getAnalysisResults()) return;
    auto cc = (slotNum == 0) ? analysis.getAnalysisResults()->mapping.GetLCC(noteIdx)
                             : analysis.getAnalysisResults()->mapping.GetRCC(noteIdx);
    for (auto it : cc.lNodes) {
        midiItems[0][it]->setBrush(QBrush(PianoRollScene::trackOneHighlight));
    }
    for (auto it : cc.rNodes) {
        midiItems[1][it]->setBrush(QBrush(PianoRollScene::trackTwoHighlight));
    }
}

void PianoRollScene::hideConnectivity(int slotNum, int noteIdx) {
    if (!analysis.getAnalysisResults()) return;
    auto cc = (slotNum == 0) ? analysis.getAnalysisResults()->mapping.GetLCC(noteIdx)
                                : analysis.getAnalysisResults()->mapping.GetRCC(noteIdx);
    for (auto it : cc.lNodes) {
        midiItems[0][it]->setBrush(QBrush(PianoRollScene::trackOne));
    }
    for (auto it : cc.rNodes) {
        midiItems[1][it]->setBrush(QBrush(PianoRollScene::trackTwo));
    }
}

void PianoRollScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    scrubber.setPos(mouseEvent->scenePos().x(), 0);
    QGraphicsScene::mousePressEvent(mouseEvent);
}
