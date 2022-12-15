#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "inc/debug.h"

#define MENU_SIZE 5

#define malloc(n) debug_malloc(n, __FILE__, __LINE__)
#define free(n) debug_free(n, __FILE__, __LINE__)

typedef struct
{
	char name[30];
	char addr[30];
	char email[30];
	char phone[10]; // one more byte for str terminator \0
} contact;

 /* contact functions */
contact *contact_create(void);
void contact_delete(contact*);
void contact_print(contact);
void contact_save(contact*);
void contact_input_field(char*, char*, size_t);


void term_init(void);
void menu_draw(char**, uint, uint);
void cursor_move(char, uint*);

bool string_modify(char*, char, size_t);

int main(void)
{
	term_init();
	char *menu_items[MENU_SIZE] =
	{
		"Show Contacts",
		"Add Contact",
		"Modify Contact",
		"Delete Contact",
		"Quit"
	};

	uint select_index = 0;

	char input = '\0';
	while (true)
	{
		if (input == '\n') { break; }
		cursor_move(input, &select_index);
		menu_draw(menu_items, MENU_SIZE, select_index);
		input = getchar();
	}

	contact *c;
	switch (select_index) {
		case 0:
			printf("\nTODO\n");
			break;
		case 1:
			c = (contact *) contact_create();

			contact_input_field("Name", c->name, sizeof(c->name));
			contact_input_field("Email", c->email, sizeof(c->email));
			contact_input_field("Adress", c->addr, sizeof(c->addr));
			contact_input_field("Phone number", c->phone, sizeof(c->phone));

			contact_print(*c);
			contact_save(c);
			contact_delete(c);

			break;
		case 2:
			printf("\nTODO\n");
			break;
		case 3:
			printf("\nTODO\n");
			break;
		case 4:
			printf("\nQuitting\n");
			exit(0);
			break;
	}
	return 0;
}

contact *contact_create()
{
	contact *c = (contact *) malloc(sizeof(contact));
	if (c == NULL)
	{
		perror("Failed to malloc a contact");
		exit(-1);
	}
	/* initializing the id */
	return c;
}

void contact_delete(contact *c) { free(c); }

void term_init()
{
	static struct termios oldt, newt;
	/*tcgetattr gets the parameters of the current terminal STDIN_FILENO will tell tcgetattr that it should write the settings of stdin to oldt*/
	tcgetattr( STDIN_FILENO, &oldt);
	/*now the settings will be copied*/
	newt = oldt;
	/*ICANON normally takes care that one line at a time will be processed that means it will return if it sees a "\n" or an EOF or an EOL*/
	newt.c_lflag &= ~(ICANON);
	/*Those new settings will be set to STDIN TCSANOW tells tcsetattr to change attributes immediately. */
	tcsetattr( STDIN_FILENO, TCSANOW, &newt);
}

void menu_draw(char **menu_items, uint menu_size, uint cursor_index)
{
	system("clear");
	printf("\t--- Welcome to the contact management app! ---\n");
	printf("\n\n");

	char cursor;
	for (uint i = 0; i < MENU_SIZE; i++)
	{
		cursor= (i == cursor_index) ? '*' : ' ';
		printf("\t%c[%d].%s\n", cursor, i + 1, menu_items[i]);
	}
}

void cursor_move(char ch, uint* index)
{
	switch(ch) {
		case 'j':
			if (*index == MENU_SIZE - 1) break;
			else *index += 1;
			break;
		case 'k':
			if (*index > 0) *index -= 1;
			break;
	}
}

void contact_input_field(char* prompt, char* field, size_t field_size)
{
	char ch;
	bool status = true;
	while (status)
	{
		system("clear");
		system("clear");
		printf("\t--- Input contact field ---\n");
		printf("%s: %s", prompt, field);
		ch = getchar();
		status = string_modify(field, ch, field_size);
	}
	/* while (true) */
	/* { */
	/* 	char field_new[field_size]; */
	/* 	system("clear"); */
	/* 	printf("\t--- Input contact field ---\n"); */

	/* 	printf("%s: ", prompt); */
	/* 	scanf("%s", field_new); */

	/* 	if (strlen(field_new) >= field_size) */
	/* 	{ */
	/* 		continue; */
	/* 	} */
	/* 	else */
	/* 	{ */
	/* 		strcat(field, field_new); */
	/* 		break; */
	/* 	} */
	/* } */
}

void contact_print(contact c)
{
	printf("\n");
	printf("\tName: %s\n", c.name);
	printf("\tEmail: %s\n", c.email);
	printf("\tAddres: %s\n", c.addr);
	printf("\tPhone: %s\n", c.phone);
}

/* Append char to a char[] of a specified size
   or deletes the last char depending on keyboard input */
bool string_modify(char *s, char c, size_t size)
{
	uint len = strlen(s);

	if (c == '\n') { return false; }
	/* Deletion of a char */
	if (c == 127)
	{
		len = (len > 0) ? len - 1 : 0;
		s[len] = '\0';
		return true;
	}
	if (len == size)
	{
		s[len] = '\0';
		return true;
	}
	s[len] = c;
	s[len + 1] = '\0';

	return true;
}


void contact_save(contact *c) {
	FILE *fd = fopen("./contacts.txt", "w");
	if (fd == NULL)
	{
		perror("Failed to open a file");
		exit(-1);
	}

	char contact_info[200];
	fprintf(fd, "%s;%s;%s;%s", c->name, c->email, c->phone, c->addr);

	if (fclose(fd))
	{
		perror("Failed to close a file");
		exit(-1);

	}
}


