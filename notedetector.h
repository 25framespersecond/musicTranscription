#ifndef NOTEDETECTOR_H
#define NOTEDETECTOR_H

#include <vector>
#include <map>
#include <utility>
#include <QString>
#include "frequencyaudiodata.h"

class NoteDetector
{
public:
    static std::map<QString, double> findNotes(const FrequencyAudioData &data, int maxNotes = 6, int harmonics = 5);
private:
    static int64_t harmonicProductSpectrum(const std::vector<double> &data, int harmonics);
    static QString noteName(double frequancy);

    static const double f0;
    static const char* notes[];
};

#endif // NOTEDETECTOR_H
