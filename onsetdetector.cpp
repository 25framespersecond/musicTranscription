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
    for (size_t index = overlap; index+window < timeData.size(); index += overlap)
    {
        auto freq = FourierTransform::cooleyTukeyFFT(timeData, index, index + window, 10);
        double diff = fftDifference(freq, prevFreq);
        onsetFunction.push_back(diff);
        prevFreq = std::move(freq);
    }
    normalize(onsetFunction);
    smooth(onsetFunction, 6);

    auto peaks = findPeaks(onsetFunction, 0.01, 1, 14);

    return TimeAudioData(peaks, timeData.getSampleRate()/overlap);
}

double OnsetDetector::fftDifference(const FrequencyAudioData &fft1, const FrequencyAudioData &fft2)
{
    assert (fft1.size() == fft2.size());
    //calculating difference between two freqency spectrums
    //of two adjusted time windows and suming all bins
    std::vector<double> diff(fft1.size());
    for (size_t index = 0; index < fft1.size(); ++index)
    {
        diff[index] = fft1.getData()[index] - fft2.getData()[index];
        if (diff[index] < 0)
            diff[index] = 0;
    }

    double totalDiff = 0;
    for (size_t index = 0; index < diff.size(); ++index)
        totalDiff += diff[index];
    return totalDiff;
}

std::vector<double> OnsetDetector::findPeaks(const std::vector<double> &func, double delta, double lambda, int window)
{
    if (window <= 0)
        window = 1;
    if (delta < 0)
        delta *= -1;
    if (lambda < 0)
        lambda *= -1;

    std::vector<double> peaks = func;
    for (size_t index = 0; index < func.size(); ++index)
    {
        size_t start = (index < window/2) ? 0 : index - window/2;
        size_t end = (index + window/2 > func.size()) ? func.size() : index + window/2;
        double threshold = delta + lambda * median(func, start, end);
        if (func[index] < threshold)
            peaks[index] = 0;
    }
    return peaks;
}

void OnsetDetector::normalize(std::vector<double> &func)
{
    double max = *std::max_element(func.begin(), func.end());
    double mean = average(func, 0, func.size());
    for(auto &ref : func)
        ref = abs(ref - mean) / max;
}

void OnsetDetector::smooth(std::vector<double> &func, int window)
{
    std::vector<double> input = func;
    for (size_t index = 0; index < func.size(); ++index)
    {
        size_t start = (index < window/2) ? 0 : index - window/2;
        size_t end = (index + window/2 > func.size()) ? func.size() : index + window/2;
        func[index] = average(input, start, end);
    }
}

double OnsetDetector::median(const std::vector<double> &func, size_t start, size_t end)
{
    //if subvector size is 0 return 0
    if (start >= end)
        return 0;

    if (end > func.size())
        return 0;
    //if subvector size = 1 return the only element of subvector
    if (end == start+1)
        return func[start];
    //creating subvector of func
    auto first = func.begin() + start;
    auto last = func.begin() + end;
    std::vector<double> part(first, last);

    //finding median using nth_element()
    size_t mid = part.size()/2;
    std::nth_element(part.begin(), part.begin()+mid, part.end());
    return part[mid];
}

double OnsetDetector::average(const std::vector<double> &func, size_t start, size_t end)
{
    if (start >= end)
        return 0;

    if (end > func.size())
        return 0;

    if (end == start + 1)
        return func[start];

    double ave = 0;
    for (size_t index = start; index < end; ++index)
    {
        ave += func[index];
    }
    size_t n = end - start;
    ave /= n;
    return ave;
}
