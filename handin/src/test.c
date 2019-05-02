#include "dns_msg.c"

int main()
{
	int i;
 	char message[4096];
	char query_str[17];
	create_query("video.cs.cmu.edu",message);
	printf("QUERY: \n");
	for(i = 0; i < 34; i++)
	{
		if((i%2) == 0) printf("\n");
		printf("%x ",message[i] & 0xff);
	}	
	printf("\n");	
	int temp = parse_query(message,query_str);
	printf("QUERY STRING: %s\n",query_str);
	
	create_response("3.0.0.1",message,temp);
	printf("RESPONSE: \n");
	for(i = 60; i < 64; i++)
	{
		if((i%2) == 0) printf("\n");
		printf("%d ",message[i] & 0xff);
	}
	
	char server_ip[7];
	parse_response(message, server_ip);
	
	return 0;
}
	
