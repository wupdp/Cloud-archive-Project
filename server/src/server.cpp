#include "../include/server.h"
#include "../include/socket.h"
#include "../include/ftp_commands.h"

void run_server(int argc, char **argv) {
    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    if (argc !=2) {						//validating the input
        cerr<<"Usage: ./a.out <port number>"<<endl;
        exit(1);
    }

    //Create a socket for the soclet
    //If sockfd<0 there was an error in the creation of the socket
    if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        cerr<<"Problem in creating the socket"<<endl;
        exit(2);
    }

    //preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(atoi(argv[1])<=1024){
        cerr<<"Port number must be greater than 1024"<<endl;
        exit(2);
    }
    servaddr.sin_port = htons(atoi(argv[1]));

    //bind the socket
    bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    //listen to the socket by creating a connection queue, then wait for clients
    listen (listenfd, LISTENQ);

    cout<<"Server running...waiting for connections."<<endl;

    for ( ; ; ) {

        clilen = sizeof(cliaddr);
        //accept a connection
        connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

        cout<<"Received request..."<<endl;

        if ( (childpid = fork ()) == 0 ) {//if it’s 0, it’s child process

            cout<<"Child created for dealing with client requests"<<endl;

            //close listening socket
            close (listenfd);
            int data_port=1024;						//for data connection
            while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
                cout<<"String received from client: "<<buf;
                // Here we will call the appropriate FTP command function based on the received command
                if (strncmp("ls", buf, 2) == 0) {
                    handle_ls_command(connfd, data_port);
                } else if (strncmp("pwd", buf, 3) == 0) {
                    handle_pwd_command(connfd);
                } else if (strncmp("cd", buf, 2) == 0) {
                    handle_cd_command(connfd, buf+3);
                } else if (strncmp("put", buf, 3) == 0) {
                    handle_put_command(connfd, data_port, buf+4);
                } else if (strncmp("get", buf, 3) == 0) {
                    handle_get_command(connfd, data_port, buf+4);
                } else {
                    cout<<"Invalid command received: " << buf << endl;
                }
            }

            if (n < 0)
                cout<<"Read error"<<endl;

            exit(0);
        }
        //close socket of the server
        close(connfd);
    }
}