/********************************************************
*  Author: Sannan Tariq                                *
*  Email: stariq@cs.cmu.edu                            *
*  Description: This code creates a simple             *
*                proxy with some HTTP parsing          *
*                and pipelining capabilities           *
*  Bug Reports: Please send email with                 *
*              subject line "15441-proxy-bug"          *
*  Complaints/Opinions: Please send email with         *
*              subject line "15441-proxy-complaint".   *
*              This makes it easier to set a rule      *
*              to send such mail to trash :)           *
********************************************************/
#include <time.h>
#include <sys/stat.h>
#include "proxy.h"
#include "mydns.h"

/*
 *  @REQUIRES:
 *  client_fd: The fd of the client you want to add
 *  is_server: A flag to tell us whether this client is a server or a requester
 *  sibling_idx: For a server it will be its client, for a client it will be its server
 *  
 *  @ENSURES: returns a pointer to a new client struct
 *
*/
client *new_client(int client_fd, int is_server, size_t sibling_idx, double tput) {
    client *new = calloc(1, sizeof(client));
    new->fd = client_fd;
    new->recv_buf = calloc(INIT_BUF_SIZE, 1);
    new->send_buf = calloc(INIT_BUF_SIZE, 1);
    new->recv_buf_len = 0;
    new->send_buf_len = 0;
    new->recv_buf_size = INIT_BUF_SIZE;
    new->send_buf_size = INIT_BUF_SIZE;
    new->sibling_idx = sibling_idx;

    new->is_server = is_server;
    new->tput = tput;
    new->chunkname = "";
    //new->ts;
    //new->tf;
    gettimeofday(&new->ts, NULL);
    gettimeofday(&new->tf, NULL);

    new->num_b = 0;
    //new->bitrates;
    new->our_bitrate = 0;
    return new;
}

void free_client(client* c) {
    free(c->recv_buf);
    free(c->send_buf);
    free(c);
    return;
}

/*
 *  @REQUIRES:
 *  client_fd: The fd of the client you want to add
 *  clients: A pointer to the array of client structures
 *  read_set: The set we monitor for incoming data
 *  is_server: A flag to tell us whether this client is a server or a requester
 *  sibling_idx: For a server it will be its client's index, for a client it will be its server index
 *  
 *  @ENSURES: Returns the index of the added client if possible, otherwise -1
 *
*/
int add_client(int client_fd, client **clients, fd_set *read_set, int is_server, size_t sibling_idx, double tput) {
    int i;
    for (i = 0; i < MAX_CLIENTS - 1; i ++) {
        if (clients[i] == NULL) {
            clients[i] = new_client(client_fd, is_server, sibling_idx, tput);
            FD_SET(client_fd, read_set);
            return i;
        }
    }
    return -1;
}

/*
 *  @REQUIRES:
 *  clients: A pointer to the array of client structures
 *  i: Index of the client to remove
 *  read_set: The set we monitor for incoming data
 *  write_set: The set we monitor for outgoing data
 *  
 *  @ENSURES: Removes the client and its sibling from all our data structures
 *
*/
int remove_client(client **clients, size_t i, fd_set *read_set, fd_set *write_set) {
    
    if (clients[i] == NULL) {
        return -1;
    }
    printf("Removing client on fd: %d\n", clients[i]->fd);
    int sib_idx;
    close(clients[i]->fd);
    FD_CLR(clients[i]->fd, read_set);
    FD_CLR(clients[i]->fd, write_set);
    sib_idx = clients[i]->sibling_idx;
    //TODO: FREE TIME STRUCTS???

    if (clients[sib_idx] != NULL) {
        printf("Removing client on fd: %d\n", clients[sib_idx]->fd);
        close(clients[sib_idx]->fd);
        FD_CLR(clients[sib_idx]->fd, read_set);
        FD_CLR(clients[sib_idx]->fd, write_set);
        free_client(clients[sib_idx]);
        clients[sib_idx] = NULL;
    }
    free(clients[i]);
    clients[i] = NULL;
    return 0;
}

int find_maxfd(int listen_fd, client **clients) {
    int max_fd = listen_fd;
    int i;
    for (i = 0; i < MAX_CLIENTS - 1; i ++) {
        if (clients[i] != NULL) {
            if (max_fd < clients[i]->fd) {
                max_fd = clients[i]->fd;
            }
        }
    }
    return max_fd;
}



/* @REQUIRES: path to f4m file, empty array
 * @ENSURES:
 *  - parses f4m file and save bitrates in the array
 */
void get_bitrates(char *f_buf, int *bitrates)
{
    int bit_len;
    int i=0;
    char *bit_ptr;
    char *q_ptr;
    char *bit_str;
    FILE *f;

    bit_ptr = f_buf;
    if(bit_ptr == NULL) printf("hey!\n");
    while(strstr(bit_ptr, "bitrate=") != NULL)
    {
        bit_ptr = strstr(bit_ptr, "bitrate=");
        bit_ptr += 9;
        q_ptr = strchr(bit_ptr, '"');
        bit_len = q_ptr - bit_ptr;
        bit_str = malloc(bit_len);
        memcpy(bit_str, bit_ptr, bit_len);
        bitrates[i] = atoi(bit_str);
        printf("this is the bitrate %d\n",bitrates[i]);
        free(bit_str);
        i++;
    }
    printf("end of get_bitrates\n");
}

/* GET NUMBER OF BITRATES IN A F4M FILE */
int get_num_bitrates(char *f_buf)
{
    char *bit_ptr;
    int num_b = 0;
    FILE *f;

    //get number of bitrates
    bit_ptr = f_buf;
    while(strstr(bit_ptr, "bitrate=") != NULL)
    { 
        num_b++;
        printf("%d\n",num_b);
        bit_ptr = strstr(bit_ptr, "bitrate=");
        bit_ptr+= 10;
    }

    return num_b;
}

/* @REQUIRES: array of bitrates, number of bitrates, throughput
 * @ENSURES: 
 *  - choose largest bitrate based on throughput */
int choose_bitrate(int *bitrates, int num_b, double tput)
{
    //printf("this is the tput: %f\n",tput);
    if(num_b == 0) {
        return -1;
    }
    int b = bitrates[0];
    //bitrates are in increasing order
    int i;
    for(i =0; i< num_b; i++){
        if(bitrates[i] * 1.5 <= tput){
            b = bitrates[i];
        }
    }
    printf("chosen bitrate: %d\n", b);
    return b;
}





/*
 *  @REQUIRES:
 *  clients: A pointer to the array of client structures
 *  i: Index of the client to remove
 *  
 *  @ENSURES: 
 *  - tries to send the data present in a clients send buffer to that client
 *  - If data is sent, returns the remaining bytes to send, otherwise -1
 *
*/
int process_client_send(client **clients, size_t i, unsigned short server_port, char *my_ip, int dns_fd) {
    int n;
    char *new_send_buffer;
    
    char server_ip[16];
    char hostname[20];
    //get hostname from send_buf
    if(clients[i]->is_server)
    {
        //hard coded the hostname
        int rc = resolve("video.cs.cmu.edu", dns_fd, (char*)server_ip);
        if (rc != 0) {
            // handle error
            printf("resolve returned something bad\n");
        }
        // connect to address in result
        clients[i]->fd = open_socket_to_server(my_ip, server_ip, server_port);
    }
    n = send(clients[i]->fd, clients[i]->send_buf, clients[i]->send_buf_len, 0);

    if (n <= 0) {
        return -1;
    }

    size_t new_size = max(INIT_BUF_SIZE, clients[i]->send_buf_len - n);
    new_send_buffer = calloc(new_size, sizeof(char));
    memcpy(new_send_buffer, clients[i]->send_buf + n, clients[i]->send_buf_len - n);
    free(clients[i]->send_buf);
    clients[i]->send_buf = new_send_buffer;
    clients[i]->send_buf_len = clients[i]->send_buf_len - n;
    clients[i]->send_buf_size = new_size;

    return clients[i]->send_buf_len;
}

/*
 *  @REQUIRES:
 *  clients: A pointer to the array of client structures
 *  i: Index of the client to remove
 *  
 *  @ENSURES: 
 *  - tries to recv data from the client and updates its internal state as appropriate
 *  - If data is received, return the number of bytes received, otherwise return 0 or -1
 *
*/
int recv_from_client(client** clients, size_t i, double alpha, FILE *log) {
    int n;
    char buf[INIT_BUF_SIZE];
    size_t new_size;
    double tput_new = 0;
    int chunk_size;

    n = recv(clients[i]->fd, buf, INIT_BUF_SIZE, 0);
    
    if (n <= 0) {
	    printf("we are receiving a bad n: %d from a server: %d\n",n, clients[i]->is_server);
        return n;
    }

    /* if client is the server, save tf and new throughput */
    if(clients[i]->is_server)
    {
	    //CHECK FOR FM4 FILE CONTENTS
        int sib_i = clients[i]->sibling_idx;
        //printf("buf (LOOKING FOR F4M CONTENTS): \n%s\n", buf);
        char *f = strstr(buf, "bitrate=");
	    if(f!=NULL){
      	    clients[sib_i]->num_b = get_num_bitrates(buf);
      	    clients[sib_i]->bitrates = malloc(sizeof(int) * clients[sib_i]->num_b);
      	    get_bitrates(buf, clients[sib_i]->bitrates);
	    }
    	
	    else{
            gettimeofday(&(clients[i]->tf), NULL);
                //	printf("Server: tf: %d, ts: %d for client %d\n", clients[i]->tf.tv_sec, clients[sib_i]->ts.tv_sec, i);
        
            int seconds =  (clients[i]->tf.tv_sec - clients[sib_i]->ts.tv_sec);
            long micro_seconds = (clients[i]->tf.tv_usec - clients[sib_i]->ts.tv_usec);
        //      printf("SECONDS: %d, MICROSECONDS: %ld\n", seconds, micro_seconds);
            
            chunk_size = get_content_length(buf, (size_t) n);
                //	printf("CHUNK SIZE: %d\n", chunk_size);
        
            tput_new = ((double) chunk_size) / micro_seconds * 1000000;
            if(chunk_size == 0){
                tput_new = 0;
            }
            else if(micro_seconds == 0){
                tput_new = clients[sib_i]->tput * 2;
            }
            //	printf("tput_new: %f, tput_old: %f\n", tput_new, clients[sib_i]->tput);
            clients[sib_i]->tput = alpha*tput_new + (1-alpha)*(clients[sib_i]->tput);
        
            //	printf("tput: %f for %d\n", clients[sib_i]->tput, sib_i);
            double duration = ((double)micro_seconds)/1000000;
            log_add(log, duration, tput_new, clients[sib_i]->tput, clients[sib_i]->our_bitrate, "3.0.0.1", clients[sib_i]->chunkname);
	    }
    }
    /*if client is browser, save ts */
    else {
	//printf("WE ARE THE BROWSER\n");
    	gettimeofday(&(clients[i]->ts), NULL);
	//printf("ts: %d for client %d\n", clients[i]->ts.tv_sec, i);
    } 

    if(strstr(buf,"GET") != NULL) 
    {

      char *end = strstr(buf,"\r\n");
      char uri[end-buf+2];
      memcpy(uri,buf,end-buf+2);
      char *a = strstr(uri,"Seg");
      char *f = strstr(uri, ".fm4");
      char *temp_end = strstr(uri,"\r\n");
      
      if(f != NULL) //IF REQUEST FOR THE F4M FILE
      {
      }

      if(a != NULL) //IF REQUEST FOR VIDEO CHUNK
      {
	    clients[i]->chunkname = uri;
	    //printf("chunkname: %s\n", clients[i]->chunkname);
	    //printf("client : %d, tput: %f\n", i, clients[i]->tput);
        char *b = malloc(a-uri);
        memcpy(b,uri,a-uri);
      	char *c = strrchr(b,'/');
        char before_bitrate[c-b+2];
        memcpy(before_bitrate,uri,c-b+1);
        before_bitrate[c-b+1] = 0;

        char after_bitrate[temp_end-a+1];
        memcpy(after_bitrate,a,temp_end-a+1);
        after_bitrate[temp_end-a] = 0;
	    //choose and replace the bitrate in the request
        clients[i]->our_bitrate = choose_bitrate(clients[i]->bitrates, clients[i]->num_b, clients[i]->tput);

        char new_uri[c-b+1+32+strlen(a)];
        sprintf(new_uri,"%s%d%s",before_bitrate, clients[i]->our_bitrate, after_bitrate);
        //printf("this is also a new uri: %s\n",new_uri);
        char new_buf[INIT_BUF_SIZE];
        sprintf(new_buf,"%s\r\n%s",new_uri,end+2); 
        n = strlen(new_buf);
	    memset(buf,0,INIT_BUF_SIZE);
        memcpy(buf,new_buf,n);
        free(b); 
      } 
    }
        
    //printf("this is the buf: %s\n",buf);
    new_size = clients[i]->recv_buf_size;

    while (n > new_size - clients[i]->recv_buf_len) {
        new_size *= 2;
        
    }
    clients[i]->recv_buf = resize(clients[i]->recv_buf, new_size, clients[i]->recv_buf_size);
    clients[i]->recv_buf_size = new_size;

    memcpy(&(clients[i]->recv_buf[clients[i]->recv_buf_len]), buf, n);
    clients[i]->recv_buf_len += n;

    //printf("\n");
    return n;
}


/*
 *  @REQUIRES:
 *  clients: A pointer to the array of client structures
 *  i: Index of the client to remove
 *  buf: The message to add to the send buffer
 *  
 *  @ENSURES: 
 *  - appends data to the client's send buffer and returns the number of bytes appended
 *
*/
int queue_message_send(client **clients, size_t i, char *buf, int msg_len) {
    size_t n = (size_t) msg_len; //strlen(buf);
    size_t new_size;

    new_size = clients[i]->send_buf_size;

    while (n > new_size - clients[i]->send_buf_len) {
        new_size *= 2;
        
    }
    clients[i]->send_buf = resize(clients[i]->send_buf, 
        new_size, clients[i]->send_buf_size);
    clients[i]->send_buf_size = new_size;

    memcpy(&(clients[i]->send_buf[clients[i]->send_buf_len]), buf, n);
    clients[i]->send_buf_len += n;
    return n;
}


/*
 *  @REQUIRES:
 *  clients: A pointer to the array of client structures
 *  i: Index of the client to remove
 *  data_available: flag whether you can call recv on this client without blocking
 *  write_set: the set containing the fds to observe for being ready to write to
 *  
 *  @ENSURES: 
 *  - tries to read data from the client, then tries to reap a complete http message
 *      and finally tries to queue the message to be forwarded to its sibling
 *  - returns number of bytes queued if no errors, -1 otherwise
 *
*/
int process_client_read(client **clients, size_t i, int data_available, fd_set *write_set, double alpha, FILE *log) {
    char *msg_rcvd;
    int nread;
    int msg_len;
    if (data_available == 1) {
        if ((nread = recv_from_client(clients, i, alpha, log)) < 0) {
            fprintf(stderr, "start_proxying: Error while receiving from client\n");
            return -1;
        }
        else if (nread == 0) {
            return -1;
        }
    }
    msg_len = find_http_message_end(clients[i]->recv_buf, clients[i]->recv_buf_len);
    if ((msg_rcvd = pop_message(&(clients[i]->recv_buf), &(clients[i]->recv_buf_len), &clients[i]->recv_buf_size)) == NULL) {
        return 0;
    }

    else {
        int sibling_idx = clients[i]->sibling_idx;
        int bytes_queued = queue_message_send(clients, sibling_idx, msg_rcvd, msg_len);
        FD_SET(clients[sibling_idx]->fd, write_set);
        return bytes_queued;
    }

}

int start_proxying(char *log_file, double alpha, unsigned short listen_port, char *fake_ip, char *dns_ip, unsigned short dns_port, char *www_ip) {
    int max_fd, nready, listen_fd;
    fd_set read_set, read_ready_set, write_set, write_ready_set;
    struct sockaddr_in cli_addr;
    socklen_t cli_size;
    client **clients;
    size_t i;
    //unsigned short listen_port = 8888;
    char *server_ip = www_ip; //"127.0.0.1";
    unsigned short server_port = 8080; //10000;
    char *my_ip = fake_ip; //"0.0.0.0";
    FILE *log = log_create(log_file);
    
    int dns_fd = init_mydns(fake_ip, dns_port);

    if ((listen_fd = open_listen_socket(listen_port)) < 0) {
        fprintf(stderr, "start_proxy: Failed to start listening\n");
        return -1;
    }

    // init_multiplexer(listen_fd, clients, read_set, write_set);

    clients = calloc(MAX_CLIENTS - 1, sizeof(client*));
    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
    FD_SET(listen_fd, &read_set);
    
    max_fd = listen_fd;
    printf("Initiating select loop\n");
    while (1) {
        read_ready_set = read_set;
        write_ready_set = write_set;
        // printf("Watining to select...\n");
        nready = select(max_fd+1, &read_ready_set, &write_ready_set, NULL, NULL);

        if (nready > 0) {
            if (FD_ISSET(listen_fd, &read_ready_set)) {
                int client_fd;
                int client_idx;
                nready --;

                cli_size = sizeof(cli_addr);

                if ((client_fd = accept(listen_fd, (struct sockaddr *) &cli_addr,
                                    &cli_size)) == -1) {
                    fprintf(stderr, "start_proxying: Failed to accept new connection");
                }

                // add the client to the client_fd list of filed descriptors
                else if ((client_idx = add_client(client_fd, clients, &read_set, 0, -1, 0))!= -1) 
                {    
                    int sibling_fd = open_socket_to_server(my_ip, server_ip, server_port);
                    int server_idx = add_client(sibling_fd, clients, &read_set, 1, client_idx, 0);
                    clients[client_idx]->sibling_idx = server_idx;
                    printf("start_proxying: Connected to %s on FD %d\n"
                    "And its sibling %s on FD %d\n", inet_ntoa(cli_addr.sin_addr),
                        client_fd, server_ip, sibling_fd);

                }
                else
                    close(client_fd);         
            }

            for (i = 0; i < MAX_CLIENTS - 1 && nready > 0; i++) {
                if (clients[i] != NULL) {
                    int data_available = 0;
                    if (FD_ISSET(clients[i]->fd, &read_ready_set)) {
                        nready --;
                        data_available = 1;
                    }

                    int nread = process_client_read(clients, i, data_available, &write_set, alpha, log);

                    if (nread < 0) {
		            	printf("nread is less than 0!!!!\n");
                        if (remove_client(clients, i, &read_set, &write_set) < 0) {
                            fprintf(stderr, "start_proxying: Error removing client\n");
                        }
                    }

                    if (nread >= 0 && nready > 0) {
                        if (FD_ISSET(clients[i]->fd, &write_ready_set)) {
                            nready --;
                            int nsend = process_client_send(clients, i, server_port, my_ip, dns_fd);
                            if (nsend < 0) {
				                printf("nsend is less than 0!!!!!\n");
                                if (remove_client(clients, i, &read_set, &write_set) < 0) {
                                    fprintf(stderr, "start_proxying: Error removing client\n");
                                }
                            }                    
                            else if (nsend == 0) {
                                FD_CLR(clients[i]->fd, &write_set);
                            }
                        }
                    }
                }
                
            }
            max_fd = find_maxfd(listen_fd, clients);
        }

    }
    

}

int main(int argc, char* argv[]) {
    char* log_file = argv[1];
    char* a = argv[2];
    char* lp = argv[3];
    char* fake_ip = argv[4];
    char* dns_ip = argv[5];
    char* dp = argv[6];
    char* www_ip = argv[7]; 
    double alpha = (double) atof(a);
    unsigned short listen_port = (unsigned short) atoi(lp);
    unsigned short dns_port = (unsigned short) atoi(dp);
    
    printf("Starting the proxy...\n");
    start_proxying(log_file, alpha, listen_port, fake_ip, dns_ip, dns_port, www_ip);
    return 0;
}
