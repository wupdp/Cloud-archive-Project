#include "client.h"

using namespace std;

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    //check of the arguments
    if (argc < 3) {
        cerr<<"Usage: ./ftclient <IP address of the server> <port number>"<<endl;
        exit(1);
    }

    int server_port = atoi(argv[2]);

    //check port number
    if (server_port < 1) {
        printf("Incorrect FTP port!\n");
        exit(2);
    }

    //check hostname (IP addres)
    char* server_ip = argv[1];
    struct hostent *host_name;
    host_name = gethostbyname(server_ip);
    if (host_name == NULL) {
        printf("Incorrect FTP hostname (IP addres)!\n");
        exit(3);
    }
    //cout << server_ip << endl;

    //Create a socket for the client
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        cerr<<"Problem in creating the socket"<<endl;
        exit(4);
    }

    //Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= inet_addr(argv[1]);
    servaddr.sin_port =  htons(server_port); //convert to big-endian order

    //Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        cerr<<"Problem in connecting to the server"<<endl;
        exit(5);
    }

    menu_client();
    struct command cmd;
    while (1) {
        if (read_command_client(buffer, sizeof buffer, &cmd) < 0) {
            printf("Invalid command\n");
            continue;
        }

        send(sockfd, cmd.serv_code, BUFFER_SIZE, 0);

        if (strcmp(cmd.code, "QUIT") == 0) {
            printf("Quit command...\n");
            break;
        }
        else if (strcmp(cmd.code, "LS") == 0) {
                cout << "Executing client_show_list..." << endl;
                client_show_list(sockfd, server_ip);
        }
        else if (strcmp(cmd.code, "GET") == 0) {
                cout << "Getting file..." << endl;
                client_get_file(sockfd, server_ip, cmd);
        }
        else if (strcmp(cmd.code, "PUT") == 0) {
                cout << "Putting file..." << endl;
                client_put_file(sockfd, server_ip, cmd);
        }
        else if (strcmp(cmd.code, "CD") == 0) {
                client_cd_action(sockfd, cmd);
        }
        else if (strcmp(cmd.code, "PWD") == 0) {
                client_pwd_action(sockfd);
        }

    }
    /*while (fgets(sendline, BUFFER_SIZE, stdin) != NULL) {

        send(sockfd, sendline, BUFFER_SIZE, 0);
        char *token,*dummy;
        dummy=sendline;
        token=strtok(dummy," ");

        else if (strcmp("!ls\n",sendline)==0)  {
            system("ls");
            cout<<"\n";
        }

        else if (strcmp("!pwd\n",sendline)==0)  {
            system("pwd");
        }

        else if (strcmp("!cd",token)==0)  {
            token=strtok(NULL," \n");
            cout<<"Path given is: "<<token<<endl;
            if(chdir(token)<0){
                cerr<<"Directory doesn't exist. Check path"<<endl;
            }
        }


        else if (strcmp("get",token)==0)  {
            char port[BUFFER_SIZE], buffer[BUFFER_SIZE],char_num_blks[BUFFER_SIZE],char_num_last_blk[BUFFER_SIZE],message[BUFFER_SIZE];
            int data_port,datasock,lSize,num_blks,num_last_blk,i;
            FILE *fp;
            recv(sockfd, port, BUFFER_SIZE,0);
            data_port=atoi(port);
            datasock=create_socket(data_port,argv[1]);
            token=strtok(NULL," \n");
            recv(sockfd,message,BUFFER_SIZE,0);
            if(strcmp("1",message)==0){
                if((fp=fopen(token,"w"))==NULL)
                    cout<<"Error in creating file\n";
                else
                {
                    recv(sockfd, char_num_blks, BUFFER_SIZE,0);
                    num_blks=atoi(char_num_blks);
                    for(i= 0; i < num_blks; i++) {
                        recv(datasock, buffer, BUFFER_SIZE,0);
                        fwrite(buffer,sizeof(char),BUFFER_SIZE,fp);
                        //cout<<buffer<<endl;
                    }
                    recv(sockfd, char_num_last_blk, BUFFER_SIZE,0);
                    num_last_blk=atoi(char_num_last_blk);
                    if (num_last_blk > 0) {
                        recv(datasock, buffer, BUFFER_SIZE,0);
                        fwrite(buffer,sizeof(char),num_last_blk,fp);
                        //cout<<buffer<<endl;
                    }
                    fclose(fp);
                    cout<<"File download done."<<endl;
                }
            }
            else{
                cerr<<"Error in opening file. Check filename\nUsage: put filename"<<endl;
            }
        }
        else{
            cerr<<"Error in command. Check Command"<<endl;
        }
        cout<<"ftp>";

    }*/

    printf("Thank you, buy!\n");

    close(sockfd);
    return 0;
}