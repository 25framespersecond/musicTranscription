#include "notedetector.h"
#include <QDebug>
#include <cmath>

//f0 is a fundumental frequency of C0 lowest note this algorithm can detect
const double NoteDetector::f0 = 16.35;
//names of notes
const char* NoteDetector::notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

QString NoteDetector::noteName(double frequancy)
{
    int noteNum = round((12*log(frequancy/f0)) / log(2));
    //note can't be lower than C0
    if (noteNum < 0)
        noteNum = 0;
    QString string(notes[noteNum%12]);
    int octaveNum = noteNum/12;
    string.append(QString::number(octaveNum));
    return string;
}

size_t NoteDetector::harmonicProductSpectrum(unsigned int harmonics) const
{
    size_t maxIndex = m_data.getData().size()/harmonics;
    std::vector<double> result = m_data.getData();

    size_t maxValueIndex = 0;

    //if initial audio data contains any silence
    //then 0Hz frequency may be found as fundumental
    //zeroing it out avoids this
    result[0] = 0;

    //multiplying frequency data by i-times downsampled version of itself
    //and also looking for max value of result
    for (size_t i = 0; i < maxIndex; ++i)
    {
        for (size_t j = 1; j <= harmonics; ++j)
        {
            result[i] *= result[i*j];
        }
        if (result[i] > result[maxValueIndex])
            maxValueIndex = i;
    }

    return maxValueIndex;
}

//this method finds a fundumental not using HPS
//then removes it from spectrum and looks for next note
//if next note isn't different from any priviously
//found note algorithm stops
void NoteDetector::findNotes(size_t maxNotes)
{
    //maxNotes is used to avoid infinitely looking for notes
    for (size_t i = 0; i < maxNotes; ++i)
    {
        int freqIndex = (int)harmonicProductSpectrum(5);

        if (m_freqIndexes.size() == 0)
        {
            m_freqIndexes.insert(std::pair<int, double>(freqIndex, m_data.getData()[freqIndex]));
        }
        else
        {
            if (noteIsFound(freqIndex))
            {
                return;
            }
            else
            {
                m_freqIndexes.insert(std::pair<int, double>(freqIndex, m_data.getData()[freqIndex]));
            }
        }

        //zeroing frequencies of fundamental note
        int overtone = freqIndex;
        int multi = 1;
        while (overtone*multi < m_data.getData().size())
        {
            m_data.getData()[overtone*multi++] = 0;
            if (multi == 5)
                break;
        }
    }
}

bool NoteDetector::noteIsFound(size_t freqIndex) const
{
    //checking if frequency was already found before
    if (m_freqIndexes.count(freqIndex - 1) || m_freqIndexes.count(freqIndex + 1))
        return true;
    else
        return false;
}

std::map<QString, double> NoteDetector::getNotes() const
{
    std::map<QString, double> result;
    auto it = m_freqIndexes.begin();
    for ( ; it != m_freqIndexes.end(); ++it)
    {
        double frequency = it->first * m_data.getInterval();
        qDebug() << frequency;
        QString name = NoteDetector::noteName(frequency);
        result.insert(std::pair<QString, double>(name, it->second));
    }
    return std::move(result);
}
