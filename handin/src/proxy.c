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
    new->ts = malloc(sizeof(struct timeval));
    new->tf = malloc(sizeof(struct timeval));
    gettimeofday(new->ts, NULL);
    gettimeofday(new->tf, NULL);

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
void get_bitrates(char *f_path, int *bitrates)
{
    struct stat st;
    int bit_len;
    int i=0;
    size_t f_size;
    char *f_buf;
    char *bit_ptr;
    char *q_ptr;
    char *bit_str;
    FILE *f;

    //get size of file
    stat(f_path, &st);                                                       
    f_size = st.st_size;

    //open and save file in buf                                                              
    f = fopen(f_path, "r");                                            
    f_buf = malloc(f_size);
    fread(f_buf, f_size, 1, f);

    bit_ptr = f_buf;
    while(strstr(bit_ptr, "bitrate=") != NULL)
    {
        bit_ptr = strstr(bit_ptr, "bitrate=");
        bit_ptr += 9;
        q_ptr = strchr(bit_ptr, '"');
        bit_len = q_ptr - bit_ptr;
        bit_str = malloc(bit_len);
        memcpy(bit_str, bit_ptr, bit_len);
        bitrates[i] = atoi(bit_str);
        free(bit_str);
        i++;
    }
    
    free(f_buf);  
}

/* GET NUMBER OF BITRATES IN A F4M FILE */
int get_num_bitrates(char *f_path)
{
    struct stat st;
    size_t f_size;
    char *f_buf;
    char *bit_ptr;
    int num_b = 0;
    FILE *f;

    //get number of bitrates
    stat(f_path, &st);                                                       
    f_size = st.st_size;
                                                            
    f = fopen(f_path, "r");                                            
    f_buf = malloc(f_size);
    fread(f_buf, f_size, 1, f);

    bit_ptr = f_buf;
    while(strstr(bit_ptr, "bitrate=") != NULL)
    {
        num_b++;
        bit_ptr = strstr(bit_ptr, "bitrate=");
        bit_ptr+= 10;
    }
    free(f_buf);

    return num_b;
}

/* @REQUIRES: array of bitrates, number of bitrates, throughput
 * @ENSURES: 
 *  - choose largest bitrate based on throughput */
int choose_bitrate(int *bitrates, int num_b, double tput)
{
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
int process_client_send(client **clients, size_t i) {
    int n;
    char *new_send_buffer;
    
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
int recv_from_client(client** clients, size_t i, double alpha) {
    int n;
    char buf[INIT_BUF_SIZE];
    char new_buf[INIT_BUF_SIZE];
    size_t new_size;
    double tput_new = 0;
    int chunk_size;

    n = recv(clients[i]->fd, buf, INIT_BUF_SIZE, 0);
    
    if (n <= 0) {
        return n;
    }

    /* if client is the server, save tf and new throughput */
    if(clients[i]->is_server)
    {
    	gettimeofday(clients[i]->tf, NULL);
        tput_new = ((double)n) / (clients[i]->tf->tv_sec - clients[i]->ts->tv_sec);
	chunk_size = get_content_length(buf, (size_t) n); 	
	clients[i]->tput = alpha*tput_new + (1-alpha)*(clients[i]->tput);
    }
    /*if client is browser, save ts */
    else {
    	gettimeofday(clients[i]->ts, NULL);
    } 

    if(strstr(buf,"GET") != NULL) 
    {

      char *end = strstr(buf,"\r\n");
      char uri[end-buf+2];
      memcpy(uri,buf,end-buf+2);
      char *a = strstr(uri,"Seg");
      char *f = strstr(uri, ".f4m");
      char *temp_end = strstr(uri,"\r\n");
      
      if(f != NULL) //IF REQUEST FOR THE FM4 FILE
      {
      	char before_add[f-uri+8];
	char file_name[f-uri+5];
  	memcpy(before_add, uri, f-uri);
	memcpy(file_name, before_add, f-uri);
	memcpy(&file_name[f-uri], ".f4m\0", 5);
	char *file_start = strchr(file_name, '/'); //FILE PATH

	memcpy(&before_add[f-uri], "_nolist\0", 8);
	char after_add[temp_end-f+1];
	memcpy(after_add, f, temp_end-f);
	after_add[temp_end-f] = 0;
	char new_uri[strlen(before_add)+2+strlen(after_add)];
	sprintf(new_uri, "%s%s", before_add, after_add);
	//char new_buf[INIT_BUF_SIZE];
	sprintf(new_buf, "%s\r\n%s", new_uri, end+2);
	printf("NEW BUF: \n%s\n", new_buf);
	//save bitrates from fm4 file	
      	clients[i]->num_b = get_num_bitrates(file_start);
      	clients[i]->bitrates = malloc(sizeof(int) * clients[i]->num_b);
      	get_bitrates(file_start, clients[i]->bitrates);
      
      }

      if(a != NULL) //IF REQUEST FOR VIDEO CHUNK
      {
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
        char new_buf[INIT_BUF_SIZE];
        sprintf(new_buf,"%s\r\n%s",new_uri,end+2);
        free(b); 
      } 
    }
        
    n = strlen(new_buf);
    new_size = clients[i]->recv_buf_size;

    while (n > new_size - clients[i]->recv_buf_len) {
        new_size *= 2;
        
    }
    clients[i]->recv_buf = resize(clients[i]->recv_buf, new_size, clients[i]->recv_buf_size);
    clients[i]->recv_buf_size = new_size;

    memcpy(&(clients[i]->recv_buf[clients[i]->recv_buf_len]), new_buf, n);
    clients[i]->recv_buf_len += n;

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
int process_client_read(client **clients, size_t i, int data_available, fd_set *write_set, double alpha) {
    char *msg_rcvd;
    int nread;
    int msg_len;
    if (data_available == 1) {
        if ((nread = recv_from_client(clients, i, alpha)) < 0) {
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
                else if ((client_idx = add_client(client_fd, clients, &read_set, 0, -1, 0))!= -1) {
                    
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

                    int nread = process_client_read(clients, i, data_available, &write_set, alpha);

                    if (nread < 0) {
                        if (remove_client(clients, i, &read_set, &write_set) < 0) {
                            fprintf(stderr, "start_proxying: Error removing client\n");
                        }
                    }

                    if (nread >= 0 && nready > 0) {
                        if (FD_ISSET(clients[i]->fd, &write_ready_set)) {
                            nready --;
                            int nsend = process_client_send(clients, i);
                            if (nsend < 0) {
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
