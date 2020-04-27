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

void PianoRollScene::drawMidiNotes(const MidiString &f, NoteInfo *toConnect, int idx) {
    if (idx >= subdiv) return;

    static const QPen normalPen = QPen(QColor(0, 0, 0));
    static const QPen wrongPen = QPen(QColor(255, 0, 0));
    static const QPen matchProbPen = QPen(QColor(255, 180, 80));
    auto whiteKeySize = pitchAxis.getWhiteKeySize();
    auto blackKeySize = pitchAxis.getBlackKeySize();

    if (idx == 0)
        width_scale = (this->width() - blackKeySize.width()) / (f.back().onset + f.back().duration);

    for (size_t i = 0; i < f.size(); ++i) {
        const QPen *pen = &normalPen;
        // if (!f[track][i].isNoteOn()) {
        //     continue;
        // }
        // Get note values
        int pitch = f[i].pitch;
        float startsec = f[i].onset;
        float duration = f[i].duration;
        // int pitch = f[track][i].getP1();
        // float startsec = f[track][i].seconds;
        // float duration = f[track][i].getDurationInSeconds();
//        std::cout << "Pitch: " << pitch << " on: " << startsec << " duration: " << duration << std::endl;

        // Calculate Coordinates
        qreal full_height = isBlackKey(pitch) ? blackKeySize.height() : whiteKeySize.height();
        qreal top = getYFromPitch(pitch) + ((float) idx / subdiv) * full_height;
        qreal left = startsec * width_scale + this->sceneRect().left() + whiteKeySize.width();
        qreal w = duration * width_scale;

        // Draw items
        auto item = new MidiNoteGraphicsItem(
                QRectF(left, top, w, (1.f / subdiv) * full_height), idx, i);
        item->setBrush(QBrush(idx == 0 ? PianoRollScene::trackOne : PianoRollScene::trackTwo));
        if (analysis.getAnalysisResults()) {
            std::optional<MidiAnalysis::out_data> res = analysis.getAnalysisResults();
            AdjacencyData a;
            connected_component cc;
            if (idx == 0) {
                a = res->mapping.GetLNodeEdges(i);
                cc = res->mapping.GetLCC(i);
            } else {
                a = res->mapping.GetRNodeEdges(i);
                cc = res->mapping.GetRCC(i);
            }
            if (cc.rNodes.size() != 1 || cc.lNodes.size() != 1) {
                pen = &matchProbPen;
            } else if ((idx == 0 && res->inpList.events[a.to_list[0].to].pitch != f[i].pitch)
                    || (idx == 1 && res->refList.events[a.to_list[0].to].pitch != f[i].pitch)) {
                pen = &wrongPen;
            }
        }
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
