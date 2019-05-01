#include "dijkstra.h"

void dijkstra(int **G, int num_v, int *visited, Node **pq)
{
	if(isEmpty(pq)){
		return;
	}
	//get next closest vertex
	int v = peek_data(pq);
	int d = peek_priority(pq);
	//remove closest vertex from queue
	pop(pq);
	//if v has already been visited skip vertex
	if(visited[v] != -1)
	{
		dijkstra(G, num_v, visited, pq);	
	}
	//otherwise add v to visited and add its neighbors to queue
	else{
	    visited[v] = d;
	    for(int i=0; i<num_v; i++)
	    {
		int w = G[v][i];
		if(w != -1){
		    push(pq, i, d+w); 
		}
	    }
	    dijkstra(G, num_v, visited, pq);
	}
}

int get_servers(char **servers, char *path)
{
    struct stat st;                                                             
    stat(path, &st);                                                       
    size_t f_size = st.st_size; //size of file
	
    FILE *f = fopen(path, "r");
    if(f== NULL){
	    printf("Error opening file: %s", path);
    }                                            
    char f_buf[f_size];
    fread(f_buf, f_size, 1, f);

    char *line_ptr = f_buf;
    char *sender;
    int len = 0;

    int num_servers = 0;

    while(strchr(line_ptr, '\n') != NULL)
    {   
        char *end = strchr(line_ptr, '\n');
        len = end - line_ptr;
        sender = (char *)malloc(len);
        memcpy(sender, line_ptr, len);

        line_ptr += len+1; //move line ptr

    	int added = 0;
    	//check if sender already added
    	for(int i=0; i<100; i++)
    	{
    	    if(strcmp(servers[i], sender)==0){
    		    added = 1;
   		        break;
    	    }
    	}
    	//if no, add sender and add neighbors
    	if(added == 0){
    	    strcpy(servers[num_v], sender);
    	    num_servers++;
    	}
        free(sender);
    }  
    return num_servers;
}


void run_dijkstra(char *full_path, char *client_ip, char *server_ip)
{
    struct stat st;                                                             
    stat(full_path, &st);                                                       
    size_t f_size = st.st_size; //size of file
	
    FILE *f = fopen(full_path, "r");
    if(f== NULL){
	    printf("Error opening file: %s", full_path);
    }                                            
    char f_buf[f_size];
    fread(f_buf, f_size, 1, f);

    char *line_ptr = f_buf;
    char *sender;
    int seq_num;
    char *neighbors;
    int len1 = 0;
    int len2 = 0;
    int len3 = 0;

    char *ips[100]; 	//the number of nodes in our graph could be > 100??
    int seqs[100];
    char *N[100];
    for(int i =0; i<100; i++){
	    ips[i] = malloc(20); //length of ip address/ router name??
    	seqs[i] = -1;
	    N[i] = malloc(200); //length of list of neighbors???
    }

    int num_v = 0;

    while(strchr(line_ptr, '\n') != NULL)
    {   
        char *space1 = strchr(line_ptr, ' ');
        len1 = space1 - line_ptr;
        sender = (char *)malloc(len1);
        memcpy(sender, line_ptr, len1);

        line_ptr += len1+1; //move line ptr
        
        char *space2 = strchr(line_ptr, ' ');
        len2 = space2 - line_ptr;
        char *temp = (char *)malloc(len2);
        memcpy(temp, line_ptr, len2);
        seq_num = atoi(temp);

        line_ptr += len2+1;

        char *eol = strchr(line_ptr, '\n');
    	len3 = eol-line_ptr;
    	neighbors = (char *)malloc(len3);
    	memcpy(neighbors, line_ptr, len3);

    	line_ptr += len3+1;

    	int added = 0;
    	//check if sender already added
    	for(int i=0; i<100; i++)
    	{
    	    if(strcmp(ips[i], sender)==0){
    		//if yes, update neighbors
    		if(seqs[i] < seq_num){
    			strcpy(N[i], neighbors);
    			seqs[i] = seq_num;
    		}
    		added = 1;
   		break;
    	    }
    	}
    	//if no, add sender and add neighbors
    	if(added == 0){
    	    strcpy(ips[num_v], sender);
    	    seqs[num_v] = seq_num;
    	    strcpy(N[num_v], neighbors);
    	    num_v++;
    	}
        free(sender);
        free(temp);
        free(neighbors);
    }  
	
    printf("num v: %d\n", num_v);

    char *n_ptr;
    int n_len;
    int in_v = 0;
    /***ADD NODES THAT ARE NOT SENDERS (only in neighbors)***/
    for(int i=0; i<num_v; i++){
	n_ptr = N[i];
	while(strchr(n_ptr, ',') != NULL){
	    char *comma = strchr(n_ptr, ',');
	    n_len = comma - n_ptr;
	    char *neigh = malloc(n_len);
	    memcpy(neigh, n_ptr, n_len);
	    //printf("neigh:%s\n", neigh);
	    
	    in_v = 0;
	    for(int j=0; j<num_v; j++){
		if(strcmp(ips[j], neigh) ==0){
		    in_v = 1;
		    break;
		}
	    }
	    if(in_v == 0){
		ips[num_v] = neigh;
		N[num_v] = ips[i];
		num_v++;
	    }
	    n_ptr += n_len+1;
	    free(neigh);
    	}
	char *end = strchr(n_ptr, '\0');
	n_len = end-n_ptr;
	char *last = malloc(n_len);
	memcpy(last, n_ptr, n_len);
	//printf("last:%s\n", last);
	
	in_v = 0;
	for(int j=0; j<num_v; j++){
	    if(strcmp(ips[j], last) ==0){
		in_v = 1;
		break;
	    }
	}
	
	if(in_v == 0){
	    ips[num_v] = last;
	    N[num_v] = ips[i];
	    num_v++;
	}
	free(last);
    }

    printf("num v: %d\n", num_v);

    /*** INITIALIZE GRAPH ***/
    int** G = (int**) malloc(num_v * sizeof(int*));
    
    for(int i = 0; i < num_v; i++)
    {
	G[i] = (int *)malloc(num_v * sizeof(int));
    }
    for(int r= 0; r<num_v; r++)
    {
	for(int c=0; c<num_v; c++)
	{
	     G[r][c] = -1;
	}
    }
    
    for(int i=0; i<num_v; i++){
	n_ptr = N[i];
	while(strchr(n_ptr, ',') != NULL){
	    char *comma = strchr(n_ptr, ',');
	    n_len = comma - n_ptr;
	    char *neigh = malloc(n_len);
	    memcpy(neigh, n_ptr, n_len);
	    //printf("neigh:%s\n", neigh);
	    
	    for(int j=0; j<num_v; j++){
		if(strcmp(ips[j], neigh) ==0){
		    G[i][j] = 1;
		    printf("G[%d][%s] = 1 \n", i, neigh);
		    break;
		}
	    }
	    n_ptr += n_len+1;
	    free(neigh);
    	}
	char *end = strchr(n_ptr, '\0');
	n_len = end-n_ptr;
	char *last = malloc(n_len);
	memcpy(last, n_ptr, n_len);
	//printf("last:%s\n", last);
	
	for(int j=0; j<num_v; j++){
	    if(strcmp(ips[j], last) ==0){
		G[i][j] = 1;
		printf("G[%d][%s] = 1 \n", i, last);
		break;
	    }
	}
	free(last);
    }

    int s;
    for(int i=0; i<num_v; i++)
    {
        if(strcmp(client_ip, ips[i]) == 0){
            printf("client found!\n");
            s = i;
            break;
        }
    }

    Node* pq = newNode(s, 0);
    int *visited = (int *)malloc(num_v * sizeof(int));
	
    for(int i=0; i<num_v; i++){
    	visited[i] = -1;
    }

    dijkstra(G, num_v, visited, &pq);

    int min_d = 1000;
    int min_i = 0;
    for(int i=0; i<num_v; i++){
        if(min_d > visited[i])
        {
            if(strstr(ips[i], "router") == NULL){
                min_d = visited[i];
                min_i = i;
            }
        }
    	//printf("dist from 6 to %d: %d\n", i, visited[i]);
    }

    sprintf(server_ip, "%s", ips[min_i]);
    
}



