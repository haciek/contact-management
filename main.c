#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "inc/debug.h"
#include "inc/macros.h"

typedef struct {
	char name[30];
	char email[30];
	char phone[10]; // one more byte for str terminator \0
	char addr[30];
} Contact;

/* contact functions */
Contact *contact_create(size_t c_count);
void contact_delete(Contact *c);
void contact_serialize(Contact *c);
void contact_serialize_all(Contact *c_list, size_t c_list_size);
void contact_deserialize_all(Contact *c_list, size_t c_list_size);
void contact_print(Contact *c);
uint contact_select(Contact *c_list, size_t c_list_size);
void contact_input_field(char *prompt, char *field, size_t field_size);
uint contact_count_saved();

void term_init(void);
void menu_draw(char **items, uint items_size, uint cursor_index);
void cursor_move(char ch, uint *index, size_t max_size);

bool string_input(char *, char, size_t);

int main(void) {
	term_init();
	char *menu_items[MENU_SIZE] = {"Show Contacts", "Add Contact",
				       "Modify Contact", "Delete Contact",
				       "Quit"};

	uint select_index = 0;
	char input = ' ';

	while (true) {
		menu_draw(menu_items, MENU_SIZE, select_index);
		input = getchar();
		if (input == KEY_ENTER) {
			break;
		}
		cursor_move(input, &select_index, MENU_SIZE);
	}

	uint cursor_index = 0;
	Contact *c = contact_create(1);

	uint contact_index = 0;
	uint c_list_size = contact_count_saved();
	Contact *c_list = contact_create(c_list_size);
	contact_deserialize_all(c_list, c_list_size);

	switch (select_index) {
	case 0:
		for (uint i = 0; i < c_list_size; i++) {
			contact_print(&c_list[i]);
		}
		break;
	case 1:
		contact_input_field("Name", c->name, sizeof(c->name));
		contact_input_field("Email", c->email, sizeof(c->email));
		contact_input_field("Phone", c->phone, sizeof(c->phone));
		contact_input_field("Address", c->addr, sizeof(c->addr));
		contact_serialize(c);
		break;
	case 2:
		contact_index = contact_select(c_list, c_list_size);
		contact_print(&c_list[contact_index]);
		char *items[5] = {"Name", "Email", "Phone", "Address", "Quit"};

		char input = ' ';
		while (true) {
			if (input == KEY_ENTER) {
				break;
			}
			cursor_move(input, &cursor_index, 5u);
			menu_draw(items, 5u, cursor_index);
			input = getchar();
		}

		switch (cursor_index) {
		case 0:
			contact_input_field("Name", c_list[contact_index].name,
					    sizeof(c_list[contact_index].name));
			break;
		case 1:
			contact_input_field(
			    "Email", c_list[contact_index].email,
			    sizeof(c_list[contact_index].email));
			break;
		case 2:
			contact_input_field(
			    "Phone", c_list[contact_index].phone,
			    sizeof(c_list[contact_index].phone));
			break;
		case 3:
			contact_input_field("Address",
					    c_list[contact_index].addr,
					    sizeof(c_list[contact_index].addr));
			break;
		case 4:
			return 0;
			break;
		}
		contact_serialize_all(c_list, c_list_size);

		break;
	case 3:
		contact_index = contact_select(c_list, c_list_size);
		contact_print(&c_list[contact_index]);

		FILE *fd = fopen(CONTACT_FILE_PATH, "w");
		if (fd == NULL) {
			perror("Failed to open a file");
			exit(-1);
		}

		for (uint i = 0; i < c_list_size; i++) {
			if (i == contact_index) {
				continue;
			}
			fprintf(fd, CONTACT_SERIALIZE_JSON, c_list[i].name,
				c_list[i].email, c_list[i].phone,
				c_list[i].addr);
		}

		if (fclose(fd)) {
			perror("Failed to close a file");
			exit(-1);
		}
		break;
	default:
		return 0;
		break;
	}

	/* if (select_index == 0) { */

	/* 	/1* for (uint i = 0; i < c_list_size; i++) { *1/ */
	/* 	/1* 	contact_print(&c_list[i]); *1/ */
	/* 	/1* } *1/ */
	/* } */
	/* if (select_index == 1) { */
	/* 	contact_input_field("Name", c->name, sizeof(c->name)); */
	/* 	contact_input_field("Email", c->email, sizeof(c->email)); */
	/* 	contact_input_field("Phone", c->phone, sizeof(c->phone)); */
	/* 	contact_input_field("Address", c->addr, sizeof(c->addr)); */
	/* 	contact_serialize(c); */
	/* } */
	/* if (select_index == 2) { */
	/* 	uint index = contact_select(c_list, c_list_size); */
	/* 	contact_print(&c_list[index]); */
	/* } */
	/* if (select_index == 3) { */
	/* 	uint index = contact_select(c_list, c_list_size); */
	/* 	contact_print(&c_list[index]); */

	/* 	FILE *fd = fopen(CONTACT_FILE_PATH, "w"); */
	/* 	if (fd == NULL) { */
	/* 		perror("Failed to open a file"); */
	/* 		exit(-1); */
	/* 	} */

	/* 	for (uint i = 0; i < c_list_size; i++) { */
	/* 		if (i == index) { */
	/* 			continue; */
	/* 		} */
	/* 		fprintf(fd, CONTACT_SERIALIZE_JSON, c_list[i].name, */
	/* 			c_list[i].email, c_list[i].phone, */
	/* 			c_list[i].addr); */
	/* 	} */

	/* 	if (fclose(fd)) { */
	/* 		perror("Failed to close a file"); */
	/* 		exit(-1); */
	/* 	} */
	/* } */
	contact_delete(c_list);
	contact_delete(c);
	return 0;
}

void term_init() {
	static struct termios oldt, newt;
	/*tcgetattr gets the parameters of the current terminal
	 * STDIN_FILENO will tell tcgetattr that it should write the
	 * settings of stdin to oldt*/
	tcgetattr(STDIN_FILENO, &oldt);
	/*now the settings will be copied*/
	newt = oldt;
	/*ICANON normally takes care that one line at a time will be
	 * processed that means it will return if it sees a "\n" or an
	 * EOF or an EOL*/
	newt.c_lflag &= ~(ICANON);
	/*Those new settings will be set to STDIN TCSANOW tells
	 * tcsetattr to change attributes immediately. */
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void menu_draw(char **items, uint items_size, uint cursor_index) {
	system("clear");
	printf("\t--- Welcome to the contact management app! ---\n");
	printf("\n\n");

	char cursor;
	for (uint i = 0; i < items_size; i++) {
		cursor = (i == cursor_index) ? '*' : ' ';
		printf("\t%c[%d].%s\n", cursor, i + 1, items[i]);
	}
}

void cursor_move(char ch, uint *index, size_t max_size) {
	switch (ch) {
	case 'j':
		if (*index == max_size - 1)
			break;
		else
			*index += 1;
		break;
	case 'k':
		if (*index > 0)
			*index -= 1;
		break;
	}
}

Contact *contact_create(size_t c_count) {
	Contact *c = (Contact *)malloc(sizeof(Contact) * c_count);
	if (c == NULL) {
		perror("Failed to malloc a contact");
		exit(-1);
	}
	/* initializing the id */
	return c;
}

void contact_delete(Contact *c) {
	if (c == NULL) {
		perror("Trying to free a null contact ptr\n");
		exit(-1);
	}
	free(c);
}

void contact_input_field(char *prompt, char *field, size_t field_size) {
	char ch;
	bool status = true;
	while (status) {
		system("clear");
		printf("\t--- Input contact field ---\n");
		printf("%s: %s", prompt, field);

		ch = getchar();
		status = string_input(field, ch, field_size);
	}
	field[strcspn(field, "\n")] = '\0';
}

void contact_print(Contact *c) {
	printf("\t--- Contact Info ---\n");
	printf("\tName:   %s\n", c->name);
	printf("\tEmail:  %s\n", c->email);
	printf("\tPhone:  %s\n", c->phone);
	printf("\tAddres: %s\n", c->addr);
	printf("\n");
}

void contact_serialize(Contact *c) {
	FILE *fd = fopen(CONTACT_FILE_PATH, "a");
	if (fd == NULL) {
		perror("Failed to open a file");
		exit(-1);
	}

	fprintf(fd, CONTACT_SERIALIZE_JSON, c->name, c->email, c->phone,
		c->addr);

	if (fclose(fd)) {
		perror("Failed to close a file");
		exit(-1);
	}
}

void contact_deserialize_all(Contact *c_list, size_t c_count) {
	FILE *fd = fopen(CONTACT_FILE_PATH, "r+");
	if (fd == NULL) {
		perror("Failed to open a file");
		exit(-1);
	}
	for (uint i = 0; i < c_count; i++) {
		fscanf(fd, CONTACT_DESERIALIZE_JSON, c_list[i].name,
		       c_list[i].email, c_list[i].phone, c_list[i].addr);
	}
	if (fclose(fd)) {
		perror("Failed to close a file");
		exit(-1);
	}
	/* removing trailing new line */
	/* c->addr[strcspn(c->addr, "\n")] = 0; */
}

uint contact_count_saved() {
	uint line_count = 0;
	FILE *fd = fopen(CONTACT_FILE_PATH, "r+");
	if (fd == NULL) {
		return 0;
	}
	while (!feof(fd)) {
		char c = fgetc(fd);
		if (c == '\n') {
			line_count++;
		}
	}
	if (fclose(fd) != 0) {
		perror("Failed to close a file");
		exit(-1);
	}
	return line_count;
}

uint contact_select(Contact *c_list, size_t c_list_size) {
	uint index = 0;
	char input = ' ';
	char cursor;

	while (true) {
		if (input == KEY_ENTER) {
			break;
		}
		system("clear");
		cursor_move(input, &index, c_list_size);

		for (uint i = 0; i < c_list_size; i++) {
			cursor = (i == index) ? '*' : ' ';
			printf("%c %d.", cursor, i + 1);
			contact_print(&c_list[i]);
		}
		input = getchar();
	}
	return index;
}

void contact_serialize_all(Contact *c_list, size_t c_list_size) {
	FILE *fd = fopen(CONTACT_FILE_PATH, "w");
	if (fd == NULL) {
		perror("Failed to open a file");
		exit(-1);
	}

	for (uint i = 0; i < c_list_size; i++) {
		fprintf(fd, CONTACT_SERIALIZE_JSON, c_list[i].name,
			c_list[i].email, c_list[i].phone, c_list[i].addr);
	}

	if (fclose(fd)) {
		perror("Failed to close a file");
		exit(-1);
	}
}

/* Append char to a char[] of a specified size
or deletes the last char depending on keyboard input */
bool string_input(char *s, char c, size_t size) {
	uint len = strlen(s);

	if (c == KEY_ENTER) {
		return false;
	}
	/* Deletion of a char */
	if (c == KEY_BACKSPACE) {
		len = (len > 0) ? len - 1 : 0;
		c = '\0';
	}
	s[len] = c;
	/* terminates a str depending on its size */
	len = (len == size) ? len : len + 1;
	s[len] = '\0';

	return true;
}
