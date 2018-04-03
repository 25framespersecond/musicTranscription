#include "onsetdetector.h"
#include <cassert>
#include <cmath>

FrequencyAudioData OnsetDetector::detectOnset(const TimeAudioData &timeData, unsigned int window, unsigned int overlap)
{
    if (!window)
        window = 1024;
    if (!overlap)
        overlap = window/2;
    if (overlap > window)
        overlap = window;
    std::vector<double> onsetFunction;
    FrequencyAudioData prevFreq = FourierTransform::cooleyTukeyFFT(timeData, 0, window, 10);
    for (size_t index = overlap; index+window < timeData.size(); index += overlap)
    {
        auto freq = FourierTransform::cooleyTukeyFFT(timeData, index, index + window, 10);
        double diff = fftDifference(freq, prevFreq);
        onsetFunction.push_back(diff);
        prevFreq = std::move(freq);
    }
    return FrequencyAudioData(onsetFunction, timeData.getSampleRate()/overlap);
}

double OnsetDetector::fftDifference(const FrequencyAudioData &fft1, const FrequencyAudioData &fft2)
{
    assert (fft1.size() == fft2.size());
    //calculating difference between two freqency spectrums
    //of two adjusted time windows and suming all bins
    std::vector<double> diff(fft1.size());
    for (size_t index = 0; index < fft1.size(); ++index)
        diff[index] = abs(fft1.getData()[index] - fft2.getData()[index]);
    double totalDiff = 0;
    for (size_t index = 0; index < diff.size(); ++index)
        totalDiff += diff[index];
    return totalDiff;
}
