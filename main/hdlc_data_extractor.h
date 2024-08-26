#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
    typedef struct
    {
        uint32_t inputW;
        uint32_t outputW;
        uint32_t totalInputWh;
        uint32_t totalOutputWh;
    } ExtractedData;

    /**
     * @brief FIll result with the data parsed from buf of size len.
     *
     * @param buf Buffer to read
     * @param len length of the buffer
     * @param result Structure containing the parsed data
     */
    void extractData(uint8_t *buf, uint16_t len, ExtractedData *result);
#ifdef __cplusplus
}
#endif