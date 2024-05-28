#include "../include/ftp_commands.h"

// Create a new SSL_CTX object
SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());

void handle_ls_command(SSL *ssl) {
    // Реализация команды ls

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

        while (total < bytesleft) {
            n = SSL_write(ssl, temp + total, bytesleft - total);
            if (n == -1) { break; }
            total += n;
            bytesleft -= n;
        }
    }

    // Отправляем специальный символ, указывающий на конец передачи данных
    SSL_write(ssl, "\0", 1);
    pclose(in);
}

void handle_pwd_command(SSL *ssl) {
    // Реализация команды pwd
    std::string curr_dir = get_current_dir();
    cout << "pwd" << curr_dir << endl;
    SSL_write(ssl, curr_dir.c_str(), curr_dir.length());
}

void handle_cd_command(SSL *ssl, const char *directory) {
    // Реализация команды cd
    cout << "Given directory: " << directory << endl;

    if (strcmp(directory, "..") == 0) {
        if (chdir("..") < 0) {
            SSL_write(ssl, "2", 2);
        } else {
            cout << "cha .." << endl;
            char cwd[1024];
            std::string new_dir = root_directory;
            std::size_t found = new_dir.find_last_of("/");

            if (found != std::string::npos) {
                new_dir.erase(found);
            }

            root_directory = new_dir;
            cout << root_directory << endl;

            SSL_write(ssl, "1", 2);
        }
    } else {
        std::string new_dir = root_directory + "/" + directory;
        if (chdir(new_dir.c_str()) < 0) {
            cout << "why nou" << endl;
            SSL_write(ssl, "2", 2);
        } else {
            root_directory = new_dir;
            cout << root_directory << endl;
            SSL_write(ssl, "1", 2);
        }
    }
}

void handle_put_command(SSL *ssl, int data_port, const char *filename) {
    // Реализация команды put
    char port[MAXLINE], buffer[MAXLINE], char_num_blks[MAXLINE], char_num_last_blk[MAXLINE], check[MAXLINE];
    int num_blks, num_last_blk, i;
    FILE *fp;

    cout << "Filename given is: " << filename << endl;
    SSL_write(ssl, port, MAXLINE); // отправка порта соединения с данными клиенту
    cout << "Handshake" << endl;
    if ((fp = fopen(filename, "r")) != NULL) {
        // размер файла
        SSL_write(ssl, "1", 1024);
        fseek(fp, 0, SEEK_END);
        long lSize = ftell(fp);
        rewind(fp);
        num_blks = lSize / MAXLINE;
        num_last_blk = lSize % MAXLINE;
        sprintf(char_num_blks, "%d", num_blks);
        SSL_write(ssl, char_num_blks, MAXLINE);

        for (i = 0; i < num_blks; i++) {
            fread(buffer, sizeof(char), MAXLINE, fp);
            SSL_write(ssl, buffer, MAXLINE);
        }
        sprintf(char_num_last_blk, "%d", num_last_blk);
        SSL_write(ssl, char_num_last_blk, MAXLINE);
        if (num_last_blk > 0) {
            fread(buffer, sizeof(char), num_last_blk, fp);
            SSL_write(ssl, buffer, MAXLINE);
        }
        cout << "File upload done" << endl;
        fclose(fp);
    } else {
        cout << "File doesn't exist" << endl;
        SSL_write(ssl, "2", 1024);
    }
}

void handle_get_command(SSL *ssl, int data_port, const char *filename) {
    // Реализация команды get
    char port[MAXLINE], buffer[MAXLINE], char_num_blks[MAXLINE], char_num_last_blk[MAXLINE], message[MAXLINE];
    int datasock, lSize, num_blks, num_last_blk, i;
    FILE *fp;

    cout << "Filename to get: " << filename << endl;
    data_port = data_port + 1;
    sprintf(port, "%d", data_port);
    SSL_write(ssl, port, MAXLINE);                    //sending port no. to client
    SSL_read(ssl, message, MAXLINE);

    cout << "go" << endl;
    if (strcmp("1", message) == 0) {
        if ((fp = fopen(filename, "w")) != NULL) {
            cout << "here" << endl;
            SSL_read(ssl, char_num_blks, MAXLINE);
            num_blks = atoi(char_num_blks);
            for (i = 0; i < num_blks; i++) {
                SSL_read(ssl, buffer, MAXLINE); // Используем datasocket для чтения данных
                fwrite(buffer, sizeof(char), MAXLINE, fp);
            }
            SSL_read(ssl, char_num_last_blk, MAXLINE);
            num_last_blk = atoi(char_num_last_blk);
            if (num_last_blk > 0) {
                SSL_read(ssl, buffer, MAXLINE); // Используем datasocket для чтения данных
                fwrite(buffer, sizeof(char), num_last_blk, fp);
            }
            fclose(fp);
        } else {
            cout << "Error in creating file" << endl;
        }
    }
}