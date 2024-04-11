#include "client.h"

using namespace std;

void menu_client() {
    printf("\nWelcome to FTP-client!\n\n");
    printf("Command that you can use:\n");
    printf("ls                  --      check file that you can get\n");
    printf("get <file-name>     --      get file from server\n");
    printf("put <file-name>     --      send file to server\n");
    printf("pwd                 --      show current directory name\n");
    printf("cd <directory-name> --      go to different directory\n");
    printf("cd ..               --      go to the directory above\n");
    printf("quit                --      shutdown\n");
}

void read_input(char* buffer, int size)
{
	char* nl = NULL;
	memset(buffer, 0, size);

	if (fgets(buffer, size, stdin) != NULL ) {
		nl = strchr(buffer, '\n');
		if (nl) *nl = '\0'; //ovewriting newline
	}
}

int create_socket(int port,char *addr)
{
    int sockfd;
    struct sockaddr_in serv_addr;

    //Create a socket for the client
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        cerr<<"Problem in creating the socket"<<endl;
        exit(2);
    }

    //Creation of the socket
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr= inet_addr(addr);
    serv_addr.sin_port =  htons(port); //convert to big-endian order

    //Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0) {
        cerr<<"Problem in creating data channel"<<endl;
        exit(3);
    }

    return(sockfd);
}

int read_command_client(char* buf, int size, struct command *cstruct)
{
	memset(cstruct->code, 0, sizeof(cstruct->code));
	memset(cstruct->arg, 0, sizeof(cstruct->arg));
	
	printf("client-input> ");	// prompt for input		
	fflush(stdout); 	

	// wait for user to enter a command
	read_input(buf, size);
    size_t len = strlen(buf);
    char *com = (char *)malloc((len + 1) * sizeof(char)); 
	strcpy(com, buf);

	char *arg = NULL;
	arg = strtok (buf," ");
	arg = strtok (NULL, " ");

	if (arg != NULL){
		// store the argument if there is one
		strncpy(cstruct->arg, arg, strlen(arg));
	}

	// buf = command
	if (strcmp(buf, "ls") == 0) {
		strcpy(cstruct->code, "LS");
        strcpy(cstruct->serv_code, "ls\n");		
	}
	else if (strcmp(buf, "get") == 0) {
		strcpy(cstruct->code, "GET");
        strcpy(cstruct->serv_code, com);	
	}
    else if (strcmp(buf, "put") == 0) {
        strcpy(cstruct->code, "PUT");
        strcpy(cstruct->serv_code, com);
    }
	else if (strcmp(buf, "quit") == 0) {
		strcpy(cstruct->code, "QUIT");	
        strcpy(cstruct->serv_code, "quit\n");	
	}
    else if (strcmp(buf, "cd") == 0) {
        strcpy(cstruct->code, "CD");
        strcpy(cstruct->serv_code, com);
    }
    else if (strcmp(buf, "pwd") == 0) {
        strcpy(cstruct->code, "PWD");
        strcpy(cstruct->serv_code, "pwd\n");	
    }
	else {
		return -1;
	}

	// store code in beginning of buffer
	memset(buf, 0, 400);
	strcpy(buf, cstruct->code);

	// if there's an arg, append it to the buffer
	if (arg != NULL) {
		strcat(buf, " ");
		strncat(buf, cstruct->arg, strlen(cstruct->arg));
	}
	return 0;
}

void client_show_list(int sockfd, char* server_ip) {
    char buff[BUFFER_SIZE],check[BUFFER_SIZE]="1",port[BUFFER_SIZE];
    int data_port, datasock;
    recv(sockfd, port, BUFFER_SIZE, 0);				//reciening data connection port
    data_port=atoi(port);
    datasock = create_socket(data_port, server_ip);
    while(strcmp("1",check)==0){ 				//to indicate that more blocks are coming
        recv(datasock,check,BUFFER_SIZE,0);
        if(strcmp("0",check)==0)			//no more blocks of data
            break;
        recv(datasock, buff, BUFFER_SIZE,0);
        cout << buff;
    }
}

void client_get_file(int sockfd, char* server_ip, struct command cmd) {
    char port[BUFFER_SIZE], buffer[BUFFER_SIZE];
    char char_num_blks[BUFFER_SIZE],char_num_last_blk[BUFFER_SIZE],message[BUFFER_SIZE];
    int data_port, datasock, lSize, num_blks, num_last_blk, i;
    FILE *fp;
    recv(sockfd, port, BUFFER_SIZE, 0);
    data_port = atoi(port);
    datasock = create_socket(data_port, server_ip);
    recv(sockfd, message, BUFFER_SIZE, 0);
    if (strcmp("1", message) == 0){
        if ((fp = fopen(cmd.arg, "w")) == NULL)
            cout<<"Error in creating file\n";
        else
        {
            recv(sockfd, char_num_blks, BUFFER_SIZE, 0);
            num_blks = atoi(char_num_blks);
            for( i = 0; i < num_blks; i++) {
                recv(datasock, buffer, BUFFER_SIZE,0);
                fwrite(buffer, sizeof(char), BUFFER_SIZE, fp);
            }
            recv(sockfd, char_num_last_blk, BUFFER_SIZE, 0);
            num_last_blk = atoi(char_num_last_blk);
            if (num_last_blk > 0) {
                recv(datasock, buffer, BUFFER_SIZE, 0);
                fwrite(buffer, sizeof(char), num_last_blk, fp);
            }
            fclose(fp);
            cout << "File <" << cmd.arg << "> downloaded succesfully." << endl;
        }
    }
    else{
        cerr << "Error in opening file. Check filename\nUsage: put filename" << endl;
    }
}

void client_put_file(int sockfd, char* server_ip, struct command cmd) {
    char port[BUFFER_SIZE], buffer[BUFFER_SIZE], char_num_blks[BUFFER_SIZE];
    char char_num_last_blk[BUFFER_SIZE];
    int data_port, datasock, lSize, num_blks, num_last_blk;
    FILE *fp;
    recv(sockfd, port, BUFFER_SIZE, 0);				//receiving the data port
    data_port = atoi(port);
    datasock = create_socket(data_port, server_ip);

    if ((fp = fopen(cmd.arg, "r"))!=NULL)
    {
        //size of file
        send(sockfd, "1", BUFFER_SIZE, 0);
        fseek (fp, 0, SEEK_END);
        lSize = ftell (fp);
        rewind (fp);
        num_blks = lSize/BUFFER_SIZE;
        num_last_blk = lSize%BUFFER_SIZE;
        sprintf(char_num_blks, "%d", num_blks);
        send(sockfd, char_num_blks, BUFFER_SIZE, 0);

        for(int i = 0; i < num_blks; i++) {
            fread(buffer,sizeof(char),BUFFER_SIZE,fp);
            send(datasock, buffer, BUFFER_SIZE, 0);
        }
        sprintf(char_num_last_blk, "%d", num_last_blk);
        send(sockfd, char_num_last_blk, BUFFER_SIZE, 0);
        if (num_last_blk > 0) {
            fread (buffer,sizeof(char),num_last_blk,fp);
            send(datasock, buffer, BUFFER_SIZE, 0);
        }
        fclose(fp);
        cout << "File " << cmd.arg << " uploaded succesfully.\n";
    }
    else{
        send(sockfd,"0",BUFFER_SIZE,0);
        cerr<<"Error in opening file. Check filename\nUsage: put filename"<<endl;
    }
}

void client_cd_action(int sockfd, struct command cmd) {
    char check[BUFFER_SIZE];
    cout << "Given path: " << cmd.arg << endl;
    recv(sockfd, check, BUFFER_SIZE, 0);
    if (strcmp("0", check)==0){
        cerr<<"Directory doesn't exist. Check Path"<<endl;
    }
}

void client_pwd_action(int sockfd) {
    char current_directory[BUFFER_SIZE];
    recv(sockfd, current_directory, BUFFER_SIZE,0);
    cout << current_directory << endl;
}