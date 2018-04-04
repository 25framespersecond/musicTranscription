#include "onsetdetector.h"
#include <cassert>
#include <cmath>
#include <algorithm>
#include <QDebug>

TimeAudioData OnsetDetector::detectOnset(const TimeAudioData &timeData, int window, int overlap)
{
    if (window <= 0)
        window = 1024;
    if (overlap <= 0)
        overlap = window/2;
    if (overlap > window)
        overlap = window;
    std::vector<double> onsetFunction;
    FrequencyAudioData prevFreq = FourierTransform::cooleyTukeyFFT(timeData, 0, window, 10);
    for (int64_t index = overlap; index+window < timeData.size(); index += overlap)
    {
        auto freq = FourierTransform::cooleyTukeyFFT(timeData, index, index + window, 10);
        double diff = fftDifference(freq, prevFreq);
        onsetFunction.push_back(diff);
        prevFreq = std::move(freq);
    }
    //normalize(onsetFunction);
    auto peaks = findPeaks(onsetFunction, 2, 3, 20);

    return TimeAudioData(peaks, timeData.getSampleRate()/overlap);
}

double OnsetDetector::fftDifference(const FrequencyAudioData &fft1, const FrequencyAudioData &fft2)
{
    assert (fft1.size() == fft2.size());
    //calculating difference between two freqency spectrums
    //of two adjusted time windows and suming all bins
    std::vector<double> diff(fft1.size());
    for (int64_t index = 0; index < fft1.size(); ++index)
    {
        diff[index] = fft1.getData()[index] - fft2.getData()[index];
        if (diff[index] < 0)
            diff[index] = 0;
    }

    double totalDiff = 0;
    for (int64_t index = 0; index < diff.size(); ++index)
        totalDiff += diff[index];
    return totalDiff;
}

std::vector<double> OnsetDetector::findPeaks(const std::vector<double> &func, double delta, double lambda, int window)
{
    std::vector<double> peaks = func;
    for (int64_t i = 0; i + window< func.size(); ++i)
    {
        double threshold = 0;
        for (int64_t j = i; j < i + window; ++j)
            threshold += func[j];
        threshold = lambda * threshold / window + delta;
        if (func[i + window/2] <= threshold)
            peaks[i + window/2] = 0;
    }
    return std::move(peaks);
}

void OnsetDetector::normalize(std::vector<double> &func)
{
    double max = *std::max_element(func.begin(), func.end());
    for(auto &ref : func)
        ref /= max;
}
