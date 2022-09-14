#include "operations.h"

/* Extra includes */
#include "common/common.h"
#include <sys/types.h>

/* Session ID table */
static client_session_t session_id_table[MAX_SERVER_SESSIONS];
static char free_session_table[MAX_SERVER_SESSIONS];

/* Session's thread table */
static pthread_t session_thread_table[MAX_SERVER_SESSIONS];

/* Server global mutex */
static pthread_mutex_t server_mutex;


static inline bool valid_session_id(int session_id) {
    return session_id >= 0 && session_id < MAX_SERVER_SESSIONS;
}


/*
 * Initilizes the server's table and pipe
 * Inputs:
 *  - server pipe's path name
 */
void tfs_server_init(char const *server_pipe_path) {
    if (tfs_init() != 0) {
        perror("[ERR]");
        exit(1);
    }

    if (pthread_mutex_init(&server_mutex, NULL) != 0) {
        perror("[ERR]");
        exit(1);
    }

    unlink(server_pipe_path);

    if (mkfifo(server_pipe_path, 0777) != 0 && errno != EEXIST) {
        pthread_mutex_destroy(&server_mutex);
        perror("[ERR]");
        exit(1);
    }

    for (size_t i = 0; i < MAX_SERVER_SESSIONS; i++) {
        free_session_table[i] = FREE;
        if (pthread_mutex_init(&session_id_table[i].client_mutex, NULL) != 0) {
            pthread_mutex_destroy(&server_mutex);
            exit(1);
        }
        if (pthread_cond_init(&session_id_table[i].client_cond, NULL) != 0) {
            pthread_mutex_destroy(&session_id_table[i].client_mutex);
            pthread_mutex_destroy(&server_mutex);
            exit(1);
        }
    }
}


/*
 * Destroys the Server's state
 * Inputs:
 *  - server's file descriptor
 */
void tfs_server_destroy(int server_fd) {
    if (pthread_mutex_destroy(&server_mutex) != 0) {
        perror("[ERR]");
        exit(1);
    }

    for (size_t i = 0; i < MAX_SERVER_SESSIONS; i++) {
        /* TODO: maybe unlink the clients? */
        if (close_until_success(session_id_table[i].client_fd) != 0) {
            perror("[ERR]");
            exit(1);
        }
    }

    if (close_until_success(server_fd) != 0) {
        perror("[ERR]");
        exit(1);
    }
}


/*
 * Locks a session's mutex
 * Inputs:
 * - session we want to lock
 * Returns: 0 if successful, -1 otherwise
 */
int session_id_lock(client_session_t *session) {
    if (pthread_mutex_lock(&session->client_mutex) != 0){
        return -1;
    }
    return 0;
}


/*
 * Unlocks a session's mutex
 * Inputs:
 * - session we want to unlock
 * Returns: 0 if successful, -1 otherwise
 */
int session_id_unlock(client_session_t *session) {
    if (pthread_mutex_unlock(&session->client_mutex) != 0){
        return -1;
    }
    return 0;
}


/*
 * Puts a session on hold (wait)
 * Inputs:
 * - session we want to wait
 * Returns: 0 if successful, -1 otherwise
 */
int session_cond_wait(client_session_t *session) {
    if (session_id_lock(session) != 0) {
        return -1;
    }

    if (pthread_cond_wait(&session->client_cond, &session->client_mutex) != 0) {
        session_id_unlock(session);
        return -1;
    }

    session_id_unlock(session);
    return 0;
}


/*
 * Sends a signal to a session
 * Inputs:
 * - session we want to signal
 * Returns: 0 if successful, -1 otherwise
 */
int session_cond_signal(client_session_t *session) {
    if (session_id_lock(session) != 0) {
        return -1;
    }

    if (pthread_cond_signal(&session->client_cond) != 0) {
        session_id_unlock(session);
        return -1;
    }

    session_id_unlock(session);
    return 0;
}


/*
 * Allocates a new entry for the current session
 * Returns: entry index if successful, -1 otherwise
 */
int session_id_alloc() {
    if (pthread_mutex_lock(&server_mutex) != 0) {
        return -1;
    }

    for (int i = 0; i < MAX_SERVER_SESSIONS; i++) {
        if (free_session_table[i] == FREE) {
            
            free_session_table[i] = TAKEN;

            if (pthread_mutex_unlock(&server_mutex) != 0) {
                return -1;
            }

            return i;
        }
    }
    pthread_mutex_unlock(&server_mutex);
    return -1;
}


/*
 * Returns a pointer to an existing client session.
 * Input:
 *  - session_id: session's identifier
 * Returns: pointer if successful, NULL if failed
 */
client_session_t *session_get(int session_id) {
    if (!valid_session_id(session_id)) {
        return NULL;
    }

    return &session_id_table[session_id];
}


/*
 * Frees an entry from the session_id table
 * Inputs:
 *  - closing session's id
 * Returns: 0 if succsessful, -1 otherwise
 */
int session_id_remove(int session_id) {
    if (!valid_session_id(session_id)) {
        return -1;
    }

    if (pthread_mutex_lock(&server_mutex) != 0) {
        return -1;
    }

    free_session_table[session_id] = FREE;

    if (pthread_mutex_unlock(&server_mutex) != 0) {
        return -1;
    }

    return 0;
}


/* 
 * Sends a message (int) to a certain client
 * Inputs:
 *  - file to write the message to
 *  - value to be sent
 * Returns: 0 if successful, -1 otherwise
 */
int send_message(int dest_fd, int ret) {
    if (write_until_success(dest_fd, &ret, RETURN_VAL_SIZE) != 0) {
        return -1;
    }
    return 0;
}


/* 
 * Handles Mount requests from the clients
 * Inputs:
 *  - arguments for mount
 * In case of an error, notifies the client
 */
void tfs_server_mount(void const *arg) {
    char *args = (char*) arg;

    /* Gets the argument we need for the mount command:
     * <client pipe path name> */
    char client_pipe_path[MAX_CPATH_LEN];
    memcpy(client_pipe_path, args, MAX_CPATH_LEN);

    /* Opens the client's pipe for every future witing */
    int client_fd = open_until_success(client_pipe_path, O_WRONLY);
    if (client_fd == -1) {
        unlink(client_pipe_path);
        exit(1);
    }

    int session_id = session_id_alloc();

    /* If session_id == -1 it means there is no more space for any more sessions
     * to the server and so we notify the client with an error */
    if (session_id == -1) {
        if (send_message(client_fd, -1) != 0) {
            exit(1);
        }
        return;
    }

    /* Writes to the client's pipe its session id */
    if (write_until_success(client_fd, &session_id, SESSION_ID_SIZE) != 0) {
        if (send_message(client_fd, -1) != 0) {
            exit(1);
        }
        return;
    }

    if (pthread_mutex_lock(&server_mutex) != 0) {
        exit(1);
    }

    /* Fills the structs's fields with the client's information */
    session_id_table[session_id].client_fd = client_fd;

    if (pthread_mutex_unlock(&server_mutex) != 0) {
       exit(1);
    }
}


/* 
 * Handles Unmount requests from the clients
 * Inputs:
 *  - arguments for unmount
 * In case of an error, notifies the client
 */
void tfs_server_unmount(void const *arg) {
    char *args = (char*) arg;

    /* Gets the argument we need for the unmount command:
     * session_id */
    int session_id;
    memcpy(&session_id, args, SESSION_ID_SIZE);

    /* Protect the session_get function call */
    if (pthread_mutex_lock(&server_mutex) != 0) {
        exit(1);
    }

    client_session_t *client_session = session_get(session_id);
    if (client_session == NULL) {
        return;
    }

    int client_fd = client_session->client_fd;

    if (pthread_mutex_unlock(&server_mutex) != 0) {
        exit(1);
    }
    
    /* Removes the session from the system */
    if (session_id_remove(session_id) != 0) {
        if (send_message(client_fd, -1) != 0) {
            exit(1);
        }
        return;
    }

    /* If the client recieves a 0 through the pipe, unmount was successful */
    int success = 0;
    /* Sends to the client its pipe's name (for unlinking by the client) */
    if (write_until_success(client_fd, &success, RETURN_VAL_SIZE) != 0) {
        if (send_message(client_fd, -1) != 0) {
            exit(1);
        }
        return;
    }
}


/* 
 * Handles Open requests from the clients
 * Inputs:
 *  - arguments for open
 * In case of an error, notifies the client
 */
void tfs_server_open(void const *arg) {
    char *args = (char*) arg;

    /* Gets the arguments we need for the open command:
     * client_fd | <file name> | flags */
    int client_fd;
    memcpy(&client_fd, args, CLIENT_FD_SIZE);
    char file_name[MAX_CPATH_LEN];
    memcpy(file_name, args + SESSION_ID_SIZE, MAX_CPATH_LEN);
    int flags;
    memcpy(&flags, args + SESSION_ID_SIZE + MAX_CPATH_LEN, SESSION_ID_SIZE);

    /* Stores the return value of tfs_open() */
    int ret;
    ret = tfs_open(file_name, flags);

    /* If for some reason tfs_close() returns -1, it won't be a problem for now,
     * as the client will deal with it accordingly */
    if (write_until_success(client_fd, &ret, RETURN_VAL_SIZE) != 0) {
        if (send_message(client_fd, -1) != 0) {
            exit(1);
        }
        return;
    }
}


void tfs_server_close(void const *arg) {
    char *args = (char*) arg;

    /* Gets the arguments we need for the close command:
     * client_fd | fhandle */
    int client_fd;
    memcpy(&client_fd, args, CLIENT_FD_SIZE);
    int fhandle;
    memcpy(&fhandle, args + SESSION_ID_SIZE, FHANDLE_SIZE);

    /* Stores the return value of tfs_close() */
    int ret;
    ret = tfs_close(fhandle);

    /* If for some reason tfs_close() returns -1, it won't be a problem for now,
     * as the client will deal with it accordingly */
    if (write_until_success(client_fd, &ret, RETURN_VAL_SIZE) != 0) {
        if (send_message(client_fd, -1) != 0) {
            exit(1);
        }
        return;
    }
}


void tfs_server_write(void const *arg) {
    char *args = (char*) arg;

    /* Gets the arguments we need for the write command:
     * client_fd | fhandle | len | <data to write> */
    int client_fd;
    memcpy(&client_fd, args, CLIENT_FD_SIZE);
    int fhandle;
    memcpy(&fhandle, args + SESSION_ID_SIZE, FHANDLE_SIZE);
    size_t len;
    memcpy(&len, args + SESSION_ID_SIZE + FHANDLE_SIZE, LEN_SIZE);
    char to_write[len];
    memcpy(to_write, args + SESSION_ID_SIZE + FHANDLE_SIZE + LEN_SIZE, MAX_RDWR_SIZE);

    /* Stores the return value of tfs_close() */
    ssize_t ret;
    ret = tfs_write(fhandle, to_write, len);

    /* If for some reason tfs_write() returns -1, it won't be a problem for now,
     * as the client will deal with it accordingly */
    if (write_until_success(client_fd, &ret, RDWR_VAL_SIZE) != 0) {
        if (send_message(client_fd, -1) != 0) {
            exit(1);
        }
        return;
    }
}


void tfs_server_read(void const *arg) {
    char *args = (char*) arg;

    /* Gets the arguments we need for the read command:
     * client_fd | fhandle | len */
    int client_fd;
    memcpy(&client_fd, args, CLIENT_FD_SIZE);
    int fhandle;
    memcpy(&fhandle, args + SESSION_ID_SIZE, FHANDLE_SIZE);
    size_t len;
    memcpy(&len, args + SESSION_ID_SIZE + FHANDLE_SIZE, LEN_SIZE);
    
    /* Stores the return value of tfs_read() */
    ssize_t ret;
    ret = tfs_read(fhandle, args + SESSION_ID_SIZE + FHANDLE_SIZE + LEN_SIZE, len);

    /* If for some reason tfs_read() returns -1, it won't be a problem for now,
     * as the client will deal with it accordingly */
    if (write_until_success(client_fd, &ret, RDWR_VAL_SIZE) != 0) {
        if (send_message(client_fd, -1) != 0) {
            exit(1);
        }
        return;
    }
}


void tfs_server_shutdown(void const *arg) {
    char *args = (char*) arg;

    /* Gets the arguments we need for the read command:
     * client_fd | fhandle */
    int client_fd;
    memcpy(&client_fd, args, CLIENT_FD_SIZE);
    int fhandle;
    memcpy(&fhandle, args + SESSION_ID_SIZE, FHANDLE_SIZE);

    /* Stores the return value of tfs_destroy_after_all_closed() */
    int ret;
    ret = tfs_destroy_after_all_closed();

    /* If for some reason tfs_destroy_after_all_closed() returns -1, it won't 
     * be a problem for now, as the client will deal with it accordingly */
    if (write_until_success(client_fd, &ret, RETURN_VAL_SIZE) != 0) {
        if (send_message(client_fd, -1) != 0) {
            exit(1);
        }
        return;
    }
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Please specify the pathname of the server's pipe.\n");
        return 1;
    }

    char *pipename = argv[1];
    printf("Starting TecnicoFS server with pipe called %s\n", pipename);

    /* Initialize the server */
    tfs_server_init(pipename);

    int server_fd;
    /* Opens the server's pipe for future reading */
    server_fd = open_until_success(pipename, O_RDONLY);
    if (server_fd == -1) {
        return -1;
    }

    /* The server will run indefinitely, waiting for requests from the clients */
    while(1) {
        /* Buffer that stores request's fields (OP_CODE + rest of the fields) */
        char request_buffer[MAX_REQUEST_SIZE];

        ssize_t offset = 0, rd;
        while ((rd = read_until_success(server_fd, request_buffer + offset, MAX_REQUEST_SIZE)) !=
                                                                MAX_REQUEST_SIZE && errno == EINTR) {
            /* "Reboots" the server if needed */
            if (rd <= 0) {
                if (close_until_success(server_fd) != 0) {
                    exit(1);
                }
                server_fd = open_until_success(pipename, O_RDONLY);
                if (server_fd == -1) {
                    exit(1);
                }
            }
            /* Updates the current offset */
            offset += rd;
        }
        
        client_session_t *client_session;
        char op_code = request_buffer[0];

        int session_id = -1;
        /* Gets the client's session id from the buffer */
        memcpy(request_buffer + OP_CODE_SIZE, &session_id, SESSION_ID_SIZE);

        if (op_code != TFS_OP_CODE_MOUNT || op_code != TFS_OP_CODE_UNMOUNT) {
            /* Protect the session_get function call */
            if (pthread_mutex_lock(&server_mutex) != 0) {
                exit(1);
            }

            client_session = session_get(session_id);
            if (client_session == NULL) {
                exit(1);
            }

            int client_fd = client_session->client_fd;

            if (pthread_mutex_unlock(&server_mutex) != 0) {
                exit(1);
            }

            /* replaces the "session_id" in the buffer with the client's file descriptor, to be
             * used by the server to connect to the client's pipe on future function calls, write,
             * read, open, close and shutdown */
            memcpy(request_buffer + OP_CODE_SIZE, &client_fd, CLIENT_FD_SIZE);
        }

        switch(op_code) {
            /* "request_buffer+1" is used to skip the OP_CODE */

            case TFS_OP_CODE_MOUNT:
                tfs_server_mount(request_buffer+OP_CODE_SIZE);
                break;
            
            case TFS_OP_CODE_UNMOUNT:
                session_cond_wait(client_session);
                pthread_create(&session_thread_table[session_id], NULL, (void*)tfs_server_unmount, request_buffer+OP_CODE_SIZE);
                session_cond_signal(client_session);
                break;
            
            case TFS_OP_CODE_OPEN:
                session_cond_wait(client_session);
                pthread_create(&session_thread_table[session_id], NULL, (void*)tfs_server_open, request_buffer+OP_CODE_SIZE);
                session_cond_signal(client_session);
                break;

            case TFS_OP_CODE_CLOSE:
                session_cond_wait(client_session);
                pthread_create(&session_thread_table[session_id], NULL, (void*)tfs_server_close, request_buffer+OP_CODE_SIZE);
                session_cond_signal(client_session);
                break;

            case TFS_OP_CODE_WRITE:
                session_cond_wait(client_session);
                pthread_create(&session_thread_table[session_id], NULL, (void*)tfs_server_write, request_buffer+OP_CODE_SIZE);
                session_cond_signal(client_session);
                break;

            case TFS_OP_CODE_READ:
                session_cond_wait(client_session);
                pthread_create(&session_thread_table[session_id], NULL, (void*)tfs_server_read, request_buffer+OP_CODE_SIZE);
                session_cond_signal(client_session);
                break;

            case TFS_OP_CODE_SHUTDOWN_AFTER_ALL_CLOSED:
                session_cond_wait(client_session);
                pthread_create(&session_thread_table[session_id], NULL, (void*)tfs_server_shutdown, request_buffer+OP_CODE_SIZE);
                session_cond_signal(client_session);
                break;

            default:
                /* Esta entrega...kinda embarassing ngl, sry :c */
                printf("Invalid OP_CODE\n");
                break;
        }
    }

    return 0;
}