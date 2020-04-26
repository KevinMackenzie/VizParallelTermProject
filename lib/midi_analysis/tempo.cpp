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
        uint32_t min_time = 0;
        uint32_t max_time = myOnset;
        float weight_accum = 0;
        for (size_t j = i; j > 0; --j) {
            if (myOnset - str[j].event.onset > window / 2) break;
            min_time = std::min(min_time, str[j - 1].event.onset);
            weight_accum += filterWeight(myOnset - (str[j].event.onset + str[j - 1].event.onset) / 2.f);
        }
        for (size_t j = i + 1; j < str.size(); ++j) {
            if (str[j].event.onset - myOnset > window / 2) break;
            max_time = std::max(max_time, str[j].event.onset);
            weight_accum += filterWeight(myOnset - (str[j].event.onset + str[j - 1].event.onset) / 2.f);
        }
        if (min_time == max_time) {
            // This is bad, but never have the tempo be 0
            ret[i] = 1234567;
        } else {
            ret[i] = weight_accum / (min_time - max_time);
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
    for (size_t i = 0; i < g.GetR().size(); ++i) {
        const auto &el = g.GetRNodeEdges(i);
        if (el.size != 1) continue;
        const auto &e = el.to_list[0];

        if (i == 0 || e.to == 0) {
            // At the beginning, there is no time stretch
            inst[i] = 0.f;
        } else {
            // Time stretch is the percent difference in inter-onset interval between the two points
            float ioiL = g.GetL()[e.to].event.onset - g.GetL()[e.to - 1].event.onset;
            float ioiR = g.GetR()[i].event.onset - g.GetR()[i - 1].event.onset;
            if (ioiL == 0)
                ioiL = 1;
            if (ioiR == 0) {
                inst[i] = inst[i - 1];
            } else {
                float ltrTempo = rTempo[i] / lTempo[e.to];
                inst[i] = 1.f - ioiR / (ltrTempo * ioiL);
            }
        }
    }

    // Note: This could be factored out to a generic "filter" function fairly easily
    const auto &str = g.GetR();
    std::vector<float> ret(str.size(), 0.0f);
    for (size_t i = 0; i < str.size(); ++i) {
        auto myOnset = str[i].event.onset;
        auto myVal = inst[i];
        uint32_t min_time = 0;
        uint32_t max_time = myOnset;
        float weight_accum = 0;
        for (size_t j = i; j > 0; --j) {
            if (myOnset - str[j].event.onset > window / 2) break;
            min_time = std::min(min_time, str[j - 1].event.onset);
            weight_accum += myVal * filterFunc(myOnset - (str[j].event.onset + str[j - 1].event.onset) / 2.f);
        }
        for (size_t j = i + 1; j < str.size(); ++j) {
            if (str[j].event.onset - myOnset > window / 2) break;
            max_time = std::max(max_time, str[j].event.onset);
            weight_accum += myVal * filterFunc(myOnset - (str[j].event.onset + str[j - 1].event.onset) / 2.f);
        }
        if (min_time == max_time) {
            // This is bad
            ret[i] = 1234567;
        } else {
            ret[i] = weight_accum / (min_time - max_time);
        }
    }
    return ret;
}
