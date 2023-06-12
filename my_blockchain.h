#ifndef _BLOCKCHAIN_H
#define _BLOCKCHAIN_H   1

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

/* Default stdin string size.  */
#define STDIN_SIZE 101

/* Values to indicate status of operations.  */
#define OK          0
#define SILENT_OK   -1

/* Messages for blockchain error status.  */
#define SUCCESS           "OK\n"

#define ERROR_MALLOCFAIL  "NOK: no more resources available on the computer\n"
#define ERROR_NIDFOUND    "NOK: \'%d\': this node already exists\n"
#define ERROR_NIDMISSING  "NOK: \'%s\': node doesn't exist\n"
#define ERROR_BIDMISSING  "NOK: %d: \'%s\': block doesn't exist\n"
#define ERROR_BIDFOUND    "NOK: %d: \'%s\': this block already exists\n"
#define ERROR_FULLBUFFER  "NOK: exceeded maximum %d characters\n", (STDIN_SIZE-1)
#define ERROR_NOCOMMAND   "NOK: command not found\n"
#define ERROR_LACKINGARG  "NOK: not enough arguments\n"
#define ERROR_UNKNOWNARG  "NOK: \'%s\': unrecognized argument\n"
#define ERROR_INVALIDNID  "NOK: \'%s\': must be an integer less than one million (negative and positive)\n"

/* Values for the state of synchronization.  */
#define SYNC_FALSE  '-'   /* Blockchain is not synchronized.    */
#define SYNC_TRUE   's'   /* Blockchain is synchronized.        */

/* Value to indicate end of command in stdin string.  */
#define END_OF_COMMAND '\n'

/* Values for the blockchain commands.  */
#define NONE        0   /* No command.                  */
#define ADND        1   /* Add a new node.              */
#define ADBK        2   /* Add a new block.             */
#define RMND        3   /* Remove a node.               */
#define RMBK        4   /* Remove a block.              */
#define LIST        5   /* List blockchain.             */
#define SYNC        6   /* Synchronise blockchain.      */
#define QUIT        7   /* Quit and save blockchain.    */

/* Macro for the index of commands.  */
#define i(N) (N-1)

/* For stacking blocks of data.  */
typedef struct block block_t;
struct block {
    char        *data;      /* Contains user data.  */
    block_t     *next;      /* Links to the next block.  */
};

/* For warehousing blockchains.  */
typedef struct node node_t;
struct node {
    int          nid;       /* Contains number id.  */
    block_t     *bid;       /* Contains user datas.  */
    node_t      *next;      /* Links to the next node.  */
};

/* For recording commands in processed format.  */
typedef struct user_command {
    char        buffer[STDIN_SIZE]; /* Contains raw user input.  */
    int         command;            /* Contains the user command.  */
    char**      argv;               /* Contains additional datas.  */
    char*       excess;
} user_command_t;

/* For the creation and management of blockchain.  */
typedef struct blockchain {
    node_t *head;           /* Contains the blockchain nodes.  */
    int     size;           /* Contains the number of nodes.  */
    char    sync;           /* Contains the nodes' sync state.  */
    user_command_t user;    /* Contains the latest user instruction.  */
} blockchain_t;


/* Backup */
void RestoreBackup(blockchain_t *blockchain) ;
bool RecoverBlockchain(int fd, blockchain_t *blkchain);
void Quit(blockchain_t *blkchain);

/* Interface */
int WriteInteger(int fd, int integer);
char** CollectArguments(char *stdin, bool skip_command);
bool GetCommand(blockchain_t* blkchain);

/* System */
int AddNode(blockchain_t *blockchain);
int AddBlock(blockchain_t *blockchain);
int RemoveNode(blockchain_t *blockchain);
int RemoveBlock(blockchain_t *blockchain);
int Synchronize(blockchain_t *blockchain);
int List(blockchain_t *blockchain);
char UpdateSync(node_t *node);

#endif /* "blockchain.h" included.  */