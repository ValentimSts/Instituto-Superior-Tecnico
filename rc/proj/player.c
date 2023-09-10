
/**
 * @file player.c
 * @author 
 * 	Group 36 (Filipa Magalhaes & Valentim Santos)
 * 
 * @brief
 * 	
 * 
 */


/* Include wall. */
#include "common/common.h"
#include <limits.h>


/* Command invoked to start the program. */
#define IP_COMMAND "-n"    // specifies the ip address (optional).


/* Client side (player known) commands. */
#define PLAYER_START_COMMAND "start"      // start PLID
#define PLAYER_SG_COMMAND "sg"            // sg PLID
#define PLAYER_PLAY_COMMAND "play"        // play letter
#define PLAYER_PL_COMMAND "pl"            // pl letter
#define PLAYER_GUESS_COMMAND "guess"      // guess word
#define PLAYER_GW_COMMAND "gw"            // gw word
#define PLAYER_SCORE_COMMAND "scoreboard" // scoreboard
#define PLAYER_SB_COMMAND "sb"            // sb
#define PLAYER_HINT_COMMAND "hint"        // hint
#define PLAYER_H_COMMAND "h"              // h
#define PLAYER_STATE_COMMAND "state"      // state
#define PLAYER_ST_COMMAND "st"            // st
#define PLAYER_QUIT_COMMAND "quit"        // quit
#define PLAYER_EXIT_COMMAND "exit"        // exit


/* Client side printed messages. */
#define GAME_OVER "-----------------------\n   G A M E   O V E R.  \n-----------------------\n\n"
#define PRINT_PL_OK(letter) printf("You are correct! The letter '%c' is part of the word.\n", letter);
#define PRINT_PL_WIN printf("Well done! You guessed the word.\n");
#define PRINT_PL_DUP printf("Your guess is duplicate (this trial didn't count).\n");
#define PRINT_PL_INV printf("Invalid guess (this trial didn't count).\n");
#define PRINT_PL_NOK(letter) printf("The letter '%c' isn't part of the word. Try again.\n", letter);
#define PRINT_PL_NOK_WORD(word) printf("'%s' is not the correct word. Try again.\n", word);
#define PRINT_PL_OVR(letter) printf("The letter '%c' isn't part of the word.\n [You've reached the maximum number of errors.]\n%s", letter, GAME_OVER);
#define PRINT_PL_OVR_WORD(word) printf("'%s' is not the correct word.\n [You've reached the maximum number of errors.]\n%s", word, GAME_OVER);


#define MAX_GSIP_SIZE 128

/* Socket timeout (in seconds). */
#define SOCKET_TIMEOUT 10 



/**
 * @brief Struct player_args.
 * 	Stores all the relevant data of each player:
 */
typedef struct player_args {
	/* The IP address given by the player
	 * (IP of the machine if ommited). */
	char GSIP[MAX_GSIP_SIZE];

	/* The port given by the player
	 * (GS_DEFAULT_PORT if ommited). */
	char GSport[MAX_GSPORT_SIZE];
	
	/* The player ID. */
	char PLID[MAX_PLID_SIZE + 1]; // +1 to account for the '\0'

	/* The maximum amount of tries the
	 * player has to guess the word. */
	size_t errors_left;

	/* The current guess the player is on. */
	size_t curr_try;

	/* The size of the word to guess*/
	size_t word_size;

	/* The current state of the word to guess.
	 * Keeps track of the letters the player has guessed. */
	char word[MAX_WORD_SIZE + 1];

} player_args;


player_args* init_player_args();
void free_player_args(player_args* PLargs);
void print_game_state(player_args* PLargs, size_t is_new_game, size_t game_ended);

void get_player_args(player_args* PLargs, int n_args, char *args[]);
void compute_player_commands(player_args* PLargs);

size_t write_to_file(int fd, const char* Fname, size_t Fsize, size_t n);

void start_game(player_args* PLargs);
void play_letter(player_args* PLargs, char letter);
void guess_word(player_args* PLargs, char* word);
void show_scoreboard(player_args* PLargs);
void show_hint(player_args* PLargs);
void show_state(player_args* PLargs);
size_t quit_game(player_args* PLargs);



int main(int argc, char *argv[]) {

	/* Initializes a new player. */
	player_args* new_player = init_player_args();
	
	/* Gets the player arguments passed by the user. */
	get_player_args(new_player, argc, argv);

	/* Waits for and computes the user's commands. */
	compute_player_commands(new_player);
	
	return 0;
}


/**
 * @brief 
 * 	Initializes a player_args struct with its
 * 	default values.
 * 
 * @return player_args 
 */
player_args* init_player_args() {
	/* Allocates memory for the player_args struct. */
	player_args* new_player = (player_args*) malloc(sizeof(player_args));

	/* Gets the name of the machine running the program. */
	char hostname[MAX_GSIP_SIZE];
	if (gethostname(hostname, MAX_GSIP_SIZE) != 0) {
		PRINT_ERR_MESS("Failed to gethostname. Please try again.\n");
		free_player_args(new_player);
		exit(EXIT_FAILURE);
	}
	
	memset(new_player->word, 0, MAX_PLID_SIZE + 1);

	/* Sets the player's fields to their default values. */
	strcpy(new_player->GSIP, hostname);
	strcpy(new_player->GSport, GS_DEFAULT_PORT);

	return new_player;
}


void free_player_args(player_args* PLargs) {
	free(PLargs);
}


/**
 * @brief
 * 	Computes the player input and sets the player_args
 *  struct's fields to its respective values (if specified).
 * 
 * @param PLargs the player_args struct.
 * @param n_args number of arguments passed.
 * @param args array of arguments.
 */
void get_player_args(player_args* PLargs, int n_args, char *args[]) {
	int error = 1;

	/* TODO: is "... -n(-p)" the same as ommiting the command? or is it invalid? */

	/* The player ommited both commands. */
	if (n_args == 1) {
		// Nothing to do.
		error = 0;
	}

	/* This would mean the player only invoked 
	 * one command (-n or -p). */
	else if (n_args == 3) {
		char* command = args[1];

		/* The player invoked the command '-n'. */
		if (strcmp(command, IP_COMMAND) == 0) {
			strcpy(PLargs->GSIP, args[2]);
			error = 0;
		}

		/* The player invoked the command '-p'. */
		else if (strcmp(command, PORT_COMMAND) == 0) {
			strcpy(PLargs->GSport, args[2]);
			error = 0;
		}
	}

	else if (n_args == 5) {
		/* Checks if the commands invoked are correct and, if so,
			* fills in the player fields accordingly. */
		if (strcmp(args[1], IP_COMMAND) == 0 &&
			strcmp(args[3], PORT_COMMAND) == 0) {

			strcpy(PLargs->GSIP, args[2]);
			strcpy(PLargs->GSport, args[4]);
			error = 0;
		}
	}

    /* The port chosen can't be a well-known port nor can it be
     * greater than 65536 (16 bit number). */
    if (atoi(PLargs->GSport) <= 1024 || atoi(PLargs->GSport) >= 65536) {
        PRINT_ERR_MESS("Invalid port. Please try again.\n");
        free_player_args(PLargs);
        exit(EXIT_FAILURE);
    }

	if (error) {
		PRINT_ERR_MESS("Invalid player arguments. Please try again.\n");
		free_player_args(PLargs);
		exit(EXIT_FAILURE);
	}
}



/**
 * @brief
 * 	Prints the current game state to the player.
 * 
 * @param PLargs Player_args struct.
 */
void print_game_state(player_args* PLargs, size_t is_new_game, size_t game_ended) {
	int i;

	if (is_new_game) {
		printf("=======================\n");
		printf("    N E W   G A M E!   \n - Player ID: %s\n - Maximum errors: %zu\n",
				PLargs->PLID, PLargs->errors_left);
		printf("=======================\n");
	}

	printf("Word: ");

	for (i = 0; i < PLargs->word_size; i++) {

		/* If we're on the last iteration. */
		if (i == PLargs->word_size - 1) { 
			if (isalpha(PLargs->word[i])) {
				printf("%c\n", toupper(PLargs->word[i]));
			}
			
			else {
				printf("_\n");
			}
		}

		else {
			if (isalpha(PLargs->word[i])) {
				printf("%c ", toupper(PLargs->word[i]));
			}

			else {
				printf("_ ");
			}
		}
	}

	/* The game has come to an end (WIN or OVR). */
	if (game_ended) {

		/* We clean the player's word buffer and reset the trial counter,
		 * for it to be ready for the next word. */
		memset(PLargs->word, 0, PLargs->word_size*sizeof(char));
		PLargs->curr_try = 0;
		printf("\n");
	}

	else {
		printf("[Errors left: %zu]\n\n", PLargs->errors_left);
	}
}



/**
 * @brief 
 * 	Waits for the player to invoke any of the available
 * 	commands and executes them accordingly.
 * 
 * @param PLargs Player_args struct.
 */
void compute_player_commands(player_args* PLargs) {
	int exit = 0;

	/* TODO: maybe have only one buffer? */
	char command_buffer[MAX_COMMAND_SIZE];
	char PLID_buffer[MAX_PLID_SIZE+1]; // +1 to account for the '\0'.
	char letter;
	char word_buffer[MAX_WORD_SIZE+1]; // +1 to account for the '\0'.
	size_t PLID;

	char test_buffer[GS_BUF_SIZE];

	while (!exit) {

		if (scanf("%s", command_buffer) == 1) {

			/* Start command ("start" or "sg"), followed by the PLID. */
			if (strcmp(command_buffer, PLAYER_START_COMMAND) == 0 ||
				strcmp(command_buffer, PLAYER_SG_COMMAND) == 0) {
				
				/* Reads the PLID passed by the player. */
				if (scanf("%zu", &PLID) == 1) {

					char plid_test[MAX_PLID_SIZE*2];

					sprintf(plid_test, "%zu", PLID);

					/* Checks if the PLID is of a valid size. */
					if (strlen(plid_test) <= MAX_PLID_SIZE) {
						sprintf(PLID_buffer, "%06zu", PLID);

						PLID_buffer[MAX_PLID_SIZE] = 0;

						/* Stores the PLID in its respective field. */
						strcpy(PLargs->PLID, PLID_buffer);
						start_game(PLargs);
					}

					else {
						printf("ERROR: The PLID must have a maximum of %d characters.\n", MAX_PLID_SIZE);
					}
				}

				else {
					PRINT_ERR_MESS("Failed to read PLID.\n");
				}
				

				/* Cleans the buffer for future usage. */
				memset(PLID_buffer, 0, MAX_PLID_SIZE+1);
			}


			/* Play command ("play" or "pl"), followed by the player's
			 * letter guess. */
			else if (strcmp(command_buffer, PLAYER_PLAY_COMMAND) == 0 ||
					 strcmp(command_buffer, PLAYER_PL_COMMAND) == 0) {

				/* Reads the letter passed by the player. */
				if (scanf("%s", test_buffer) == 1) {
					
					/* If the player tries to pass more than one
					 * character, an error message is thrown. */
					if (strlen(test_buffer) != 1) {
						PRINT_ERR_MESS("Play must be only one letter.\n");
					}

					else {
						letter = tolower(test_buffer[0]);
						play_letter(PLargs, letter);
					}

					/* Cleans the buffer for future usage. */
					memset(test_buffer, 0, GS_BUF_SIZE);
				}

				else {
					PRINT_ERR_MESS("Failed to read the letter.\n");
				}
			}


			/* Guess command ("guess" or "gw"), followed by the player's
			 * word guess. */
			else if (strcmp(command_buffer, PLAYER_GUESS_COMMAND) == 0 ||
					 strcmp(command_buffer, PLAYER_GW_COMMAND) == 0) {

				/* Reads the word passed by the player. */
				if (scanf("%s", test_buffer) == 1) {
					size_t word_size = strlen(test_buffer);

					/* Checks if the word passed is accepted (has between
					 * 3 and 30 letters). If it doesn't, an error is thrown. */
					if (word_size < MIN_WORD_SIZE || word_size > MAX_WORD_SIZE) {
						PRINT_ERR_MESS("Word must have between 3 and 30 letters.\n");
					}

					else {
						strcpy(word_buffer, test_buffer);			
						guess_word(PLargs, word_buffer);
					}
				}

				else {
					PRINT_ERR_MESS("Failed to read PLID.\n");
				}

				/* Cleans the buffer for future usage. */
				memset(word_buffer, 0, MAX_WORD_SIZE+1);
			}

			
			/* Scoreboard command ("scoreboard" or "sb"). */
			else if (strcmp(command_buffer, PLAYER_SCORE_COMMAND) == 0 ||
					 strcmp(command_buffer, PLAYER_SB_COMMAND) == 0) {
				
				show_scoreboard(PLargs);
			}


			/* Hint command ("hint" or "h"). */
			else if (strcmp(command_buffer, PLAYER_HINT_COMMAND) == 0 ||
					 strcmp(command_buffer, PLAYER_H_COMMAND) == 0) {

				show_hint(PLargs);
			}


			/* State command ("state" or "st"). */
			else if (strcmp(command_buffer, PLAYER_STATE_COMMAND) == 0 ||
					 strcmp(command_buffer, PLAYER_ST_COMMAND) == 0) {

				show_state(PLargs);
			}


			/* Quit command ("quit"). */
			else if (strcmp(command_buffer, PLAYER_QUIT_COMMAND) == 0) {
				quit_game(PLargs);
			}


			/* Exit command ("exit"). */
			else if (strcmp(command_buffer, PLAYER_EXIT_COMMAND) == 0) {
				if (quit_game(PLargs) == EXIT_SUCCESS) {
					exit = 1;
				}
			}
			

			else {
				PRINT_ERR_MESS("Invalid command. Please try again.\n");
			}


			/* Cleans the stdin for the next command. */
			fgets(test_buffer, GS_BUF_SIZE, stdin);

			memset(test_buffer, 0, GS_BUF_SIZE);
		}

		else {
			PRINT_ERR;
		}
	}
}



/**
 * @brief
 *	[UDP Connection]
 * 	Starts the game.
 * 
 * @param PLargs Player_args struct.
 */
void start_game(player_args* PLargs) {

	char message[GS_BUF_SIZE];
	char response[GS_BUF_SIZE];


	/* =========================== UDP SOCKET INITIALIZATION ======================== */

	int fd, errcode;
	ssize_t check;
	socklen_t addrlen;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;


	/* Initializes the UDP socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        PRINT_ERR;
        return;
    }


	/* Sets the socket options. */

	int true = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) < 0) {
		// Error setting socket options.
	}

	struct timeval timeout;      
    timeout.tv_sec = SOCKET_TIMEOUT;
    timeout.tv_usec = 0;
    
    if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		// Error setting socket options.
	}


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

	/* Gets the server's address info. */
    errcode = getaddrinfo(PLargs->GSIP, PLargs->GSport, &hints, &res);
    if (errcode != 0) {
        PRINT_ERR;
        return;
    }

	/* ============================================================================== */


	/* Creates and sends the message to the server. */
	sprintf(message, "%s %s\n", SERVER_START_COMMAND, PLargs->PLID);
	message[strlen(message)] = '\0';


    check = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);
    if (check == -1) {
        PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
        return;
    }


    addrlen = sizeof(addr);
    check = recvfrom(fd, response, GS_BUF_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
    if (check == -1) {
        PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
        return;
    }


	char delim[] = STRTOK_DELIM;
	char* saveptr;

	/* Extracts the data from the message received. */
	char* server_response = strtok_r(response, delim, &saveptr);

	/* Checks if the server response is the expected one (RSG). */
	if (strcmp(server_response, SERVER_START_RESPONSE) == 0) {
		char* status_message = strtok_r(NULL, delim, &saveptr);

		/* If the status is NOK, the player already has an ongoing game. */
		if (strcmp(status_message, STATUS_NOK) == 0) {
			printf("The player [%s] already has an ongoing game.\n", PLargs->PLID);
		}

		else {
			/* Gets the rest of the data from the server response. */
			char* tmp_word_size = strtok_r(NULL, delim, &saveptr);
			char* tmp_max_errors = strtok_r(NULL, delim, &saveptr);

			/* Fills in the player_args fields. */
			PLargs->word_size = atoi(tmp_word_size);
			PLargs->errors_left = atoi(tmp_max_errors);
			PLargs->curr_try = 0;

			/* Prints the "new game started" message.
			 * Using the 'is_new_game' flag set to 1.*/
			print_game_state(PLargs, 1, 0);
		}
	}

	else {
		PRINT_ERR;
	}

    freeaddrinfo(res);
    close(fd);
}



/**
 * @brief
 *	[UDP Connection]
 *	Plays a letter.
 * 
 * @param PLargs 
 * @param letter 
 */
void play_letter(player_args* PLargs, char letter) {

	char message[GS_BUF_SIZE];
	char response[GS_BUF_SIZE];


	/* =========================== UDP SOCKET INITIALIZATION ======================== */

	int fd, errcode;
	ssize_t check;
	socklen_t addrlen;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;


	/* Initializes the UDP socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        PRINT_ERR;
        return;
    }


	/* Sets the socket options. */

	int true = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) < 0) {
		// Error setting socket options.
	}

	struct timeval timeout;      
    timeout.tv_sec = SOCKET_TIMEOUT;
    timeout.tv_usec = 0;
    
    if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		// Error setting socket options.
	}


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

	/* Gets the server's address info. */
    errcode = getaddrinfo(PLargs->GSIP, PLargs->GSport, &hints, &res);
    if (errcode != 0) {
        PRINT_ERR;
        return;
    }

	/* ============================================================================== */


	/* Increment the current player trial. */
	PLargs->curr_try++;

	/* Creates and sends the message to the server. */
	sprintf(message, "%s %s %c %zu\n", SERVER_PLAY_COMMAND, PLargs->PLID, letter, PLargs->curr_try);
	message[strlen(message)] = '\0';


    check = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);
    if (check == -1) {
        PRINT_ERR;
        return;
    }


    addrlen = sizeof(addr);
    check = recvfrom(fd, response, GS_BUF_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
    if (check == -1) {
        PRINT_ERR;
        return;
    }


	char delim[] = STRTOK_DELIM;
	char* saveptr;

	char* command = strtok_r(response, delim, &saveptr);
	char* status = strtok_r(NULL, delim, &saveptr);
	char* trial = strtok_r(NULL, delim, &saveptr);

	size_t game_ended = 0;


    if (strcmp(status, STATUS_INV) == 0) {
		/* Decrement the current player trial. */
	 	PLargs->curr_try--;
		PRINT_PL_INV;
		print_game_state(PLargs, 0, game_ended);
	}
	
	else if (strcmp(status, STATUS_DUP) == 0) {
        /* Decrement the current player trial . */
	 	PLargs->curr_try--;
		PRINT_PL_DUP;
		print_game_state(PLargs, 0, game_ended);
    }
	
	/* The player won, end the game and print message. */
	else if (strcmp(status, STATUS_WIN) == 0) {

		/* Before ending the game, we fill in the player's word
		 * with the letter[s] guessed just now. */

		size_t i;

		for (i = 0; i < PLargs->word_size; i++) {
			
			if (!isalpha(PLargs->word[i]))
				PLargs->word[i] = letter;
		}

		game_ended = 1; // signals that the game has ended.
	 	PRINT_PL_WIN;
		print_game_state(PLargs, 0, game_ended);
	}

	else if (strcmp(status, STATUS_OVR) == 0) {
	 	/* End the game and print message. */

		game_ended = 1; // signals that the game has ended.
	 	PRINT_PL_OVR(letter);
	}

	else if (strcmp(status, STATUS_OK) == 0) {

		/* Before continuing the game, we fill in the player's word
		 * with the letter[s] guessed just now. */
		char* n_occurences = strtok_r(NULL, delim, &saveptr);
		size_t i, n = atoi(n_occurences);

		for (i = 0; i < n; i++) {
			char* tmp_pos = strtok_r(NULL, delim, &saveptr);

			size_t pos = atoi(tmp_pos);

			PLargs->word[pos-1] = letter;
		}

		PRINT_PL_OK(letter);
		print_game_state(PLargs, 0, game_ended);
	}
	
	else if (strcmp(status, STATUS_NOK) == 0) {
		PLargs->errors_left--;
		
		PRINT_PL_NOK(letter);
		print_game_state(PLargs, 0, game_ended);
	}
	
	else if (strcmp(status, STATUS_ERR) == 0) {
		/* Decrement the current player trial . */
	 	PLargs->curr_try--;
		PRINT_ERR;
    }

	else {
		/* Decrement the current player trial . */
	 	PLargs->curr_try--;
		PRINT_ERR;
	}


    freeaddrinfo(res);
    close(fd);
}


/**
 * @brief
 *	[UDP Connection]
 * 	Attempt to guess the word.
 * 
 * @param PLargs 
 * @param word 
 */
void guess_word(player_args* PLargs, char* word) {

	char message[GS_BUF_SIZE];
	char response[GS_BUF_SIZE];


	/* =========================== UDP SOCKET INITIALIZATION ======================== */

	int fd, errcode;
	ssize_t check;
	socklen_t addrlen;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;


	/* Initializes the UDP socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        PRINT_ERR;
        return;
    }


	/* Sets the socket options. */

	int true = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) < 0) {
		// Error setting socket options.
	}

	struct timeval timeout;      
    timeout.tv_sec = SOCKET_TIMEOUT;
    timeout.tv_usec = 0;
    
    if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		// Error setting socket options.
	}


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

	/* Gets the server's address info. */
    errcode = getaddrinfo(PLargs->GSIP, PLargs->GSport, &hints, &res);
    if (errcode != 0) {
        PRINT_ERR;
        return;
    }

	/* ============================================================================== */


	/* Increment the current player trial. */
	PLargs->curr_try++;

	/* Creates and sends the message to the server. */
	sprintf(message, "%s %s %s %zu\n", SERVER_GUESS_COMMAND, PLargs->PLID, word, PLargs->curr_try);
	message[strlen(message)] = '\0';

    check = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);
    if (check == -1) {
        PRINT_ERR;
        return;
    }


    addrlen = sizeof(addr);
    check = recvfrom(fd, response, GS_BUF_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
    if (check == -1) {
        PRINT_ERR;
        return;
    }


	char delim[] = STRTOK_DELIM;
	char* saveptr;

	char* command = strtok_r(response, delim, &saveptr);
	char* status = strtok_r(NULL, delim, &saveptr);
	char* trial = strtok_r(NULL, delim, &saveptr);

	size_t game_ended = 0;

	/* Checks if the server response is the expected one (RWG). */
	if (strcmp(command, SERVER_GUESS_RESPONSE) != 0) {
		PRINT_ERR;
	}

    if (strcmp(status, STATUS_INV) == 0) {
		/* Decrement the current player trial. */
	 	PLargs->curr_try--;
		PRINT_PL_INV;
	}
	
	else if (strcmp(status, STATUS_DUP) == 0) {
        /* Decrement the current player trial . */
	 	PLargs->curr_try--;
		PRINT_PL_DUP;
    }
	
	/* The player won, end the game and print message. */
	else if (strcmp(status, STATUS_WIN) == 0) {
		strcpy(PLargs->word, word);

		game_ended = 1; // signals that the game has ended.
	 	PRINT_PL_WIN;
	}

	else if (strcmp(status, STATUS_OVR) == 0) {
	 	/* End the game and print message. */

		game_ended = 1; // signals that the game has ended.
	 	PRINT_PL_OVR_WORD(word);
	}
	
	else if (strcmp(status, STATUS_NOK) == 0) {
		PLargs->errors_left--;

		PRINT_PL_NOK_WORD(word);
	}
	
	else if (strcmp(status, STATUS_ERR) == 0) {
		/* Decrement the current player trial . */
	 	PLargs->curr_try--;
		PRINT_ERR;
    }

	else {
		/* Decrement the current player trial . */
	 	PLargs->curr_try--;
		PRINT_ERR;
	}

	print_game_state(PLargs, 0, game_ended);

    freeaddrinfo(res);
    close(fd);
}


/**
 * @brief
 *	[UDP Connection]
 * 	Quits the game.
 * 
 * @param PLargs 
 * @return EXIT_SUCCESS (or EXIT_FAILURE if an error occured).
 */
size_t quit_game(player_args* PLargs) {

	char message[GS_BUF_SIZE];
	char response[GS_BUF_SIZE];


	/* =========================== UDP SOCKET INITIALIZATION ======================== */

	int fd, errcode;
	ssize_t check;
	socklen_t addrlen;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;


	/* Initializes the UDP socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        PRINT_ERR;
        return EXIT_FAILURE;
    }


	/* Sets the socket options. */

	int true = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) < 0) {
		// Error setting socket options.
	}

	struct timeval timeout;      
    timeout.tv_sec = SOCKET_TIMEOUT;
    timeout.tv_usec = 0;
    
    if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		// Error setting socket options.
	}


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

	/* Gets the server's address info. */
    errcode = getaddrinfo(PLargs->GSIP, PLargs->GSport, &hints, &res);
    if (errcode != 0) {
        PRINT_ERR;
        return EXIT_FAILURE;
    }

	/* ============================================================================== */


	/* Creates the message to be sent to the server. */
	sprintf(message, "%s %s\n", SERVER_QUIT_EXIT_COMMAND, PLargs->PLID);
	message[strlen(message)] = '\0';

	/* Sends the message to the server (in this case the PLID).*/
    check = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);
    if (check == -1) {
        PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
        return EXIT_FAILURE;
    }


    addrlen = sizeof(addr);
    check = recvfrom(fd, response, GS_BUF_SIZE, 0, (struct sockaddr*) &addr, &addrlen);
    if (check == -1) {
        PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
        return EXIT_FAILURE;
    }


	char delim[] = STRTOK_DELIM;
	char* saveptr;

	/* Extracts the data from the message received. */
	char* server_response = strtok_r(response, delim, &saveptr);

	/* Checks if the server response is the expected (RQT). */
	if (strcmp(server_response, SERVER_QUIT_EXIT_RESPONSE) == 0) {
		char* status = strtok_r(NULL, delim, &saveptr);

		/* There is an ongoing game. */
		if (strcmp(status, STATUS_OK) == 0) {
			memset(PLargs->word, 0, PLargs->word_size);
			PLargs->curr_try = 0;

			printf(GAME_OVER);
			freeaddrinfo(res);
    		close(fd);
			return EXIT_SUCCESS;
		}

		/* There is no ongoing game. */
		else if (strcmp(status, STATUS_NOK) == 0) {
			freeaddrinfo(res);
    		close(fd);
			return EXIT_SUCCESS;
		}

	}

	/* Error occured. */
	PRINT_ERR;
    freeaddrinfo(res);
    close(fd);

	return EXIT_FAILURE;
}


/**
 * @brief
 * [TCP Connection]
 * 	Requests to show the top-10 scoreboard.
 * 
 * @param PLargs 
 */
void show_scoreboard(player_args* PLargs) {

	char message[GS_BUF_SIZE];
	char response[GS_BUF_SIZE];


	/* =========================== TCP SOCKET INITIALIZATION ======================== */

	int fd, errcode;
	ssize_t check;
	socklen_t addrlen;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;

	/* Initializes the TCP socket. */
	fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        PRINT_ERR;
        return;
    }


	/* Sets the socket options. */

	int true = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) < 0) {
		// Error setting socket options.
	}

	struct timeval timeout;      
    timeout.tv_sec = SOCKET_TIMEOUT;
    timeout.tv_usec = 0;
    
    if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		// Error setting socket options.
	}


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

	/* Gets the server's address info. */
    errcode = getaddrinfo(PLargs->GSIP, PLargs->GSport, &hints, &res);
    if (errcode != 0) {
        PRINT_ERR;
        return;
    }

	/* Connects to TCP server. */
	check = connect(fd, res->ai_addr, res->ai_addrlen);
	if (check == -1) {
        PRINT_ERR;
        return;
    }

	/* ============================================================================== */

	/* Creates the message to be sent to the server. */
	sprintf(message, "%s\n", SERVER_SCORE_COMMAND);
	message[strlen(message)] = '\0';


	/* Sends the message to the server. */
    check = write(fd, message, strlen(message));
    if (check == -1) {
        PRINT_ERR;
        return;
    }


	/* Reads the message sent from the server. */
    check = read(fd, response, GS_BUF_SIZE);
    if (check == -1) {
        PRINT_ERR;
        return;
    }


	char header[GS_BUF_SIZE];
	size_t header_offset = 0;

	char delim[] = STRTOK_DELIM;
	char* saveptr;

	char* command = strtok_r(response, delim, &saveptr);

	sprintf(header + header_offset, "%s ", command);
	header_offset = strlen(header);


	/* Checks if the server response is the expected one (RHL). */
	if (strcmp(command, SERVER_SCORE_RESPONSE) != 0) {
		PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
		return;
	}


	char* status = strtok_r(NULL, delim, &saveptr);

	sprintf(header + header_offset, "%s ", status);
	header_offset = strlen(header);


    if (strcmp(status, STATUS_OK) != 0) {

		if (strcmp(status, STATUS_EMPTY) == 0) {
			printf("No game has been finished yet.\n");
		}

		else {
			PRINT_ERR;
		}

		freeaddrinfo(res);
    	close(fd);
		return;
	}


	size_t Fsize;
	char Fname[MAX_FILE_NAME_SIZE];
	char Fdata[GS_BUF_SIZE];

	char* tmp_Fname = strtok_r(NULL, delim, &saveptr);
	char* tmp_Fsize = strtok_r(NULL, delim, &saveptr);

	sprintf(header + header_offset, "%s %s ", tmp_Fname, tmp_Fsize);
	header_offset = strlen(header);


	/* Stores the Fname. */
	strcpy(Fname, tmp_Fname);
	Fsize = atoi(tmp_Fsize);

	char* Fdata_first_part = strtok_r(NULL, "", &saveptr);
	strcpy(Fdata, Fdata_first_part);


	FILE* score_file;
	
	score_file = fopen(Fname, "w"); 
	if (score_file == NULL) {
		PRINT_ERR;
	}


	size_t n = check - header_offset;
	
	/* Writes the first part of the data to the file. */
	if (fwrite(Fdata, 1, n, score_file) != n) {
		PRINT_ERR;
	}


	if (fclose(score_file) == EOF) {
		PRINT_ERR;
	}


	/* Writes to the file the rest of Fdata. */
	size_t remaining = write_to_file(fd, Fname, Fsize, n);

	if (n + remaining != Fsize) {
		PRINT_ERR;
	}


	/* Prints the scoreboard and its info to the terminal. */
	printf("The scoreboard file was stored in the current directory.\n");
	printf(" - File name: [%s];\n - File size: [%zu bytes].\n\n", Fname, Fsize);

	score_file = fopen(Fname, "r");
	if (score_file == NULL) {
		PRINT_ERR;
	}

	/* Print the scoreboard */
	char c = fgetc(score_file);
	while (c != EOF) {
		printf("%c", c);
		c = fgetc(score_file);
	}
	printf("\n");

	if (fclose(score_file) == EOF) {
		PRINT_ERR;
	}
}



/**
 * @brief
 * 	// TODO
 * 
 * @param fd 
 * @param Fname
 * @param Fsize
 * @param n
 * @return number of bytes written?? (or -1 if an error occured).
 */
size_t write_to_file(int fd, const char* Fname, size_t Fsize, size_t n) {

	size_t already_written = 0;
	already_written += n;

	FILE* fp;

	fp = fopen(Fname, "a");
	if (fp == NULL) {
		return -1;
	}

	size_t check, package_size, total = 0;

	char Fdata[GS_BUF_SIZE];


	while (already_written != Fsize) {
		/* Finds the number of bytes to read in this iteration,
		 * taking in account the number of bytes already read (n). */
		package_size = (Fsize - already_written) > GS_BUF_SIZE ? GS_BUF_SIZE : (Fsize - already_written);

		check = read(fd, Fdata, package_size);
		if (check == -1) {
			return -1;
		}

		if (fwrite(Fdata, 1, package_size, fp) != package_size) {
			return -1;
		}

		already_written += package_size;
		total += package_size;
	}

	if (fclose(fp) == EOF) {
		return -1;
	}
	

	return total;
}



/**
 * @brief 
 *	[TCP Connection]
 *	Asks to receive an image illustrating the class 
 *  to which the word belongs and saves it locally.
 * 
 * @param PLargs 
 */
void show_hint(player_args* PLargs) {
	
	char message[GS_BUF_SIZE];
	char response[GS_BUF_SIZE];


	/* =========================== TCP SOCKET INITIALIZATION ======================== */

	int fd, errcode;
	ssize_t check;
	socklen_t addrlen;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;

	/* Initializes the TCP socket. */
	fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        PRINT_ERR;
        return;
    }


	/* Sets the socket options. */

	int true = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) < 0) {
		// Error setting socket options.
	}

	struct timeval timeout;      
    timeout.tv_sec = SOCKET_TIMEOUT;
    timeout.tv_usec = 0;
    
    if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		// Error setting socket options.
	}


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

	/* Gets the server's address info. */
    errcode = getaddrinfo(PLargs->GSIP, PLargs->GSport, &hints, &res);
    if (errcode != 0) {
        PRINT_ERR;
        return;
    }

	/* Connects to TCP server. */
	check = connect(fd, res->ai_addr, res->ai_addrlen);
	if (check == -1) {
        PRINT_ERR;
        return;
    }

	/* ============================================================================== */


	/* Creates and sends the message to the server. */
	sprintf(message, "%s %s\n", SERVER_HINT_COMMAND, PLargs->PLID);
	message[strlen(message)] = '\0';


    check = write(fd, message, strlen(message));
    if (check == -1) {
        PRINT_ERR;
        return;
    }


	/* Read server's response. */
    check = read(fd, response, GS_BUF_SIZE);
    if (check == -1) {
        PRINT_ERR;
        return;
    }


	char header[GS_BUF_SIZE];
	size_t header_offset = 0;

	char delim[] = STRTOK_DELIM;
	char* saveptr;

	char* command = strtok_r(response, delim, &saveptr);

	sprintf(header + header_offset, "%s ", command);
	header_offset = strlen(header);


	/* Checks if the server response is the expected one (RHL). */
	if (strcmp(command, SERVER_HINT_RESPONSE) != 0) {
		PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
		return;
	}

	char* status = strtok_r(NULL, delim, &saveptr);


	sprintf(header + header_offset, "%s ", status);
	header_offset = strlen(header);

    if (strcmp(status, STATUS_NOK) == 0) {
		PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
		return;
	}

	else if (strcmp(status, STATUS_OK) == 0) {
		size_t Fsize;
		char Fname[MAX_FILE_NAME_SIZE];
		char Fdata[GS_BUF_SIZE];

		char* tmp_Fname = strtok_r(NULL, delim, &saveptr);
		char* tmp_Fsize = strtok_r(NULL, delim, &saveptr);

		sprintf(header + header_offset, "%s %s ", tmp_Fname, tmp_Fsize);
		header_offset = strlen(header);


		/* Temporarily stores the hint path in Fname. */
		strcpy(Fname, tmp_Fname);
		Fsize = atoi(tmp_Fsize);


		size_t Fdata_size = check - header_offset;
		memcpy(Fdata, response + header_offset, Fdata_size);


		FILE* file;
		
		file = fopen(Fname, "w"); 
		if (file == NULL) {
			PRINT_ERR;
			freeaddrinfo(res);
    		close(fd);
			return;
		}

		
		/* Writes the first part of the data to the file. */
		if (fwrite(Fdata, 1, Fdata_size, file) != Fdata_size) {
			PRINT_ERR;
			freeaddrinfo(res);
    		close(fd);
			return;
		}


		if (fclose(file) == EOF) {
			PRINT_ERR;
			freeaddrinfo(res);
    		close(fd);
			return;
		}


		/* Writes to the file the rest of Fdata. */
		size_t remaining = write_to_file(fd, Fname, Fsize, Fdata_size);

		if (Fdata_size + remaining != Fsize) {
			PRINT_ERR;
			freeaddrinfo(res);
    		close(fd);
			return;
		}


		printf("The hint file was stored in the current directory.\n");
		printf(" - File name: [%s];\n - File size: [%zu bytes].\n\n", Fname, Fsize);

	}

	else {
		PRINT_ERR;
	}
	

    freeaddrinfo(res);
    close(fd);
}


/**
 * @brief 
 *	[TCP Connection]
 *	Asks about the state of the ongoing game.
 * 
 * @param PLargs 
 */
void show_state(player_args* PLargs) {

	char message[GS_BUF_SIZE];
	char response[GS_BUF_SIZE];


	/* =========================== TCP SOCKET INITIALIZATION ======================== */

	int fd, errcode;
	ssize_t check;
	socklen_t addrlen;
	struct addrinfo hints, *res;
	struct sockaddr_in addr;

	/* Initializes the TCP socket. */
	fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        PRINT_ERR;
        return;
    }


	/* Sets the socket options. */

	int true = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) < 0) {
		// Error setting socket options.
	}

	struct timeval timeout;      
    timeout.tv_sec = SOCKET_TIMEOUT;
    timeout.tv_usec = 0;
    
    if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		// Error setting socket options.
	}


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

	/* Gets the server's address info. */
    errcode = getaddrinfo(PLargs->GSIP, PLargs->GSport, &hints, &res);
    if (errcode != 0) {
        PRINT_ERR;
        return;
    }

	/* Connects to TCP server. */
	check = connect(fd, res->ai_addr, res->ai_addrlen);
	if (check == -1) {
        PRINT_ERR;
        return;
    }

	/* ============================================================================== */


	/* Creates the message to be sent to the server. */
	sprintf(message, "%s %s\n", SERVER_STATE_COMMAND, PLargs->PLID);
	message[strlen(message)] = '\0';

	/* Sends the message to the server (in this case the PLID).*/
    check = write(fd, message, strlen(message));
    if (check == -1) {
        PRINT_ERR;
        return;
    }


	/* Reads the message sent from the server. */
    check = read(fd, response, GS_BUF_SIZE);
    if (check == -1) {
        PRINT_ERR;
        return;
    }

	
	char header[GS_BUF_SIZE];
	size_t header_offset = 0;

	char delim[] = STRTOK_DELIM;
	char* saveptr;

	char* command = strtok_r(response, delim, &saveptr);

	sprintf(header + header_offset, "%s ", command);
	header_offset = strlen(header);


	/* Checks if the server response is the expected one (RHL). */
	if (strcmp(command, SERVER_STATE_RESPONSE) != 0) {
		PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
		return;
	}


	char* status = strtok_r(NULL, delim, &saveptr);

	sprintf(header + header_offset, "%s ", status);
	header_offset = strlen(header);


    if (strcmp(status, STATUS_NOK) == 0) {		
		PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
		return;
	}
	

	size_t Fsize;
	char Fname[MAX_FILE_NAME_SIZE];
	char Fdata[GS_BUF_SIZE];

	char* tmp_Fname = strtok_r(NULL, delim, &saveptr);
	char* tmp_Fsize = strtok_r(NULL, delim, &saveptr);

	sprintf(header + header_offset, "%s %s ", tmp_Fname, tmp_Fsize);
	header_offset = strlen(header);


	/* Stores the Fname. */
	strcpy(Fname, tmp_Fname);
	Fsize = atoi(tmp_Fsize);


	char* Fdata_first_part = strtok_r(NULL, "", &saveptr);
	strcpy(Fdata, Fdata_first_part);


	FILE* score_file;
	
	score_file = fopen(Fname, "w"); 
	if (score_file == NULL) {
		PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
		return;
	}


	size_t n = check - header_offset;
	
	/* Writes the first part of the data to the file. */
	if (fwrite(Fdata, 1, n, score_file) != n) {
		PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
		return;
	}


	if (fclose(score_file) == EOF) {
		PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
		return;
	}


	/* Writes to the file the rest of Fdata. */
	size_t remaining = write_to_file(fd, Fname, Fsize, n);

	if (n + remaining != Fsize) {
		PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
		return;
	}


	/* Prints the scoreboard and its info to the terminal. */
	printf("The state file was stored in the current directory.\n");
	printf(" - File name: [%s]\n - File size: [%zu bytes].\n\n", Fname, Fsize);

	score_file = fopen(Fname, "r");
	if (score_file == NULL) {
		PRINT_ERR;
		freeaddrinfo(res);
    	close(fd);
		return;
	}


	char c = fgetc(score_file);
	while (c != EOF) {
		printf("%c", c);
		c = fgetc(score_file);
	}

	printf("\n");


	if (fclose(score_file) == EOF) {
		PRINT_ERR;
	}

	freeaddrinfo(res);
    close(fd);
}
