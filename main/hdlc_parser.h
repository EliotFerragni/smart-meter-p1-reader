#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    uint32_t inputW;
    uint32_t outputW;
    uint32_t totalInputWh;
    uint32_t totalOutputWh;
} DataParsed;

/**
 * @brief FIll result with the data parsed from buf of size len. 
 * 
 * @param buf Buffer to read
 * @param len length of the buffer
 * @param result Structure containing the parsed data
 */
void parseHdlc(uint8_t* buf, uint16_t len, DataParsed* result);
#ifdef __cplusplus
}
#endif