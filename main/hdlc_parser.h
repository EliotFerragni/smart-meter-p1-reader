#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    uint32_t inputW;
    uint32_t outputW;
    uint32_t totalInputKWh;
    uint32_t totalOutputKWh;
} DataParsed;

void parseHdlc(uint8_t* buf, uint16_t len, DataParsed* result);
#ifdef __cplusplus
}
#endif