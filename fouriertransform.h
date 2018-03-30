#ifndef FOURIERTRANSFORM_H
#define FOURIERTRANSFORM_H

#include <vector>
#include <complex>
#include "timeaudiodata.h"
#include "frequencyaudiodata.h"

using Complex = std::complex<double>;

class FourierTransform
{
public:

    //discrete fourier transform with O(N^2) of data in range [start, end)
    static FrequencyAudioData dft(const TimeAudioData &data, size_t start, size_t end);

    //Cooley-Tukey fast discrete fourier transform with O(NlogN) of data in range [start, end)
    static FrequencyAudioData cooleyTukeyFFT(const TimeAudioData &data, size_t start, size_t end, unsigned int fftSizeFactor = 15);

private:
    static size_t reverseBitOrder(size_t a);
    static void ctfft(std::vector<Complex> &x);
    static std::vector<double> hannWindow(const std::vector<double> &data, size_t start, size_t dataSize);
    static void zeroPadding(std::vector<double> &data, size_t fftSize);
};



#endif // FOURIERTRANSFORM_H
