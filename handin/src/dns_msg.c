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

void create_response(char *response, char *id, char *message)
{
	int i;
	char flags[10];
	int QType = 1; // 16 bits (2 octets)
    int QClass = 1; // 16 bits (2 octets)
	

	create_flags(0,flags);
	for(i = 0; i < 10; i++)
	{
		message[2+i] = flags[i];
	}
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
    char QC1 = (char)((QClass >> 8) & 255);
    char QC2 = (char)(QClass & 255);
	char TTL = (char) 0;
	char L1 = (char) 0;
    char L2 = (char) ((1 << 2) & 225);
	char R1 = (char) 
	char R2
	char R3
	char R4
 	char R5
	char R6
	char R7
	char R8
    sprintf(message + strlen(message), "%s%c%c%c%c%c%c%c%c", message, QT1, QT2, QC1, QC2, TTL, TTL, TTL, TTL);
	sprintf(message + strlen(message), "%s%c%c", message, L1, L2);
    sprintf
 
	
	
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


int parse_query(char *data, char *qname)
{
    int id = btoi(data, 2, 0);
    //Header is 12 bytes total
    int ctr = 0;
    int i = 13;
    char l = data[12]; //Question starts at 13th byte
    while(l != 0)   //QNAME ends with a zero byte
    {
        qname[ctr] = data[i];
        ctr++;
        i += 1;
        l -= 1;
        if (l == 0){
            l = data[i];
            i += 1;
            qname[ctr] = '.';
            ctr++;
        }
    }
    qname[ctr-1] = '\0';
    printf("qname:%s\n", qname);
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
    sprintf(server_ip, "%c.%c.%c.%c", data[i], data[i+1], data[i+2], data[i+3]);
    //addr = [str(a) for a in addr]
    //RR_ADDR = '.'.join(addr)
    // print RR_NAME, RR_TYPE, RR_CLASS, RR_TTL, RR_DL
    printf("server ip: %s\n", server_ip);
}
