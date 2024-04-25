#include "../include/socket.h"

int create_socket(int port)
{
    int sockfd;
    struct sockaddr_in servaddr;

    //Create a socket for the client
    //If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        std::cerr<<"Problem in creating the socket"<<std::endl;
        return -1; // return error code
    }

    //Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= htonl(INADDR_ANY);
    servaddr.sin_port =  htons(port); //convert to big-endian order

    //Bind the socket
    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        std::cerr<<"Problem in binding the socket"<<std::endl;
        return -2; // return error code
    }

    //Listen on the socket
    if (listen(sockfd, 1)<0) {
        std::cerr<<"Problem in listening on the socket"<<std::endl;
        return -3; // return error code
    }

    return sockfd;
}

int accept_conn(int sock) {
    int dataconnfd;
    socklen_t dataclilen;
    struct sockaddr_in datacliaddr;

    dataclilen = sizeof(datacliaddr);
    if ((dataconnfd = accept (sock, (struct sockaddr *) &datacliaddr, &dataclilen)) <0) {
        std::cerr<<"Problem in accepting the data socket"<<std::endl;
        exit(2);
    }

    return dataconnfd;
}