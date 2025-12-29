#include "lzmadecoder.h"
#include "LzmaDec.h"

#include <QFile>
#include <QDebug>
#include <QDataStream>
#include <QtEndian>
#include <QBitArray>

LzmaDecoder::LzmaDecoder() {}

QVector<Tick> LzmaDecoder::decodeBi5(const QByteArray &data) {
    QVector<Tick> ticks;
    if (data.size() < 13) return ticks;

    uint64_t uncompressedSize = 0;
    memcpy(&uncompressedSize, data.constData() + 5, 8);

    const unsigned char* src = (const unsigned char*)data.constData() + 13;
    SizeT srcLen = data.size() - 13;
    SizeT destLen = (SizeT)uncompressedSize;

    QByteArray out;
    out.resize(destLen);

    ISzAlloc alloc = { [](ISzAllocPtr, size_t size) { return malloc(size); },
                      [](ISzAllocPtr, void* address) { free(address); } };

    ELzmaStatus status;
    int res = LzmaDecode((Byte*)out.data(), &destLen, src, &srcLen,
                         (Byte*)data.constData(), 5, LZMA_FINISH_ANY, &status, &alloc);

    if (res == SZ_OK) {
        int count = out.size() / 20;
        ticks.reserve(count);

        for (int i = 0; i < count; ++i) {
            const unsigned char* r = (const unsigned char*)out.data() + (i * 20);
            Tick t;
            t.timeMS = qFromBigEndian<int32_t>(r + 0);
            t.ask    = qFromBigEndian<int32_t>(r + 4) / 100000.0;
            t.bid    = qFromBigEndian<int32_t>(r + 8) / 100000.0;

            uint32_t av, bv;
            memcpy(&av, r + 12, 4);
            memcpy(&bv, r + 16, 4);
            // Для float используем чтение через сырой uint32
            uint32_t avBE = qFromBigEndian<uint32_t>(av);
            uint32_t bvBE = qFromBigEndian<uint32_t>(bv);
            memcpy(&t.askVol, &avBE, 4);
            memcpy(&t.bidVol, &bvBE, 4);

            ticks.append(t);
        }
    }
    return ticks;
}

/*void LzmaDecoder::inspectBi5File() {
    QString path = "C:/Users/Vital/EURUSD_20241015_10.bi5";
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "!!! ОШИБКА: Не удалось открыть файл по пути:" << path;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    qDebug() << "АНАЛИЗ ФАЙЛА:" << path;
    qDebug() << "Общий размер файла (сжатого):" << data.size() << "байт";

    if (data.size() < 13) {
        qDebug() << "Файл слишком мал для LZMA";
        return;
    }

    // Читаем первые 13 байт (стандартный заголовок LZMA)
    QByteArray header = data.left(13);
    qDebug() << "Сырой заголовок (HEX):" << header.toHex(' ');

    // 1. Свойства LZMA (1 байт)
    unsigned char prop = (unsigned char)header[0];
    int lc = prop % 9;
    int lp = (prop / 9) % 5;
    int pb = (prop / 9) / 5;

    qDebug() << "--- Свойства (Byte 0) ---";
    qDebug() << "  lc (literal context bits):" << lc;
    qDebug() << "  lp (literal pos bits):" << lp;
    qDebug() << "  pb (pos bits):" << pb;

    // 2. Размер словаря (4 байта, Little Endian)
    // Байты [1, 2, 3, 4]
    uint32_t dictSize = 0;
    memcpy(&dictSize, data.constData() + 1, 4);
    qDebug() << "--- Словарь (Bytes 1-4) ---";
    qDebug() << "  Dictionary Size:" << dictSize << "байт (" << (dictSize / 1024.0) << "KB )";

    // 3. Размер распакованных данных (8 байт, Little Endian)
    // Байты [5, 6, 7, 8, 9, 10, 11, 12]
    uint64_t uncompressedSize = 0;
    memcpy(&uncompressedSize, data.constData() + 5, 8);

    qDebug() << "--- Размер данных (Bytes 5-12) ---";
    if (uncompressedSize == 0xFFFFFFFFFFFFFFFF) {
        qDebug() << "  Uncompressed Size: НЕИЗВЕСТЕН (0xFF...)";
    } else {
        qDebug() << "  Uncompressed Size:" << uncompressedSize << "байт";
    }

    qDebug() << "--- Проверка первых байт после заголовка  ---";
    // 4. Проверка первых байт после заголовка (поток данных)
    QByteArray startOfData = data.mid(13, 10);
    qDebug() << "Первые 10 байт данных (после 13 байт):" << startOfData.toHex(' ');

    QByteArray startOfDataAlt = data.mid(5, 10);
    qDebug() << "Первые 10 байт данных (после 5 байт):" << startOfDataAlt.toHex(' ');

    int tickSize = 20; // Каждый тик в .bi5 весит 20 байт
    if (uncompressedSize > 0 && uncompressedSize != 0xFFFFFFFFFFFFFFFF) {
        int totalTicks = uncompressedSize / tickSize;
        qDebug() << "Прогноз количества тиков в файле:" << totalTicks;
    }

    qDebug() << "--- Чтение первых тиков ---";

    // Инициализация переменных для SDK
    const unsigned char* src = (const unsigned char*)data.constData() + 13;
    SizeT srcLen = data.size() - 13;
    SizeT destLen = (SizeT)uncompressedSize;

    QByteArray out;
    out.resize(destLen);

    // Настройка аллокатора памяти
    ISzAlloc alloc = {
        [](ISzAllocPtr, size_t size) { return malloc(size); },
        [](ISzAllocPtr, void* address) { free(address); }
    };

    ELzmaStatus status;
    // Распаковка
    int res = LzmaDecode(
        (Byte*)out.data(), &destLen,
        src, &srcLen,
        (Byte*)data.constData(), 5,
        LZMA_FINISH_ANY, &status, &alloc
        );

    if (res == SZ_OK) {
        //qDebug() << "--- УСПЕШНАЯ РАСПАКОВКА SDK ---";

        int tickSize = 20;
        int ticksToPrint = 30; // Сколько тиков выводить
        int totalTicksInFile = out.size() / tickSize;

        // Ограничиваем цикл, чтобы не выйти за пределы файла
        int limit = qMin(ticksToPrint, totalTicksInFile);

        for (int i = 0; i < limit; ++i) {
            // Смещаемся на 20 байт для каждого нового тика
            const unsigned char* r = (const unsigned char*)out.data() + (i * tickSize);

            // Читаем значения (Big Endian)
            int32_t timeMS = qFromBigEndian<int32_t>(r + 0);
            int32_t ask    = qFromBigEndian<int32_t>(r + 4);
            int32_t bid    = qFromBigEndian<int32_t>(r + 8);

            // Читаем объемы (Float Big Endian)
            uint32_t rawAskVol = qFromBigEndian<uint32_t>(r + 12);
            uint32_t rawBidVol = qFromBigEndian<uint32_t>(r + 16);
            float askVol, bidVol;
            memcpy(&askVol, &rawAskVol, 4);
            memcpy(&bidVol, &rawBidVol, 4);

            qDebug() << QString("ТИК №%1:").arg(i + 1);
            qDebug() << "  Время (мс):" << timeMS;
            qDebug() << "  Ask Price: " << (ask / 100000.0);
            qDebug() << "  Bid Price: " << (bid / 100000.0);
            qDebug() << "  Ask Volume: " << askVol;
            qDebug() << "  Bid Volume: " << bidVol;
            //qDebug() << "  HEX:       " << QByteArray((const char*)r, 20).toHex(' ');
            qDebug() << "-----------------------";
        }

        if (totalTicksInFile > limit) {
            qDebug() << "До конца еще " << (totalTicksInFile - limit) << " тиков в файле.";
        }

    } else {
        qDebug() << "ОШИБКА LZMA SDK. Код ошибки:" << res;
    }
}*/
