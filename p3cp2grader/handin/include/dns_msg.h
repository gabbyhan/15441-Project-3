#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void create_response(char *response, char *message, int query);

void create_query(char *query_str, char *message);

int parse_query(char *data, char *qname);

void parse_response(char *data, char *server_ip);
