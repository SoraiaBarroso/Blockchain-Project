#include "my_blockchain.h"

/*===========[3.1 BACKUP SYSTEM: CREATE BACKUP]=================


=================================================*/

/* Write empty spaces and one newline.  */
void WriteEnd(int fd, int size)
{
    const char* spaces = "          ";
    while (size > 10) size -= write(fd, spaces, 10);
    if    (size >  0) write(fd, spaces, size-1);
    write(fd, "\n", 1);
}

/* Write to fd STDIN_SIZE lines of nid and bids.  */
void DepositNode(int fd, int nid, block_t *bid)
{
    /* Use the following three elements in such format:
    STDIN_SIZE = "[NID:][ BID][ BID][ BID][... \n]" 
    STDIN_SIZE = "[ BID][ BID][[...            \n]"      */

    /* Write [NID:]  */
    int use = WriteInteger(fd, nid)+write(fd, ":", 1);
    int stock = STDIN_SIZE-use;
    
    while (bid != NULL) {
        use = strlen(bid->data)+1;
        if ((stock = stock-use) > 1) {
            /* Write [ BID]  */
            write(fd, " ", 1);
            write(fd, bid->data, use-1);
            bid = bid->next;
        } else {
            /* Write [... \n] when line is full */
            WriteEnd(fd, use+stock);
            stock = STDIN_SIZE;
        }
    }
    /* Write the final [... \n] */
    WriteEnd(fd, stock);
}

/* Quit and save blockchain on a backup file.  */
void Quit(blockchain_t *blkchain)
{
    if (blkchain->user.command == QUIT) {
        printf("Backing up blockchain...");
        int fd = open("blockchain_backup.txt", O_CREAT | O_WRONLY, 00644);

        /* Deposit all nodes and its blocks into multiple fixed-sized lines */
        node_t *node = blkchain->head;
        while (node) {
            DepositNode(fd, node->nid, node->bid);
            node = node->next;
        }
        /* Create a distinctive file end-marker */
        write(fd, "~~[END]~~\n\n", sizeof("~~[END]~~\n\n")-1);
        WriteEnd(fd, STDIN_SIZE+1);
        close(fd);
    }
    printf("\n");
}

/*===========[3.2 BACKUP SYSTEM: RECOVER BACKUP]=================


===================================================*/

/* Write to fd STDIN_SIZE lines of nid and bids.  */
bool RecoverBlockchain(int fd, blockchain_t *blkchain)
{
    char buffer[STDIN_SIZE];
    char **argv, **freeup;
    char *arg[3] = {NULL, NULL, NULL};
    blkchain->user.argv = arg;

    int status = OK;
    while (read(fd, buffer, STDIN_SIZE) == STDIN_SIZE) {
        /* Checker for end-marker before collecting arguments.  */
        if (*buffer == '~') break;
        if (buffer[i(STDIN_SIZE)] != '\n') {
            return true;
        }
        freeup = argv = CollectArguments(buffer, false);

        /* Add node to blockchain.  */
        if (*buffer != '\0') {
            arg[0] = *argv, arg[1] = NULL;
            status += AddNode(blkchain);
            arg[1] = *argv++;
        }
        /* Add blocks to node.  */
        while (*argv != NULL) {
            arg[0] = *argv++;
            status += AddBlock(blkchain);
        }
        free(freeup);
    }
    blkchain->sync = UpdateSync(blkchain->head);
    return status;
}

/* Restore blockchain from a backup file.  */
void RestoreBackup(blockchain_t *blockchain) 
{
    int fd = open("blockchain_backup.txt", O_RDONLY);
    if (fd == -1) {
        printf("No Backup Found: Starting New Blockchain\n");
        return;
    }

    printf("Restoring From Backup\n");
    if (RecoverBlockchain(fd, blockchain) != OK) {
        printf("NOK: backup corrupted/not enough memory\n");
    }
    close(fd);
}