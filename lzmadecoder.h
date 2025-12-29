#ifndef LZMADECODER_H
#define LZMADECODER_H

#include <QByteArray>
#include <QListWidget>
#include <vector>

struct Tick {
    int32_t timeMS;
    double ask;
    double bid;
    float askVol;
    float bidVol;
};

class LzmaDecoder {
public:
    LzmaDecoder();
    //void inspectBi5File();
    QVector<Tick> decodeBi5(const QByteArray &compressedData);
};

#endif // LZMADECODER_H
