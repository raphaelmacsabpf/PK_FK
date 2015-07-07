#include <string.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_TOKEN_LENGTH 256

#define DELIMITER " (),;\t"

int parser(char newTokens[][MAX_TOKEN_LENGTH]);