#include "../include/server.h"

std::string root_directory;

void run_server(int argc, char **argv) {

    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    if (argc !=3) {
        std::cerr<<"Usage: ./server <port number> <root directory>"<<std::endl;
        exit(1);
    }

    if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        std::cerr<<"Problem in creating the socket"<<std::endl;
        exit(2);
    }

    root_directory = argv[2];

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(atoi(argv[1])<=1024){
        std::cerr<<"Port number must be greater than 1024"<<std::endl;
        exit(2);
    }
    servaddr.sin_port = htons(atoi(argv[1]));

    bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen (listenfd, LISTENQ);

    std::cout<<"Server running...waiting for connections."<<std::endl;

    for ( ; ; ) {

        clilen = sizeof(cliaddr);
        connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

        std::cout<<"Received request..."<<std::endl;

        if ( (childpid = fork ()) == 0 ) {

            std::cout<<"Child created for dealing with client requests"<<std::endl;

            close (listenfd);
            int data_port=1024;

            while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
                std::cout<<"String received from client: "<<buf;
                if (strncmp("ls", buf, 2) == 0) {
                    handle_ls_command(connfd);
                } else if (strncmp("pwd", buf, 3) == 0) {
                    handle_pwd_command(connfd);
                } else if (strncmp("cd", buf, 2) == 0) {
                    handle_cd_command(connfd, buf + 3);
                } else if (strncmp("put", buf, 3) == 0) {
                    handle_put_command(connfd, data_port, buf + 4);
                } else if (strncmp("get", buf, 3) == 0) {
                    handle_get_command(connfd, data_port, buf + 4);
                } else {
                    std::cout << "Invalid command received: " << buf << std::endl;
                }
            }

            if (n < 0)
                std::cout<<"Read error"<<std::endl;

            exit(0);
        }
        close(connfd);
    }
    close(listenfd);
}