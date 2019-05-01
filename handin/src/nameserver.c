#include "nameserver.h"

int start_server(int argc, char* argv[])
{
	int sockfd, len, n;
	uint16_t id, control;
	uint16_t req_id;
	uint64_t counts;
	char q_str[17];
	char server_ip[16];
	memset(server_ip,0,16);
	struct sockaddr_in servaddr, cliaddr;
	char buffer[4096];
	char response[4096];
	int opt_value = 1;
	char *type = argv[1];
    int load_balancer = 0;
    char *log_file = argv[2];
    char *ip = argv[3];
    int port = atoi(argv[4]);
    char *servers_file = argv[5];
    char *lsas_file = argv[6];
    if(strcmp("-r",type) == 0) load_balancer = 1;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		fprintf(stderr,"Could not create listening socket.\n");
		return -1;
	}
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt_value, sizeof(int)) < 0)
	{
		fprintf(stderr,"Failed to set option to reuse address.\n");
		return -1;
	}
	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	if(inet_aton(ip, &(servaddr.sin_addr)) == 0)
 	{
		fprintf(stderr, "IP conversion failure.\n");
		return -1;
	}
	servaddr.sin_port = htons(port);
	if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) , 0)
	{
		fprintf(stderr,"Failed to bind listening socket.\n");
		return -1;
	}
    /* get the servers for round robin !*/
    char *servers_list[100];
    for(int i =0; i<100; i++){
        servers_list[i] = malloc(16); //length of ip address/ router name??
    }
    int num_servers = get_servers(servers_list, servers_file);
    printf("num servers: %d\n", num_servers);
    for(int i=0; i<num_servers; i++){
        printf("%s\n", servers_list[i]);
    }
    int rr_ctr = 0;
    sprintf(server_ip, "%s", servers_list[rr_ctr]);
    
    printf("server ip:%s\n", server_ip);
    /*TODO: find closest server using dijkstra's */
    

	while(1)
	{
		n = recvfrom(sockfd, (char *) buffer, 4096, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);
        rr_ctr = (rr_ctr+1)%num_servers;

        parse_query(buffer,q_str);
		printf("%s\n",q_str);
		if(strstr(q_str,"video.cs.cmu.edu") == NULL)
		{
            create_response(NULL,buffer,2);
        }
        else{
			create_response(server_ip,buffer,0);
        }
		sendto(sockfd,(char *) buffer,4096,MSG_DONTWAIT, (struct sockaddr *) &cliaddr, len);
    }
	return 0;
}

int main(int argc, char *argv[])
{
	return start_server(argc, argv);
}
