#include "CrcCheck.h"
#include <QByteArray>
#include <QDebug>

const ParamModel CrcCheck ::modelList[] = {
    //                     宽度  多项式      初始值     结果异或值    输入反转 输出反转
    { "CRC-4/ITU",          4,  0x03,       0x00,       0x00,       true,  true  },
    { "CRC-5/EPC",          5,  0x09,       0x09,       0x00,       false, false },
    { "CRC-5/ITU",          5,  0x15,       0x00,       0x00,       true,  true  },
    { "CRC-5/USB",          5,  0x05,       0x1F,       0x1F,       true,  true  },
    { "CRC-6/ITU",          6,  0x03,       0x00,       0x00,       true,  true  },
    { "CRC-7/MMC",          7,  0x09,       0x00,       0x00,       false, false },
    { "CRC-8",              8,  0x07,       0x00,       0x00,       false, false },
    { "CRC-8/ITU",          8,  0x07,       0x00,       0x55,       false, false },
    { "CRC-8/ROHC",         8,  0x07,       0xFF,       0x00,       true,  true  },
    { "CRC-8/MAXIM",        8,  0x31,       0x00,       0x00,       true,  true  },
    { "CRC-16/IBM",         16, 0x8005,     0x0000,     0x0000,     true,  true  },
    { "CRC-16/MAXIM",       16, 0x8005,     0x0000,     0xFFFF,     true,  true  },
    { "CRC-16/USB",         16, 0x8005,     0xFFFF,     0xFFFF,     true,  true  },
    { "CRC-16/MODBUS",      16, 0x8005,     0xFFFF,     0x0000,     true,  true  },
    { "CRC-16/CCITT",       16, 0x1021,     0x0000,     0x0000,     true,  true  },
    { "CRC-16/CCITT-FALSE", 16, 0x1021,     0xFFFF,     0x0000,     false, false },
    { "CRC-16/X25",         16, 0x1021,     0xFFFF,     0xFFFF,     true,  true  },
    { "CRC-16/XMODEM",      16, 0x1021,     0x0000,     0x0000,     false, false },
    { "CRC-16/DNP",         16, 0x3D65,     0x0000,     0xFFFF,     true,  true  },
    { "CRC-32",             32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true,  true  },
    { "CRC-32/MPEG-2",      32, 0x04C11DB7, 0xFFFFFFFF, 0x00000000, false, false }
};

const int CrcCheck ::modelListSize = sizeof(CrcCheck ::modelList)/sizeof(CrcCheck ::modelList[0]);

CrcCheck ::CrcCheck ()
{

}


/// <summary>
/// 二进制颠倒
/// </summary>
/// <param name="n">要颠倒的值(8位)</param>
/// <returns>颠倒后的值</returns>
quint8 reverseBits(quint8 n) {
    quint8 rev = 0;
    for (int i = 0; i < 8 && n > 0; ++i) {
        rev |= (n & 1) << (7 - i);
        n >>= 1;
    }
    return rev;
}

/// <summary>
/// 二进制颠倒
/// </summary>
/// <param name="n">要颠倒的值(16位)</param>
/// <returns>颠倒后的值</returns>
quint16 reverseBits(quint16 n) {
    quint16 rev = 0;
    for (int i = 0; i < 16 && n > 0; ++i) {
        rev |= (n & 1) << (15 - i);
        n >>= 1;
    }
    return rev;
}

/// <summary>
/// 二进制颠倒
/// </summary>
/// <param name="n">要颠倒的值(32位)</param>
/// <returns>颠倒后的值</returns>
quint32 reverseBits(quint32 n) {
    quint32 rev = 0;
    for (int i = 0; i < 32 && n > 0; ++i) {
        rev |= (n & 1) << (31 - i);
        n >>= 1;
    }
    return rev;
}

/// <summary>
/// 计算CRC校验值
/// </summary>
/// <param name="buf">要计算的数组</param>
/// <param name="modelIndex">参数模型索引</param>
/// <returns>crc校验和</returns>
quint16 CrcCheck::computeCrcVal(QByteArray buf, int modelIndex)
{
    int bufCount = buf.count();    // 数组的长度
    quint8 modelWidth = modelList[modelIndex].width;    // 参数模型宽度
    quint32 crc32;
    if (modelWidth == 32)
    {
        quint32 poly = reverseBits(modelList[modelIndex].poly);
        quint32 xorout = modelList[modelIndex].xorout;
        quint32 crc = modelList[modelIndex].init;
        for (int x = 0; x < bufCount; x++) {
            crc = crc ^ buf[x];
            for (int y = 0; y < 8; y++) {
                if ((crc & 0x0001) == 1) {
                    crc = crc >> 1;
                    crc = crc ^ poly;
                }
                else
                    crc = crc >> 1;
            }
        }
        crc = crc ^ xorout;
        crc32 = crc;
    }
    else if (modelWidth == 16)
    {
        quint16 poly = (quint16)modelList[modelIndex].poly;
        quint16 xorout = modelList[modelIndex].xorout;
        quint16 crc = modelList[modelIndex].init;
        if (modelList[modelIndex].refin)    // 输入输出反转
        {
            poly = reverseBits(poly);
            for (int x = 0; x < bufCount; x++) {
                crc = crc ^ buf[x];
                for (int y = 0; y < 8; y++) {
                    if (crc & 0x0001) {
                        crc = crc >> 1;
                        crc = crc ^ poly;
                    }
                    else
                        crc = crc >> 1;
                }
            }
        }
        else                                // 输入输出不反转
        {
            for (int x = 0; x < bufCount; x++) {
                crc = crc ^ (buf[x] << 8);
                for (int y = 0; y < 8; y++) {
                    if (crc & 0x8000) {
                        crc = crc << 1;
                        crc = crc ^ poly;
                    }
                    else
                        crc = crc << 1;
                }
            }
        }
        crc = crc ^ xorout;
        crc32 = crc;
    }
    else
    {
        quint8 poly = reverseBits((quint8)modelList[modelIndex].poly);
        quint8 xorout = modelList[modelIndex].xorout;
        quint8 crc = modelList[modelIndex].init;
        for (int x = 0; x < bufCount; x++) {
            crc = crc ^ buf[x];
            for (int y = 0; y < 8; y++) {
                if ((crc & 0x0001) == 1) {
                    crc = crc >> 1;
                    crc = crc ^ poly;
                }
                else
                    crc = crc >> 1;
            }
        }
        crc = crc ^ xorout;
        crc32 = crc;
    }

    return crc32;
}

