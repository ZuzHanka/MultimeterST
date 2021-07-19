/*
 * comlink.cpp
 *
 *  Created on: Jul 19, 2021
 *      Author: jan.humpl
 */

#include "comlink.hpp"

#include "bsp.hpp"


uint16_t Comlink::test_data = 0;

Comlink::protocol_t Comlink::buffer;

bool Comlink::request_1()
{
	bool status = true;

	// Prepare command to send.
	buffer.header.command = REQ_1;
	buffer.header.length = sizeof(buffer.header);

	// Send command.
	if (status) status = comlink_transmit((uint8_t*) &buffer, buffer.header.length);

	// Receive response header.
	const uint32_t REQ_1_TIMEOUT = 1000;
	if (status) status = comlink_receive((uint8_t*) &buffer.header, sizeof(header_t), REQ_1_TIMEOUT);

	// Check valid command code:
	if (buffer.header.command != RESP_1) status = false;

	// Receive full response.
	if (buffer.header.length > sizeof(buffer.header))
	{
		if (status) status = comlink_receive((uint8_t*) &buffer.payload, buffer.header.length - sizeof(buffer.header), REQ_1_TIMEOUT);
	}

	// Check response length.
	if (buffer.header.length != (sizeof(header_t) + sizeof(buffer.payload.resp_1))) status = false;

	// Store received data
	test_data = (status) ? (buffer.payload.resp_1.value) : (0);

	return status;
}

bool Comlink::response_1()
{
	bool status = true;

	// Check command format.
	if (buffer.header.length != sizeof(header_t)) status = false;

	// Prepare response to send.
	buffer.header.command = RESP_1;
	buffer.payload.resp_1.value = adc_get_sample_mV(CHANNEL_1);
	buffer.header.length = sizeof(buffer.header) + sizeof(buffer.payload.resp_1);

	// Send response.
	if (status) status = comlink_transmit((uint8_t*) &buffer, buffer.header.length);

	return status;
}

bool Comlink::response_loop()
{
	bool status = true;

	// Receive request header.
	if (status) status = comlink_receive((uint8_t*) &buffer.header, sizeof(header_t));

	// Check valid command code:
	if (buffer.header.command >= NUM_CMD) status = false;

	// Receive full response.
	if (buffer.header.length > sizeof(buffer.header))
	{
		const uint32_t REQ_TIMEOUT = 1000;
		if (status) status = comlink_receive((uint8_t*) &buffer.payload, buffer.header.length - sizeof(buffer.header), REQ_TIMEOUT);
	}

	// Give master time to prepare receiving.
	delay(20);

	if (status)
	{
		switch (buffer.header.command)
		{
			case REQ_1 : status = response_1(); break;
			default : status = false; break;
		}
	}

	return status;
}
