#include "../include/ftp_commands.h"

// Create a new SSL_CTX object
SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());

void handle_ls_command(SSL* ssl) {
    // Реализация команды ls
    std::string curr_dir = get_current_dir();
    chdir(root_directory.c_str());

    FILE *in;
    char temp[MAXLINE];

    cout << "ls start" << endl;
    cout << curr_dir << endl;
    // Отправляем список файлов клиенту
    if (!(in = popen("ls", "r"))) {
        std::cout << "error" << std::endl;
    }

    // Читаем и отправляем вывод команды ls построчно
    while (fgets(temp, sizeof(temp), in) != NULL) {
        cout << temp << endl;
        //SSL_write(ssl, "1", 2);
        int total = 0;        // how many bytes we've sent
        int bytesleft = strlen(temp); // how many we have left to send
        int n;

        while(total < strlen(temp)) {
            n = SSL_write(ssl, temp + total, bytesleft);
            if (n == -1) { break; }
            total += n;
            bytesleft -= n;
        }
    }

    cout << "fin" << endl;
    // Отправляем специальный символ, указывающий на конец передачи данных
    SSL_write(ssl, "2", 1);

    pclose(in);
    chdir(curr_dir.c_str());
}

void handle_pwd_command(SSL* ssl) {
    // Реализация команды pwd
    std::string curr_dir = get_current_dir();
    cout << "pwd" << curr_dir << endl;
    SSL_write(ssl, curr_dir.c_str(), curr_dir.length());
}

void handle_cd_command(SSL* ssl, const char* directory) {
    // Реализация команды cd
    cout << "Given directory: " << directory << endl;
   
    if(strcmp(directory, "..") == 0) {
        if(chdir("..")<0){
            SSL_write(ssl, "2", 2);
        }
        else{
            cout << "cha .." << endl;
            char cwd[1024];
            // if (getcwd(cwd, sizeof(cwd)) != NULL) {
            //     root_directory = std::string(cwd);
            // }
            std::string new_dir = root_directory;
            std::size_t found = new_dir.find_last_of("/");

            if (found != std::string::npos) {
                new_dir.erase(found);
            }

            root_directory = new_dir;
            cout << root_directory << endl;    

            SSL_write(ssl, "1", 2);
        }
    }
    else {
        std::string new_dir = root_directory + "/" + directory;
        if(chdir(new_dir.c_str())<0){
            cout << "why nou" << endl;
            SSL_write(ssl, "2", 2);
        }
        else{
           /* char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                root_directory = std::string(cwd);
            } */
            root_directory = new_dir;
            cout << root_directory << endl;
            SSL_write(ssl, "1", 2);
        }
    }
}

void handle_put_command(SSL* ssl, int data_port, const char* filename) {
    // Реализация команды put
    //if(access(filename,F_OK)!=-1){
        char port[MAXLINE], buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE], check[MAXLINE];
        int num_blks,num_last_blk,i;
        FILE *fp;
    
        cout << "Filename given is: " << filename << endl;
        //data_port=data_port+1;
        
       // sprintf(port,"%d",data_port);
       // int datasock = create_socket(data_port); // создание сокета для передачи данных
        SSL_write(ssl, port, MAXLINE); // отправка порта соединения с данными клиенту
       // datasock = accept_conn(datasock); // принятие соединения
        
        /*cout << "datasock" << endl;
        // Создание новой SSL структуры для соединения
        SSL *ssl_datasock = SSL_new(ctx);
        cout << "ssl new" << endl;
        // Ассоциирование сокета с SSL структурой
        SSL_set_fd(ssl_datasock, datasock);
        cout << "ssl set fd" << endl;
        // Выполнение SSL рукопожатия
        if (SSL_accept(ssl_datasock) <= 0) {
            // Обработка неудачного рукопожатия
            std::cerr << "SSL handshake failed" << std::endl;
            return;
        }*/

        cout << "Handshake" << endl;
        if ((fp=fopen(filename,"r"))!=NULL)
        {
            // размер файла
            SSL_write(ssl, "1", 1024);
            fseek (fp , 0 , SEEK_END);
            long lSize = ftell (fp);
            rewind (fp);
            num_blks = lSize/MAXLINE;
            num_last_blk = lSize%MAXLINE;
            sprintf(char_num_blks,"%d",num_blks);
            SSL_write(ssl, char_num_blks, MAXLINE);

            for(i= 0; i < num_blks; i++) {
                fread (buffer,sizeof(char),MAXLINE,fp);
                SSL_write(ssl, buffer, MAXLINE);
            }
            sprintf(char_num_last_blk,"%d",num_last_blk);
            SSL_write(ssl, char_num_last_blk, MAXLINE);
            if (num_last_blk > 0) {
                fread (buffer,sizeof(char),num_last_blk,fp);
                SSL_write(ssl, buffer, MAXLINE);
            }
            cout << "File upload done" << endl;
            fclose(fp);
        }
        else{
            cout << "File doesn't exist" << endl;
            SSL_write(ssl,"2",1024);
        }

        // Закрытие SSL соединения и освобождение ресурсов
        //SSL_shutdown(ssl_datasock);
        //SSL_free(ssl_datasock);
      //  close(datasock);
  //  } else {
   //     std::cerr<<"File " << filename << " does not exist"<<std::endl;
   // }
}

void handle_get_command(SSL* ssl, int data_port, const char* filename) {
    // Реализация команды get
    //if(access(filename,F_OK)!=-1){
        char port[MAXLINE],buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE], message[MAXLINE];
        int datasock,lSize,num_blks,num_last_blk,i;
        FILE *fp;

        cout << "Filename to get: " << filename << endl;
        data_port=data_port+1;
        sprintf(port,"%d",data_port);
        //datasock=create_socket(data_port);				//creating socket for data connection
        SSL_write(ssl, port,MAXLINE);					//sending port no. to client
       // data_port = atoi(port);
        //datasock=accept_conn(datasock);					//accepting connnection by client

        // Создание новой SSL структуры для соединения
       // SSL *ssl_datasock = SSL_new(ctx);
        // Ассоциирование сокета с SSL структурой
       // SSL_set_fd(ssl_datasock, datasock);
        // Выполнение SSL рукопожатия
        //if (SSL_accept(ssl_datasock) <= 0) {
            // Обработка неудачного рукопожатия
        //    std::cerr << "SSL handshake failed" << std::endl;
        //    return;
        //}

        SSL_read(ssl, message, MAXLINE);

        cout << "go" << endl;
        if (strcmp("1", message) == 0) {
            if ((fp=fopen(filename,"w"))!=NULL)
            {
                cout << "here" << endl;
                SSL_read(ssl, char_num_blks, MAXLINE);
                num_blks=atoi(char_num_blks);
                for(i= 0; i < num_blks; i++) {
                    SSL_read(ssl, buffer, MAXLINE); // Используем datasocket для чтения данных
                    fwrite(buffer,sizeof(char),MAXLINE,fp);
                }
                SSL_read(ssl, char_num_last_blk, MAXLINE);
                num_last_blk=atoi(char_num_last_blk);
                if (num_last_blk > 0) {
                    SSL_read(ssl, buffer, MAXLINE); // Используем datasocket для чтения данных
                    fwrite(buffer,sizeof(char),num_last_blk,fp);
                }
                fclose(fp);
            }
            else{
                cout << "Error in creating file" << endl;
            }
        }

        // Закрытие SSL соединения и освобождение ресурсов
     //   SSL_shutdown(ssl_datasock);
     //   SSL_free(ssl_datasock);
      //  close(datasock);
    //} else {
    //    std::cerr<<"File " << filename << " does not exists"<<std::endl;
    //}
}