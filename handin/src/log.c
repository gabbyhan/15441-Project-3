#include "log.h"

FILE* log_create(char *file_name)
{
    FILE *f = fopen(file_name,"a+");
}

/* <time> <duration> <tput> <avg-tput> <bitrate> <server-ip> <chunkname>
 * Pass in file name, don't pass in time, pass in the other 6 arguments
 */
void log_add(FILE *f, int duration, int tput, int avg_tput, int bitrate, char *server_ip, char* chunkname)
{
    fprintf(f,"Video CDN Log\n");
    char line[8912] = {0};

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char date[256] = {0};
    strftime(date,sizeof(date),"%c",tm);
    
    sprintf(line,"[%s] %d %d %d %d %s \"%s\"\n", date, duration, tput, avg_tput, bitrate, server_ip, chunkname);
    fprintf(f,line);

}

void log_close(FILE *f)
{
    fclose(f);
}

