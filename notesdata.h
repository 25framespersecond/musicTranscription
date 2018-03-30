#ifndef NOTESDATA_H
#define NOTESDATA_H

#include <vector>
#include <map>
#include <utility>
#include <QString>
#include "frequencyaudiodata.h"

class NotesData
{
public:
    NotesData(const FrequencyAudioData & data) : m_data(data) {}
    size_t harmonicProductSpectrum(unsigned int harmonics) const;
    void findNotes(size_t maxNotes);
    std::map<QString, double> getNotes() const;
private:
    bool noteIsFound(size_t freqIndex) const;
    static QString noteName(double frequancy);

    FrequencyAudioData m_data;
    std::map<int, double> m_freqIndexes;

    static const double f0;
    static const char* notes[];
};

#endif // NOTESDATA_H
