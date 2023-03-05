
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define SERVER_PORT  12345


int main(int argc, char *argv[]) {
    FILE *file;
    char *filename;
    char buf[64 * 1024];
    int rc;
    int sockfd;
    char *ip;
    struct sockaddr_in addr;

    if (argc <= 2) {
        printf("Не введен IP или имя файла\n");
        return 1;
    }
    filename = argv[2];
    ip = argv[1];


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }


    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        printf("ошибка inet_pton\n");
        close(sockfd);
        return 1;
    }


    rc = connect(sockfd,
                 (struct sockaddr *) &addr,
                 sizeof(struct sockaddr_in));
    if (rc < 0) {
        perror("Подключение");
        close(sockfd);
        return 1;
    }
    printf("Подключение завершено\n");

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Открытие файла");
        fclose(file);
        close(sockfd);
        return 1;
    }

    uint32_t file_name_len = strlen(filename);
// размер имени файла
    rc = send(sockfd, &file_name_len, 4, 0);
    if (rc < 0) {
        perror("Размер имени файла");
        fclose(file);
        close(sockfd);
        return 1;
    }

//размер файла
    fseek(file, 0L, SEEK_END);

    uint32_t file_size = ftell(file);

    fseek(file, 0L, SEEK_SET);
    printf("Длина файла %d\n", file_size);

    if (file_size < 0) {
        perror("Ошибка");
        fclose(file);
        close(sockfd);
        return 1;
    }
    send(sockfd, &file_size, 4, 0);

    rc = send(sockfd, filename, file_name_len, 0);// имя файла
    if (rc < 0) {
        perror("Имя файла");
        fclose(file);
        close(sockfd);
        return 1;
    }

//файл
    int tail_size = file_size % sizeof(buf);
    int y = file_size / sizeof(buf);

    for (int i = 0; i < y; i++) {
        fread(buf, sizeof(buf), 1, file);
        rc = send(sockfd, buf, sizeof(buf), 0);
        if (rc < 0) {
            perror("Отправка");
            fclose(file);
            close(sockfd);
            return 1;
        }

    }
    if (tail_size > 0) {
        fread(buf, tail_size, 1, file);
        rc = send(sockfd, buf, tail_size, 0);
        if (rc < 0) {
            perror("Отправка");
            fclose(file);
            close(sockfd);
            return 1;
        }
    }

    fclose(file);

    close(sockfd);
    return 0;
}



