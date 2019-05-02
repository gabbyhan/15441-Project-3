#include "dns_msg.c"

int main()
{
	int i;
 	char message[4096];
	char query_str[17];
	create_query("video.cs.cmu.edu",message);
	printf("QUERY: \n");
	for(i = 0; i < 29; i++)
	{
		if((i%2) == 0) printf("\n");
		printf("%x ",message[i] & 0xff);
	}	
	printf("\n");	
	parse_query(message,query_str);
	printf("QUERY STRING: %s\n",query_str);
	
	create_response("3.0.0.1",message,query_str);
	printf("RESPONSE: \n");
	for(i = 0; i < strlen(message); i++)
	{
		if((i%2) == 0) printf("\n");
		printf("%x ",message[i] & 0xff);
	}
	return 0;
}
	
