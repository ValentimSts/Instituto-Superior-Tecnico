
/**
 * @file common.h
 * @author 
 * 	Group 36 (Filipa Magalhaes & Valentim Santos)
 * 
 * @brief
 * 	Header file containing all the includes and macros
 *  shared between the "player.c" and "GS.c" files.
 */


#ifndef COMMON_H
#define COMMON_H


/* Include wall. */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>


/* Address and port used only for testing. */
#define TEJO_ADDR "tejo.tecnico.ulisboa.pt"
#define TEST_PORT "58001"


/* Default GS port (in case one isn't specified). */
#define GS_DEFAULT_PORT "58036"


/* Command invoked when starting both the player
 * and server programs, specifies the port (optional). */
#define PORT_COMMAND "-p" 


#define MAX_PLID_SIZE 6 // Maximum length for thr PLID.
#define MAX_GSPORT_SIZE 64 // Maximum size for the port string.
#define MIN_WORD_SIZE 3 // Minimum size for any game word.
#define MAX_WORD_SIZE 30 // Maximum size for any game word.


/* Maximum length of any word's hint path.
 * the hint path has the following format: 
 * - images/word_name
 * making it 8 characters for 'images/' and 
 * a maximum of 30 for the 'word_name'. */
#define MAX_HINT_PATH_SIZE 38


#define GS_BUF_SIZE 128
#define MAX_COMMAND_SIZE 16
#define MAX_STATUS_SIZE 8
#define MAX_FILE_NAME_SIZE 64





#define STRTOK_DELIM " \n"


/* Server side commands (sent to the server). */
#define SERVER_START_COMMAND "SNG"      // SNG PLID
#define SERVER_PLAY_COMMAND "PLG"       // PLG PLID letter trial
#define SERVER_GUESS_COMMAND "PWG"      // PWG PLID word trial
#define SERVER_SCORE_COMMAND "GSB"      // GSB
#define SERVER_HINT_COMMAND "GHL"       // GHL PLID
#define SERVER_STATE_COMMAND "STA"      // STA PLID
#define SERVER_QUIT_EXIT_COMMAND "QUT"  // QUT PLID
// #define SERVER_EXIT_COMMAND "QUT"    // QUT PLID
#define SERVER_REVEAL_COMMAND "REV"     // REV PLID


/* Server side responses (received by the client). */
#define SERVER_START_RESPONSE "RSG"     // RSG status [n_letters max_errors]
#define SERVER_PLAY_RESPONSE "RLG"      // RLG status trial [n pos*]
#define SERVER_GUESS_RESPONSE "RWG"     // RWG status trials
#define SERVER_SCORE_RESPONSE "RSB"     // RSB status [Fname Fsize Fdata]
#define SERVER_HINT_RESPONSE "RHL"      // RHL status [Fname Fsize Fdata]
#define SERVER_STATE_RESPONSE "RST"     // RST status [Fname Fsize Fdata]
#define SERVER_QUIT_EXIT_RESPONSE "RQT" // RQT status
// #define SERVER_EXIT_RESPONSE "RQT"   // RQT status
#define SERVER_REVEAL_RESPONSE "RRV"    // RRV word/status


/* Possible server status messages:
 *  OK - letter guess was correct, 
 *       generic server reply.
 * 
 *  WIN - letter guess completes the word,
 *        word guess was correct.
 * 
 *  DUP - letter was sent previously.
 * 
 *  NOK - letter is incorrect (there are
 *        more attempts).
 * 
 *  OVR - letter is incorrect (there are no
 *        more attempts).
 * 
 *  INV - invalid trial number.
 *  
 *  ACT - there is an ongoing game.
 *        
 *  FIN - there is no ongoing game.
 *        
 *  ERR - error message.
 */
#define STATUS_OK "OK"
#define STATUS_WIN "WIN"
#define STATUS_DUP "DUP"
#define STATUS_NOK "NOK"
#define STATUS_OVR "OVR"
#define STATUS_INV "INV"
#define STATUS_EMPTY "EMPTY"
#define STATUS_ACT "ACT"
#define STATUS_FIN "FIN"
#define STATUS_ERR "ERR"

#define PRINT_ERR_MESS(message) printf("ERROR: %s\n", message);
#define PRINT_ERR printf("ERROR: Couldn't complete the requested action. Please try again.\n\n");


#endif /* COMMON_H */