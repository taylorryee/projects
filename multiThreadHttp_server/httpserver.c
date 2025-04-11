#include "connection.h"
#include "queue.h"
#include "request.h"
#include "response.h"
#include "rwlock.h"
#include "asgn2_helper_funcs.h"
#include "Filelock.h"

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>
void handle_connection(int);

void handle_get(conn_t *);
void handle_put(conn_t *);
void handle_unsupported(conn_t *);
queue_t *tqueue;
List locks;
void *worker_thread();
pthread_mutex_t lock;

int main(int argc, char **argv) {
    int threads = 4; // Default value for threads
    int port
        = 0; // Initialize port with a default value (actual port value should be set after parsing)
    int opt;
    int portFlag = 0; // Flag to check if the port number is set

    while ((opt = getopt(argc, argv, "t:")) != -1) {
        switch (opt) {
        case 't':
            threads = atoi(optarg); // Extract the value for threads
            break;
        default: fprintf(stderr, "Usage: %s [-t threads] <port>\n", argv[0]); exit(EXIT_FAILURE);
        }
    }

    // Check if there's a non-option argument (which should be the port)
    if (optind < argc) {
        port = atoi(argv[optind]); // Extract the port number
        portFlag = 1; // Set the port flag to indicate port number is set
    } else {
        fprintf(stderr, "Missing port number\n");
        exit(EXIT_FAILURE);
    }

    // If the port number wasn't set during option parsing, check for -t option again
    if (!portFlag) {
        optind = 1; // Reset getopt index to re-parse options
        while ((opt = getopt(argc, argv, "t:")) != -1) {
            switch (opt) {
            case 't':
                threads = atoi(optarg); // Extract the value for threads
                break;
            default:
                fprintf(stderr, "Usage: %s [-t threads] <port>\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }

        // Check if there's a non-option argument (which should be the port)
        if (optind < argc) {
            port = atoi(argv[optind]); // Extract the port number
        } else {
            fprintf(stderr, "Missing port number\n");
            exit(EXIT_FAILURE);
        }
    }

    signal(SIGPIPE, SIG_IGN);
    Listener_Socket sock;
    listener_init(&sock, port);
    tqueue = queue_new(threads);
    locks = new_list();
    //pthread_mutex_init(&lock,NULL);
    //pthread_mutex_lock(&lock);

    pthread_t *thread_pool = malloc(threads * sizeof(pthread_t));
    for (int i = 0; i < threads; ++i) {
        pthread_create(&thread_pool[i], NULL, worker_thread, NULL);
    }
    while (1) {
        intptr_t connfd = listener_accept(&sock);
        queue_push(tqueue, (void *) connfd);

        //close(connfd);
    }
    //file_lock_delete(locks);
    return EXIT_SUCCESS;
}
void *worker_thread() {
    while (1) {

        intptr_t elem = 0;
        queue_pop(tqueue, (void **) &elem);
        handle_connection(elem);
        close(elem);
    }
}

void handle_connection(int connfd) {

    conn_t *conn = conn_new(connfd);

    const Response_t *res = conn_parse(conn);

    if (res != NULL) {
        conn_send_response(conn, res);
    } else {

        const Request_t *req = conn_get_request(conn);
        if (req == &REQUEST_GET) {
            handle_get(conn);
        } else if (req == &REQUEST_PUT) {
            handle_put(conn);
        } else {
            handle_unsupported(conn);
        }
    }

    conn_delete(&conn);
}

void handle_get(conn_t *conn) {

    char *uri = conn_get_uri(conn);
    char *request_id = "Request-Id";

    const Response_t *res = NULL;

    // Check if file already exists before opening it.

    file_lock_new(locks, uri);
    file_lock_read_lock(locks, uri);

    bool existed = access(uri, F_OK) == 0;
    if (!existed) {
        res = &RESPONSE_BAD_REQUEST;
        goto out;
    }

    int fd = open(uri, O_RDONLY, 0600);
    if (fd < 0) {

        if (errno == EACCES) {
            res = &RESPONSE_FORBIDDEN;
            goto out;
        } else if (errno == ENOENT) {
            res = &RESPONSE_NOT_FOUND;
            goto out;
        } else {
            res = &RESPONSE_INTERNAL_SERVER_ERROR;
            goto out;
        }
    }

    struct stat file_info;

    if (fstat(fd, &file_info) != 0) {
        perror("Error getting file information");
        exit(EXIT_FAILURE);
        
    }

    off_t file_size = file_info.st_size;
    if (S_ISDIR(file_info.st_mode)) {
        res = &RESPONSE_FORBIDDEN;
        goto out;
    }
    pthread_mutex_lock(&lock);
    fprintf(stdout, "MUTEX LOCK\n");
    fflush(stdout);

    res = conn_send_file(conn, fd, (uint64_t) file_size);

    if (res == NULL) {
        res = &RESPONSE_OK;
    } else {
        res = &RESPONSE_INTERNAL_SERVER_ERROR;
        goto out;
    }

    fprintf(stderr, "GET,/%s,%" PRIu16 ",%s\n", uri, response_get_code(res),
        conn_get_header(conn, request_id));

    file_lock_read_unlock(locks, uri);
    //read_file_lock(locks,uri);

    close(fd);

out:

    pthread_mutex_unlock(&lock);
    fprintf(stdout, "UNMUTEX LOCK\n");
    fflush(stdout);
    //file_lock_read_unlock(locks,uri);
}
void handle_unsupported(conn_t *conn) {

    // send responses
    conn_send_response(conn, &RESPONSE_NOT_IMPLEMENTED);
}

void handle_put(conn_t *conn) {

    char *uri = conn_get_uri(conn);
    char *request_id = "Request-Id";

    const Response_t *res = NULL;

    pthread_mutex_lock(&lock);
    fprintf(stdout, "MUTEX LOCK\n");
    fflush(stdout);
    file_lock_new(locks, uri);

    file_lock_write_lock(locks, uri);

    bool existed = access(uri, F_OK) == 0;

    // Open the file..
    int fd = open(uri, O_CREAT | O_TRUNC | O_WRONLY, 0600);
    if (fd < 0) {

        if (errno == EACCES || errno == EISDIR || errno == ENOENT) {
            res = &RESPONSE_FORBIDDEN;
            goto out;
        } else {
            res = &RESPONSE_INTERNAL_SERVER_ERROR;
            goto out;
        }
    }

    //file_lock_write_lock(locks,uri);

    res = conn_recv_file(conn, fd);

    if (res == NULL && existed) {
        res = &RESPONSE_OK;
    } else if (res == NULL && !existed) {
        res = &RESPONSE_CREATED;
    }
    fprintf(stderr, "PUT,/%s,%" PRIu16 ",%s\n", uri, response_get_code(res),
        conn_get_header(conn, request_id));

    file_lock_write_unlock(locks, uri);

    conn_send_response(conn, res);

    close(fd);
    pthread_mutex_unlock(&lock);
    fprintf(stdout, "MUTEX UNLOCK\n");
    fflush(stdout);
    return;

out:
    file_lock_write_unlock(locks, uri);
    pthread_mutex_unlock(&lock);
    fprintf(stdout, "MUTEX UNLOCK\n");
    fflush(stdout);
    close(fd);
}
