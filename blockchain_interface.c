#include "my_blockchain.h"

/*==========[1.1 INTERFACE SYSTEM: PROMPT INPUT]=================


=================================================================*/

/* Write integer to file descriptor. */
int WriteInteger(int fd, int integer) 
{
    if (integer == 0) {
        return write(fd, "0", 1);
    }

    int size = 0;  
    if (integer < 0) {
        size += write(fd, "-", 1);
        integer *= -1;
    }

    char digit;
    long digit_value;    
    for (int digits = 10; (digit = --digits); ) {
        /* Count leftmost digit */
        for (digit_value = 1; --digit; digit_value *= 10) {};
        for (digit = '0'; digit_value <= integer; digit++) {
            integer -= digit_value;
        }
        /* Print digit */
        if (size > 0 || digit != '0') {
            size += write(fd, &digit, 1);
        }
    }
    return size;
}

/* Copies source to destination. Return string length.  */
int StrcpyStrlen(char *src, char *dst)
{
    char *original = dst;
    while ((*dst++ = *src++)) {};
    return dst-original-1;
}

/* Send blockchain information and request feedback from stdin. */
int SendPrompt(blockchain_t* blockchain)
{
    /* Write the blockchain prompt */
    write(STDOUT_FILENO, "[", 1);
    write(STDOUT_FILENO, &blockchain->sync, 1);
    WriteInteger(STDOUT_FILENO, blockchain->size);
    write(STDOUT_FILENO, "]>", 2);

    /* Request for executeable input */
    if (blockchain->user.excess != NULL) {
        return StrcpyStrlen(blockchain->user.excess, blockchain->user.buffer);
    } else {
        write(STDIN_FILENO, " ", 1);
        return read(STDIN_FILENO, blockchain->user.buffer, STDIN_SIZE);
    }
}

/*=========[1.2 INTERFACE SYSTEM: COLLECT ARGUMENTS]=================


========================================================*/

/* Spot the next user argument in stdin. When no more 
   argument is spotted, return NULL. Also, null-terminate
   the end of arguments.  */
char* NextArg(char** current_offset) 
{
    /* Find argument */        
    char* stdin = *current_offset;
    while (*stdin == ' ') *stdin++ = '\0';    
    if (*stdin == '\n') {
        return NULL;
    }

    /* Move offset to next arg */
    char* argument = stdin;
    while (*stdin != ' ' && *stdin != '\n') {
        stdin++;
    }
    *current_offset = stdin;
    return argument;
}

/* Allocate exactly enough for every argument in string.  */
char** MallocArgv(char* string)
{
    while (*string == ' ') string++;

    /* Count number of arguments */ 
    int argument_count = 0;
    while (*string != '\n') {
        if (*string++ == ' ' && *string != ' ') {
            argument_count++;
        }
    }
    if (string[-1] != ' ' && argument_count > 0) {
        argument_count++;
    }

    return malloc((argument_count+1)*sizeof(char*));
}

/* Return a pointer to the first character past newline.  */
char *SkipNewline(char *string)
{
    while (*string++ != END_OF_COMMAND) {};
    return string;
}

/* Organize user string arguments into an array format.  */
char** CollectArguments(char *stdin, bool skip_command)
{
    /* Skip command arguments */ 
    if (skip_command == true) {
        if (*stdin == NONE) return NULL;
        stdin = SkipNewline(stdin);
    }

    /* Collect non-command arguments */ 
    char **argv = MallocArgv(stdin);
    for (int i = 0; (argv[i++] = NextArg(&stdin)); ) {};
    return argv;
}

/*==========[1.3 INTERFACE SYSTEM: REQUEST VALID COMMAND]=================


==================================================================*/

/* Search to identify a valid command in stdin. Result is returned
   and is also inscribed into the stdin string.  */
   
int IdentifyCommand(char *keyhole, char **List_of_commands)
{
    char *stdin = keyhole;
    char *key;

    /* Find which command match the keyhole */
    for (int id = 0; (key = List_of_commands[id++]); keyhole = stdin) {
        for (keyhole--; *++keyhole == ' '; ) {};
        while (*key++ == *keyhole++) {
            if (*key == '\0') {
                if (*keyhole != ' ' && *keyhole != '\n') {
                    continue;
                }
                keyhole[-1] = END_OF_COMMAND;
                return id;
            }
        }
    }
    printf(ERROR_NOCOMMAND);
    stdin[0] = NONE;
    return NONE;
}

/* If excess commands are accidentally captured in buffer, return its pointer.  */
char *SaveExcess(char *stdin)
{
    if (*stdin == NONE) {
        stdin = SkipNewline(stdin);
    } else {
        stdin = SkipNewline(SkipNewline(stdin));
        stdin[-1] = '\0';
    }
    return (*stdin != '\0'? stdin : NULL);
}

/* Continually request and process user input from stdin until
   an executable blockchain instruction is received.  */
bool GetCommand(blockchain_t* blkchain)
{
    blkchain->user.command = NONE;
    char *command_words[8] = {
        [i(ADND)] = "add node",
        [i(RMND)] = "rm node",
        [i(ADBK)] = "add block",
        [i(RMBK)] = "rm block",
        [i(LIST)] = "ls",
        [i(SYNC)] = "sync",
        [i(QUIT)] = "quit",
        [7] = NULL
    };
    
    for (int i; blkchain->user.command == NONE; ) {
        /* Continually prompt user for a valid command */
        switch (i = SendPrompt(blkchain)) {
            case -1: case 0: return false;
            case 1: {
                printf(ERROR_NOCOMMAND);
                continue;
            }
            case STDIN_SIZE: {
                printf(ERROR_FULLBUFFER);
                while (read(STDIN_FILENO, blkchain->user.buffer, STDIN_SIZE) == STDIN_SIZE) {};
                continue;
            }
        }
        /* Process the text sent by user  */
        blkchain->user.buffer[i] = '\0';
        blkchain->user.command = IdentifyCommand(blkchain->user.buffer, command_words);
        blkchain->user.argv    = CollectArguments(blkchain->user.buffer, true);
        blkchain->user.excess  = SaveExcess(blkchain->user.buffer);
    }
    return blkchain->user.command != QUIT;
}