#ifndef FFMPEGPARSER_H
#define FFMPEGPARSER_H

#include <QString>
#include <QList>

struct VideoStreamInfo {
    QString codec;
    int width = 0;
    int height = 0;
};

struct AudioStreamInfo {
    QString codec;
    int channels = 0;
    int sampleRate = 0;
};

struct MediaInfo {
    double duration = 0;
    qint64 size = 0;
    QList<VideoStreamInfo> videoStreams;
    QList<AudioStreamInfo> audioStreams;
};

class FFmpegParser
{
public:
    static MediaInfo parseFFprobeOutput(const QString &output);
};

#endif