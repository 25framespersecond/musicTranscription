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
        return QString("0");
    QString string(notes[noteNum%12]);
    int octaveNum = noteNum/12;
    string.append(QString::number(octaveNum));
    return string;
}

int64_t NoteDetector::harmonicProductSpectrum(const std::vector<double> &data, int harmonics)
{
    int64_t maxIndex = data.size()/harmonics;
    std::vector<double> result = data;

    int64_t maxValueIndex = 0;

    //if initial audio data contains any silence
    //then 0Hz frequency may be found as fundumental
    //zeroing it out avoids this
    result[0] = 0;

    //multiplying frequency data by i-times downsampled version of itself
    //and also looking for max value of result
    for (int64_t i = 0; i < maxIndex; ++i)
    {
        for (int j = 1; j <= harmonics; ++j)
        {
            result[i] *= result[i*j];
        }
        if (result[i] > result[maxValueIndex])
            maxValueIndex = i;
    }

    return maxValueIndex;
}

//this method finds a fundumental note using HPS
//then removes it from spectrum and looks for next note
//if next note isn't different from any priviously
//found note, algorithm stops
std::map<QString, double> NoteDetector::findNotes(const FrequencyAudioData &freq, int maxNotes, int harmonics)
{
    if(maxNotes <= 0)
        maxNotes = 1;
    if(harmonics <= 0)
        harmonics = 1;

    std::map<QString, double> notes;
    std::vector<double> data = freq.getData();
    //maxNotes is used to avoid infinitely looking for notes
    double maxPower;
    for (int64_t i = 0; i < maxNotes; ++i)
    {
        int64_t fundBin = harmonicProductSpectrum(data, harmonics);
        double fundFreq = fundBin * freq.getInterval();
        double power = data[fundBin];
        if (notes.size() == 0)
        {
            maxPower = power;
            notes.insert(std::pair<QString, double>(noteName(fundFreq), power));
        }
        else
        {
            if ( power < maxPower * 0.1 )
                break;
            else
                notes.insert(std::pair<QString, double>(noteName(fundFreq), power));
        }

        //filtering fundamental frequency and its overtones
        for (int harmon = 1; harmon <= harmonics; ++harmon)
        {
            //TODO: remove magic numbers
            int64_t currentBin = harmon * fundBin;
            int64_t leftBound, rightBound;
            leftBound = currentBin - 6;
            if (leftBound < 0)
                leftBound = 0;

            rightBound = currentBin + 7;
            if (rightBound > data.size())
                rightBound = data.size();
            for (int64_t index = leftBound; index < rightBound; ++index)
                data[index] = 0;
        }
    }
    return std::move(notes);
}
