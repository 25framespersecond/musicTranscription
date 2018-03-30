#include "wavfile.h"
#include <QtGlobal>
#include <cstring>
#include <QtEndian>

struct ChunkDescriptor
{
    char id[4];
    quint32 size;
};

struct WaveHeader
{

    struct RiffChunk
    {
        ChunkDescriptor descriptor;
        char format[4];
    };
    struct FmtChunk
    {
        ChunkDescriptor descriptor;
        quint16 audioFormat;
        quint16 numChannels;
        quint32 sampleRate;
        quint32 byteRate;
        quint16 blockAlign;
        quint16 bitsPerSample;
    };



    RiffChunk riff;
    FmtChunk fmt;
};


WavFile::WavFile(QObject *parent)
    : QFile(parent)
{
}

bool WavFile::open(const QString &name)
{
    QFile::close();
    setFileName(name);
    if(!QFile::open(QIODevice::ReadOnly))
        return false;

    if(!readHeader())
        return false;


    if (!readAudio())
        return false;

    QFile::close();
    return true;

}

bool WavFile::writeFile(const TimeAudioData &data, const QString &name)
{
    QFile::close();
    setFileName(name);
    m_data = data.getData();
    m_sampleRate = data.getSampleRate();
    m_bytespersample = 2;

    //currently those two parameters don't play any role in how file is written
    //it's always written as LittleEndian in only one channel
    m_byteOrder = LittleEndian;
    m_numChannels = 1;
    if(!QFile::open(QIODevice::WriteOnly))
        return false;

    if (!writeHeader())
        return false;

    if (!writeAudio())
        return false;
    QFile::close();
    return true;
}

bool WavFile::readHeader()
{
    seek(0);
    WaveHeader header;
    bool result = read(reinterpret_cast<char *>(&header), sizeof(WaveHeader)) == sizeof(WaveHeader);
    if (result)
    {
        //check format
       if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0
               || memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0)
               && memcmp(&header.riff.format, "WAVE", 4) == 0
               && memcmp(&header.fmt.descriptor.id, "fmt ", 4) == 0
               && header.fmt.audioFormat == 1)
       {
            //find data subchunk (skip any extraParams)
            char dataID[4];
            do
            {
                if (peek(dataID, sizeof(dataID)) != sizeof(dataID))
                    return false;
                seek(pos()+1);
            } while (memcmp(dataID, "data", sizeof(dataID)));

            //setting file pointer position after "data"
            seek(pos()+3);
            header.fmt.descriptor.size = 16;


            if (read(reinterpret_cast<char*>(&m_dataSize), sizeof(m_dataSize)) != sizeof(m_dataSize))
                return false;

            //setting format
            if (memcmp(&header.riff.descriptor, "RIFF", 4) == 0)
                m_byteOrder = LittleEndian;
            else
            {
                //TODO: make reading BigEndian possible
                //for now it will return false as if file could not be read
                m_byteOrder = BigEndian;
                return false;
            }
            m_bytespersample = header.fmt.bitsPerSample / 8;
            m_numChannels = header.fmt.numChannels;
            m_sampleRate = header.fmt.sampleRate;
            m_numSamples = m_dataSize/(m_bytespersample*m_numChannels);
       }
       else
           return false;

    }
    return result;
}

bool WavFile::readAudio()
{
    quint8 *data  = new quint8[m_dataSize];
    if (read(reinterpret_cast<char*>(data), m_dataSize) != m_dataSize)
        return false;


    std::vector<double> left;
    std::vector<double> right;

    //8-bit samples
    if (m_bytespersample == 1)
    {

        int step = m_numChannels;
        //reading left channel
        for(quint32 index=0; index < m_dataSize; index += step)
        {
            left.push_back(static_cast<qint8>(data[index])/128.0);
        }

        //reading right channel
        if (m_numChannels > 1)
        {
            for (quint32 index = 1; index < m_dataSize; index += step)
            {
                right.push_back(static_cast<qint8>(data[index])/128.0);
            }

            //writing data as a sum of left and right channels
            for (quint32 index = 0; index < left.size(); ++index)
            {
                m_data.push_back((left[index]+right[index])*0.5);
            }
        }
        else
        {
            //if only left channel just store it as data
            m_data = left;
        }
    }

    //16-bit samples
    else if (m_bytespersample == 2)
    {

        int step = m_numChannels;
        //reading left channel

        for(quint32 index=0; index < m_dataSize; index += 2*step)
        {
            qint16 c = (data[index + 1] << 8) | data[index];
            left.push_back(c/32768.0);
        }

        //reading right channel
        if (m_numChannels > 1)
        {

            for (quint32 index = 2; index < m_dataSize; index += 2*step)
            {
                qint16 c = (data[index + 1] << 8) | data[index];
                right.push_back(c/32768.0);
            }

            //writing data as a sum of left and right channels
            for (quint32 index = 0; index < left.size(); ++index)
            {
                m_data.push_back((left[index]+right[index])*0.5);
            }
        }
        else
        {
            //if only left channel just store it as data
            m_data = left;
        }
    }
    //TODO: 24-bit and 32-bit samples
    //for now return false for 24 and 32 bit files
    else
        return false;

    delete[] data;
    return true;
}

TimeAudioData WavFile::getAudioData() const
{
    return TimeAudioData(m_data, m_sampleRate);
}

bool WavFile::writeHeader()
{
    seek(0);

    WaveHeader header;

    m_dataSize = static_cast<quint32>(m_data.size()*2);

    //RIFF chunk
    memcpy(&header.riff.descriptor.id, "RIFF", 4);
    //4 - is RIFF size, 8+16 is Wave chunk size, 8 + m_dataSize is data chunk size
    header.riff.descriptor.size = (4+(8+16)+(8+m_dataSize));
    memcpy(&header.riff.format, "WAVE", 4);

    //Wave chunk
    memcpy(&header.fmt.descriptor.id, "fmt ", 4);

    header.fmt.descriptor.size = 16;

    header.fmt.audioFormat = 1;
    header.fmt.numChannels = 1;
    header.fmt.sampleRate = m_sampleRate;
    //next parameters calculated as if there's only one channel
    //TODO: make it possible two write two channels and in BigEndian format
    header.fmt.byteRate = m_sampleRate*m_bytespersample;
    header.fmt.blockAlign = m_bytespersample;
    header.fmt.bitsPerSample = m_bytespersample*8;


    if (QFile::write(reinterpret_cast<char*>(&header), sizeof(header)) != sizeof(header))
        return false;

    if (QFile::write("data", 4) != 4)
        return false;
    if (QFile::write(reinterpret_cast<char*>(&m_dataSize), sizeof(m_dataSize)) != sizeof(m_dataSize))
        return false;


    return true;
}

bool WavFile::writeAudio()
{
    //currently only writes data in one channel
    for(quint32 index = 0; index < m_data.size(); ++index)
    {
        qint16 sample = static_cast<qint16>(m_data[index]*32768.0);
        if (QFile::write(reinterpret_cast<char*>(&sample), 2) != 2)
            return false;
    }
    return true;
}
