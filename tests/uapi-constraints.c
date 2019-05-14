#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/can/j1939.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "../libj1939.h"
#include "tap.h"

static struct sockaddr_can canself, canpeer;
static const char *canself_str, *canpeer_str;

fixture(populate_canname) {
	canself_str = getenv("CAN_SELF");
	if (!canself_str)
		canself_str = "can0:0x90";
	libj1939_parse_canaddr(strdup(canself_str), &canself);

	canpeer_str = getenv("CAN_PEER");
	if (!canpeer_str)
		canpeer_str = "can0:0xa0";
	libj1939_parse_canaddr(strdup(canpeer_str), &canpeer);
}

plan(3);

subtest(sock_stream_j1939) {
	subtest_plan(1);
	int sock = socket(PF_CAN, SOCK_STREAM, CAN_J1939);
	ok(sock < 0, "SOCK_STREAM CAN_J1939 should be an error");
}

subtest(socket_use_no_connect) {
	int sock;
	int ret;

	sock = socket(PF_CAN, SOCK_DGRAM, CAN_J1939);
	xok(sock >= 0, "creating J1939 socket: %m");

	ret = bind(sock, (struct sockaddr *)&canself, sizeof(canself));
	xok(ret >= 0, "binding to interface %s: %m", canself_str);

	ret = send(sock, "", 1, 0);
	xok(ret < 0, "sending without bind should be an error: %m");

	ret = write(sock, "", 1);
	xok(ret < 0, "writing without bind should be an error: %m");
}

subtest(socket_use_no_bind) {
	int sock;
	int ret;

	sock = socket(PF_CAN, SOCK_DGRAM, CAN_J1939);
	xok(sock >= 0, "creating J1939 socket: %m");

	ret = connect(sock, (struct sockaddr *)&canpeer, sizeof(canpeer));
	xok(ret < 0, "connecting without bind should be an error");
}

done_testing()
