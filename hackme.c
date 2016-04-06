#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#define TRUE 1
#define FALSE 0



typedef struct user_s {
	char username[32];
	char password[32];
} user_t;


char * gets(char *);


user_t *users =0;
size_t numuser =0;

user_t *curauth = 0;

char * fileb = 0;
size_t filebs = 0;


user_t * finduser(char * name){
	int i;
	for(i = 0; i < numuser && strncmp(name, users[i].username,32); i++);
	if(i >= numuser) return FALSE;
	return users+i;
}
int authuser(void){
	curauth = 0;
	int i = 0;
	char buffer[32] = {0};
	printf("Username:\n");
	gets(buffer);
	//find user
	user_t *tt = finduser(buffer);
	if(!tt){
		printf("No such user exists!\n");
		return FALSE;
	}
	i = 0;
	printf("Password:\n");
	gets(buffer);
	if(!strncmp(buffer,tt->password,32)) i = TRUE;
	printf("diagnostics: %i, %i, %i\n", buffer, &i, 10);
	if(i) curauth = tt;
	else printf("Bad password\n");
	return i;
}


int adduser(char *name, char *pw){
	if(finduser(name)) return FALSE;
	numuser++;
	users = realloc(users, numuser * sizeof(user_t));
	strncpy(users[numuser-1].username, name, 32);
	strncpy(users[numuser-1].password, pw, 32);
	mkdir(name, 666);
	return TRUE;
}

int listusers(void){
	int i;
	printf("List of users:\n");
	printf("===============================\n");
	for(i = 0; i < numuser; i++){
		printf("%s\n", users[i].username);
//		printf("\"%s\"\n", users[i].password);
	}
	printf("===============================\n");
	return TRUE;
}
int loadusers(const char * userpath){
	char buffer[128] = {0};
	int i;
	FILE *f = fopen(userpath, "r");
	if(!f) return FALSE;
	while(fgets(buffer, 128, f)){
		//grab newline
		for(i = 0; i < 128 && buffer[i] && buffer[i] != '\n'; i++);
		if(i < 128)buffer[i] = 0;
		//grab tab
		for(i = 0; i < 128 && buffer[i] && buffer[i] != '\t'; i++);
		if(i >= 128) continue;
		buffer[i] = 0;
		adduser(buffer, buffer+i+1);
	}
	fclose(f);
}

int createnote(user_t *u){
	size_t intl = strlen(u->username) + 10;
	char buffer[128] = {0};
	char c;
	printf("Create a new note for user %s\nEnter note name:\n", u->username);
	gets(buffer);
		size_t ulen = strlen(buffer) + intl;
		if(intl > filebs){
			filebs = intl;
			fileb = realloc(fileb, filebs);
		}
		sprintf(fileb, "%s/%s", u->username, buffer);
	FILE *f = fopen(fileb, "w");
	if(!f) return FALSE;
	printf("writing to note %s\nPress ctrl-d when done\n");
	while((c = getc(stdin)) != EOF)fputc(c,f);
	fclose(f);
}
int listnotes(user_t *u){
	printf("List of notes for user %s:\n", u->username);
	struct dirent *de;
	DIR *d = opendir(u->username);
	if(!d) return FALSE;
	size_t intl = strlen(u->username) + 10;
	while((de = readdir(d))){
		char c = 0;
		size_t len = 0;
		size_t ulen = strlen(de->d_name) + intl;
		if(intl > filebs){
			filebs = intl;
			fileb = realloc(fileb, filebs);
		}
		sprintf(fileb, "%s/%s", u->username, de->d_name);
		FILE * f = fopen(fileb, "r");
		if(!f) continue;
		fseek(f, 0, SEEK_END);
		len = ftell(f);
		rewind(f);
		if(len <1 || len> 10000) continue;
		printf("%i, From %s\n", len, de->d_name);
		printf("===============================\n");
		while((c = fgetc(f)) != EOF) putc(c,stdout);
		printf("\n===============================\n");
		fclose(f);
	}
	closedir(d);
	return TRUE;
}

int createuser(const char * users){
	user_t u;
	printf("Create a new user\nEnter username:\n");
	gets(u.username);
	if(finduser(u.username)){
		printf("User already exists!\n");
		return FALSE;
	}
	printf("Enter password\n");
	gets(u.password);
	adduser(u.username, u.password);
	FILE *f = fopen(users, "a");
	if(!f) return FALSE;
	fprintf(f, "%s\t%s\n", u.username, u.password);
	fclose(f);
	return TRUE;
}


int showsource(){
	pid_t c;
	int ret;
	if((c = fork()) == -1){
		printf("error\n");
		return FALSE;
	}
	if(c){
		waitpid(c, &ret, 0);
	} else {
		char * oots[] = {"less", "./hackme.c", 0};
		execvp("less", oots);
		exit(0);
	}

	return TRUE;
}




int main(void){
	printf("Welcome to hackme\nThe goal is to either get access to user admin, get the passwords of users, or gain control of the system.\nThere are many ways to do these, including some i have not thought of!\n");
	loadusers("./users");
	char last =' ';
	while(TRUE){
		printf("Main menu: Current user: %s\n\t0: Show source code\n\t1: List usernames\n\t2: login/change user\n\t3: Add a new user\n", curauth ? curauth->username : "NONE");
		if(curauth)printf("\t4: Show notes\n\t5: Add a new note\n");
		char t;
		while((t = getc(stdin)) != '\n') last = t;
		switch(last){
			case '0':
				showsource();
			break;
			case '1':
				listusers();
			break;
			case '2':
				authuser();
			break;
			case '3':
				createuser("./users");
			break;
			case '4':
				if(curauth){
					listnotes(curauth);
					break;
				}
			case '5':
				if(curauth){
					createnote(curauth);
					break;
				}
			default:
				printf("Invalid option\n");
			break;
		}
	}
	return 1;
}
