#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================= *
 *	Valentim Santos - ist199343	 *
 * ============================= */

/* Define wall */
#define MAX_CHAR 65536

#define HELP "help"
#define QUIT "quit"
#define SET "set"
#define PRINT "print"
#define FIND "find"
#define LIST "list"
#define SEARCH "search"
#define DELETE "delete"

#define PRINT_HELP "help: Imprime os comandos disponíveis."
#define PRINT_QUIT "quit: Termina o programa."
#define PRINT_SET "set: Adiciona ou modifica o valor a armazenar."
#define PRINT_PRINT "print: Imprime todos os caminhos e valores."
#define PRINT_FIND "find: Imprime o valor armazenado."
#define PRINT_LIST "list: Lista todos os componentes imediatos de um sub-caminho."
#define PRINT_SEARCH "search: Procura o caminho dado um valor."
#define PRINT_DELETE "delete: Apaga um caminho e todos os subcaminhos."




/* reads the path given by the user ( Ex: ///a//b////c/ ), and "cleans" it, getting
   rid of any unnecessary '/' as well as the blank spaces that come before the inpu */
char* input_parse() {

	int len, i, j = 0, in = 0;
	char raw_str[MAX_CHAR], clean_str[MAX_CHAR], c;
	char* path = NULL;

	if((c = getchar()) == '\n' || c == '\0') {
		/* if the first char read is either a '\n' or '\0' returns NULL */
		return path;
	}

	scanf("%s", raw_str);
	/* adds a '/' to the beggining of the path, if needed */
	if(raw_str[0] != '/') {
		clean_str[j++] = '/';
	}

	len = strlen(raw_str);

	for(i = 0; i<len; i++) {
		/* "cleans" the string, getting rid of any extra '/' found */
		if(raw_str[i] == '/' && !in) {
			clean_str[j++] = '/';
			in = 1;
		}
		else if(raw_str[i] == '/' && in) {
			continue;
		}
		else if(raw_str[i] != '/') {
			clean_str[j++] = raw_str[i];
			in = 0;
		}
	}
	/* checks if the end of the string has a '/' if yes then it "deletes" it */
	if(clean_str[j-1] == '/') {
		clean_str[j-1] = '\0';
	}
	else {
		clean_str[j] = '\0';
	}
	
	path = (char*) malloc(sizeof(char)*(strlen(clean_str)+1));
	strcpy(path, clean_str);
	return path;
}

// [/a, /a/b. /a/b/c]

/* receives a path and returns a pointer to an lstay of pointers, each one containing a
   sub-path of the original sub-path ( Ex: /a/b/c --> /a, /a/b, /a/b/c ) */ 
char** sub_paths(char* path) {

	int i, len, path_counter = 0, dim = 5;

	char buffer[MAX_CHAR];
	static char** paths = NULL;

	/* gives paths a size of 5 to start with */
	paths = (char**) malloc(sizeof(char*) * dim);

	/* Ex: path = "/a/b/c" */
	len = strlen(path);
	buffer[0] = '/';

	for(i = 1; i<=len; i++) {

		if(path[i] == '/' || path[i] == '\0') {
			/* if a '/' is found it means that we found the end of a sub-path 
			   and need to store it in the paths lstay */

			if(path_counter == dim) {
				dim *= 2;
				paths = (char**) realloc(paths, sizeof(char*) * dim);
			}

		  	paths[path_counter] = (char*) malloc(sizeof(char)*(strlen(buffer)+1));
			strcpy(paths[path_counter++], buffer);
		}
		
		buffer[i] = path[i];
		/* without this line, the function would ALLWAYS break at iteration 16 and ONLY 16
		   dont know why, but doing this fixed it */
		buffer[i+1] = '\0';
	}

	for(i = path_counter; i<dim; i++)
		free(paths[i]);

	return paths;
}




/* function used to read the value a user inputs, ignores any ' ' and '\t' that come both 
   before and after the string/strings the user gave as input */
char* read_value() {

	int n, invalid = 0;
	char buffer[MAX_CHAR], c;
	char* value;

	/* gets the first string of value, ignoring any blank spaces that come before it */
	scanf("%s", buffer);
	n = strlen(buffer);

	/* gets rid of any spaces that come after the value string, as these
	   shouldn't be included */
	while((c = getchar()) != '\n' && c != '\0') {
		
		if((c == ' ' || c == '\t') && invalid) {
			buffer[n-1] = 0;
			break;
		}

		else if(c != ' ' && c != '\t') {
			invalid = 0;
			buffer[n++] = c;
			buffer[n+1] = '\0';
		}

		else if((c == ' ' || c == '\t') && !invalid){
			buffer[n++] = c;
			invalid = 1;
			buffer[n+1] = '\0';
		}

	}

	value = malloc(sizeof(char) * (strlen(buffer)+1));
	strcpy(value, buffer);
	return value;
}




int main(){

	static char* word;
	
	printf("%c\n", word[0]);
	return 0;
}