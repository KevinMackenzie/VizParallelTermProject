#include "analysis.h"

float filterWeight(float f) {
    const int base = 2;
    const float pow_div = 0.001f;// Halves in size every 1000 time units
    return powf(base, -fabsf(f) * pow_div);
}

std::vector<float> filterOnsetFrequency(MidiString &str, uint32_t window) {
    std::vector<float> ret(str.size(), 0.0f);
    for (size_t i = 0; i < str.size(); ++i) {
        auto myOnset = str[i].event.onset;
        uint32_t time_accum = 0;
        float weight_accum = 0;
        for (size_t j = i; j > 0; --j) {
            if (myOnset - str[j].event.onset > window / 2) break;
            time_accum += str[j].event.onset - str[j - 1].event.onset;
            weight_accum += 1; // filterWeight(myOnset - (str[j].event.onset + str[j - 1].event.onset) / 2.f);
        }
        for (size_t j = i + 1; j < str.size(); ++j) {
            if (str[j].event.onset - myOnset > window / 2) break;
            time_accum += str[j].event.onset - str[j - 1].event.onset;
            weight_accum += 1; // filterWeight(myOnset - (str[j].event.onset + str[j - 1].event.onset) / 2.f);
        }
        if (time_accum == 0) {
            // This is bad, but never have the tempo be 0
            ret[i] = 1234567;
        } else {
            ret[i] = weight_accum / time_accum;
        }
    }
    return ret;
}

float filterFunc(float f) {
    const int base = 2;
    const float pow_div = 0.02f; // Halves in size every 50 time units
    return powf(base, -fabsf(f) * pow_div);
}

std::vector<float> filterTimeStretch(const WeightedBipartiteGraph<MidiChar> &g, const std::vector<float> &lTempo,
                                     const std::vector<float> &rTempo, uint32_t window) {
    std::vector<float> inst(g.GetR().size(), 0.0f);
    for (size_t i = 1; i < g.GetR().size(); ++i) {
        const auto &el = g.GetRNodeEdges(i);
        const auto &el0 = g.GetRNodeEdges(i - 1);
        auto ioiR = g.GetR()[i].event.onset - g.GetR()[i - 1].event.onset;
        if (el.size != 1 || el0.size != 1 || ioiR == 0) {
            inst[i] = inst[i - 1];
            continue;
        }

        const auto &e = el.to_list[0];
        const auto &e1 = el0.to_list[0];

        // Time stretch is the percent difference in inter-onset interval between the two points
        auto ioiL = abs((int) g.GetL()[e.to].event.onset - (int) g.GetL()[e1.to].event.onset);
        if (ioiL == 0)
            ioiL = 1;
        auto ltrTempo = rTempo[i] / lTempo[e.to];
        inst[i] = 1.f - ioiR / (ltrTempo * ioiL);
    }

    // Note: This could be factored out to a generic "filter" function fairly easily
    const auto &str = g.GetR();
    std::vector<float> ret(str.size(), 0.0f);
    for (size_t i = 0; i < str.size(); ++i) {
        auto myOnset = str[i].event.onset;
        float weight_accum = 0;
        float val_accum = 0;
        for (size_t j = i; j > 0; --j) {
            if (myOnset - str[j].event.onset > window / 2) break;
            auto f = filterFunc(myOnset - (str[j].event.onset + str[j - 1].event.onset) / 2.f);
            weight_accum += f;
            val_accum += inst[j] * f;
        }
        for (size_t j = i + 1; j < str.size(); ++j) {
            if (str[j].event.onset - myOnset > window / 2) break;
            auto f = filterFunc(myOnset - (str[j].event.onset + str[j - 1].event.onset) / 2.f);
            weight_accum += f;
            val_accum += inst[j] * f;
        }
        if (weight_accum == 0) {
            ret[i] = inst[i];
        } else {
            ret[i] = val_accum / weight_accum;
        }
    }
    return ret;
}
