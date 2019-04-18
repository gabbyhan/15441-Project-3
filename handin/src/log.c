#include "log.h"

FILE* log_create(char *file_name)
{
    FILE *f = fopen(file_name,"a+");
}

void log_add(FILE *f,char *request_line,char *status,char *ip,pid_t p, int size)
{
    fprintf(f,"Liso\n");
    char line[8912] = {0};
    int pid = (int) p;
    fprintf(f,"%d",p);
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char date[256] = {0};
    strftime(date,sizeof(date),"%c",tm);
    sprintf(line,"%s - - [%s] \"%s\" %s %d\n",ip,date,request_line,status,size);
    fprintf(f,line);

}

void log_close(FILE *f)
{
    fclose(f);
}

