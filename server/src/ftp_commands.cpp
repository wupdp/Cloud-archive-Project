#include "../include/ftp_commands.h"

void handle_ls_command(int connfd, int data_port) {
    // Реализация команды ls
    FILE *in;
    char temp[MAXLINE],port[MAXLINE];
    int datasock;
    data_port=data_port+1;
    sprintf(port,"%d",data_port);
    datasock=create_socket(data_port);			//creating socket for data connection
    send(connfd, port,MAXLINE,0);				//sending data connection port no. to client
    datasock=accept_conn(datasock);	 			//accepting connection from client
    if(!(in = popen("ls", "r"))){
        std::cout<<"error"<<std::endl;
    }
    while(fgets(temp, sizeof(temp), in)!=NULL){
        send(datasock,"1",MAXLINE,0);
        send(datasock, temp, MAXLINE, 0);
    }
    send(datasock,"0",MAXLINE,0);
    pclose(in);
}

void handle_pwd_command(int connfd) {
    // Реализация команды pwd
    std::string curr_dir = get_current_dir();
    send(connfd, curr_dir.c_str(), MAXLINE, 0);
}

void handle_cd_command(int connfd, const char* directory) {
    // Реализация команды cd
    if(chdir(directory)<0){
        send(connfd,"0",MAXLINE,0);
    }
    else{
        send(connfd,"1",MAXLINE,0);
    }
}

void handle_put_command(int connfd, int data_port, const char* filename) {
    // Реализация команды put
    char port[MAXLINE], buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE];
    int datasock,num_blks,num_last_blk,i;
    FILE *fp;
    data_port=data_port+1;
    sprintf(port,"%d",data_port);
    datasock=create_socket(data_port);				//creating socket for data connection
    send(connfd, port,MAXLINE,0);					//sending data connection port to client
    datasock=accept_conn(datasock);					//accepting connection
    if ((fp=fopen(filename,"r"))!=NULL)
    {
        //size of file
        fseek (fp , 0 , SEEK_END);
        long lSize = ftell (fp);
        rewind (fp);
        num_blks = lSize/MAXLINE;
        num_last_blk = lSize%MAXLINE;
        sprintf(char_num_blks,"%d",num_blks);
        send(connfd, char_num_blks, MAXLINE, 0);

        for(i= 0; i < num_blks; i++) {
            fread (buffer,sizeof(char),MAXLINE,fp);
            send(datasock, buffer, MAXLINE, 0);
        }
        sprintf(char_num_last_blk,"%d",num_last_blk);
        send(connfd, char_num_last_blk, MAXLINE, 0);
        if (num_last_blk > 0) {
            fread (buffer,sizeof(char),num_last_blk,fp);
            send(datasock, buffer, MAXLINE, 0);
        }
        fclose(fp);
    }
    else{
        send(connfd,"0",MAXLINE,0);
    }
}

void handle_get_command(int connfd, int data_port, const char* filename) {
    // Реализация команды get
    char port[MAXLINE],buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE];
    int datasock,lSize,num_blks,num_last_blk,i;
    FILE *fp;
    data_port=data_port+1;
    sprintf(port,"%d",data_port);
    datasock=create_socket(data_port);				//creating socket for data connection
    send(connfd, port,MAXLINE,0);					//sending port no. to client
    datasock=accept_conn(datasock);					//accepting connnection by client
    if ((fp=fopen(filename,"w"))!=NULL)
    {
        recv(connfd, char_num_blks, MAXLINE,0);
        num_blks=atoi(char_num_blks);
        for(i= 0; i < num_blks; i++) {
            recv(datasock, buffer, MAXLINE,0);
            fwrite(buffer,sizeof(char),MAXLINE,fp);
        }
        recv(connfd, char_num_last_blk, MAXLINE,0);
        num_last_blk=atoi(char_num_last_blk);
        if (num_last_blk > 0) {
            recv(datasock, buffer, MAXLINE,0);
            fwrite(buffer,sizeof(char),num_last_blk,fp);
        }
        fclose(fp);
    }
    else{
        send(connfd,"0",MAXLINE,0);
    }
}