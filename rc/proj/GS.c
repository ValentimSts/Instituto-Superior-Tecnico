
/**
 * @file GS.c
 * @author
 *  Group 36 (Filipa Magalhaes & Valentim Santos)
 * 
 * @brief 
 *
 */


/* Include wall. */
#include "common/common.h"
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>


#define SCOREBOARD_SIZE 10

#define MAX_TCP_QUEUE 5

#define VERBOSE_COMMAND "-v"

#define PLAY_CODE 'T'
#define GUESS_CODE 'G'
#define WIN_CODE 'W'
#define FAIL_CODE 'F'
#define QUIT_CODE 'Q'




typedef struct scoreboard {
    
    /* The scoreboard struct stores the information
     * relevant to build the server's scoreboard. */

    size_t score[SCOREBOARD_SIZE]; // Player scores.

    char PLID[SCOREBOARD_SIZE][MAX_PLID_SIZE + 1]; // Player IDs.

    char word[SCOREBOARD_SIZE][MAX_WORD_SIZE + 1]; // Player words.

    size_t n_succ[SCOREBOARD_SIZE]; // Number of successful trials.

    size_t n_total[SCOREBOARD_SIZE]; // Total number os trials.

    size_t n_scores; // Number of scores in the scoreboard.

} scoreboard;


typedef struct server_args {

    /* The name of the server's word file. */
    char word_file[MAX_FILE_NAME_SIZE];

    /* The current word (line in the word_file)
     * to be guessed by the player. */
    int curr_word_line;

    /* The port where the server accepts
     * requests from (GS_DEFAULT_PORT if ommited). */
	char GSport[MAX_GSPORT_SIZE];

    /* Tells us if the server is to be run
     * in verbose mode or not (0 if ommited).*/
    size_t is_verbose;

    /* Stores the current GS's scoreboard. */
    scoreboard* GSscoreboard;

} server_args;


server_args* init_server_args();
void free_server_args(server_args* GSargs);

void get_server_args(server_args* GSargs, int n_args, char *args[]);
void compute_server_commands_UDP(server_args* GSargs);
void compute_server_commands_TCP(server_args* GSargs);

char* parse_player_message(server_args* GSargs, char* message);
void get_status(server_args* GSargs);

char* get_word_line(server_args* GSargs);
char* GS_start_game(server_args* GSargs, char* PLID);

size_t valid_trial(FILE* player_fp,  char* trial, char* guess, size_t is_play_command);
size_t is_dup_play(FILE* player_fp, char* play, size_t is_play_command);
size_t process_game_end(char* player_file, const char end_code, size_t curr_trial);

char* check_correct_letter(char* player_file, char* letter, int curr_trial, size_t is_verbose);
char* GS_play_letter(server_args* GSargs, char* PLID, char* letter, char* trial);

char* check_correct_word(char* player_file, char* word_guess, int curr_trial, size_t is_verbose);
char* GS_guess_word(server_args* GSargs, char* PLID, char* word_guess, char* trial);

size_t get_scoreboard(server_args* GSargs);
char* GS_show_scoreboard(server_args* GSargs);

char* GS_show_hint(server_args* GSargs, char* PLID);

char* GS_show_state(server_args* GSargs, char* PLID);

char* GS_quit_game(server_args* GSargs, char* PLID);



int main(int argc, char* argv[]) {

    /* Initializes a new player. */
    server_args* new_server = init_server_args();

    /* Gets the server arguments passed by the user. */
    get_server_args(new_server, argc, argv);

    pid_t pid = fork();

    if (pid < 0) {
        /* An error occured with fork. */
        printf("ERR(main): fork failed.\n");
        return 1;
    }

    else if (pid == 0) {
        /* Child process. */
        compute_server_commands_UDP(new_server);
    }

    else {
        /* Parent process. */
        compute_server_commands_TCP(new_server);
    }


    return 0;
}



/**
 * @brief 
 * 	Initializes a server_args struct with its
 * 	default values.
 * 
 * @return server_args.
 */
server_args* init_server_args() {
    /* Allocates memory for the server_args struct. */
    server_args* new_server = (server_args*) malloc(sizeof(server_args));

    /* Allocates memory for the GS's scoreboard.  */
    scoreboard* new_scoreboard = (scoreboard*) malloc(sizeof(scoreboard));
    new_server->GSscoreboard = new_scoreboard;

    new_server->GSscoreboard->n_scores = 0;

	/* Sets the server's fields to their default values. */
	strcpy(new_server->GSport, GS_DEFAULT_PORT);
    new_server->is_verbose = 0;
    new_server->curr_word_line = 0;

	return new_server;
}



/**
 * @brief 
 *  Frees the memory allocated for the 
 *  server_args struct.
 * 
 * @param GSargs server_args struct.
 */
void free_server_args(server_args* GSargs) {
    free(GSargs->GSscoreboard);
    free(GSargs);
}



/**
 * @brief
 * 	Computes the user input and sets the server_args
 *  struct's fields to its respective values (if specified).
 * 
 * @param GSargs Server_args struct.
 * @param n_args Number of arguments passed.
 * @param args Array of arguments.
 */
void get_server_args(server_args* GSargs, int n_args, char *args[]) {

    size_t error = 1;

    if (n_args > 1) {
        
        /* The word_file is a mandatory argument. */
        if (access(args[1], F_OK) == 0) { // the file exists.
            strcpy(GSargs->word_file, args[1]);
        }
        else { // the file doesn't exist.
            printf("ERR(get_server_args): invalid word_file.\n");
            free_server_args(GSargs);
            exit(EXIT_FAILURE);
        }

        /* Both commands were ommited. */
        if (n_args == 2) {
            error = 0;
        }

        /* Only the "-v" command was invoked. */
        if (n_args == 3) {
            /* The player invoked the command '-n'. */
            if (strcmp(args[2], VERBOSE_COMMAND) == 0) {
                GSargs->is_verbose = 1;
                error = 0;
            }
        }

        /* Only the "-p" command was invoked. */
        else if (n_args == 4) {
            /* Checks if the command invoked is correct and, if so,
            * fills in the server fields accordingly. */
            if (strcmp(args[2], PORT_COMMAND) == 0) {
                strcpy(GSargs->GSport, args[3]);
                error = 0;
            }
        }

        /* Both the "-p" and "-v" commands were invoked. */
        else if (n_args == 5) {
            if (strcmp(args[2], PORT_COMMAND) == 0 &&
                strcmp(args[4], VERBOSE_COMMAND) == 0) {

                strcpy(GSargs->GSport, args[3]);
                GSargs->is_verbose = 1;
                error = 0;
            }
        }
    }

    /* The port chosen can't be a well-known port. */
	if (atoi(GSargs->GSport) <= 1024) {
		error = 1;
	}

    /* If somewhere an error occured, it will be dealt with. */
    if (error) {
        printf("ERR(get_server_args): invalid server arguments.\n");
        free_server_args(GSargs);
        exit(EXIT_FAILURE);
    }
}



/**
 * @brief 
 *  Computes the clients's commands received via UDP socket.
 * 
 * @param GSargs Server_args struct.
 */
void compute_server_commands_UDP(server_args* GSargs) {
    
	char message[GS_BUF_SIZE];
	char response[GS_BUF_SIZE];


    /* =========================== UDP SOCKET INITIALIZATION ======================== */

    int fd, errcode;
	ssize_t check;
	socklen_t addrlen;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;


    /* Creates the UDP socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        printf("ERR(compute_server_commands_UDP): UDP socket failed.\n");
        free_server_args(GSargs);
        exit(EXIT_FAILURE);
    }


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    hints.ai_flags = AI_PASSIVE;


    errcode = getaddrinfo(NULL, GSargs->GSport, &hints, &res);
    if (errcode != 0) {
        printf("ERR(compute_server_commands_UDP): getaddrinfo failed.\n");
        free_server_args(GSargs);
        exit(EXIT_FAILURE);
    }
    

    check = bind(fd, res->ai_addr, res->ai_addrlen);
    if (check == -1) {
        printf("ERR(compute_server_commands_UDP): bind failed.\n");
        free_server_args(GSargs);
        exit(EXIT_FAILURE);
    }

    /* ============================================================================== */


    /* Waits for the client to send UDP requests. */
    while (1) {
        addrlen = sizeof(addr);

        check = recvfrom(fd, message, GS_BUF_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
        if (check == -1) {
            printf("ERR(compute_server_commands_UDP): recvfrom failed.\n");
            free_server_args(GSargs);
            exit(EXIT_FAILURE);
        }


        /* Reads the player message and creates the server response. */
        char* tmp = parse_player_message(GSargs, message);

        /* An error ocurred. */
        if (tmp == NULL) {
            if (GSargs->is_verbose)
                printf("ERR(compute_server_commands_UDP): error occured.\n");

            /* An error occured, and so, an error status 
             * is sent to the client. */
            memset(response, 0, GS_BUF_SIZE);
            sprintf(response, "%s\n", STATUS_ERR);
        }

        else {
            strcpy(response, tmp);


            /* Since tmp is a malloc'd string, we free its memory
             * before ending this iteration. */
            free(tmp);
        }

        response[strlen(response)] = '\0';

        /* Sends the server response to the client. */
        check = sendto(fd, response, strlen(response)+1, 0, (struct sockaddr*) &addr, addrlen);

        if (check == -1) {
            printf("ERR(compute_server_commands_UDP): sendto failed.\n");
            free_server_args(GSargs);
            exit(EXIT_FAILURE);
        }
    }


    freeaddrinfo(res);
    close(fd); 
}



/**
 * @brief 
 *  Computes the clients's commands received via TCP socket.
 * 
 * @param GSargs Server_args struct.
 */
void compute_server_commands_TCP(server_args* GSargs) {
    
	char message[GS_BUF_SIZE];
	char response[GS_BUF_SIZE];
    

    /* =========================== TCP SOCKET INITIALIZATION ======================== */
    
    int fd, errcode;
	ssize_t check;
	socklen_t addrlen;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;
    

    /* Creates the TCP socket. */
	fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("ERR(compute_server_commands_TCP): TDP socket failed.\n");
        free_server_args(GSargs);
        exit(EXIT_FAILURE);
    }
    

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    hints.ai_flags = AI_PASSIVE;


    errcode = getaddrinfo(NULL, GSargs->GSport, &hints, &res);
    if (errcode != 0) {
        printf("ERR(compute_server_commands_TCP): getaddrinfo failed.\n");
        free_server_args(GSargs);
        exit(EXIT_FAILURE);
    }
    

    check = bind(fd, res->ai_addr, res->ai_addrlen);
    if (check == -1) {
        printf("ERR(compute_server_commands_TCP): bind failed.\n");
        free_server_args(GSargs);
        exit(EXIT_FAILURE);
    }
    

    if (listen(fd, MAX_TCP_QUEUE) == -1) {
        printf("ERR(compute_server_commands_TCP): listen failed.\n");
        free_server_args(GSargs);
        exit(EXIT_FAILURE);
    }

    /* ============================================================================== */


    /* Waits for the client to send TCP requests. */
    while (1) {
        addrlen = sizeof(addr);

        int client_fd;

        /* Accepts a new connection from a client. */
        client_fd = accept(fd, (struct sockaddr*) &addr, &addrlen);
        if (client_fd == -1) {
            printf("ERR(compute_server_commands_TCP): accept failed.\n");
            return;
        }

        
        /* Creates a child process that will deal with the newly
         * accepted client request. */
        pid_t child_pid = fork();


        if (child_pid < 0) {
            printf("ERR(compute_server_commands_TCP): fork failed.\n");
            return;
        }

        /* Child process. */
        else if (child_pid == 0) {

            /* Closes the server socket, as we don't need it in
             * the child process. */
            close(fd);


            /* Reads and executes the client's request. */
            check = read(client_fd, message, GS_BUF_SIZE);
            if (check == -1) {
                printf("ERR(compute_server_commands_TCP): fork failed.\n");
                exit(EXIT_FAILURE);
            }

            /* Reads the player message and creates the server response. */
            char* tmp = parse_player_message(GSargs, message);


            if (tmp == NULL) {
                if (GSargs->is_verbose) 
                    printf("ERR(parse_player_message TCP): error occured.\n");

                /* An error occured, and so, an error status 
                 * is sent to the client. */
                memset(response, 0, GS_BUF_SIZE);
                sprintf(response, "%s\n", STATUS_ERR);

                size_t n = write(client_fd, response, strlen(response)+1);
                if (n == -1) {
                    // error.
                }
            }

            else {

                /* Based on what command the client invoked, the server
                 * will send different data, so we'll act accordingly. */

                strcpy(response, tmp);

                char response_type[MAX_COMMAND_SIZE], status[MAX_STATUS_SIZE];


                size_t Fsize;
                char Fname[GS_BUF_SIZE];
                FILE* Fdata;


                size_t header_size, to_read;
                size_t data_size, data_sent, n;


                sscanf(response, "%s %s", response_type, status);


                if (strcmp(status, STATUS_NOK) == 0 ||
                    strcmp(status, STATUS_EMPTY) == 0) {
                    
                    printf("response: %s\n", response);

                    /* There is no extradata (Fdata) to be sent. */
                    n = write(client_fd, response, strlen(response));
                    if (n == -1) {
                        // error.
                    }

                    close(client_fd);

                    /* Since tmp is a malloc'd string, we free its memory
                    * before ending this iteration. */
                    free(tmp);

                    exit(EXIT_SUCCESS);
                }


                /* The following will occur:
                 *
                 * - Open the file containing the data and loop over it, copying  
                 *   its contents to the response buffer in small portions.
                 * 
                 * - These portions are then sent to the client via TCP socket,
                 *   a second loop, with several writes.  */
                

                sscanf(response, "%*s %*s %s", Fname);


                Fdata = fopen(Fname, "r");
                if (Fdata == NULL) {
                    printf("ERR(compute_server_commands_TCP): fopen failed.\n");
                    // error.
                }


                /* Gets the size of the hint file to be sent. */
                fseek(Fdata, 0, SEEK_END);
                Fsize = ftell(Fdata);
                fseek(Fdata, 0, SEEK_SET);


                /* Clears the response buffer. */
                memset(response, 0, GS_BUF_SIZE);


                /* If the commands invoked are scoreboard or state, the 
                 * Fname sent to the player will be different. */
                if (strcmp(response_type, SERVER_SCORE_RESPONSE) == 0) {
                    strcpy(Fname, "scoreboard.txt\0");
                }

                else if (strcmp(response_type, SERVER_HINT_RESPONSE) == 0) {
                    char* tmp_Fname = Fname;
                    char* saveptr;

                    /* The Fname sent to the player will be the hint file name,
                     * without the path. */
                    char* ptr = strtok_r(tmp_Fname, "/", &saveptr);
                    char* real_Fname = strtok_r(NULL, "", &saveptr);

                    strcpy(Fname, real_Fname);
                }

                else if (strcmp(response_type, SERVER_STATE_RESPONSE) == 0) {
                    strcpy(Fname, "state.txt\0");
                }


                char* ptr_Fname = Fname;
                /* The response buffer will contain the response header, which
                    * will be sent to the client, along with some of Fdata on the first package. */
                sprintf(response, "%s %s %s %zu ", response_type, status, ptr_Fname, Fsize);                
                

                if (GSargs->is_verbose)
                    printf("\t--> file '%s' [%zu bytes];\n", ptr_Fname, Fsize);
                

                /* The first package sent to the client will have the response header at
                 * the beginning of the buffer, so the initial header size is taken into consideration.  */
                header_size = strlen(response);


                to_read = GS_BUF_SIZE - header_size;

                /* Reads the first part of Fdata and stores it in the same package as
                 * the header. */
                data_size = fread((response + header_size), 1, to_read, Fdata);

                /* Adds the missing header bytes to the data size. */
                data_size += header_size;
                data_sent = 0;


                size_t total_Fsize, total_data_sent;
                total_Fsize = Fsize + header_size;
                total_data_sent = 0;


                /* Sends the rest of Fdata to the client. */
                while (data_size != 0) {
                    if (data_size == -1) {
                        // error.
                    }


                    /* Sends the data to the client. */
                    while (data_sent != data_size) {

                        n = write(client_fd, response, data_size);
                        if (n == -1) {
                            // error.
                        }

                        data_sent += n;
                    }

                    /* Updates the total amount of data sent to the client. */
                    total_data_sent += data_sent;

                    /* Finds the right amount of bytes to read for the next 
                     * interation of the loop. */
                    to_read = ((total_Fsize - total_data_sent) > GS_BUF_SIZE) ? GS_BUF_SIZE : (total_Fsize - total_data_sent);

                    memset(response, 0, GS_BUF_SIZE);

                    /* Reads the next package and stores it in the response buffer. */
                    data_size = fread(response, 1, to_read, Fdata);


                    data_sent = 0; 
                }


                if (fclose(Fdata) == EOF) {
                    // error.
                }

                close(client_fd);

                /* Since tmp is a malloc'd string, we free its memory
                 * before ending this iteration. */
                free(tmp);
            }


            /* Ends the child process. */
            exit(EXIT_SUCCESS);
        }

        /* Parent process. */
        else {
            /* Closes the client socket, as we don't need it in
             * the parent process. */
            close(client_fd);

            continue;
        }
    }

    freeaddrinfo(res);
    close(fd); 
}



/**
 * @brief 
 *  Reads the message sent by the client and returns the server's response.
 * 
 * @param GSargs Server_args struct.
 * @param message Client's message.
 * 
 * @return Server response. 
 */
char* parse_player_message(server_args* GSargs, char* message) {
    char* response;

    char delim[] = STRTOK_DELIM;
    char* saveptr;

    /* Gets the command sent to the server, as well 
     * as the PLID. */
    char* command = strtok_r(message, delim, &saveptr);
    char* PLID = strtok_r(NULL, delim, &saveptr);


    /* If the verbose option is on, the server prints a 
     * description of the command received (PLID). */
    if (GSargs->is_verbose)
        printf("PLID = %s | ", PLID);


    if (strcmp(command, SERVER_START_COMMAND) == 0) {
        if (GSargs->is_verbose)
            printf("New game;\n");
        
        response = GS_start_game(GSargs, PLID);
    }


    else if (strcmp(command, SERVER_PLAY_COMMAND) == 0) {
        char* letter = strtok_r(NULL, delim, &saveptr);
        char* trial = strtok_r(NULL, delim, &saveptr);

        if (GSargs->is_verbose)
            printf("Play letter: '%s' [trial %s];\n", letter, trial);

        response = GS_play_letter(GSargs, PLID, letter, trial);
    }


    else if (strcmp(command, SERVER_GUESS_COMMAND) == 0) {
        char* word_guess = strtok_r(NULL, delim, &saveptr);
        char* trial = strtok_r(NULL, delim, &saveptr);

        if (GSargs->is_verbose)
            printf("Guess word: '%s' [trial %s];\n", word_guess, trial);

        response = GS_guess_word(GSargs, PLID, word_guess, trial);
    }


    else if (strcmp(command, SERVER_SCORE_COMMAND) == 0) {
        if (GSargs->is_verbose)
            printf("Send scoreboard;\n");
        
        response = GS_show_scoreboard(GSargs);
    }


    else if (strcmp(command, SERVER_HINT_COMMAND) == 0) {
        if (GSargs->is_verbose)
            printf("Send hint;\n");

        response = GS_show_hint(GSargs, PLID);
    }


    else if (strcmp(command, SERVER_STATE_COMMAND) == 0) {
        if (GSargs->is_verbose)
            printf("Send state;\n");

        response = GS_show_state(GSargs, PLID);
    }


    else if (strcmp(command, SERVER_QUIT_EXIT_COMMAND) == 0) {
        if (GSargs->is_verbose)
            printf("Exit game;\n");

        response = GS_quit_game(GSargs, PLID);
    }


    else {
        return NULL;
    }

    return response;
}



/**
 * @brief
 *  Gets a word (sequentially) from the
 *  word file specified before.
 * 
 * @param GSargs 
 * @return char* 
 */
char* get_word_line(server_args* GSargs) {
    FILE* word_file;

    size_t count = 0;

    char selected[GS_BUF_SIZE];
    size_t selected_len;

    char c;

    /* Opens the server's word file. */
    word_file = fopen(GSargs->word_file, "r");
    if (word_file == NULL) {
        return NULL;
    }

    /* Counts the number of lines in the file. */
    for (c = getc(word_file); c != EOF; c = getc(word_file)) {
        if (c == '\n') {
            count++;
        }
    }
    
    /* Counts the line number of the word sequentially. */
    int new_word_line;
    if (GSargs->curr_word_line == count) {
        new_word_line = 1;
    }
    else {
        new_word_line = GSargs->curr_word_line + 1;
    }

    GSargs->curr_word_line = new_word_line;

    /* Selects a random line number. */
    //size_t rand_line_number = rand() % count;

    /* Resets the file pointer to the start of the file. */
    rewind(word_file);

    count = 1;
    while (fgets(selected, GS_BUF_SIZE, word_file) != NULL) {
        /* If the selected line is the one we're looking for,
         * we break from the loop. */
        if (count == new_word_line) {
            break;
        }

        count++;
    }

    if (fclose(word_file) == EOF) {
        return NULL;
    }

    /* Returns an identical malloc'd string (freed later). */
    return strdup(selected);
}



/**
 * @brief 
 *  Calculates the maximum amount of errors
 *  for the word length passed.
 * 
 * @param word_len Length of the word to be guessed.
 * @return Max errors for the word.
 */
size_t get_max_errors(size_t word_len) {
    size_t max_errors;
    
    if (word_len <= 6) {
        max_errors = 7;
    }

    else if (word_len >= 7 && word_len <= 10) {
        max_errors = 8;
    }

    else {
        max_errors = 9;
    }

    return max_errors;
}



/**
 * @brief 
 *  Starts the game.
 * 
 * @param GSargs 
 * @param PLID 
 * @return the response the server will send to the player.
 */
char* GS_start_game(server_args* GSargs, char* PLID) {
    char buffer[GS_BUF_SIZE];

    char player_file[MAX_FILE_NAME_SIZE];

    /* Gets the name of the player file of the PLID given. */
    snprintf(player_file, MAX_FILE_NAME_SIZE, "game_%s.txt", PLID);

    /* Checks if that player file exists or not, to know if the
     * player has any ongoing games or not. */
    if (access(player_file, F_OK) == 0) {

        /* Since the player file exists, the player already has
         * an ongoing game sets the response status to "NOK". */
	    snprintf(buffer, GS_BUF_SIZE, "%s %s", SERVER_START_RESPONSE, STATUS_NOK);
    }
    else {
        /* The player file doesn't exist. The server will create one 
         * so the player can start a new game. */
        char delim[] = STRTOK_DELIM;
        char* saveptr;

        /* Gets a random word selected from the word_file, as well
         * as its respective hint file name. */
        char* tmp = get_word_line(GSargs);
        if (tmp == NULL) {
            printf("ERR(GS_start_game): error getting the word.\n");
            return NULL;
        }

        char* word = strtok_r(tmp, delim, &saveptr);
        char* hint_path = strtok_r(NULL, delim, &saveptr);
        
        size_t n_letters = strlen(word);

        /* Gets the maximum errors based on the length of
         * the word picked. */
        size_t max_errors = get_max_errors(n_letters);

        FILE* fp;

        /* Creates the player file. */
        fp = fopen(player_file, "w");
        if (fp == NULL) {
            return NULL;
        }


        /* Writes to the file the word and hint
         * path associated with it. */
        
        /* Buffer: word hint_path letters_guessed_correctly successful_guesses \n */
        fprintf(fp, "%s %s %02d %02d\n", word, hint_path, 0, 0);


        /* Closes the player file. */
        if (fclose(fp) == EOF) {
            return NULL;
        }


        /* Creates the message to be sent to the client,
         * setting the status to "OK". */
	    sprintf(buffer, "%s %s %zu %zu\n", SERVER_START_RESPONSE, STATUS_OK, n_letters, max_errors);

        if (GSargs->is_verbose)
            printf("--> Word: '%s' [%zu letters]\n", word, n_letters);

        /* Frees the memory allocated for the random line. */
        free(tmp);
    }


    /* Returns an identical malloc'd string (freed later). */
    return strdup(buffer);
}



/**
 * @brief 
 *  [AUXILIARY to GS_play_letter and GS_guess_word]
 *  Checks if the trial passed is valid
 *  or not (positive integer expected).
 * 
 * @param player_fp 
 * @param trial 
 * @return EXIT_SUCCESS if it's a valid trial (EXIT_FAILURE otherwise)
 */
size_t valid_trial(FILE* player_fp,  char* trial, char* guess, size_t is_play_command) {
    size_t i;
    const size_t size = strlen(trial);

    /* First we check if the trial is a positive number (>0). */
    for (i = 0; i < size; i++) {
        /* If any of the trial's charachters aren't
         * a digit, then it isn't a valid trial. */
        if (!isdigit(trial[i])) {
            return EXIT_FAILURE;
        }
    }

    /* We know the trial is valid, so we convert it into
     * an integer. */
    int trial_number = atoi(trial);

    size_t n_lines = 0;
    char c;

    /* Counts the number of lines in the file. */
    for (c = getc(player_fp); c != EOF; c = getc(player_fp)) {
        if (c == '\n') {
            n_lines++;
        }
    }

    /* If the trial isn't the same as the number of lines in the
     * player file then the trial is invalid. */
    if (trial_number != n_lines) {
        return EXIT_FAILURE;
    }


    char check_play[GS_BUF_SIZE], last_play[GS_BUF_SIZE];
    
    char code = is_play_command ? PLAY_CODE : GUESS_CODE;



    return EXIT_SUCCESS;
}



/**
 * @brief 
 *  [AUXILIARY to GS_play_letter and GS_guess_word]
 *  Checks if the current player play for the command is a
 *  duplicate or not (if the letter/word was sent in a previous
 *  trial).
 * 
 * @param player_fp A file pointer to the player's game file.
 * @param play The current player guess. 
 * @param is_play_command Flag that specifies if the command is PLAY or GUESS.
 * @return EXIT_SUCCESS if the play is a duplicate, EXIT_FAILURE otherwise.
 */
size_t is_dup_play(FILE* player_fp, char* play, size_t is_play_command) {
    char c;
    size_t dup_play = 0;

    size_t count = 0;
    char check_play[GS_BUF_SIZE];

    char code;
    

    /* Reads the first line of the player file as it contains info
     * about the word and not about the player's plays. */
    fgets(check_play, GS_BUF_SIZE, player_fp);

    /* Checks the current letter was sent in a previous play.  */
    while (fgets(check_play, GS_BUF_SIZE, player_fp) != NULL) {

        sscanf(check_play, "%c", &code);


        /* We're dealing with a PLG command so we'll only look at
         * the letter guesses.  */
        if (is_play_command && code == PLAY_CODE) {
            char prev_play;

            sscanf(check_play, "%*c %c\n", &prev_play);


            if (prev_play == play[0]) {
                /* The current play is a duplicate play. */
                return EXIT_SUCCESS;
            }
        }

        /* We're dealing with a PWG command so we'll only look at
         * the word guesses.  */
        else if (!is_play_command && code == GUESS_CODE) {
            char prev_play[MAX_WORD_SIZE+1];

            sscanf(check_play, "%*c %s\n", prev_play);

            if (strcmp(prev_play, play) == 0) {
                /* The current play is a duplicate play. */
                return EXIT_SUCCESS;
            }
        }

        
    }

    return EXIT_FAILURE;
}



/**
 * @brief
 * Processes the end of the game, updating the player's filename.
 * 
 * @param player_file 
 * @param end_code 
 * @param curr_trial 
 * @return EXIT_SUCCESS (or EXIT_FAILURE if an error occured)
 */
size_t process_game_end(char* player_file, const char end_code, size_t curr_trial) {
    FILE* player_fp;

    /* Opens the player file in read mode. */
    player_fp = fopen(player_file, "r");
    if (player_fp == NULL) {
        return EXIT_FAILURE;
    }

    char player_info[GS_BUF_SIZE];

    /* Gets the player's info. */
    fgets(player_info, GS_BUF_SIZE, player_fp);

    /* Closes the player file. */
    if (fclose(player_fp) == EOF) {
        return EXIT_FAILURE;
    }


    char word[MAX_WORD_SIZE + 1];
    size_t correct_guesses;

    /* Retrieves the player data needed. */
    sscanf (player_info, "%s %*s %*s %zu\n", word, &correct_guesses);


    /* Since the player invoked the quit or exit command,
     * we'll add that information to the game file. */
    if (end_code == QUIT_CODE) {
        
        /* Opens the player file in append mode. */
        player_fp = fopen(player_file, "a");
        if (player_fp == NULL) {
            return EXIT_FAILURE;
        }

        fprintf(player_fp, "%c\n", end_code);

        /* Closes the player file. */
        if (fclose(player_fp) == EOF) {
            return EXIT_FAILURE;
        }
    }


    char delim[] = "_.";
	char* saveptr;

    /* Creates a duplicate of the player filename. */
    char tmp_filename[strlen(player_file) + 1];
    strcpy(tmp_filename, player_file);

    /* Retrieves the PLID from the filename passed. */
    char* tmp_plid;
    tmp_plid = strtok_r(player_file, delim, &saveptr);
    tmp_plid = strtok_r(NULL, delim, &saveptr);

    char plid[MAX_PLID_SIZE + 1];
    strcpy(plid, tmp_plid);


    /* The player dir is: games/PLID (6 chars for 'games/'
     * MAX_PLID_SIZE for the PLID and 1 for the '\0'). */
    size_t player_dirname_size = 6 + MAX_PLID_SIZE + 1;
    char player_dirname[player_dirname_size];

    sprintf(player_dirname, "games/%s", plid);


    DIR* dir = opendir(player_dirname);

    /* The player directory exists. */
    if (dir) {
        closedir(dir);
    }

    /* The player directory does not exist. */
    else if (errno == ENOENT) {
        size_t check;

        /* Creates the player dir (with all the permissions on). */
        check = mkdir(player_dirname, 0777);
        if (check != 0) {
            return EXIT_FAILURE;
        }
    }

    /* opendir() failed. */
    else {
        return EXIT_FAILURE;
    }


    /* The new filename will follow the format:
     * - YYYYMMDD_HHMMSS_code.txt (making it 21 characters long)
     * We make it 22 to account for the '\0' at the end. */
    size_t new_filename_size = 22;
    char new_filename[new_filename_size];

    /* Get the current date and time. */
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);


    /* The destiny path for the file will have the following format:
     * - games/PLID/YYYYMMDD_HHMMSS_code.txt. */
    sprintf(new_filename, "games/%s/%d%02d%02d_%02d%02d%02d_%c.txt", plid, tm.tm_year + 1900,
            tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, end_code);


    /* Renames the player's file to the new destiny path name,
     * effectively moving it to that same path. */
    if (rename(tmp_filename, new_filename) != 0) {
        return EXIT_FAILURE;
    }


    /* ------------ calculate score/create score file ------------ */

    if (end_code == WIN_CODE) {
        /* A score file will only be created if the
         * player has won the game. */

        /* Determines the score for the player. */
        double tmp_score = ((double)correct_guesses / (double)curr_trial) * 100;
        int score = (int)tmp_score;

        if (score > 100) {
            score = 100;
        }

        else if (score < 0) {
            score = 0;
        }


        /* The score filename has the following format:
        * - scores/scr_PLID_DDMMYYYY_HHMMSS.txt (making it 38 characters long)
        * We make it 39 to account for the '\0' at the end. */
        size_t max_score_file_size = 39;
        char score_file_name[max_score_file_size];


        sprintf(score_file_name, "scores/%03d_%s_%02d%02d%d_%02d%02d%02d.txt", score, plid, tm.tm_mon + 1,
                tm.tm_mday, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);


        FILE* score_fp;

        score_fp = fopen(score_file_name, "w");
        if (score_fp == NULL) {
            return EXIT_FAILURE;
        }


        /* Score info (first line of the score file) has the following format:
        * - score PLID word n_succ n_trials (max of 56 characters, counting
        *   with the whitespaces, the '\n' and the '\0'). */
        size_t info_size = 3 + MAX_PLID_SIZE + MAX_WORD_SIZE + 2 + 9 + 5 + 1;
        char score_info[info_size];


        sprintf(score_info, "%d %s %s %zu %zu\n", score, plid, word, correct_guesses, curr_trial);

        /* Writes the score info to the score file. */
        size_t to_write = strlen(score_info);
        if (fwrite(score_info, 1, to_write, score_fp) != to_write) {
            return EXIT_FAILURE;
        }
        

        if (fclose(score_fp) == EOF) {
            return EXIT_FAILURE;
        }

    }


    return EXIT_SUCCESS;
}



/**
 * @brief
 *  [AUXILIARY to GS_play_letter]
 *  Checks if the letter guessed by the client is correct or not
 *  and creates the appropriate response to be sent.
 * 
 * @param player_file The player's game file.
 * @param letter Letter guessed by the client.
 * @param curr_trial Current player trial.
 * @param is_verbose 
 * 
 * @return The server's response to a 'play' command.
 */
char* check_correct_letter(char* player_file, char* letter, int curr_trial, size_t is_verbose) {
    FILE* player_fp;

    /* Opens the player file. */
    player_fp = fopen(player_file, "r+");
    if (player_fp == NULL) {
        return NULL;
    }


    /* Information we get from the player's game file. */
    char player_word[MAX_WORD_SIZE + 1];
    char hint_path[MAX_HINT_PATH_SIZE + 1];
    size_t letters_guessed, correct_guesses;

    char player_info[GS_BUF_SIZE];


    fgets(player_info, GS_BUF_SIZE, player_fp);

    /* Gets the player's info from their game file. */
    sscanf(player_info, "%s %s %zu %zu\n", player_word, hint_path, &letters_guessed, &correct_guesses);


    size_t word_len = strlen(player_word);
    size_t i, count = 0;

    /* String where we'll (possibly) store the letter's
     * posisitions in the word. */
    size_t max_positions_size = MAX_WORD_SIZE*2;
    char positions[max_positions_size + 1];
    positions[0] = 0;


    size_t tmp_size = 3;
    for (i = 0; i < word_len; i++) {
        /* If the current charachter is the same as the letter
         * guessed, we increment the counter and store the
         * charachter's position.*/

        if (tolower(player_word[i]) == tolower(letter[0])) {
            char tmp[tmp_size];
            sprintf(tmp, "%zu ", i+1);

            /* Store that position in our array. */
            strcpy(positions, strcat(positions, tmp));
            count++;
        }
    }


    /* Deletes the last whitespace of our positions
     * string and stores a '\0' in its place.  */
    positions[count*2 + 1] = 0;


    /* Resets the file pointer to the start of the file. */
    rewind(player_fp);


    char response_buffer[GS_BUF_SIZE];
    char code;

    /* Stores the indentifier for the current play (ex: "T a\n"). */
    size_t play_id_size = 4; // 1(char) + 1(whitespace) + 1(char) + 1('\n').
    char play_identifier[play_id_size + 1];

    size_t max_errors = get_max_errors(word_len);

    
    if (count == 0) {
        /* The player's guess was incorrect */
        size_t num_errors = curr_trial - correct_guesses;

        /* The player has no more attempts (OVR status). */
        if (num_errors == max_errors+1) {
            sprintf(response_buffer, "%s %s %d\n", SERVER_PLAY_RESPONSE, STATUS_OVR, curr_trial);
            code = FAIL_CODE;

            if (is_verbose) 
                printf("\t--> Incorrect guess [%s];\n", STATUS_OVR);
        }

        /* The player has more attempts (NOK status). */
        else {
            sprintf(response_buffer, "%s %s %d\n", SERVER_PLAY_RESPONSE, STATUS_NOK, curr_trial);
            code = PLAY_CODE;

            if (is_verbose) 
                printf("\t--> Incorrect guess [%s];\n", STATUS_NOK);
        }
        
    }

    else {
        /* The letter guessed is correct, so we update the player's game file. */
        letters_guessed += count;
        correct_guesses++;
        

        char* tmp_player_word = player_word;
        char* tmp_hint_path = hint_path;


        sprintf(player_info, "%s %s %02zu %02zu\n", tmp_player_word, tmp_hint_path, letters_guessed, correct_guesses);

        size_t size = strlen(player_info);
        player_info[size] = 0;

        
        
        if (fwrite(player_info, 1, size, player_fp) != size) {
            fclose(player_fp);
            return NULL;
        }

        /* Resets the file pointer to the start of the file. */
        rewind(player_fp);


        size_t max_n_pos_size = 2 + max_positions_size;
        char n_pos[max_n_pos_size + 1];
        /* Create the final string to be returned: [n pos*]. */
        sprintf(n_pos, "%zu %s", count, positions);

        char* tmp_n_pos = n_pos;

        /* The player has guessed the word (WIN status). */
        if (letters_guessed == word_len) {
            sprintf(response_buffer, "%s %s %d\n", SERVER_PLAY_RESPONSE, STATUS_WIN, curr_trial);
            code = WIN_CODE;

            if (is_verbose) 
                printf("\t--> Correct guess [%s];\n", STATUS_WIN);
        }
        
        /* The player's guess was correct (OK status). */
        else {
            sprintf(response_buffer, "%s %s %d %s\n", SERVER_PLAY_RESPONSE, STATUS_OK, curr_trial, tmp_n_pos);
            code = PLAY_CODE;

            if (is_verbose) 
                printf("\t--> Correct guess [%s];\n", STATUS_OK);
        }
    }
    

    if (fclose(player_fp) == EOF) {
        return NULL;
    }


    /* Re-opens the player file in append mode. */
    player_fp = fopen(player_file, "a");
    if (player_fp == NULL) {
        return NULL;
    }


    size_t size = strlen(play_identifier);
    if (fprintf(player_fp, "%c %s\n", code, letter) < 0) {
        fclose(player_fp);
        return NULL;
    }


    if (fclose(player_fp) == EOF) {
        return NULL;
    }


    /* If the player failed or guessed the word, we'll rename the player's
     * game file, create a score file and store them accordingly. */
    if (code == WIN_CODE || code == FAIL_CODE) {
        if (process_game_end(player_file, code, curr_trial) == EXIT_FAILURE) {
            printf("\tError renaming player's game file.\n");
        }
    }


    /* Returns an identical malloc'd string (freed later). */
    return strdup(response_buffer);
}



/**
 * @brief
 *  Checks if the letter is part of the word to be guessed.
 * 
 * @param GSargs 
 * @param PLID 
 * @param letter
 * @param trial 
 * @return the response the server will send to the player.
 */
char* GS_play_letter(server_args* GSargs, char* PLID, char* letter, char* trial) {
    char buffer[GS_BUF_SIZE];
    snprintf(buffer, GS_BUF_SIZE, "%s %s\n", SERVER_PLAY_RESPONSE, STATUS_ERR);

    char player_file[MAX_FILE_NAME_SIZE];

    /* Gets the name of the player file of the PLID given. */
    snprintf(player_file, MAX_FILE_NAME_SIZE, "game_%s.txt", PLID);

    /* Checks if that player file exists or not. */
    if (access(player_file, F_OK) == 0) {

        if (strlen(letter) != 1 || !isalpha(letter[0])) {
            return strdup(buffer);
        }


        FILE* player_fp;

        player_fp = fopen(player_file, "r");
        if (player_fp == NULL) {
            return strdup(buffer);
        }


        if (valid_trial(player_fp, trial, letter, 1) == EXIT_FAILURE) {
            if (GSargs->is_verbose) 
                printf("\t--> Invalid guess [%s];\n", STATUS_INV);

            /* Invalid trial number. */
            snprintf(buffer, GS_BUF_SIZE, "%s %s %s\n", SERVER_PLAY_RESPONSE, STATUS_INV, trial);
            fclose(player_fp);
            return strdup(buffer);
        }


        if (fclose(player_fp) == EOF) {
            return strdup(buffer);
        }


        player_fp = fopen(player_file, "r");
        if (player_fp == NULL) {
            return strdup(buffer);
        }


        if (is_dup_play(player_fp, letter, 1) == EXIT_SUCCESS) {
            if (GSargs->is_verbose) 
                printf("\t--> Duplicate guess [%s];\n", STATUS_INV);

            /* Duplicate play. */
            snprintf(buffer, GS_BUF_SIZE, "%s %s %s\n", SERVER_PLAY_RESPONSE, STATUS_DUP, trial);
            fclose(player_fp);
            return strdup(buffer);
        }


        if (fclose(player_fp) == EOF) {
            return strdup(buffer);
        }


        int curr_trial = atoi(trial);

        char* tmp  = check_correct_letter(player_file, letter, curr_trial, GSargs->is_verbose);
        if (tmp == NULL) {
            return strdup(buffer);
        }


        strcpy(buffer, tmp);

        /* Frees the memory allocated. */
        free(tmp);
    }    

    return strdup(buffer);
}


/**
 * @brief
 *  [AUXILIARY to GS_guess_word]
 *  Checks if the word is equivalent to the word to be guessed.
 * 
 * @param player_file 
 * @param word_guess 
 * @param curr_trial
 * @param is_verbose 
 * @return char*
 */
char* check_correct_word(char* player_file, char* word_guess, int curr_trial, size_t is_verbose) {
    FILE* player_fp;

    /* Opens the player file. */
    player_fp = fopen(player_file, "r+");
    if (player_fp == NULL) {
        return NULL;
    }


    /* Information we get from the player's game file. */
    char player_word[MAX_WORD_SIZE + 1];
    char hint_path[MAX_HINT_PATH_SIZE + 1];
    size_t letters_guessed, correct_guesses;


    char player_info[GS_BUF_SIZE];


    fgets(player_info, GS_BUF_SIZE, player_fp);
    /* Gets the player's info from their game file. */
    sscanf(player_info, "%s %s %zu %zu\n", player_word, hint_path, &letters_guessed, &correct_guesses);


    size_t word_len = strlen(player_word);
    size_t max_errors = get_max_errors(word_len);
    

    char response_buffer[GS_BUF_SIZE];
    char code;

    /* Stores the indentifier for the current play (ex: "G banana\n"). */
    size_t play_id_size = 2 + MAX_WORD_SIZE + 1; // 1(char) + 1(whitespace) + 1(char) + 1('\n').
    char play_identifier[play_id_size+1];

    
    if (strcasecmp(player_word, word_guess) != 0) {
        /* The player's guess was incorrect */
        size_t num_errors = curr_trial - correct_guesses;

        /* The player has no more attempts (OVR status). */
        if (num_errors == max_errors+1) {
            sprintf(response_buffer, "%s %s %d\n", SERVER_GUESS_RESPONSE, STATUS_OVR, curr_trial);
            code = FAIL_CODE;

            if (is_verbose) 
                printf("\t--> Correct guess [%s];\n", STATUS_OVR);
        }

        /* The player has more attempts (NOK status). */
        else {
            sprintf(response_buffer, "%s %s %d\n", SERVER_GUESS_RESPONSE, STATUS_NOK, curr_trial);
            code = GUESS_CODE;

            if (is_verbose) 
                printf("\t--> Incorrect guess [%s];\n", STATUS_NOK);
        }
    }

    else {
        /* The guess is correct, so we update the player's game file. */
        letters_guessed = word_len;
        correct_guesses++;

        char* tmp_player_word = player_word;
        char* tmp_hint_path = hint_path;


        /* Resets the file pointer to the start of the file. */
        rewind(player_fp);
        

        sprintf(player_info, "%s %s %02zu %02zu\n", tmp_player_word, tmp_hint_path, letters_guessed, correct_guesses);

        size_t size = strlen(player_info);
        if (fwrite(player_info, 1, size, player_fp) != size) {
            fclose(player_fp);
            return NULL;
        }


        /* The player has guessed the word (WIN status). */
        sprintf(response_buffer, "%s %s %d\n", SERVER_GUESS_RESPONSE, STATUS_WIN, curr_trial);
        code = WIN_CODE;

        if (is_verbose) 
            printf("\t--> Correct guess [%s];\n", STATUS_WIN);
    }
    

    if (fclose(player_fp) == EOF) {
        return NULL;
    }


    /* Creates the play identifier (to store in the player's game file). */
    sprintf(play_identifier, "%c %s\n", code, word_guess);

    /* Re-opens the player file in append mode. */
    player_fp = fopen(player_file, "a");
    if (player_fp == NULL) {
        return NULL;
    }


    size_t size = strlen(play_identifier);
    if (fwrite(play_identifier, 1, size, player_fp) != size) {
        return NULL;
    }


    if (fclose(player_fp) == EOF) {
        return NULL;
    }


    /* If the player failed or guessed the word, we'll rename the player's
     * game file, create a score file and store them accordingly. */
    if (code == WIN_CODE || code == FAIL_CODE) {
        if (process_game_end(player_file, code, curr_trial) == EXIT_FAILURE) {
            PRINT_ERR_MESS("Failed to rename player's game file.\n");
            return NULL;
        }
    }


    /* Returns an identical malloc'd string (freed later). */
    return strdup(response_buffer);
}


/**
 * @brief
 *  Checks if the word is equivalent to the word to be guessed.
 * 
 * @param GSargs 
 * @param PLID 
 * @param word_guess
 * @param trial 
 * @return the response the server will send to the player.
 */
char* GS_guess_word(server_args* GSargs, char* PLID, char* word_guess, char* trial) {
    char buffer[GS_BUF_SIZE];
    snprintf(buffer, GS_BUF_SIZE, "%s %s\n", SERVER_GUESS_RESPONSE, STATUS_ERR);

    char player_file[MAX_FILE_NAME_SIZE];

    /* Gets the name of the player file of the PLID given. */
    snprintf(player_file, MAX_FILE_NAME_SIZE, "game_%s.txt", PLID);


    /* Checks if that player file exists or not. */
    if (access(player_file, F_OK) == 0) {
        
        /* Check for invalid syntax. */
        if (strlen(word_guess) == 1) {
            if (GSargs->is_verbose) 
                printf("\t--> [%s]\n", STATUS_ERR);
            return strdup(buffer);
        }

        FILE* player_fp;

        player_fp = fopen(player_file, "r");
        if (player_fp == NULL) {
            return NULL;
        }

        /* Invalid trial number. */
        if (valid_trial(player_fp, trial, word_guess, 0) == EXIT_FAILURE) {
            if (GSargs->is_verbose) 
                printf("\t--> Invalid guess [%s];\n", STATUS_INV);

            snprintf(buffer, GS_BUF_SIZE, "%s %s %s\n", SERVER_GUESS_RESPONSE, STATUS_INV, trial);
            fclose(player_fp);
            return strdup(buffer);
        }

        /* Resets the file pointer to the start of the file. */
        rewind(player_fp);

        /* Duplicate play. */
        if (is_dup_play(player_fp, word_guess, 0) == EXIT_SUCCESS) {
            if (GSargs->is_verbose) 
                printf("\t--> Duplicate guess [%s];\n", STATUS_DUP);

            snprintf(buffer, GS_BUF_SIZE, "%s %s %s\n", SERVER_GUESS_RESPONSE, STATUS_DUP, trial);
            fclose(player_fp);
            return strdup(buffer);
        }

        if (fclose(player_fp) == EOF) {
            return strdup(buffer);
        }


        int curr_trial = atoi(trial);

        char* tmp = check_correct_word(player_file, word_guess, curr_trial, GSargs->is_verbose);
        if (tmp == NULL) {
            return strdup(buffer);
        }

        strcpy(buffer, tmp);

        /* Frees the memory allocated. */
        free(tmp);
        return strdup(buffer);
    }
    
    if (GSargs->is_verbose) 
            printf("\t--> [%s]\n", STATUS_ERR);

    return strdup(buffer);
}



/**
 * @brief
 *  [AUXILIARYY FUNCTION FOR GS_guess_word()]
 *  Updates the scoreboard struct with the scores of
 *  the information from the finished games.
 * 
 * @param GSargs Server_args struct.
 * 
 * @return TODO 
 */
size_t update_scoreboard(server_args* GSargs) {

    char Fname[GS_BUF_SIZE];
    size_t n_entries, i_file = 0;


    /* struct where the score files will be accessed. */
    struct dirent **filelist;
    n_entries = scandir("scores/", &filelist, 0, alphasort);


    scoreboard* scoreboard = GSargs->GSscoreboard;


    FILE *score_fp;

    /* No game has yet to be finished. */
    if (n_entries <= 2) {
        return 0;
    }

    else {
        size_t i = n_entries - 1;


        for (i; i >= 0; i--) {
            
            /* Makes sure the score file's name is valid. */
            if (filelist[i]->d_name[0] != '.') {
                
                char* tmp = filelist[i]->d_name;
                sprintf(Fname, "scores/%s", tmp);

                score_fp = fopen(Fname, "r");
                
                if (score_fp != NULL) {

                    /* Fills the scoreboard with all the values needed. */
                    fscanf(score_fp, "%zu %s %s %zu %zu",
                           &scoreboard->score[i_file],
                           scoreboard->PLID[i_file],
                           scoreboard->word[i_file],
                           &scoreboard->n_succ[i_file],
                           &scoreboard->n_total[i_file]);
                    

                    fclose(score_fp);
                    i_file++;
                }

                else {
                    // error opening file.
                }
            }

            /* Frees the score file we finished analyzing. */
            free(filelist[i]);


            /* When the scoreboard is filled with SCOREBOARD_SIZE entries
             * the loop is ended. */
            if (i_file == SCOREBOARD_SIZE) {
                break;
            }
        }

        free(filelist);
    }

    /* Updates the number of entries the scoreboard has. */
    scoreboard->n_scores = i_file;
    printf("n_scores = %zu\n", scoreboard->n_scores);

    return i_file;
}


/**
 * @brief
 *  [AUXILIARY to build_scoreboard]
 *  Builds one scoreboard line of the top-10 scoreboard.
 * 
 * @param GSargs
 * @param line
 * @return TODO
 */
char* build_scoreboard_line(server_args* GSargs, size_t line) {
    scoreboard* scoreboard = GSargs->GSscoreboard;

    char scoreboard_line[GS_BUF_SIZE];

    /* Parses the scoreboard info and creates the string 
     * that'll represent the line passed. */
    sprintf(scoreboard_line, "%03zu %s %s %zu %zu",
            scoreboard->score[line],
            scoreboard->PLID[line],
            scoreboard->word[line],
            scoreboard->n_succ[line],
            scoreboard->n_total[line]);

    /* Returns an identical malloc'd string (freed later). */
    return strdup(scoreboard_line);
}


/**
 * @brief
 *  [AUXILIARY to GS_show_scoreboard]
 *  Builds the top-10 scoreboard.
 * 
 * @param GSargs 
 * @return TODO
 */
char* build_scoreboard(server_args* GSargs) {
    scoreboard* scoreboard = GSargs->GSscoreboard;

    char Fname[64];

    pid_t pid = getpid();


    /* Stores the top scores file name based on the
     * process ID from before. */
    sprintf(Fname, "topscores_%07d.txt", pid);


    FILE* score_fp;

    /* Creates the file where the top scores will be stored. */
    score_fp = fopen(Fname, "w");

    if (score_fp == NULL) {
        return NULL;
    }


    size_t i, n_entries = scoreboard->n_scores;

    if (n_entries == 0) {
        return NULL;
    }

    fprintf(score_fp, "[Top#] | Score | PLID | Word | Succ. tries | Total tries\n");

    /* Writes to the top scores file the parsed version of
     * each of the scoreboard's lines. */
    for (i = 0; i < n_entries; i++) {
        char* scoreboard_line = build_scoreboard_line(GSargs, i);

        fprintf(score_fp, "[%zu] %s\n", i+1, scoreboard_line);

        /* Frees the scoreboard line's memory as it was malloc'd. */
        free(scoreboard_line);
    }


    if (fclose(score_fp) == EOF) {
        return NULL;
    }


    /* Returns an identical malloc'd string (freed later). */
    return strdup(Fname);
}



/**
 * @brief
 *  Builds the top-10 scoreboard to send to the player.
 * 
 * @param GSargs 
 * @return the response the server will send to the player.
 */
char* GS_show_scoreboard(server_args* GSargs) {
    char buffer[GS_BUF_SIZE];
    sprintf(buffer, "%s %s\n", SERVER_SCORE_RESPONSE, STATUS_EMPTY);


    if (update_scoreboard(GSargs) == 0) {
        if (GSargs->is_verbose)
            printf("\t--> [%s];\n", STATUS_EMPTY);
        return strdup(buffer);
    }


    char* Fname = build_scoreboard(GSargs);
    if (Fname == NULL) {
        if (GSargs->is_verbose)
            printf("\t--> [%s];\n", STATUS_EMPTY);
        return strdup(buffer);
    }


    sprintf(buffer, "%s %s %s\n", SERVER_SCORE_RESPONSE, STATUS_OK, Fname);

    free(Fname);

    /* Returns an identical malloc'd string (freed later). */
    return strdup(buffer);
}


/**
 * @brief
 *  Builds the top-10 scoreboard to send to the player.
 * 
 * @param GSargs
 * @param PLID
 * @return the response the server will send to the player.
 */
char* GS_show_hint(server_args* GSargs, char* PLID) {
    char buffer[GS_BUF_SIZE];
    sprintf(buffer, "%s %s\n", SERVER_HINT_RESPONSE, STATUS_NOK);

    char player_file[MAX_FILE_NAME_SIZE];

    /* Gets the name of the player file of the PLID given. */
    sprintf(player_file, "game_%s.txt", PLID);


    /* Checks if that player file exists or not. */
    if (access(player_file, F_OK) == 0) {

        /* Opens the player's file to read its info (first line). */
        FILE* fp;

        fp = fopen(player_file, "r");
        if (fp == NULL) {
            return strdup(buffer);
        }


        /* Information we get from the player's game file: hint path. */
        char Fname[MAX_HINT_PATH_SIZE + 1];

        fscanf(fp, "%*s %s", Fname);


        if (fclose(fp) == EOF) {
            return strdup(buffer);
        }


        sprintf(buffer, "%s %s %s\n", SERVER_HINT_RESPONSE, STATUS_OK, Fname);
    }

    /* Returns an identical malloc'd string (freed later). */
    return strdup(buffer);
}


/**
 * @brief
 *  Parses a line from a player's game file.
 * 
 * @param line The line to be parsed.
 * 
 * @return The parsed line.
 */
char* parse_game_file_line(char* line) {

    char code;
    char guess[MAX_WORD_SIZE + 1];

    /* Reads the desired data from the line passed. */
    sscanf(line, "%c %s", &code, guess);


    char state_line[GS_BUF_SIZE];

    /* Chooses how to parse the line. */
    switch (code) {

        case PLAY_CODE:
            sprintf(state_line, "Letter guess: [%s].", guess);
            break;

        case GUESS_CODE:
            sprintf(state_line, "Word guess: [%s].", guess);
            break;

        case WIN_CODE:
            sprintf(state_line, "Won the game, final guess: [%s].", guess);
            break;

        case FAIL_CODE:
            sprintf(state_line, "Lost the game, final guess: [%s].", guess);
            break;

        case QUIT_CODE:
            sprintf(state_line, "Quit the game.");
            break;
    }

    
    /* Returns an identical malloc'd string (freed later). */
    return strdup(state_line);
}


/**
 * @brief
 *  Gets the last game played by the plyer with
 *  the PLID passed..
 * 
 * @param PLID Player's ID.
 * @param Fname Buffer where the file path will be stored.
 * 
 * @return 1 if a game was found, 0 otherwise.
 */
size_t get_last_game(char *PLID , char* Fname) {

    char dirname[20];
    sprintf(dirname, "games/%s/", PLID);


    size_t n_entries, found = 0;

    /* struct where the score files will be accessed. */
    struct dirent **filelist;
    n_entries = scandir(dirname, &filelist, 0, alphasort);

    if (n_entries <= 0) {
        return 0;
    }
    
    else {
        size_t i;

        for (i = n_entries-1; i >= 0; i--) {

            /* Makes sure the score file's name is valid. */
            if (filelist[i]->d_name[0] != '.') {


                /* Stores in Fname the name of the player file just found. */
                sprintf(Fname, "games/%s/%s", PLID, filelist[i]->d_name);
                found = 1;
            }

            free(filelist[i]);

            if (found) {
                break;
            }
        }

        free(filelist);
    }

    return found;
}



/**
 * @brief
 *  Builds the state to send to the player.
 * 
 * @param GSargs 
 * @param PLID
 * @return the response the server will send to the player.
 */
char* GS_show_state(server_args* GSargs, char* PLID) {
    
    size_t FIN_flag = 0;

    char buffer[GS_BUF_SIZE];
    sprintf(buffer, "%s %s\n", SERVER_STATE_RESPONSE, STATUS_NOK);


    char player_file[MAX_FILE_NAME_SIZE];

    

    sprintf(player_file, "game_%s.txt", PLID);

    /* Checks if that player has any ongoing games. */
    if (access(player_file, F_OK) == 0) {
        
        // Nothing to do.
    }

    else {
        /* Clears the player file buffer. */
        memset(player_file, 0, MAX_FILE_NAME_SIZE);


        /* Since the player doesn't have any ongoing games,
         * we'll check the player's game file for any already
         * finished games. */
        

        if (get_last_game(PLID, player_file) == 0) {
            /* The player has no ongoing games nor any
            * finished ones.*/

            return strdup(buffer);
        }


        /* Lets the program know there are no ongoing
         * games for the PLID passed. */
        FIN_flag = 1;
    }


    /* A player game file has been found, its data
     * will be parsed to be sent to the client. */


    char Fname[MAX_FILE_NAME_SIZE];
    sprintf(Fname, "state_%s.txt", PLID);


    FILE* player_fp;

    player_fp = fopen(player_file, "r");
    if (player_fp == NULL) {
        // error.
        return strdup(buffer);
    }


    FILE* state_fp;

    state_fp = fopen(Fname, "w");
    if (state_fp == NULL) {
        // error.
        return strdup(buffer);
    }


    size_t trial = 1;
    char state_line[GS_BUF_SIZE];

    /* Discards the first line of the file. */
    fgets(state_line, GS_BUF_SIZE, player_fp);


    if (FIN_flag) {
        fprintf(state_fp, "Finished game:\n");
    }

    else {
        fprintf(state_fp, "Ongoing game:\n");
    }
    
    /* Parses each line from the player'sgame file. */
    while(fgets(state_line, GS_BUF_SIZE, player_fp)) {
        char* tmp_state_line = parse_game_file_line(state_line);

        fprintf(state_fp, "Trial [%zu]: %s\n", trial, tmp_state_line);

        trial++;

        free(tmp_state_line);
    }


    if (fclose(state_fp) == EOF) {
        // error.
        return strdup(buffer);
    }


    if (fclose(player_fp) == EOF) {
        // error.
        return strdup(buffer);
    }


    if (FIN_flag) {
        sprintf(buffer, "%s %s %s\n", SERVER_STATE_RESPONSE, STATUS_FIN, Fname);
    }

    else {
        sprintf(buffer, "%s %s %s\n", SERVER_STATE_RESPONSE, STATUS_ACT, Fname);
    }
    
    
    return strdup(buffer);
}


/**
 * @brief
 *  Quits the game.
 * 
 * @param GSargs
 * @param PLID
 * @return the response the server will send to the player.
 */
char* GS_quit_game(server_args* GSargs, char* PLID) {
    char buffer[GS_BUF_SIZE];
    sprintf(buffer, "%s %s\n", SERVER_QUIT_EXIT_RESPONSE, STATUS_ERR);

    char player_file[MAX_FILE_NAME_SIZE];

    sprintf(player_file, "game_%s.txt", PLID);
    
    /* Checks if that player has ongoing games. */
    if (access(player_file, F_OK) == 0) {
        
        FILE* player_fp;

        player_fp = fopen(player_file, "r");
        if (player_fp == NULL) {
            return strdup(buffer);
            
        }
        
        size_t curr_trial = 0;
        char c;

        /* Counts the number of lines in the player's game
         * file to know what the current trial i. */
        for (c = getc(player_fp); c != EOF; c = getc(player_fp)) {
            if (c == '\n') {
                curr_trial = curr_trial + 1;
            }
        }

        if (fclose(player_fp) == EOF) {
            return strdup(buffer);
        }

        if (process_game_end(player_file, QUIT_CODE, curr_trial) == EXIT_SUCCESS) {
            sprintf(buffer, "%s %s\n", SERVER_QUIT_EXIT_RESPONSE, STATUS_OK);
        }
    }

    else {
        sprintf(buffer, "%s %s\n", SERVER_QUIT_EXIT_RESPONSE, STATUS_NOK);
    }


    /* Returns an identical malloc'd string (freed later). */
    return strdup(buffer);
}
