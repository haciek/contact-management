#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "inc/debug.h"
#include "inc/macros.h"

typedef struct
{
	char name[30];
	char email[30];
	char phone[10]; // one more byte for str terminator \0
	char addr[30];
} Contact;

/* contact functions */
Contact *contact_create(size_t c_count);
void contact_delete(Contact* c);
void contact_serialize(Contact* c);
void contact_deserialize(Contact *c_list, size_t c_count);
void contact_print(Contact* c);
void contact_menu(char, uint, Contact*);
void contact_input_field(char* prompt, char* field, size_t field_size);

void term_init(void);
void menu_draw(char**, uint, uint);
void cursor_move(char, uint*);

bool string_input(char*, char, size_t);

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
	char input = ' ';

	while (true)
	{
		menu_draw(menu_items, MENU_SIZE, select_index);
		input = getchar();
		if (input == KEY_ENTER) { break; }
		cursor_move(input, &select_index);
	}

	uint cursor_index = 0;
	Contact *c = contact_create(0);

	if (select_index == 0)
	{
		/* TODO */
		/* count lines to determine the size of the contact list */
		/* FILE *fd = fopen(CONTACT_FILE_PATH, "r"); */

		/* reading all saved contacts to c_list */
		Contact *c_list = contact_create(2);
		contact_deserialize(c_list, 2);

		contact_print(&c_list[0]);
		contact_print(&c_list[1]);
		contact_delete(c);
	}
	else if (select_index == 1)
	{
		/* creanting and saving a contact */
		contact_input_field("Name", c->name, sizeof(c->name));
		contact_input_field("Email", c->email, sizeof(c->email));
		contact_input_field("Adress", c->addr, sizeof(c->addr));
		contact_input_field("Phone number", c->phone, sizeof(c->phone));

		contact_print(c);
		contact_serialize(c);
		contact_delete(c);

	}

	return 0;
}


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
		cursor = (i == cursor_index) ? '*' : ' ';
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

Contact *contact_create(size_t c_count)
{
	Contact *c = (Contact *) malloc(sizeof(Contact) * c_count);
	if (c == NULL)
	{
		perror("Failed to malloc a contact");
		exit(-1);
	}
	/* initializing the id */
	return c;
}

void contact_delete(Contact *c) {
	if (c == NULL)
	{
		perror("Trying to free a null contact ptr\n");
		exit(-1);
	}
	free(c);
}

void contact_input_field(char* prompt, char* field, size_t field_size)
{
	char ch;
	bool status = true;
	while (status)
	{
		system("clear");
		printf("\t--- Input contact field ---\n");
		printf("%s: %s", prompt, field);

		ch = getchar();
		status = string_input(field, ch, field_size);
	}
}

void contact_print(Contact *c)
{
	printf("\t--- Contact Info ---\n");
	printf("\tName:   %s\n", c->name);
	printf("\tEmail:  %s\n", c->email);
	printf("\tPhone:  %s\n", c->phone);
	printf("\tAddres: %s\n", c->addr);
	printf("\n");
}

void contact_serialize(Contact *c) {
	FILE *fd = fopen(CONTACT_FILE_PATH, "a");
	if (fd == NULL)
	{
		perror("Failed to open a file");
		exit(-1);
	}

	fprintf(fd, CONTACT_SERIALIZE_JSON, c->name, c->email, c->phone, c->addr);

	if (fclose(fd))
	{
		perror("Failed to close a file");
		exit(-1);

	}
}

void contact_deserialize(Contact *c_list, size_t c_count)
{
	FILE *fd = fopen(CONTACT_FILE_PATH, "r+");
	if (fd == NULL)
	{
		perror("Failed to open a file");
		exit(-1);
	}
	for (uint i = 0; i < c_count; i++)
	{
		fscanf(fd, CONTACT_DESERIALIZE_JSON, c_list[i].name, c_list[i].email, c_list[i].phone, c_list[i].addr);
	}

	if (fclose(fd))
	{
		perror("Failed to close a file");
		exit(-1);

	}
	/* removing trailing new line */
	/* c->addr[strcspn(c->addr, "\n")] = 0; */
}

/* Append char to a char[] of a specified size
   or deletes the last char depending on keyboard input */
bool string_input(char *s, char c, size_t size)
{
	uint len = strlen(s);

	if (c == KEY_ENTER) { return false; }
	/* Deletion of a char */
	if (c == KEY_BACKSPACE)
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
