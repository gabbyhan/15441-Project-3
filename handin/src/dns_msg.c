#include "dns_msg.h"

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

void create_message(int query, char *id, char *response)
{
	
	if(query == 0)
	{
	
	}
	if(query == 1)
	{
	}  
