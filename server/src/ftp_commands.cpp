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
    std::string dir_str = directory;  // Преобразуем const char* в std::string
    // Обработка перехода на уровень выше
    if (dir_str == "..") {
        char resolved_path[PATH_MAX];
        if (realpath(".", resolved_path) != NULL) {
            std::string current_dir = resolved_path;
            size_t last_slash = current_dir.find_last_of("/");
            if (last_slash != std::string::npos) {
                std::string parent_dir = current_dir.substr(0, last_slash);
                if (!parent_dir.empty() && chdir(parent_dir.c_str()) != -1) {
                    SSL_write(ssl, "1", strlen("1"));
                    return;
                }
            }
        }
    } else {
        // Обработка перехода в другую директорию
        if (chdir(dir_str.c_str()) != -1) {
            SSL_write(ssl, "1", strlen("1"));
            return;
        }
    }
    // Отправка сообщения об ошибке при неудачном переходе
    SSL_write(ssl, "0", strlen("0"));
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
    // Проверяем существование файла на сервере
    if (access(filename, F_OK) != -1) {
        char port[MAXLINE], buffer[MAXLINE], char_num_blks[MAXLINE], char_num_last_blk[MAXLINE];
        int datasock, lSize, num_blks, num_last_blk, i;
        FILE *fp;

        // Создаем сокет для передачи данных
        data_port = data_port + 1;
        sprintf(port, "%d", data_port);
        datasock = create_socket(data_port);
        SSL_write(ssl, port, MAXLINE); // Отправляем порт клиенту
        datasock = accept_conn(datasock); // Принимаем соединение

        // Создаем SSL соединение для данных
        SSL *ssl_datasock = SSL_new(ctx);
        SSL_set_fd(ssl_datasock, datasock);
        if (SSL_accept(ssl_datasock) <= 0) {
            std::cerr << "SSL handshake failed" << std::endl;
            return;
        }

        // Открываем файл для чтения
        if ((fp = fopen(filename, "rb")) != NULL) {
            // Определяем размер файла
            fseek(fp, 0, SEEK_END);
            lSize = ftell(fp);
            rewind(fp);
            num_blks = lSize / MAXLINE;
            num_last_blk = lSize % MAXLINE;
            sprintf(char_num_blks, "%d", num_blks);
            SSL_write(ssl, char_num_blks, MAXLINE); // Отправляем количество блоков клиенту

            // Читаем и отправляем содержимое файла по блокам
            for (i = 0; i < num_blks; i++) {
                fread(buffer, sizeof(char), MAXLINE, fp);
                SSL_write(ssl_datasock, buffer, MAXLINE); // Используем datasocket для отправки данных
            }

            // Отправляем последний блок данных
            sprintf(char_num_last_blk, "%d", num_last_blk);
            SSL_write(ssl, char_num_last_blk, MAXLINE);
            if (num_last_blk > 0) {
                fread(buffer, sizeof(char), num_last_blk, fp);
                SSL_write(ssl_datasock, buffer, num_last_blk); // Используем datasocket для отправки данных
            }
            fclose(fp);
        } else {
            SSL_write(ssl, "0", MAXLINE); // Отправляем код ошибки (файл не найден)
        }

        // Закрываем SSL соединение для данных и освобождаем ресурсы
        SSL_shutdown(ssl_datasock);
        SSL_free(ssl_datasock);
        close(datasock);
    } else {
        std::cerr << "File " << filename << " does not exist" << std::endl;
    }
}
