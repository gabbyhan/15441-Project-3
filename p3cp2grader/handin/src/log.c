#include "log.h"

FILE* log_create(char *file_name)
{
    FILE *f = fopen(file_name,"a+");
    return f;
}

/* <time> <duration> <tput> <avg-tput> <bitrate> <server-ip> <chunkname>
 * Pass in file name, don't pass in time, pass in the other 6 arguments
 */
void log_add(FILE *f, double duration, double tput, double avg_tput, int bitrate, char *server_ip, char* chunkname)
{
    char line[8912] = {0};

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char date[256] = {0};
    //strftime(date,sizeof(date),"%c",tm);
    
    sprintf(line,"%ld %f %f %f %d %s \"%s\"\n", t, duration, tput, avg_tput, bitrate, server_ip, chunkname);
    fprintf(f,line);

}

void log_close(FILE *f)
{
    fclose(f);
}

