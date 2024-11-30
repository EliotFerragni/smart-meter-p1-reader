#include <string>
#include "hdlc_data_extractor.h"

namespace
{
    // OBIS codes
    const uint8_t INPUT_W[] = {0x01, 0x00, 0x01, 0x07, 0x00, 0xFF};         // 1.0.1.7.0.255
    const uint8_t OUTPUT_W[] = {0x01, 0x00, 0x02, 0x07, 0x00, 0xFF};        // 1.0.2.7.0.255
    const uint8_t TOTAL_INPUT_WH[] = {0x01, 0x00, 0x01, 0x08, 0x00, 0xFF};  // 1.0.1.8.0.255
    const uint8_t TOTAL_OUTPUT_WH[] = {0x01, 0x00, 0x02, 0x08, 0x00, 0xFF}; // 1.0.2.8.0.255

    const std::string INPUT_W_STR(INPUT_W, INPUT_W + sizeof(INPUT_W));
    const std::string OUTPUT_W_STR(OUTPUT_W, OUTPUT_W + sizeof(OUTPUT_W));
    const std::string TOTAL_INPUT_WH_STR(TOTAL_INPUT_WH, TOTAL_INPUT_WH + sizeof(TOTAL_INPUT_WH));
    const std::string TOTAL_OUPTUT_WH_STR(TOTAL_OUTPUT_WH, TOTAL_OUTPUT_WH + sizeof(TOTAL_OUTPUT_WH));

    const uint8_t DATA_OFFSET = 8;

    /**
     * @brief Read the given buffer to extract the uint32_t value
     *
     * @param buf Buffer of at least uint32_t size
     * @return uint32_t value read from buffer
     */
    uint32_t uint32FromBuffer(const uint8_t *buf)
    {
        return (*(buf) << 24) + (*(buf + 1) << 16) + (*(buf + 2) << 8) + (*(buf + 3));
    }

    bool findField(const std::string &pattern, const std::string &buf, size_t &foundPos)
    {
        // - 1 to correspond to the index
        foundPos = buf.find(pattern) - 1;
        if (foundPos == std::string::npos)
        {
            // not found
            return false;
        }
        // found
        return true;
    }
}

/**
 * Works with HDLC/DLSM/COSEM frames.
 * Strictly speaking, it doesn't decode the HDLC frames, nor the DLSM. In fact we don't care about HDLC overhead or DLSM format.
 * The only things interesting are the values we want. So search for the OBIS field of the values and extract only the corresponding value.
 *
 * To add others fields, dump frames from your smartmeter, parse them with https://www.gurux.fi/GuruxDLMSTranslator under Messages,
 * then find the OBIS code corresponding to the wanted value. The value will be DATA_OFFSET after the the OBIS field.
 * If the frames are encrypted, you need to decrypt them before being able to use this function.
 */
void extractData(uint8_t *buf, uint16_t len, ExtractedData *result)
{
    size_t index;
    std::string buffer(buf, buf + len);
    if (findField(INPUT_W_STR, buffer, index))
    {
        result->inputW = uint32FromBuffer(&buf[index + DATA_OFFSET]);
    }
    if (findField(OUTPUT_W_STR, buffer, index))
    {
        result->outputW = uint32FromBuffer(&buf[index + DATA_OFFSET]);
    }
    if (findField(TOTAL_INPUT_WH_STR, buffer, index))
    {
        result->totalInputWh = uint32FromBuffer(&buf[index + DATA_OFFSET]);
    }
    if (findField(TOTAL_OUPTUT_WH_STR, buffer, index))
    {
        result->totalOutputWh = uint32FromBuffer(&buf[index + DATA_OFFSET]);
    }
}