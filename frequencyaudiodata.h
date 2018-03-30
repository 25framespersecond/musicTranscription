#ifndef FREQUENCYAUDIODATA_H
#define FREQUENCYAUDIODATA_H

#include "iaudiodata.h"

//class to store frequency domain data
class FrequencyAudioData : public IAudioData
{
public:
    FrequencyAudioData() : IAudioData() {}
    explicit FrequencyAudioData(const std::vector<double> &data, quint32 sampleRate) :
        IAudioData(data, sampleRate)
    {

    }

    virtual double getInterval() const override
    {
        return m_sampleRate/(2.0*m_data.size());
    }
private:
};

#endif // FREQUENCYAUDIODATA_H
