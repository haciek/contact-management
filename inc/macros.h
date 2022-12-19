#define CONTACT_FILE_PATH "./contacts.json"
#define CONTACT_SERIALIZE_JSON                                                 \
   "{ name: \"%s\", email: \"%s\", phone: \"%s\", addr: \"%s\"},\n"
#define CONTACT_DESERIALIZE_JSON                                               \
   "{ name: \"%[^\"]\", email: \"%[^\"]\", phone: \"%[^\"]\", addr: "          \
   "\"%[^\"]\" },\n"

#define MENU_SIZE 5
#define KEY_ENTER 0x0A
#define KEY_BACKSPACE 0x7F

#define malloc(n) debug_malloc(n, __FILE__, __LINE__)
#define free(n) debug_free(n, __FILE__, __LINE__)
