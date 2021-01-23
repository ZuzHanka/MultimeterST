/*
 * communication.cpp
 *
 *  Created on: 5 Dec 2020
 *      Author: ZuzHanka
 */

#include "communication.hpp"

#include "bsp.hpp"

#include <cstdio>
#include <cstring>

bool welcome() {
	char msg[] = "MultimeterST";
	char pre_esc[] = "\x1b[2J\x1b[H\x1b[1m\x1b[4m\x1b[33;1m";
	char post_esc[] = "\x1b[0m";
	return send_msg(msg, pre_esc, post_esc);
}

bool send_msg(const char *msg, const char *pre_esc, const char *post_esc) {
	char message[200];
	strcpy(message, pre_esc);
	strcpy(message + strlen(message), msg);
	strcpy(message + strlen(message), post_esc);
	return terminal_transmit(message, strlen(message));
}



