#include "PianoRollScene.h"
#include "MidiNoteGraphicsItem.h"

PianoRollScene::PianoRollScene(QRectF rect, int low_note, int high_note, QWidget *parent)
        : QGraphicsScene(rect, parent), low_note(low_note), high_note(high_note) {
    numWhiteKeys = 0;

    for (unsigned int i = low_note; i <= high_note; ++i) {
        if (!isBlackKey(i)) {
            ++numWhiteKeys;
        }
    }

    qreal unit = this->height() / numWhiteKeys;
    whiteKeySize = QSizeF(6 * unit, unit);
    blackKeySize = QSizeF(4 * unit, unit / 2);

    this->drawStaff();
    this->drawKeyboard();
}

void PianoRollScene::drawKeyboard() {
    QPen *pen = new QPen(QColor(0, 0, 0));
    QBrush *blackBrush = new QBrush(QColor(0, 0, 0));

    float left = this->sceneRect().left();

    //Draw white keys
    float top = this->sceneRect().top();
    for (int i = high_note; i >= low_note; --i) {
        if (!isBlackKey(i)) {
            this->addRect(QRectF(QPointF(left, top), whiteKeySize));
            top += whiteKeySize.height();
        }
    }

    // Draw black keys after (on top)
    top = this->sceneRect().top();
    for (int i = high_note; i >= low_note; --i) {
        if (isBlackKey(i)) {
            this->addRect(QRectF(QPointF(left, top - blackKeySize.height() / 2), blackKeySize), *pen, *blackBrush);
        } else {
            top += whiteKeySize.height();
        }
    }

}

bool PianoRollScene::isBlackKey(int midiIdx) {
    int pc = midiIdx % 12;
    return (pc < 4 && pc % 2 == 1) || (pc > 4 && pc % 2 == 0);
}

void PianoRollScene::drawMidiNotes(smf::MidiFile &f, NoteInfo *toConnect) {
    QPen *pen = new QPen(QColor(0, 0, 0));
    QBrush *greenBrush = new QBrush(QColor(0, 127, 0));

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
        qreal top = getYFromPitch(pitch);
        qreal left = startsec * width_scale + this->sceneRect().left() + whiteKeySize.width();
        qreal w = duration * width_scale;

        // Draw items
        auto item = new MidiNoteGraphicsItem(
                QRectF(left, top, w, isBlackKey(pitch) ? blackKeySize.height() : whiteKeySize.height()),
                f[track][i].getKeyNumber());
        item->setBrush(*greenBrush);
        item->setPen(*pen);
        this->addItem(item);

        // Connect mouseover signal to noteInfo
        connect(item, &MidiNoteGraphicsItem::moused, toConnect, &NoteInfo::displayNote);
    }
}

void PianoRollScene::drawStaff() {
    QPen pen = QPen(QColor(0, 0, 0));
    qreal top = this->sceneRect().top();
    qreal left = this->sceneRect().left();
    qreal right = this->sceneRect().right();
    for (unsigned int i = 0; i <= this->numWhiteKeys; ++i) {
        this->addLine(left, top, right, top, pen);
        top += this->whiteKeySize.height();
    }
}

qreal PianoRollScene::getYFromPitch(int pitch) {
    int whiteKeyIdx = 0;
    for (int i = this->high_note; i > pitch; --i) {
        if (!isBlackKey(i)) {
            ++whiteKeyIdx;
        }
    }

    float whiteKeyY = whiteKeyIdx * this->whiteKeySize.height();
    if (isBlackKey(pitch)) {
        return whiteKeyY - (this->blackKeySize.height() / 2);
    }
    return whiteKeyY;
}
