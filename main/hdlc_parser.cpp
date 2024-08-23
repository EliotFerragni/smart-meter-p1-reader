#include <cstring>
#include <algorithm>
#include <iterator>

#include "hdlc_parser.h"

#ifdef __cplusplus
extern "C"
{
#include <stdio.h>
}
#endif

namespace
{
    const uint8_t inputW[6] = {0x01, 0x00, 0x01, 0x07, 0x00, 0xFF};
    const uint8_t outputW[6] = {0x01, 0x00, 0x02, 0x07, 0x00, 0xFF};
    const uint8_t totalInputKWh[6] = {0x01, 0x00, 0x01, 0x08, 0x00, 0xFF};
    const uint8_t totalOutputKWh[6] = {0x01, 0x00, 0x02, 0x08, 0x00, 0xFF};

    const std::string inputWStr(inputW, inputW + 6);
    const std::string outputWStr(outputW, outputW + 6);
    const std::string totalInputKWhStr(totalInputKWh, totalInputKWh + 6);
    const std::string totalOutputKWhStr(totalOutputKWh, totalOutputKWh + 6);

    const uint8_t dataOffset = 8;
}

uint32_t val32FromBuffer(uint8_t* buf)
{
    return (*(buf) << 24) + (*(buf+1) << 16) + (*(buf+2) << 8) + (*(buf+3));
}

bool findField(const std::string pattern, const std::string buf, size_t &foundPos)
{
    foundPos = buf.find(pattern);
    if (foundPos == std::string::npos)
    {
        // not found
        return false;
    }
    // found
    return true;
}

void parseHdlc(uint8_t* buf, uint16_t len, DataParsed* result)
{
    size_t pos;
    std::string buffer(buf, buf + len);
    if (findField(inputWStr, buffer, pos))
    {
        result->inputW = val32FromBuffer(&buf[pos-1 + dataOffset]);
    }
    if (findField(outputWStr, buffer, pos))
    {
        result->outputW = val32FromBuffer(&buf[pos-1 + dataOffset]);
    }
    if (findField(totalInputKWhStr, buffer, pos))
    {
        result->totalInputKWh = val32FromBuffer(&buf[pos-1 + dataOffset]);
    }
    if (findField(totalOutputKWhStr, buffer, pos))
    {
        result->totalOutputKWh = val32FromBuffer(&buf[pos-1 + dataOffset]);
    }
}