#include "fouriertransform.h"
#include <cmath>
#include <utility>
#include <QDebug>

const double PI = 3.14159265359;

//this function is made only for educational purposes
//and should not be used as it's too expensive O(N^2)
FrequencyAudioData FourierTransform::dft(const TimeAudioData &audio, size_t start, size_t end)
{
   std::vector<double> result;
   if(start >= end || start >= audio.getData().size() || end > audio.getData().size())
       return FrequencyAudioData(result, audio.getSampleRate());

   size_t numOfElements = end-start;

   double Wn = -(2 * PI) / (double)numOfElements;
   for(size_t freq = 0; freq <= numOfElements/2; ++freq)
   {
       double Xreal = 0;
       double Ximag = 0;
       for( size_t index = 0; index < numOfElements; ++index)
       {
           Xreal += audio.getData()[start+index]*cos(Wn*index*freq);
           Ximag += audio.getData()[start+index]*sin(Wn*index*freq);
       }
       double magnitude = sqrt(Xreal*Xreal+Ximag*Ximag);
       result.push_back(magnitude);

   }
   return FrequencyAudioData(result, audio.getSampleRate());
}

FrequencyAudioData FourierTransform::cooleyTukeyFFT(const TimeAudioData &audio, size_t start, size_t end, unsigned int fftSizeFactor)
{
    //return nothing if start and end are out of range
    if(start >= end || start >= audio.getData().size() || end > audio.getData().size())
        return FrequencyAudioData(std::vector<double>(), audio.getSampleRate());

    //finding fftSize
    size_t dataSize = end - start;
    if (fftSizeFactor > sizeof(size_t)*8)
        fftSizeFactor = sizeof(size_t)*8;
    size_t fftSize = static_cast<size_t>(1) << fftSizeFactor;
    if (dataSize > fftSize)
        dataSize = fftSize;

    //applying Hann Window
    std::vector<double> input = hannWindow(audio.getData(), start, dataSize);

    //zero padding for fft size
    zeroPadding(input, fftSize);

    //rearranging input array into reverse bit order array, also converting it to complex numbers
    std::vector<Complex> output(fftSize);
    int m = log2(fftSize);
    for (size_t index = 0; index < fftSize; ++index)
    {
        //reversing bits
        size_t rev_index = static_cast<size_t>(reverseBitOrder(index) >> (sizeof(reverseBitOrder(index))*8 - m));
        output[index] = input[rev_index];
    }

    //running fft on output
    ctfft(output);

    //geting magnitudes from vector of complex numbers and populating vector of doubles with it
    std::vector<double> result;
    result.reserve(fftSize);
    for(size_t index = 0; index < output.size()/2; ++index)
    {
        result.push_back(abs(output[index]));
    }
    return  FrequencyAudioData(result, audio.getSampleRate());
}

quint64 FourierTransform::reverseBitOrder(quint64 a)
{
    //devide and conquer strategy
    //first swaping half of all bits then quarters and so on
    a = (a & 0x00000000ffffffff) << 32 | (a & 0xffffffff00000000) >> 32;
    a = (a & 0x0000ffff0000ffff) << 16 | (a & 0xffff0000ffff0000) >> 16;
    a = (a & 0x00ff00ff00ff00ff) << 8  | (a & 0xff00ff00ff00ff00) >> 8;
    a = (a & 0x0f0f0f0f0f0f0f0f) << 4  | (a & 0xf0f0f0f0f0f0f0f0) >> 4;
    a = (a & 0x3333333333333333) << 2  | (a & 0xcccccccccccccccc) >> 2;
    a = (a & 0x5555555555555555) << 1  | (a & 0xaaaaaaaaaaaaaaaa) >> 1;
    return a;
}

//Cooley-Tukey transform (in-place, decimation in time)
void FourierTransform::ctfft(std::vector<Complex> &x)
{
    size_t N = x.size();
    size_t step = 2;
    size_t halfStep = step/2;
    //going through stages
    while(step <= N)
    {
        //going through DFTs
        for(size_t i = 0; i < N; i += step)
        {
            //adding complex numbers inside each DFT (taking into account twiddle factor)
            for(size_t j=0; j < halfStep; ++j)
            {
                //calculating twiddle factor (W) as e^(-2)*PI*j/step
                Complex W = std::polar(1.0, -2.0 * PI * j / step) * x[i + j + halfStep];
                Complex temp = x[i + j] + W;
                x[i + j + halfStep] = x[i + j] - W;
                x[i + j] = temp;
            }
        }
        halfStep = step;
        step <<= 1;
    }
}

std::vector<double> FourierTransform::hannWindow(const std::vector<double> &data, size_t start, size_t dataSize)
{
    std::vector<double> result(dataSize);
    for (size_t index = 0; index < dataSize; ++index)
    {
        double multiplier = 0.5 * (1 - cos(2*PI*index/(dataSize-1)));
        result[index] = multiplier * data[start+index];
    }
    return std::move(result);
}

void FourierTransform::zeroPadding(std::vector<double> &data, size_t fftSize)
{
    data.reserve(data.size()+fftSize);
    for (size_t index = data.size(); index < fftSize; ++index)
    {
        data.push_back(0);
    }
}
