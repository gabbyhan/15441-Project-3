#include "nameserver.h"

//converts n bytes of bytes starting at c into an int
int btoi(char *bytes, int n, int c)
{
    int result = 0;
    for(int i=0; i<n; i++)
    {
        result = (result<<8) + bytes[c];
    }
    return result;
}

void create_flags(int query, char *flags)
{
	if(query == 0)
	{
		flags[0]= 128;
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
	if(query == 2)
	{
		flags[0]= 128;
		flags[1] = 3;

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

void create_response(char *response, char *message, int query)
{
	int i;
	char *first = calloc(3,1);
	char *second = calloc(3,1);
	char *third = calloc(3,1);
	char *fourth;
	char flags[10];
	int QType = 1; // 16 bits (2 octets)
    int QClass = 1; // 16 bits (2 octets)
	create_flags(query,flags);
	for(i = 0; i < 9; i++)
	{
		sprintf(message+2+i,"%c",flags[i]);
	}
	
	char *rest = strchr(response, '.');
	memcpy(first,response,rest-response);
	char *rest1 = strchr(rest+1,'.');
	
	memcpy(second,rest+1,rest1-rest-1);
	
	char *rest2 = strrchr(rest1+1,'.');
	memcpy(third,rest1+1,rest2-rest1-1);
	fourth = rest2+1;	
	char R1 = (char)(atoi(first) & 255);
	char R2 = (char) (atoi(second) & 255);
	char R3 = (char) (atoi(third) & 255);
	char R4 = (char) (atoi(fourth) & 255);
	char TTL = (char) 0;
	char L1 = (char) 0;
    char L2 = (char) ((1 << 2) & 255);
	for(i = 0; i < 22; i++)
	{
    	sprintf(message+34+i, "%c", message[12+i]);
	}
    sprintf(message+34+i, "%c%c%c%c%c%c%c%c",TTL,TTL,L1,L2,R1,R2,R3,R4);
	
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
    sprintf(message, "%c%c%c%c%c%c%c%c%c%c%c%c", id1, id2, flags[0], flags[1], flags[2],flags[3], flags[4], flags[5], flags[6], flags[7],flags[8], flags[9]);
	
    int QType = 1; // 16 bits (2 octets)
    int QClass = 1; // 16 bits (2 octets)
	
    // Calc lengths for Query string
    int q_len = strlen(query_str);
    int num_lens = 0;
    for(int i= 0; i< q_len; i++)
    {
        if(query_str[i] == '.'){
            num_lens++;
        }
    }
    int lens[num_lens];
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
	
	int message_len = 13 + q_len;
    
	// Insert lengths and names into message

    sprintf(message + 12, "%c", (char)(lens[0]));
    int l = 1;
	
    for (k=0; k< q_len; k++)
    {
        char c = query_str[k];
        if(c == '.')
        {
            sprintf(message+13+k, "%c", (char)(lens[l]));
            l += 1;
        }
        else{
            sprintf(message+13+k, "%c", c);
        }    
    }

	
    sprintf(message + message_len, "%c", 0);
    message_len++;
 
    char QT1 = (char)((QType >> 8) & 255);
    char QT2 = (char)(QType & 255);
    sprintf(message + message_len, "%c%c", QT1, QT2);
	message_len+=2;
    
	char QC1 = (char)((QClass >> 8) & 255);
    char QC2 = (char)(QClass & 255);
    sprintf(message + message_len, "%c%c", QC1, QC2);
	message_len+=2;
}


int parse_query(char *data, char *qname)
{
    //Header is 12 bytes total
    int ctr = 0;
    int i = 13;
    int l = (int)data[12]; //Question starts at 13th byte
    while(l != 0)   //QNAME ends with a zero byte
    {
        qname[ctr] = data[i];
        ctr++;
        i += 1;
        l -= 1;
        if (l == 0){
            l = (int)data[i];
            i += 1;
            qname[ctr] = '.';
            ctr++;
        }
    }
    qname[ctr-1] = '\0';
    //R_INPUT = ''.join(R_INPUT[:-1])
    //
    return i;
}

void parse_response(char *data, char *server_ip)
{
    int i= 60;
    sprintf(server_ip, "%d.%d.%d.%d", (int)data[i], (int)data[i+1], (int)data[i+2], (int)data[i+3]);
    printf("server ip: %s\n", server_ip);
}
