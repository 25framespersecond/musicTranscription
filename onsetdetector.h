#ifndef ONSETDETECTOR_H
#define ONSETDETECTOR_H

#include <vector>
#include "fouriertransform.h"
#include "frequencyaudiodata.h"
#include "timeaudiodata.h"

class OnsetDetector
{
public:
    static TimeAudioData detectOnset(const TimeAudioData &timeData, int window = 2048, int overlap = 1024);
private:
    static double fftDifference(const FrequencyAudioData &fft1, const FrequencyAudioData &fft2);
    static std::vector<double> findPeaks(const std::vector<double> &func, double delta, double lambda, int window);
    static void normalize(std::vector<double> &func);
    static void smooth(std::vector<double> &func, int window = 5);
    static double median(const std::vector<double> &func, size_t start, size_t end);
    static double average(const std::vector<double> &func, size_t start, size_t end);
};

#endif // ONSETDETECTOR_H
