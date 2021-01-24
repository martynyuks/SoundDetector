#pragma once

#include <QString>
#include <QList>

struct WavHeader
{
    char chunkID[4]; //"RIFF" = 0x46464952
    unsigned long chunkSize; //28 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes] + sum(sizeof(chunk.id) + sizeof(chunk.size) + chunk.size)
    char format[4]; //"WAVE" = 0x45564157
    char subchunk1ID[4]; //"fmt " = 0x20746D66
    unsigned long subchunk1Size; //16 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes]
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned long sampleRate;
    unsigned long byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
};

struct WavChunk
{
    char ID[4]; //"data" = 0x61746164
    unsigned long size;  //Chunk data bytes
};

struct SoundRange
{
    int soundStart; //в мс
    int soundEnd; //в мс

    SoundRange(int soundStartMs, int soundEndMs)
    {
        soundStart = soundStartMs;
        soundEnd = soundEndMs;
    }
};

class WavSoundDetectProcessor
{
private:
    WavHeader _header;
    short * _samples = NULL;
    int _sampleCount;
    bool openWavFile(const QString& wavFileName);
    static short maxSoundVolume(short samples[], int startIndex, int endIndex);
    static bool writeSoundIntervalsIntoFile(const QString& fileName, QList<SoundRange>& soundRanges);
    static QList<SoundRange> detectSoundIntervals(short samples[], const int sampleCount, const int samplFreq, const int detectStep,
                            const int silenceVolume, const int silenceInterval);
public:
    WavSoundDetectProcessor();
    ~WavSoundDetectProcessor();
    bool processWavFile(const QString& wavFileName, const QString& outFileName,
                        const int samRangeTime, const int silenceVolume, const int silenceInterval);
};
