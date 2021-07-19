/*
 * comlink.hpp
 *
 *  Created on: Jul 19, 2021
 *      Author: jan.humpl
 */

#ifndef COMLINK_HPP_
#define COMLINK_HPP_

#include <cstdint>

class Comlink
{
public:

	enum commands_t : uint8_t
	{
		ERR,
		REQ_1,
		RESP_1,
		NUM_CMD
	};

	struct pr_response_1_t
	{
		uint16_t value;
	};

	struct header_t
	{
		uint8_t length;
		commands_t command;
	};

	struct protocol_t
	{
		header_t header;
		union
		{
			pr_response_1_t resp_1;
		} payload;
	};

	static bool request_1();
	static bool response_1();
	static bool response_loop();

	static uint16_t test_data; // remove, just for debug

private:

	static protocol_t buffer;
};



#endif /* COMLINK_HPP_ */
