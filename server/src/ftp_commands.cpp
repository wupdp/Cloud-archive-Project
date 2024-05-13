#include "../include/ftp_commands.h"

// Create a new SSL_CTX object
SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());

void handle_ls_command(SSL* ssl) {
    // Реализация команды ls
    std::string curr_dir = get_current_dir();

    FILE *in;
    char temp[MAXLINE];

    // Отправляем список файлов клиенту
    if (!(in = popen("ls", "r"))) {
        std::cout << "error" << std::endl;
    }

    // Читаем и отправляем вывод команды ls построчно
    while (fgets(temp, sizeof(temp), in) != NULL) {
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

    // Отправляем специальный символ, указывающий на конец передачи данных
    SSL_write(ssl, "\0", 1);

    pclose(in);
    chdir(curr_dir.c_str());
}

void handle_pwd_command(SSL* ssl) {
    // Реализация команды pwd
    std::string curr_dir = get_current_dir();
    SSL_write(ssl, curr_dir.c_str(), curr_dir.length()+1);
}

void handle_cd_command(SSL* ssl, const char* directory) {
    std::string current_dir = get_current_dir();
    std::string new_dir = directory;

    // Если переданный путь - "..", то перейдем на уровень выше
    if (new_dir == "..") {
        // Проверяем, что мы не находимся в корневой директории
        if (current_dir != "/") {
            size_t found = current_dir.find_last_of("/\\");
            if (found != std::string::npos) {
                current_dir = current_dir.substr(0, found);
                if (chdir(current_dir.c_str()) != -1) {
                    SSL_write(ssl, "1", MAXLINE);
                    return;
                }
            }
        }
    } else {
        // Иначе объединим текущий путь и переданный путь
        new_dir = current_dir + "/" + directory;
        if (chdir(new_dir.c_str()) != -1) {
            SSL_write(ssl, "1", MAXLINE);
            return;
        }
    }

    // Если chdir не удалось или пытаемся вернуться из корневой директории, отправим сообщение об ошибке
    SSL_write(ssl, "0", MAXLINE);
}

void handle_put_command(SSL* ssl, int data_port, const char* filename) {
    // Реализация команды put
    if(access(filename,F_OK)!=-1){
        char port[MAXLINE], buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE];
        int num_blks,num_last_blk,i;
        FILE *fp;
        data_port=data_port+1;
        sprintf(port,"%d",data_port);
        int datasock = create_socket(data_port); // создание сокета для передачи данных
        SSL_write(ssl, port,MAXLINE); // отправка порта соединения с данными клиенту
        datasock = accept_conn(datasock); // принятие соединения

        // Создание новой SSL структуры для соединения
        SSL *ssl_datasock = SSL_new(ctx);
        // Ассоциирование сокета с SSL структурой
        SSL_set_fd(ssl_datasock, datasock);
        // Выполнение SSL рукопожатия
        if (SSL_accept(ssl_datasock) <= 0) {
            // Обработка неудачного рукопожатия
            std::cerr << "SSL handshake failed" << std::endl;
            return;
        }

        if ((fp=fopen(filename,"r"))!=NULL)
        {
            // размер файла
            fseek (fp , 0 , SEEK_END);
            long lSize = ftell (fp);
            rewind (fp);
            num_blks = lSize/MAXLINE;
            num_last_blk = lSize%MAXLINE;
            sprintf(char_num_blks,"%d",num_blks);
            SSL_write(ssl, char_num_blks, MAXLINE);

            for(i= 0; i < num_blks; i++) {
                fread (buffer,sizeof(char),MAXLINE,fp);
                SSL_write(ssl_datasock, buffer, MAXLINE);
            }
            sprintf(char_num_last_blk,"%d",num_last_blk);
            SSL_write(ssl, char_num_last_blk, MAXLINE);
            if (num_last_blk > 0) {
                fread (buffer,sizeof(char),num_last_blk,fp);
                SSL_write(ssl_datasock, buffer, MAXLINE);
            }
            fclose(fp);
        }
        else{
            SSL_write(ssl,"0",MAXLINE);
        }

        // Закрытие SSL соединения и освобождение ресурсов
        SSL_shutdown(ssl_datasock);
        SSL_free(ssl_datasock);
        close(datasock);
    } else {
        std::cerr<<"File " << filename << " does not exist"<<std::endl;
    }
}

void handle_get_command(SSL* ssl, int data_port, const char* filename) {
    // Реализация команды get
    if(access(filename,F_OK)!=-1){
        char port[MAXLINE],buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE];
        int datasock,lSize,num_blks,num_last_blk,i;
        FILE *fp;
        data_port=data_port+1;
        sprintf(port,"%d",data_port);
        datasock=create_socket(data_port);				//creating socket for data connection
        SSL_write(ssl, port,MAXLINE);					//sending port no. to client
        datasock=accept_conn(datasock);					//accepting connnection by client

        // Создание новой SSL структуры для соединения
        SSL *ssl_datasock = SSL_new(ctx);
        // Ассоциирование сокета с SSL структурой
        SSL_set_fd(ssl_datasock, datasock);
        // Выполнение SSL рукопожатия
        if (SSL_accept(ssl_datasock) <= 0) {
            // Обработка неудачного рукопожатия
            std::cerr << "SSL handshake failed" << std::endl;
            return;
        }

        if ((fp=fopen(filename,"w"))!=NULL)
        {
            SSL_read(ssl, char_num_blks, MAXLINE);
            num_blks=atoi(char_num_blks);
            for(i= 0; i < num_blks; i++) {
                SSL_read(ssl_datasock, buffer, MAXLINE); // Используем datasocket для чтения данных
                fwrite(buffer,sizeof(char),MAXLINE,fp);
            }
            SSL_read(ssl, char_num_last_blk, MAXLINE);
            num_last_blk=atoi(char_num_last_blk);
            if (num_last_blk > 0) {
                SSL_read(ssl_datasock, buffer, MAXLINE); // Используем datasocket для чтения данных
                fwrite(buffer,sizeof(char),num_last_blk,fp);
            }
            fclose(fp);
        }
        else{
            SSL_write(ssl,"0",MAXLINE);
        }

        // Закрытие SSL соединения и освобождение ресурсов
        SSL_shutdown(ssl_datasock);
        SSL_free(ssl_datasock);
        close(datasock);
    } else {
        std::cerr<<"File " << filename << " does not exists"<<std::endl;
    }
}
