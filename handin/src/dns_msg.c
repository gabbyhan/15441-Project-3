#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void create_flags(int query, char *flags)
{
	if(query == 0)
	{
		flags[0]= 8;
		flags[1] = 0;

		flags[2] = 0;
		flags[3] = 0;
		flags[4] = 0;
		flags[5] = 1;
		flags[6] = 0;
		flags[7] = 0;
		flags[8] = 0;
		flags[9] = 0;
	}
	else
	{
		flags[0] = 0;
		flags[1] = 0;

		flags[2] = 0;
		flags[3] = 1;
		flags[4] = 0;
		flags[5] = 0;
		flags[6] = 0;
		flags[7] = 0;
		flags[8] = 0;
		flags[9] = 0;	
	}
}

void create_response(char *response, char *id)
{
/*
    // build RR's
    RR_ADDR = ""
    if not query:
        RR_NAME = 49164 //C0 0C
        message += s(RR_NAME)

        RR_QTYPE = 1 //A record
        RR_QCLASS = 1 //IN
        RR_TTL = 0 // no caching
        RR_DATALENGTH = 4
        if ROUND_ROBIN:
            RR_ADDR = SERVERS[SERV_CURR]
            SERV_CURR = (SERV_CURR + 1) % len(SERVERS)
        else:
            RR_ADDR = getBestServer(addr[0], SERVERS, lsa_file)
            
        message += s(RR_QTYPE)+s(RR_QCLASS) + \
                struct.pack('>I',RR_TTL) + s(RR_DATALENGTH)
        RR_ADDR = [int(float(a)) for a in RR_ADDR.split('.')]
        for a in RR_ADDR:
            message += b(a)
        t = int(time.time())
        out_addr = '.'.join([str(r) for r in RR_ADDR])
*/
}

void create_query(char *query_str, char *message)
{
    //create the header
    char flags[10];
    create_flags(1, flags);

    //ID: 16 bit - generated randomly
    int id = rand() % 65535;
    char id1 = ((id >> 8) & 255);
    char id2 = id & 255;
    sprintf(message, "%c%c%s", id1, id2, flags);
 
    int QType = 1; // 16 bits (2 octets)
    int QClass = 1; // 16 bits (2 octets)
	
    // Calc lengths for Query string
    int q_len = strlen(query_str);
    int num_lens = 1;
    for(int i= 0; i< q_len; i++)
    {
        if(query_str[i] == '.'){
            num_lens++;
        }
    }
    int lens[num_lens+1];
    int i = 0;
    int j, k;
    int ctr = 0;
    int sum_lens = 0;
    while (i <= q_len)
    {
        k = 0;
        for(j=i; j< q_len; j++)
        {
            if(query_str[j] == '.'){
                lens[ctr] = k;
                sum_lens +=k;
                ctr++;
                break;
            }
            k += 1;
        }
        i = j+1;
    }
    int total_read = sum_lens + num_lens;
    lens[num_lens] = q_len - total_read;

    // Insert lengths and names into message
    sprintf(message + strlen(message), "%s%c", message, (char)(lens[0]));
    int l = 1;
    for (int i=0; i< q_len; i++)
    {
        char c = query_str[i];
        if(c == '.')
        {
            sprintf(message + strlen(message), "%s%c", message, (char)(lens[l]));
            l += 1;
        }
        else{
            sprintf(message + strlen(message), "%s%c", message, c);
        }    
    }
    sprintf(message + strlen(message), "%s%c", message, 0);
    
    char QT1 = (char)((QType >> 8) & 255);
    char QT2 = (char)(QType & 255);
    sprintf(message + strlen(message), "%s%c%c", message, QT1, QT2);
    char QC1 = (char)((QClass >> 8) & 255);
    char QC2 = (char)(QClass & 255);
    sprintf(message + strlen(message), "%s%c%c", message, QC1, QC2);

}
