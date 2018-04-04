#ifndef ONSETDETECTOR_H
#define ONSETDETECTOR_H

#include <vector>
#include "fouriertransform.h"
#include "frequencyaudiodata.h"
#include "timeaudiodata.h"

class OnsetDetector
{
public:
    static TimeAudioData detectOnset(const TimeAudioData &timeData, unsigned int window = 1024, unsigned int overlap = 512);
private:
    static double fftDifference(const FrequencyAudioData &fft1, const FrequencyAudioData &fft2);
    static std::vector<double> findPeaks(const std::vector<double> &func, double delta, double lambda, unsigned int window);
    static void normalize(std::vector<double> &func);
};

#endif // ONSETDETECTOR_H
