#include "data.h"
#include "testing.h"

#ifndef INFLUX_H
#define INFLUX_H

unsigned int construct_influx_write_body(char* body, char *node_id);
void construct_influx_route(char* route, char* base, char* user, char* pass, char* database);
test_t influx_test();

#endif

