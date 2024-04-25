#include "../include/ftp_commands.h"

void handle_ls_command(int connfd) {
    // Реализация команды ls
    std::string curr_dir = get_current_dir();
    chdir(root_directory.c_str());

    FILE *in;
    char temp[MAXLINE];
    int datasock;
    struct sockaddr_in data_addr;
    socklen_t data_len = sizeof(data_addr);

    // Создаем сокет для передачи данных
    datasock = socket(AF_INET, SOCK_STREAM, 0);
    if (datasock < 0) {
        std::cerr << "Problem in creating the data socket" << std::endl;
        return;
    }

    // Привязываем сокет к любому свободному порту
    memset(&data_addr, 0, sizeof(data_addr));
    data_addr.sin_family = AF_INET;
    data_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    data_addr.sin_port = 0;
    if (bind(datasock, (struct sockaddr *) &data_addr, sizeof(data_addr)) < 0) {
        std::cerr << "Problem in binding the data socket" << std::endl;
        close(datasock);
        return;
    }

    // Получаем номер выбранного порта
    if (getsockname(datasock, (struct sockaddr *) &data_addr, &data_len) < 0) {
        std::cerr << "Problem in getting the data socket name" << std::endl;
        close(datasock);
        return;
    }
    int data_port = ntohs(data_addr.sin_port);

    // Отправляем номер порта клиенту
    char port[MAXLINE];
    sprintf(port, "%d", data_port);
    send(connfd, port, MAXLINE, 0);

    // Принимаем соединение от клиента
    if (listen(datasock, 1) < 0) {
        std::cerr << "Problem in listening on the data socket" << std::endl;
        close(datasock);
        return;
    }
    int dataconnfd = accept(datasock, NULL, NULL);
    if (dataconnfd < 0) {
        std::cerr << "Problem in accepting the data socket" << std::endl;
        close(datasock);
        return;
    }

    // Отправляем список файлов клиенту
    if (!(in = popen("ls", "r"))) {
        std::cout << "error" << std::endl;
    }
    while (fgets(temp, sizeof(temp), in) != NULL) {
        send(dataconnfd, "1", MAXLINE, 0);
        send(dataconnfd, temp, MAXLINE, 0);
    }
    send(dataconnfd, "0", MAXLINE, 0);
    pclose(in);

    // Закрываем сокеты
    close(dataconnfd);
    close(datasock);
    chdir(curr_dir.c_str());
}

void handle_pwd_command(int connfd) {
    // Реализация команды pwd
    std::string curr_dir = get_current_dir();
    send(connfd, curr_dir.c_str(), MAXLINE, 0);
}

void handle_cd_command(int connfd, const char* directory) {
    // Реализация команды cd
    std::string new_dir;
    if (strcmp(directory, "..") == 0) {
        size_t pos = root_directory.find_last_of("/");
        if (pos != std::string::npos) {
            new_dir = root_directory.substr(0, pos);
        }
    } else {
        new_dir = root_directory + "/" + directory;
    }

//    // Проверяем, что новый путь находится внутри root_directory
//    if (new_dir.length() < root_directory.length()) {
//        std::cerr << "Cannot change directory beyond root directory" << std::endl;
//        send(connfd, "0", MAXLINE, 0);
//        return;
//    }


    if(chdir(new_dir.c_str())<0){
        send(connfd,"0",MAXLINE,0);
    }
    else{
        root_directory = new_dir;
        send(connfd,"1",MAXLINE,0);
    }
}

void handle_put_command(int connfd, int data_port, const char* filename) {
    // Реализация команды put
    if(access(filename,F_OK)!=-1){
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
    } else {
        std::cerr<<"File " << filename << " does not exist"<<std::endl;
    }
}

void handle_get_command(int connfd, int data_port, const char* filename) {
    // Реализация команды get
    if(access(filename,F_OK)==-1){
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
    } else {
        std::cerr<<"File " << filename << " does not exists"<<std::endl;
    }
}