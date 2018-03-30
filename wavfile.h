#ifndef WAVFILE_H
#define WAVFILE_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QDebug>
#include <vector>
#include "timeaudiodata.h"

struct WavHeader;

class WavFile : public QFile
{
    Q_OBJECT

public:

    enum ByteOrder
    {
        LittleEndian,
        BigEndian
    };

    WavFile(QObject *parent = 0);

    bool open(const QString &name);
    bool writeFile(const TimeAudioData &data, const QString &name);

    TimeAudioData getAudioData() const;

private:
    bool readHeader();
    bool readAudio();

    bool writeHeader();
    bool writeAudio();

    ByteOrder m_byteOrder;
    quint16 m_bytespersample;
    quint16 m_numChannels;
    quint32 m_sampleRate;

    quint32 m_dataSize;
    quint32 m_numSamples;

    std::vector<double> m_data;

};

#endif // WAVFILE_H
