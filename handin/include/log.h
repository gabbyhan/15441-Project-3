#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <time.h>

FILE* log_create(char *file_name);

void log_add(FILE *f,char *request_line, char *status,char *ip,pid_t p, int size);

void log_close(FILE *f);
