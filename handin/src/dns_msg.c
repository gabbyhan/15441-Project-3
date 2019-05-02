#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void create_response(char *response, char *message, char *query_str)
{
	int i;
	char *first;
	char *second;
	char *third;
	char *fourth;
	char flags[10];
	int QType = 1; // 16 bits (2 octets)
    int QClass = 1; // 16 bits (2 octets)
		

	create_flags(0,flags);
	
	for(i = 0; i < strlen(flags); i++)
	{
		if((i%2) == 0) printf("\n");
		printf("%x ",flags[i] & 0xff);
	}	
	for(i = 0; i < 10; i++)
	{
		message[2+i] = flags[i];
	}
    int q_len = strlen(query_str);
    int num_lens = 1;
    for(i= 0; i< q_len; i++)
    {
        if(query_str[i] == '.'){
            num_lens++;
        }
    }
    int lens[num_lens+1];
    i = 0;
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
    sprintf(message, "%s%c", message, (char)(lens[0]));
    int l = 1;
    for (i=0; i< q_len; i++)
    {
        char c = query_str[i];
        if(c == '.')
        {
            sprintf(message, "%s%c", message, (char)(lens[l]));
            l += 1;
        }
        else{
            sprintf(message, "%s%c", message, c);
        }    
    }

	char *rest = strchr(response, '.');
	strncpy(first,response,rest-response);
	char *rest1 = strchr(rest,'.');
	strncpy(second,rest,rest1-rest);
	char *rest2 = strchr(rest1,'.');
	strncpy(third,rest1,rest2-rest);
	fourth = rest2+1;
	char R1 = (char)(atoi(first) & 255);
	char R2 = (char) (atoi(second) & 255);
	char R3 = (char) (atoi(third) & 255);
	char R4 = (char) (atoi(fourth) & 255);
    sprintf(message, "%s%c", message, 0);	
	char QT1 = (char)((QType >> 8) & 255);
    char QT2 = (char)(QType & 255);
    char QC1 = (char)((QClass >> 8) & 255);
    char QC2 = (char)(QClass & 255);
	char TTL = (char) 0;
	char L1 = (char) 0;
    char L2 = (char) ((1 << 2) & 225);
    sprintf(message, "%s%c%c%c%c%c%c%c%c", message, QT1, QT2, QC1, QC2, TTL, TTL, TTL, TTL);
	sprintf(message, "%s%c%c", message, L1, L2);
    sprintf(message, "%s%c%c%c%c",message, R1, R2, R3, R4);
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
	
	int message_len = 12 + q_len+1;
    
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

	
    sprintf(message + message_len, "%s%c", message, 0);
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
    int id = btoi(data, 2, 0);
    //Header is 12 bytes total
    int ctr = 0;
    int i = 12;

	for(i = 0; i < 29; i++)
	{
		if((i%2) == 0) printf("\n");
		printf("%x ",data[i] & 0xff);
	}	
	printf("\n");	
    char l = data[12]; //Question starts at 13th byte
	printf("l: %c\n",l);
    while(l != 0)   //QNAME ends with a zero byte
    {
        qname[ctr] = data[i];
        ctr++;
		//printf("i: %d, l: %d\n", i,l);
        i += 1;
        l -= 1;
        if (l == 0){
            l = data[i];
			//printf("next length: %d\n", l);
            i += 1;
            qname[ctr] = '.';
            ctr++;
        }
    }
    qname[ctr-1] = '\0';
    //R_INPUT = ''.join(R_INPUT[:-1])
    //
    return id;
}

void parse_response(char *data, char *server_ip)
{
    //get number of answers : USE THIS ELSEWHERE
    /*char num_ansr[2];
    num_ansr[0]= data[6];
    num_ansr[1] = data[7];

    int num_answers = atoi(num_ansr);
    */

    //R_INPUT = [] --> this is the original hostname
    //Header is 12 bytes total
    int i = 13;
    char l = data[12]; //Question starts at 13th byte
    while(l != 0)   //QNAME ends with a zero byte
    {
        //R_INPUT += data[i]
        i += 1;
        l -= 1;
        if (l == 0){
            l = data[i];
            i += 1;
            //R_INPUT += '.'
        }
    }
    //R_INPUT = ''.join(R_INPUT[:-1])

    //R_QType = us(data[i:i+2]) // A record
    i += 2;
    //R_QClass = us(data[i:i+2]) // IN
    i += 2;
    
    /*** WE ONLY CARE ABOUT RDATA --> IT IS THE IP ADDRESS ***/
    
    //RR_ADDR = ""
    //RR_NAME = ""
    int name = btoi(data, 2, i);
    if (name == 49164){ // C0 0C
        //RR_NAME = us(data[i:i+2]) // should be C0 0C
        i += 2;
    }
    else{
        char l = data[i];
        i += 1;
        while (l != 0){
            //RR_NAME += data[i]
            i += 1;
            l -= 1;
            if (l == 0){
                l = data[i];
                i += 1;
                //RR_NAME += '.'
            }
        }
        //RR_NAME = ''.join(RR_NAME[:-1])
    }

    //RR_TYPE = us(data[i:i+2]) # A record
    i += 2;
    //RR_CLASS = us(data[i:i+2]) # IN
    i += 2;
    //RR_TTL = (us(data[i:i+2])<<8) + (us(data[i+2:i+4]))
    i += 4;
    //RR_DL = us(data[i:i+2])
    i += 2;

    //addr = [ub(data[i:i+1]),ub(data[i+1:i+2]),ub(data[i+2:i+3]),ub(data[i+3:i+4])]
    sprintf(server_ip, "%d.%d.%d.%d", (int)data[i], (int)data[i+1], (int)data[i+2], (int)data[i+3]);
    //addr = [str(a) for a in addr]
    //RR_ADDR = '.'.join(addr)
    // print RR_NAME, RR_TYPE, RR_CLASS, RR_TTL, RR_DL
    printf("server ip: %s\n", server_ip);
}
