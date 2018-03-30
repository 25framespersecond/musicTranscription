#ifndef TIMEAUDIODATA_H
#define TIMEAUDIODATA_H

#include "iaudiodata.h"

//class to store time domain data
class TimeAudioData : public IAudioData
{
public:
    TimeAudioData() : IAudioData() {}
    explicit TimeAudioData(const std::vector<double> &data, quint32 sampleRate) :
        IAudioData(data, sampleRate)
    {

    }

    virtual double getInterval() const override
    {
        return 1.0/m_sampleRate;
    }
private:
};

#endif // TIMEAUDIODATA_H
