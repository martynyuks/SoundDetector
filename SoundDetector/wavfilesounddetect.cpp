#include "wavfilesounddetect.h"
#include <cstdio>
#include <cstring>
#include <QFileInfo>

WavSoundDetectProcessor::WavSoundDetectProcessor()
{

}

WavSoundDetectProcessor::~WavSoundDetectProcessor()
{
    if (_samples != NULL) delete[] _samples;
}

bool WavSoundDetectProcessor::openWavFile(const QString& wavFileName)
{
    QFileInfo fileInfo(wavFileName);
    if (!fileInfo.exists()) return false;
    if (!wavFileName.endsWith(".wav")) return false;
    FILE *fin;
    fopen_s(&fin, wavFileName.toStdString().c_str(), "rb");
    if (fin == NULL) return false;

    WavHeader header;
    fread(&header, sizeof(header), 1, fin);
    if (memcmp(header.chunkID, "RIFF", 4) != 0 ||
        memcmp(header.format, "WAVE", 4) != 0)
    {
        fclose(fin);
        return false;
    }
    _header = header;

    WavChunk chunk;
    while (!feof(fin))
    {
        fread(&chunk, sizeof(chunk), 1, fin);
        if (*(unsigned int *)&chunk.ID == 0x61746164) break;
        fseek(fin, chunk.size, SEEK_CUR);
    }

    int sampleSize = header.bitsPerSample / 8;
    _sampleCount = chunk.size * 8 / header.bitsPerSample;

    _samples = new short[_sampleCount];
    memset(_samples, 0, sizeof(short) * _sampleCount);

    for (int i = 0; i < _sampleCount; i++)
    {
        fread(&_samples[i], sampleSize, 1, fin);
    }

    fclose(fin);

    return true;
}

short WavSoundDetectProcessor::maxSoundVolume(short samples[], int startIndex, int endIndex)
{
    int maxVal = samples[startIndex];
    for (int i = startIndex + 1; i < endIndex; i++)
    {
        if (samples[i] > maxVal)
        {
            maxVal = samples[i];
        }
    }
    return maxVal;
}

QList<SoundRange> WavSoundDetectProcessor::detectSoundIntervals(short samples[], const int sampleCount, const int samFreq,
                                            const int samRangeTime, const int silenceVolume, const int silenceInterval)
{
    QList<SoundRange> soundRanges;
    bool soundDetected = false;
    int startSoundTime = 0;
    int endSoundTime = 0;
    int startSampleIndex = 0;
    int endSampleIndex = 0;
    int sampleRangeCount = sampleCount / (samFreq * samRangeTime / 1000) + 1;
    int curEndSoundTime = 0;
    int soundVolume;
    for (int i = 0, startSampleTime = 0; i < sampleRangeCount; i++, startSampleTime += samRangeTime)
    {
        startSampleIndex = samFreq * startSampleTime / 1000;
        endSampleIndex = samFreq * (startSampleTime + samRangeTime) / 1000;
        if (endSampleIndex >= sampleCount) endSampleIndex = sampleCount - 1;
        soundVolume = maxSoundVolume(samples, startSampleIndex, endSampleIndex);
        if (!soundDetected && soundVolume >= silenceVolume)
        {
            soundDetected = true;
            startSoundTime = startSampleTime;
            curEndSoundTime = 0;
        }
        else if (soundDetected)
        {
            if (i == sampleRangeCount - 1)
            {
                soundDetected = false;
                if (curEndSoundTime == 0)
                {
                    endSoundTime = endSampleIndex * 1000 / samFreq;
                }
                else
                {
                    endSoundTime = curEndSoundTime;
                }
                soundRanges.push_back(SoundRange(startSoundTime, endSoundTime));
            }
            else if (soundVolume < silenceVolume)
            {
                if (curEndSoundTime == 0)
                {
                    curEndSoundTime = startSampleTime;
                }
                if (startSampleTime - curEndSoundTime >= silenceInterval)
                {
                    soundDetected = false;
                    endSoundTime = curEndSoundTime;
                    soundRanges.push_back(SoundRange(startSoundTime, endSoundTime));
                }
            }
            else if (soundVolume >= silenceVolume)
            {
                curEndSoundTime = 0;
            }
        }
    }
    return soundRanges;
}

bool WavSoundDetectProcessor::writeSoundIntervalsIntoFile(const QString& fileName, QList<SoundRange>& soundRanges)
{
    FILE *fout;
    fopen_s(&fout, fileName.toStdString().c_str(), "w");
    if (fout == NULL) return false;
    fprintf(fout, "Sound intervals\n");
    fprintf(fout, "start, sec\tend, sec\n");
    for (int i = 0; i < soundRanges.size(); i++)
    {
        fprintf(fout, "%.2f\t%.2f\n", soundRanges[i].soundStart * 1E-3, soundRanges[i].soundEnd * 1E-3);
    }
    fclose(fout);
    return true;
}

bool WavSoundDetectProcessor::processWavFile(const QString& wavFileName, const QString& outFileName,
                                    const int samRangeTime, const int silenceVolume, const int silenceInterval)
{
    bool success = openWavFile(wavFileName);
    if (!success) return success;
    QList<SoundRange> soundRanges = detectSoundIntervals(_samples, _sampleCount, _header.sampleRate, samRangeTime, silenceVolume, silenceInterval);
    success = writeSoundIntervalsIntoFile(outFileName, soundRanges);
    return success;
}
