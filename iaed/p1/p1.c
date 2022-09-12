/* ----------------------------- */
/*       Valentim Santos         */
/*       ist199343	         */
/* ----------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* DEFINE wall with all the values of constants used 
 * - MAX_ACTI --> the maximum number of activities
 * - MAX_STR --> the maximum number of chars an activity's name contains as well as a user's name (plus 1 to account '\0')
 * - MAX_TASKS --> the maximum number of tasks 
 * - MAX_DESCRIPTION --> the maximum number of chars that each description contains (plus 2 to account for extra ' ' and '\0')
 * - MAX_USERS --> maximum number of users */
#define MAX_ACTI 10
#define MAX_STR 21 
#define MAX_TASKS 10000
#define MAX_DESCRIPTION 52 
#define MAX_USERS 50 


/* DEFINE wall with all the error messages used */
#define TOO_MANY_TASKS "too many tasks"
#define DUP_DESC "duplicate description"
#define INV_DUR "invalid duration"
#define INV_TIME "invalid time"
#define USR_EXISTS "user already exists"
#define TOO_MANY_USR "too many users"
#define NO_SUCH_TASK "no such task"
#define TASK_START "task already started"
#define NO_USR "no such user"
#define NO_ACT "no such activity"
#define TOO_MANY_ACT "too many activities"
#define DUP_ACT "duplicate activity"
#define INV_DESC "invalid description"



/* time is a global variable that always starts at 0(zero)
 * and gets incremented when the funtion timer() is called */
int time = 0;

/* struct Task containing all the "stats" of a task */
typedef struct {
		/* task id */
		int id;
		/* task description */
		char description[MAX_DESCRIPTION];
		/* the task's user  */
		char user[MAX_STR];
		/* the current tasks's activity */
		char acti[MAX_STR];
		/* the speculated duration of the task */
		int length;
		/* the moment the task starts --> leaves the "TO DO" activity */
		int start;

	} Task;

/* prototypes for all the main functions used */
void add_task(Task tasks[]);
void list_tasks(Task tasks[]);
void add_user(char users[MAX_USERS][MAX_STR]);
void timer();
void move_task(Task tasks[], char users[MAX_USERS][MAX_STR], char activities[MAX_ACTI][MAX_STR]);
void act_tasks(Task tasks[], char activities[MAX_ACTI][MAX_STR]);
void add_activity(char activities[MAX_ACTI][MAX_STR]);

/* prototypes for all the extra/auxiliary functions used */
void alpha_sort(Task tasks[], int sort[]);
void swap(int i, int j, int sort[]);
void number_sort(Task tasks[], int len, int sort[]);
void read_input(char str[]);
void print_task(Task tasks[]);


int main()
{	
	int i;
	char operation;
	int quit = 0;

	Task tasks[MAX_TASKS];
	char activities[MAX_ACTI][MAX_STR] = {"TO DO", "IN PROGRESS", "DONE"};
	char users[MAX_USERS][MAX_STR];

	for(i = 0; i < MAX_TASKS; ++i) {
		/* puts all the tasks's ids at zero;
		 * a task with an id of zero does not exist in the system */
		tasks[i].id = 0;
	}

	while(!quit) {

		scanf("%c", &operation);
		switch(operation)
		{
			case 'q':
				/* terminates the program */
				quit = 1;
				break;

			case 't':
				/* adds a new task to the system */
				add_task(tasks);
				break;

			case 'l':
				/* lists all the tasks in the system */
				list_tasks(tasks);
				break;

			case 'n':
				/* advances the time */
				timer();
				break;

			case 'u':
				/* adds a new user or lists all existing ones */
				add_user(users);
				break;

			case 'm':
				/* moves a task from one activity to another */
				move_task(tasks, users, activities);
				break;

			case 'd':
				/* lists all the tasks in a specific activity */
				act_tasks(tasks, activities);
				break;

			case 'a':
				/* adds an activity or lists all existing ones */
				add_activity(activities);
				break;
		}
	}
	return 0;
}




/* recieves the tasks array and modifies it, adding one more task to it or raising an error */
void add_task(Task tasks[])
{
	/*truncated --> (int)task duration (int part of the float read) */
	int truncated, i;
	double check;
	char descrip[MAX_DESCRIPTION];

	/* static value used to keep control over how many tasks already exist in the system.
	 * Is incremented as new tasks are added */
	static int counter_id = 0;

	scanf("%lf", &check);
	truncated = (int)check;

	fgets(descrip, MAX_DESCRIPTION, stdin);
	descrip[strcspn(descrip, "\n")] = 0;

	if(counter_id == MAX_TASKS) {
		printf("%s\n", TOO_MANY_TASKS);
		return;
	}

	for(i = 0; i <= counter_id; ++i){
		/* checks if the new discription is a duplicate one */
		if(!strcmp(descrip, tasks[i].description)) {
			printf("%s\n", DUP_DESC);
			return;
		}
	}

	if(check != truncated || truncated <= 0) {
		/* if the truncated version of the number is defferent, that means that the number is indeed a float 
		 * and not an integer, thus raising an error (10 != 10.1 but 10 = 10.0) */
		printf("%s\n", INV_DUR);
		return;
	}

	/* to the system the ids start at 0(zero) but to the user they start at 1 */
	tasks[counter_id].id = counter_id+1;  

	strcpy(tasks[counter_id].description, descrip);
	strcpy(tasks[counter_id].acti, "TO DO");
	tasks[counter_id].length = truncated;
	tasks[counter_id].start = 0;

	printf("task %d\n", counter_id+1);
	counter_id++;
}




/* lists all the tasks in the system or only the ones the user wants (if specified) */
void list_tasks(Task tasks[])
{	
	int i, len = 0;
	char c;

	/* sort --> array that will contain the ids of the tasks sorted alphabeticaly */ 
	int sort[MAX_TASKS];

	for(i = 0; i < MAX_TASKS; ++i) {
		sort[i] = tasks[i].id;

		if(tasks[i].id != 0)
			len++;
	}

	
	if((c = getchar()) == '\n' || c == '\0') {
		alpha_sort(tasks, sort);
		
		for(i = 0; i < len; ++i) {
			printf("%d %s #%d%s\n", tasks[sort[i]-1].id, tasks[sort[i]-1].acti, 
									 tasks[sort[i]-1].length, tasks[sort[i]-1].description);
		}
		return;
	}

	else {
		/* the input was "l <id1 id2 id3 ...>", the function will continously read the ids
		 * and print them as it reads them, until the end */                               
		print_task(tasks);

		while((c = getchar()) != '\n' && c != '\0') {
			print_task(tasks);
		}
	}

}



/* when called, this function will add to the global timer or raise an error */
void timer()
{	
	int t;
	double check;

	scanf("%lf", &check);
	t = (int)check;

	if(t != check || check < 0) {
		/* if the truncated version of the number is different than the original one, it means that 
		 * the number is indeed a float and not an integer, thus raising an error
		 * (10 != 10.1 but 10 == 10.0) */
		printf("%s\n", INV_TIME);
		return;
	}
	
	time += t;
	printf("%d\n", time);
	
}



/* receives the array of users and modifies it, adding one (if specified), or listing all the existing ones if not */
void add_user(char users[MAX_USERS][MAX_STR])
{
	int i;
	/* static value used to keep control over how many users already exist in the system.
	 * Is incremented as new users are added */
	static int user_counter = 0;

	char new_user[MAX_STR];
	read_input(new_user);

	if(new_user[0] != '\0') {
		/* if the first char of the input is a letter then then input was "u <name>" */

		for(i = 0; i <= user_counter; ++i) {
			/* checks if the new_user has the same name as an already existing one */
			if(!strcmp(new_user, users[i])){
				printf("%s\n", USR_EXISTS);
				return;
			}
		}

		if(user_counter == MAX_USERS) {
		/* the max amount of users has been reached */ 
			printf("%s\n", TOO_MANY_USR);
			return;
		}

		strcpy(users[user_counter], new_user);
		user_counter++;
	}

	else {
		/* the input was only "u", which means we want to list all the users */
		for(i = 0; i < user_counter; ++i)
			printf("%s\n", users[i]);
	}

}



/* modifies the tasks's activity, user and start stats, printing both the 
 * duration and slack of a task if it has ended and raising errors accordingly*/
void move_task(Task tasks[], char users[MAX_USERS][MAX_STR], char activities[MAX_ACTI][MAX_STR])
{
	int i, check = 0, in_id, used, slack;
	char user[MAX_STR], act[MAX_STR];

	scanf("%d %s", &in_id, user);
	read_input(act);

	if(tasks[in_id-1].id == 0) {
		printf("%s\n", NO_SUCH_TASK);
		return;
	}

	if(strcmp(tasks[in_id-1].acti, "TO DO") && !strcmp(act, "TO DO")) {
		printf("%s\n", TASK_START);
		return;
	}

	for(i = 0; strcmp(users[i], "\0") != 0; ++i) {
		/* checks if the user exists */
		if(!strcmp(users[i], user))
			check = 1;

	} if(!check){
		printf("%s\n", NO_USR);
		return;
	}

	check = 0;

	for(i = 0; strcmp(activities[i], "\0") != 0; ++i) {
		/* checks if the activity exists */
		if(!strcmp(activities[i], act))
			check = 1;

	} if(!check){
		printf("%s\n", NO_ACT);
		return;
	}
	
	if(!strcmp(act, tasks[in_id-1].acti)) {
		/* if the new activity is the same as before, returns nothing */
		return;
	}

	if(strcmp(act, "TO DO") != 0 && !strcmp(tasks[in_id-1].acti, "TO DO")) {
		/* if the task goes to a different activity, other than "TO DO" the start time is the current time */
		tasks[in_id-1].start = time;
	}

	strcpy(tasks[in_id-1].user, user);
	strcpy(tasks[in_id-1].acti, act);

	if(!strcmp(act, "DONE"))
	{
		used = time - tasks[in_id-1].start;
		slack = used - tasks[in_id-1].length;

		printf("duration=%d slack=%d\n", used, slack);
	}

}




/* lists all the tasks in a certain activity, specified by the user */
void act_tasks(Task tasks[], char activities[MAX_ACTI][MAX_STR])
{
	int i, j, check = 0, sort[MAX_TASKS] = {0};
	char act[MAX_STR];

	read_input(act);

	for(i = 0; strcmp(activities[i], "\0") != 0; ++i) {
		/* checks if the activity exists */
		if(!strcmp(activities[i], act)) check = 1;

	} if(!check){
		printf("%s\n", NO_ACT);
		return;
	}

	for(i = 0, j = 0; tasks[i].id != 0; ++i) {
		/* looks for tasks in the same activity as the one chosen to then print them */
		if(!strcmp(tasks[i].acti, act)) {
			sort[j] = tasks[i].id;
			j++;
		}
	}
	
	/* sorts the tasks alphabeticaly and then numericaly so that the tasks are sorted in ascending order of duration
	 * and alphabeticaly when the durations are the same */
	alpha_sort(tasks, sort);
	number_sort(tasks, j, sort);
	
	for(i = 0; sort[i] != 0; ++i){
		printf("%d %d%s\n", tasks[sort[i]-1].id, tasks[sort[i]-1].start, tasks[sort[i]-1].description);
	}
}




/* adds an activity to the system if specified by the user, if not, lists all the existing ones */
void add_activity(char activities[MAX_ACTI][MAX_STR])
{
	int i;
	char act[MAX_STR];

	/* act counter starts at 3 because there already exist 3 activities from the start of the program 
	 * and increments to keep track of the amount of activities in the system */
	static int act_counter = 3;

	read_input(act);

	if(!strcmp(act, "\0")) {
	/* if act == '\0' then the user wants to list all the activities (input was only 'a') */
		for(i = 0; i < act_counter; ++i) {
			printf("%s\n", activities[i]);
		}
		return;
	}

	if(act_counter == MAX_ACTI) {
		printf("%s\n", TOO_MANY_ACT);
		return;
	}

	else {
		/* the input was "a <activity>" which means the user wants to add an activity */

		for(i = 0; i < act_counter; ++i) {
		/* checks if there is an already existing activity with the same name, if yes raises an error */
			if(!strcmp(activities[i], act)) {
				printf("%s\n", DUP_ACT);
				return;
			}
		}

		for(i = 0; act[i] != '\0'; ++i) {
		/* checks if any of the activity's charachters are lower case, if yes raises an error */
			if(islower(act[i])) {
				printf("%s\n", INV_DESC);
				return;
			}
		}

		strcpy(activities[act_counter], act);
		act_counter++;
	}
}



/* aux function ued to swap different nodes in the sorting functions */
void swap(int i, int j, int sort[])
{
	int temp;

	temp = sort[i]; 
	sort[i] = sort[j];
	sort[j] = temp;
}



/* Bubble sort algorithm
 * function to sort the ids of all the tasks in alphabetical order of their description
 * storing the respective ids in the sort array */
void alpha_sort(Task tasks[], int sort[])
{
	int i, j;

	for(i = 0; sort[i] != 0; ++i) {

		for(j = i+1; sort[j] != 0; ++j) {

			if(strcmp(tasks[sort[i]-1].description, tasks[sort[j]-1].description) > 0) {
				swap(i, j, sort);
			}
		}
	}
}



/* Insertion sort algorithm
 * function to sort the ids of all the tasks in numerical order of their estimated duration
 * storing them in the sort array */
void number_sort(Task tasks[], int len, int sort[])
{
    int i, key, j;

    for (i = 1; i < len; i++) {
        key = sort[i];
        j = i - 1;
 
        /* Move elements of arr[0..i-1], that are
          greater than key, to one position ahead
          of their current position */


        while (j >= 0 && (tasks[sort[j]-1].start > tasks[key-1].start)) {
            sort[j + 1] = sort[j];
            j = j - 1;
        }
        sort[j + 1] = key;
    }
}



/* aux function that reads a string and stores it in the str array, ignoring any
 * '\t' or blank spaces before the actual string */
void read_input(char str[])
{	
	int i;
	char c;

	if((c = getchar()) == '\n' || c == '\0') {
		/* if the first char read is either a '\n' or '\0' puts the first char of the srt array as a '\0' */
		str[0] = 0;
		return;
	}

	scanf("%s", str);
	/* scans the first word of the string, dismissing the ' ' and '\t' that come before it */
	i = strlen(str);
	while((c = getchar()) != '\n' && c != '\0') {
		/* fills the rest of the arrays with the remaining chars */
		str[i++] = c;
	}
	/* at the end adds a '\0' to end the string */
	str[i] = '\0';
}



/* prints the id, activity, duration and description of the task (used in the list_tasks function) */
void print_task(Task tasks[])
{
	int id;
	/* scans the id to then print its stats */
	scanf("%d", &id);

	if(tasks[id-1].id != 0) {
		/* there is no ' ' after the %d because the string read and stored in the task's despription
		 * already has the ' ' needed in the beggining */
			printf("%d %s #%d%s\n", tasks[id-1].id, tasks[id-1].acti,
									tasks[id-1].length, tasks[id-1].description);
		}
	else {
		printf("%d: %s\n", id, NO_SUCH_TASK);
	}
}