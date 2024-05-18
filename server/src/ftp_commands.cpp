#include "../include/ftp_commands.h"

// Create a new SSL_CTX object
SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());

void handle_ls_command(SSL *ssl) {
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

        while (total < strlen(temp)) {
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

void handle_pwd_command(SSL *ssl) {
    // Реализация команды pwd
    std::string curr_dir = get_current_dir();
    SSL_write(ssl, curr_dir.c_str(), curr_dir.length() + 1);
}

void handle_cd_command(SSL *ssl, const char *directory) {
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

void handle_get_command(SSL *ssl, int *data_port, const char *filename) {
    // Проверяем существование файла на сервере
    if (access(filename, F_OK) != -1) {
        SSL_write(ssl, "1", strlen("1") + 1);

        char port[MAXLINE], buffer[MAXLINE], char_num_blks[MAXLINE], char_num_last_blk[MAXLINE];
        int datasock, lSize, num_blks, num_last_blk, i;
        FILE *fp;

        // Создаем сокет для передачи данных
        (*data_port)++;
        sprintf(port, "%d", (*data_port));
        datasock = create_socket((*data_port));
        SSL_write(ssl, port, strlen(port) + 1); // Отправляем порт клиенту
        datasock = accept_conn(datasock); // Принимаем соединение

        // Открываем файл для чтения
        if ((fp = fopen(filename, "rb")) != NULL) {
            SSL_write(ssl, "1", strlen("1") + 1);

            // Определяем размер файла
            fseek(fp, 0, SEEK_END);
            lSize = ftell(fp);
            rewind(fp);
            num_blks = lSize / MAXLINE;
            num_last_blk = lSize % MAXLINE;
            sprintf(char_num_blks, "%d", num_blks);
            // Отправляем количество блоков клиенту
            SSL_write(ssl, char_num_blks, strlen(char_num_blks) + 1);

            // Читаем и отправляем содержимое файла по блокам
            for (i = 0; i < num_blks; i++) {
                fread(buffer, sizeof(char), MAXLINE, fp);
                send(datasock, buffer, MAXLINE, 0);
            }

            // Отправляем последний блок данных
            sprintf(char_num_last_blk, "%d", num_last_blk);
            SSL_write(ssl, char_num_last_blk, strlen(char_num_last_blk) + 1);

            if (num_last_blk > 0) {
                fread(buffer, sizeof(char), num_last_blk, fp);
                send(datasock, buffer, num_last_blk, 0);
            }

            fclose(fp);
        } else {
            SSL_write(ssl, "0", strlen("0") + 1); // Отправляем код ошибки (файл не найден)
        }

        close(datasock);
    } else {
        SSL_write(ssl, "0", strlen("0") + 1);
        std::cerr << "File <" << filename << "> does not exist" << std::endl;
    }
}

void handle_put_command(SSL *ssl, int *data_port, const char *filename) {
    char port[MAXLINE], buffer[MAXLINE], char_num_blks[MAXLINE], char_num_last_blk[MAXLINE];
    int num_blks, num_last_blk, i;
    char message[BUFFER_SIZE];
    FILE *fp;

    // Создание нового сокета для передачи данных
    (*data_port)++;
    sprintf(port, "%d", (*data_port));
    int datasock = create_socket((*data_port));
    if (datasock < 0) {
        std::cerr << "Failed to create data socket" << std::endl;
        SSL_write(ssl, "ERROR", strlen("ERROR") + 1);
        return;
    } else {
        SSL_write(ssl, "SUCCESS", strlen("SUCCESS") + 1);
    }

    SSL_write(ssl, port, strlen(port) + 1); // Отправляем порт клиенту

    SSL_read(ssl, message, sizeof(message));
    if (strcmp(message, "ERROR") == 0) {
        std::cerr << "Client cannot create data socket" << std::endl;
        return;
    }

    datasock = accept_conn(datasock);
    if (datasock < 0) {
        std::cerr << "Failed to accept data connection" << std::endl;
        SSL_write(ssl, "ERROR", strlen("ERROR") + 1);
        return;
    } else {
        SSL_write(ssl, "SUCCESS", strlen("SUCCESS") + 1);
    }

    SSL_read(ssl, message, sizeof(message));
    if (strcmp("1", message) == 0) {
        fp = fopen(filename, "wb");
        if (fp != NULL) {
            // Уведомляем клиент, что готовы принимать данные
            SSL_write(ssl, "READY", strlen("READY") + 1);

            SSL_read(ssl, char_num_blks, sizeof(char_num_blks));
            num_blks = atoi(char_num_blks);

            // Чтение и запись блоков данных в файл
            for (i = 0; i < num_blks; i++) {
                int bytes_received = recv(datasock, buffer, sizeof(buffer), 0);
                fwrite(buffer, sizeof(char), bytes_received, fp);
            }

            // Чтение размера последнего блока данных
            SSL_read(ssl, char_num_last_blk, sizeof(char_num_last_blk));
            num_last_blk = atoi(char_num_last_blk);
            if (num_last_blk > 0) {
                int bytes_received = recv(datasock, buffer, num_last_blk, 0);
                fwrite(buffer, sizeof(char), bytes_received, fp);
            }
            fclose(fp);
        } else {
            std::cerr << "Error creating file on server side" << std::endl;
            SSL_write(ssl, "ERROR", strlen("ERROR") + 1);
            return;
        }
    } else {
        std::cerr << "Client failed to initialize file transfer" << std::endl;
        SSL_write(ssl, "ERROR", strlen("ERROR") + 1);
    }

    close(datasock);
}
