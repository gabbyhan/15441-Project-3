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
int init_mydns(const char *dns_ip, unsigned int dns_port)
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
 * @param  service  The desired port number as a string.
 * @param  hints  Should be null. resolve() ignores this parameter.
 * @param  res  The result. resolve() should allocate a struct addrinfo, which
 * the caller is responsible for freeing.
 *
 * @return 0 on success, -1 otherwise
 */

int resolve(const char *node, const char *service, 
            const struct addrinfo *hints, struct addrinfo **res)
{
    (*res) = (struct addrinfo *)malloc(sizeof(struct addrinfo));
   
    //struct sockaddr_in cliaddr;
    
    char message[4096];
    char data[4096];
    char server_ip[16];
    create_query(node, message);

    sendto(sockfd,(char *) message,4096,MSG_DONTWAIT, (struct sockaddr *) &cliaddr, len);

    int n = recvfrom(sockfd, (char *) buffer, 4096, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);
    parse_response(data, server_ip);

}
