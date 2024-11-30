#pragma once

/**
 * @brief Init the reading and parsing of the data coming from the smartmeter
 *
 * @param uart_rx_pin uart rx pin
 * @param uart_port_num  uart port number
 * @param uart_baudrate  uart baudrate
 * @param smartmeter_data_req_pin  smartmeter data request pin
 */
void smartmeter_reader_init(uint32_t uart_rx_pin, uint32_t uart_port_num, uint32_t uart_baudrate, uint32_t smartmeter_data_req_pin);