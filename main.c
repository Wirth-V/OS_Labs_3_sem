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
#include <linux/limits.h>
#include <fcntl.h>
#include <ctype.h>
#include "uthash.h"

#define SERVER_PORT  12345 //этот параметр меняется

#define TRUE             1
#define FALSE            0

#define SERVER_BUFFER_SIZE (64*1024)
#define SERVER_MAX_CONN 200

struct server_context_t {
    // сокет
    int sock;
    // длина имени файла
    uint32_t file_name_len;
    // ожидаемая длина файла
    uint32_t file_len;
    // файловый дескриптор
    int fd;
    // укалатель на первый свободный байт в буфере
    char *buf_ptr;
    // количество байт, считанных в буфер прямо сейчас
    int recv_len;
    // 64К будет достаточно
    char buf[SERVER_BUFFER_SIZE];
    // состояние
    int state;
    // количество байт в буфере
    int bytes_in_buf;
    UT_hash_handle hh;         /* makes this structure hashable */
};

void init_server_context(struct server_context_t *ctx) {
    ctx->file_name_len = 0;
    ctx->file_len = 0;
    ctx->buf_ptr = ctx->buf;
    ctx->state = 0;
    ctx->bytes_in_buf = 0;
    ctx->fd = -1;
    ctx->recv_len = 0;
}

void drop_some_bytes_in_server_context(struct server_context_t *ctx, int bytes) {
    if(bytes > ctx->bytes_in_buf)
        bytes = ctx->bytes_in_buf;
    if(bytes < 0)
        return;
    ctx->bytes_in_buf -= bytes;
    if(ctx->bytes_in_buf)
        memmove(ctx->buf, ctx->buf + bytes, ctx->bytes_in_buf);
    ctx->buf_ptr -= bytes;
}

void copy_file_name(struct server_context_t *ctx, char *fname) {
    
    for(int i=0; i<ctx->file_name_len; i++) {
        if(isprint(ctx->buf[i]) && ctx->buf[i] != '/')
            fname[i] = ctx->buf[i];
        else
            fname[i] = '_';
    }
    fname[ctx->file_name_len] = 0;
}

int create_output_file(struct server_context_t *ctx) {
    char *fname = malloc(ctx->file_name_len + 1);
    copy_file_name(ctx, fname);
    printf("создаем файл %s\n", fname);
    ctx->fd = creat(fname, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    free(fname);
    if(ctx->fd < 0) {
        perror("ошибка создания файла:");
        return 1;
    }
    drop_some_bytes_in_server_context(ctx, ctx->file_name_len);
    return 0;
}

int write_output_file(struct server_context_t *ctx) {
    int rc;
    if(ctx->fd < 0)
        return 1;
    if(ctx->bytes_in_buf == 0)
        return 0;
    if(ctx->file_len < ctx->bytes_in_buf) {
        printf("получено больше данных, чем ожидалось (%d, %d)\n", ctx->file_len, ctx->bytes_in_buf);
        ctx->bytes_in_buf = ctx->file_len;
    }
    rc = write(ctx->fd, ctx->buf, ctx->bytes_in_buf);
    if(rc < 0) {
        perror("ошибка записи в файл:");
        return 1;
    }
    ctx->file_len -= rc;
    drop_some_bytes_in_server_context(ctx, rc);
    if(ctx->file_len == 0) {
        close(ctx->fd);
        ctx->fd = -1;
        ctx->state = 777;
    }
    return 0;
}

int process_received_bytes(struct server_context_t *ctx) {
    int rc;
    switch (ctx->state) {
        case 0:
            if (ctx->bytes_in_buf < 4)
                return 0;
            ctx->file_name_len = *((uint32_t*)ctx->buf);
            printf("длина файла: %d\n", ctx->file_name_len);
            drop_some_bytes_in_server_context(ctx, 4);
            if(ctx->file_name_len == 0 || ctx->file_name_len > PATH_MAX) {
                printf("Неверная длина файла\n");
                return 1;
            }
            ctx->state = 10;
            return process_received_bytes(ctx);
        case 10:
            // извлечение длины файла
            if (ctx->bytes_in_buf < 4)
                return 0;
            ctx->file_len = *((uint32_t*)ctx->buf);
            drop_some_bytes_in_server_context(ctx, 4);
            printf("Ожидаем файл длиной %d\n", ctx->file_len);
            ctx->state++;
            return process_received_bytes(ctx);
        case 11:
            // извлечение имени файла
            if(ctx->bytes_in_buf < ctx->file_name_len) {
                return 0;
            }
            if(create_output_file(ctx))
                return 1;
            ctx->state++;
            return process_received_bytes(ctx);
        case 12:
            // извлечение файла
            return write_output_file(ctx);
        case 777:
            break;

    }
    return 0;
}


void add_context(int sock, struct server_context_t **ctx) {
    struct server_context_t *s = (struct server_context_t *)malloc(sizeof(struct server_context_t));
    init_server_context(s);
    s->sock = sock;
    HASH_ADD_INT( *ctx, sock, s );  /* id: name of key field */
}

void delete_context(int sock, struct server_context_t **ctx) {
    struct server_context_t *s;
    HASH_FIND_INT( *ctx, &sock, s );  /* s: output pointer */
    if(s) {
        if(s->fd >= 0) {
            close(s->fd);
            s->fd = -1;
        }
        HASH_DEL(*ctx, s);
        free(s);
    }
}

main(int argc, char *argv[]) {
    int len, rc, on = 1;
    int listen_sd = -1, new_sd = -1;
    int desc_ready, end_server = FALSE, compress_array = FALSE;
    int close_conn;
    char buffer[80];
    struct sockaddr_in addr;
    int timeout;
    struct pollfd fds[SERVER_MAX_CONN];
    // все контексты сервера
    struct server_context_t *context = NULL;  /* important! initialize to NULL */
    // контекст для текущего сокета
    struct server_context_t *ctx = NULL;

    int nfds = 1, current_size = 0, i, j;


    for(int i=0; i<sizeof(ctx)/sizeof(struct server_context_t); i++) {
        init_server_context(&ctx[i]);
    }

    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd < 0) {
        perror("socket() failed");
        exit(-1);
    }


    rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR,
                    (char *) &on, sizeof(on));
    if (rc < 0) {
        perror("setsockopt() failed");
        close(listen_sd);
        exit(-1);
    }

    rc = ioctl(listen_sd, FIONBIO, (char *) &on);
    if (rc < 0) {
        perror("ioctl() failed");
        close(listen_sd);
        exit(-1);
    }


    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
   // memcpy(&addr.sin_addr, &in6addr_any, sizeof(inaddr_any));
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERVER_PORT);
    rc = bind(listen_sd,
              (struct sockaddr *) &addr, sizeof(addr));
    if (rc < 0) {
        perror("bind() failed");
        close(listen_sd);
        exit(-1);
    }


    rc = listen(listen_sd, 32);
    if (rc < 0) {
        perror("listen() failed");
        close(listen_sd);
        exit(-1);
    }


    memset(fds, 0, sizeof(fds));


    fds[0].fd = listen_sd;
    fds[0].events = POLLIN;

    timeout = (3 * 60 * 1000);


    do {

        printf("Waiting ...\n");
        rc = poll(fds, nfds, timeout);


        if (rc < 0) {
            perror("  poll() failed");
            break;
        }


        if (rc == 0) {
            printf("  poll() timed out.  End program.\n");
            break;
        }



        current_size = nfds;
        for (i = 0; i < current_size; i++) {

            if (fds[i].revents == 0)
                continue;


         //   if (fds[i].revents != POLLIN) {
         //       printf("  Error! revents = %d\n", fds[i].revents);
         //       end_server = TRUE;
         //       break;

         //   }
            if (fds[i].fd == listen_sd) {

                printf("  Listening socket is readable\n");


                do {

                    new_sd = accept(listen_sd, NULL, NULL);
                    if (new_sd < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("  accept() failed");
                            end_server = TRUE;
                        }
                        break;
                    }


                    printf("  New incoming connection - %d\n", new_sd);
                    fds[nfds].fd = new_sd;
                    fds[nfds].events = POLLIN;
                    add_context(fds[nfds].fd, &context);
                    nfds++;

                } while (new_sd != -1);
            }



            else {
                printf("  Descriptor %d is readable\n", fds[i].fd);
                close_conn = FALSE;


                do {
                    HASH_FIND_INT( context, &(fds[i].fd), ctx );  /* s: output pointer */
                    if(ctx == NULL) {
                        printf("не найден контекст для сокета %d\n", fds[i].fd);
                        close_conn = TRUE;
                        break;
                    }

                    rc = recv(fds[i].fd, ctx->buf_ptr, SERVER_BUFFER_SIZE - ctx->bytes_in_buf, 0);
                    if (rc < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("  recv() failed");
                            close_conn = TRUE;
                        }
                        break;
                    }

                    if (rc == 0) {
                        printf("  Connection closed\n");
                        close_conn = TRUE;
                        break;
                    }

                    ctx->recv_len = rc;
                    ctx->buf_ptr += ctx->recv_len;
                    ctx->bytes_in_buf += ctx->recv_len;
                    printf("  %d bytes received\n", ctx->recv_len);
                    rc = process_received_bytes(ctx);
//                    printf("состояние контекста %d\n", ctx->state);
//                    rc = send(fds[i].fd, buffer, len, 0);
//                    if (rc < 0) {
//                        perror("  send() failed");
//                        close_conn = TRUE;
//                        break;
//                    }
                    if(rc || ctx->state == 777) {
                        close_conn = TRUE;
                        break;
                    }
                } while (FALSE);

                if (close_conn) {
                    printf("закрываем сокет %d\n", fds[i].fd);
                    close(fds[i].fd);
                    delete_context(fds[i].fd, &context);
                    fds[i].fd = -1;
                    compress_array = TRUE;
                }
            }
        }


        if (compress_array) {
            compress_array = FALSE;
            for (i = 0; i < nfds; i++) {
                if (fds[i].fd == -1) {
                    for (j = i; j < nfds; j++) {
                        fds[j].fd = fds[j + 1].fd;
                    }
                    i--;
                    nfds--;
                }
            }
        }

    } while (end_server == FALSE);
    for (i = 0; i < nfds; i++) {
        if (fds[i].fd >= 0)
            close(fds[i].fd);
    }
}
