#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

/* #include <stddef.h> */
/* #include <stdio.h> */
/* #include <stdlib.h> */

#include "lib/debug.h"

#define MENU_SIZE 5

#define malloc(n) debug_malloc(n, __FILE__, __LINE__)

typedef struct
{
	uint id;
	char name[50];
	char addr[50];
	char email[50];
	char phone[9];
} contact;

contact* create_contact();
void delete_contact(contact*);
void print_contact(contact);
void save_contact(contact);
void input_contact_field(char*, size_t);

void init_term();

uint menu(char**, uint);
void menu_cursor_move(char, uint*);

bool modify_str(char*, char, size_t);

int main(void)
{
	init_term();
	char *menu_items[MENU_SIZE] =
	{
		"Show Contacts",
		"Add Contact",
		"Modif Contact",
		"Delete Contact",
		"Quit"
	};

	uint selected = menu(menu_items, MENU_SIZE);

	if (selected == 1)
	{
		contact *c = (contact *) create_contact();

		printf("\tName: ");
		input_contact_field(c->name, sizeof(c->name));
		printf("\tEmail: ");
		input_contact_field(c->email, sizeof(c->email));
		printf("\tAddress: ");
		input_contact_field(c->addr, sizeof(c->addr));
		printf("\tPhone number: ");
		input_contact_field(c->phone, sizeof(c->phone));
		print_contact(*c);
		delete_contact(c);
		print_contact(*c);
		print_contact(*c);
		delete_contact(c);
		print_contact(*c);
	}
	return 0;
}

contact *create_contact()
{
	contact *c = (contact *) malloc(sizeof(contact));
	/* initializing the id */
	c->id = 0u;
	return c;
}

void delete_contact(contact *c) { free(c); }

void init_term()
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

uint menu(char **menu_items, uint menu_size)
{
	uint i, cursor_index = 0;
	char ch = ' ', cursor_char;

	while (true)
	{
		system("clear");
		printf("\t--- Welcome to the contact management app! ---\n");
		printf("\n\n");

		menu_cursor_move(ch, &cursor_index);
		for (i = 0; i < MENU_SIZE; i++)
		{
			cursor_char = (i == cursor_index) ? '*' : ' ';
			printf("\t%c[%d].%s\n", cursor_char, i + 1, menu_items[i]);
		}
		ch = getchar();
		if(ch == '\n') { break; }
	}
	return cursor_index;
}

void menu_cursor_move(char ch, uint* index)
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

void input_contact_field(char* field, size_t field_size)
{
	char ch;
	bool status = true;
	while (status)
	{
		ch = getchar();
		status = modify_str(field, ch, field_size);
	}

}

void print_contact(contact c)
{
	printf("\n");
	printf("\tName: %s\n", c.name);
	printf("\tEmail: %s\n", c.email);
	printf("\tAddres: %s\n", c.addr);
	printf("\tPhone: %s\n", c.phone);
}

/* Append char to a char[] of a specified size
   or deletes the last char depending on keyboard input */
bool modify_str(char *s, char c, size_t size)
{
	uint len = strlen(s);

	if (c == '\n') { return false; }
	/* Backspace '\b' */
	if (c == 127)
	{
		s[len - 1] = '\0';
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

