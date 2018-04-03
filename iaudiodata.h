#ifndef IAUDIODATA_H
#define IAUDIODATA_H

#include <vector>
#include <utility>
#include <QtGlobal>

//interface class for frequency and time domain data classes
class IAudioData
{
public:
    IAudioData() {}
    explicit IAudioData(const std::vector<double> &data, quint32 sampleRate) :
        m_data(data), m_sampleRate(sampleRate) {}
    //virtual destructor, default move and copy assignment/constructor
    virtual ~IAudioData() {}
    IAudioData(const IAudioData&) =             default;
    IAudioData& operator=(const IAudioData&) =  default;
    IAudioData(IAudioData&&) =                  default;
    IAudioData& operator=(IAudioData&&) =       default;

    const std::vector<double> &getData() const { return m_data; }
    std::vector<double> &getData() { return m_data; }
    size_t size() const { return m_data.size(); }
    quint32 getSampleRate() const { return m_sampleRate; }
    virtual double getInterval() const = 0;
protected:
    std::vector<double> m_data;
    quint32 m_sampleRate;
};

#endif // IAUDIODATA_H
