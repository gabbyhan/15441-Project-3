#include "mydns.h"

/*
 * Initialize your client DNS library with the IP address and port number of
 * your DNS server.
 *
 * @param  dns_ip  The IP address of the DNS server.
 * @param  dns_port  The port number of the DNS server.
 *
 * @return 0 on success, -1 otherwise
 */
int init_mydns(char *dns_ip, unsigned int dns_port)
{
    int sockfd;
    struct sockaddr_in servaddr;
    int opt_value = 1;

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
	if(inet_aton(dns_ip, &(servaddr.sin_addr)) == 0)
 	{
		fprintf(stderr, "IP conversion failure.\n");
		return -1;
	}
	servaddr.sin_port = htons(dns_port);
	if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) , 0)
	{
		fprintf(stderr,"Failed to bind listening socket.\n");
		return -1;
	}
    return sockfd;
}

/**
 * Resolve a DNS name using your custom DNS server.
 *
 * Whenever your proxy needs to open a connection to a web server, it calls
 * resolve() as follows:
 *
 * struct addrinfo *result;
 * int rc = resolve("video.cs.cmu.edu", "8080", null, &result);
 * if (rc != 0) {
 *     // handle error
 * }
 * // connect to address in result
 * free(result);
 *
 *
 * @param  node  The hostname to resolve.
 * @param  sockfd dns socket fd
 * @param  dns_addr DNS address
 * @param  res  The result. resolve() should allocate a struct addrinfo, which
 * the caller is responsible for freeing.
 *
 * @return 0 on success, -1 otherwise
 */

int resolve(char *node, int sockfd, char *result)
{
    char message[4096];
    char response[4096];
    char server_ip[16];
    create_query(node, message);

    if(-1 == send(sockfd,(char *) message,4096,MSG_DONTWAIT)){
        return -1;
    }

    int n = recvfrom(sockfd, (char *)response, 4096, MSG_WAITALL, NULL, NULL);
    parse_response(response, server_ip);

    printf("resolve returned ip!: %s\n", server_ip);
    sprintf(result, "%s", server_ip);
    return 0;
}


