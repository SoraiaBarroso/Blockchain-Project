#include "my_blockchain.h"

/*==========[2.1 BLOCKCHAIN SYSTEM: ADD NODE]=================


===================================================*/

/* Return a node with the number identifier. 
   If none is found, NULL is returned.  */
node_t *PullNode(int nid, node_t *node)
{
    while (node != NULL) {
        if (node->nid == nid) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

/* Push a new node with the number identifier
   to the end of List. If malloc failed, 
   NULL is returned.  */
node_t *PushNode(int nid, node_t **head)
{
    /* Create new Node  */ 
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL) {
        return NULL;
    } else {
        new->nid = nid;
        new->bid = NULL;
        new->next = NULL;
    }

    /* Push node to end of List  */
    node_t *current = *head;
    if (current == NULL) {
        *head = new;
    } else {
        while(current->next != NULL) {
            current = current->next;
        }
        current->next = new;
    }
    return new;
}
bool InvalidNid(char *nid)
{
    if (*nid == '-') nid++;
    char *length = nid;
    while (*nid != '\0') {
        if (*nid < '0' || *nid++ > '9') {
            return (nid[-1] != ':' || nid[0] != '\0');
        }
    }
    return nid-length > 9;
}
/* Official ADND function.  */
int AddNode(blockchain_t *blockchain)
{
    /* Usage format: [NID]...  */ 
    char **argv_nid = blockchain->user.argv;
    if (!*argv_nid) {
        return printf(ERROR_LACKINGARG);
    }
    
    /* If [NID]... does not yet exist, push it.  */ 
    int error = 0;
    for ( ; *argv_nid != NULL ; argv_nid++) {
        if (InvalidNid(*argv_nid)) {
            error += printf(ERROR_INVALIDNID, *argv_nid);
            continue;
        }
        int nid = atoi(*argv_nid);
        if (PullNode(nid, blockchain->head) != NULL) {
            error += printf(ERROR_NIDFOUND, nid);
            continue;
        }
        if (PushNode(nid, &blockchain->head) == NULL) {
            return printf(ERROR_MALLOCFAIL);
        }
        blockchain->size++; 
    }
    return error;
}

/*===========[2.2 BLOCKCHAIN SYSTEM: ADD BLOCK]=================


===================================================*/

/* Return true if the two strings are exact match.  */
bool IsMatch(char *str1, char *str2)
{
    while (*str1 && *str1++ == *str2++) {};
    return (str1[-1] == str2[-1]);
}

/* Return an allocated exact copy of string.  */
char *StrcpyMalloc(char *string)
{
    /* Count string length.  */
    char *end = string;
    while (*end++ != '\0') {};
    
    /* Malloc and copy string.  */
    int size = end-string;
    if ((string = malloc(size)) != NULL) {
        for (string += size; size--; ) {
            *--string = *--end;
        }
    }
    return string;
}

/* Return a block with the block identifier. 
   If none is found, NULL is returned.  */
block_t *PullBlock(char *data, block_t *block)
{
    while (block != NULL) {
        if (IsMatch(block->data, data)) {
            return block;
        }
        block = block->next;
    }
    return NULL;
}
/* Create a new block with BID identifier.  */ 
block_t *CreateBlock(char *BID)
{
    block_t *new;
    if ((new  = malloc(sizeof(block_t))) != NULL) {
        if ((new->data = StrcpyMalloc(BID)) == NULL) {
            free(new);
            new = NULL;
        }
    }
    return new;
}
/* Link NEW block at the end of List.  */ 
int StackBlock(block_t *NEW, block_t **BLOCK)
{
    if (NEW == NULL) return -1;
    block_t *current = *BLOCK;
    if (current == NULL) {
        *BLOCK = NEW;
    } else {
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = NEW;
    }
    return OK;
}

/* Push a new block with the block identifier
   to the end of List. If malloc failed, 
   NULL is returned.  */
block_t *PushBlock(char *data, node_t *node)
{
    block_t *new = CreateBlock(data);
    if (new != NULL) {
        new->next = NULL;
        StackBlock(new, &node->bid);
    }
    return new;
}

/* Official ADBK function.  */
int AddBlock(blockchain_t *blockchain)
{
    /* Usage format: [BID] [NID]... */ 
    char *argv_bid = blockchain->user.argv[0];
    char **argv_nid = blockchain->user.argv+1;
    if (!argv_bid || !*argv_nid) {
        return printf(ERROR_LACKINGARG);
    }

    /* [NID = *]: Push [BID] to all nodes  */
    node_t *node = blockchain->head;
    if (IsMatch(*argv_nid, "*")) {
        while (node != NULL) {
            if (PullBlock(argv_bid, node->bid) == NULL) {
                if (PushBlock(argv_bid, node) == NULL) {
                    return printf(ERROR_MALLOCFAIL);
                }
            }
            node = node->next;
        }
        return OK;
    }

    /* Push [BID] to [NID]...  */
    int error = 0;
    for ( ; *argv_nid != NULL; argv_nid++) {
        if (InvalidNid(*argv_nid)) {
            error += printf(ERROR_NIDMISSING, *argv_nid);
            continue;
        }
        int nid = atoi(*argv_nid);
        node = PullNode(nid, blockchain->head);
        if (node == NULL) {
            error += printf(ERROR_NIDMISSING, *argv_nid);
            continue;
        }
        if (PullBlock(argv_bid, node->bid) != NULL) {
            error += printf(ERROR_BIDFOUND, nid, argv_bid);
            continue;
        }    
        if (PushBlock(argv_bid, node) == NULL) {
            return printf(ERROR_MALLOCFAIL);
        }
    }
    return error;
}

/*==========[2.3 BLOCKCHAIN SYSTEM: REMOVE NODE]=================


===================================================*/

/* For a blockList, free all allocated memory  */
void FreeBlockList(block_t *block)
{
    block_t *next = block;
    while ((block = next) != NULL) {
        next = block->next;
        if (block->data) {
            free(block->data);
        }
        free(block);
    }
}

/* In contrast to PullNode, this function detaches
   the pulled node from its linked-List.  */
node_t *PullOutNode(int nid, node_t **head)
{
    node_t *prev = NULL;
    node_t *node = *head;
    if (node == NULL) {
        return NULL;
    }
    if (node->nid == nid) {
        *head = node->next;
        return node;
    }
    while (prev = node, (node = node->next) != NULL) {
        if (node->nid == nid) {
            prev->next = node->next;
            return node;
        }
    }
    return NULL;
}

/* Official RMND function.  */
int RemoveNode(blockchain_t *blockchain)
{
    /* Usage Format: [NID]...  */
    char **argv_nid = blockchain->user.argv;
    if (!*argv_nid) {
        return printf(ERROR_LACKINGARG);
    }

    /* [NID = '*']: Clear all  */
    node_t *node = blockchain->head;
    node_t *next = node;    
    if (IsMatch(*argv_nid, "*")) {
        while ((node = next) != NULL) {
            next = node->next;
            FreeBlockList(node->bid);
            free(node);
        }
        blockchain->head = NULL;
        blockchain->size = 0;
        return OK;
    }

    /* Find and remove [NID]...  */
    int error = 0;
    for ( ; *argv_nid != NULL; argv_nid++) {
        if (InvalidNid(*argv_nid)) {
            error += printf(ERROR_NIDMISSING, *argv_nid);
            continue;
        }
        int nid = atoi(*argv_nid);
        node = PullOutNode(nid, &blockchain->head);
        if (node == NULL) {
            error += printf(ERROR_NIDMISSING, *argv_nid);
            continue;
        }
        free(node);
        blockchain->size--;
    }
    return error;
;
}

/*==========[2.4 BLOCKCHAIN SYSTEM: REMOVE BLOCK]=================


===================================================*/

/* In contrast to PullBlock, this function detaches
   the pulled block from its linked-List.  */
block_t *PullOutBlock(char *argv_bid, block_t **node)
{
    block_t *back = NULL;
    block_t *block = *node;
    if (block == NULL) {
        return NULL;
    }
    if (IsMatch(block->data, argv_bid)) {
        *node = block->next;
        return block;
    }
    while (back = block, (block = block->next) != NULL) {
        if (IsMatch(block->data, argv_bid)) {
            back->next = block->next;
            return block;
        }
    }
    return NULL;
}

/* Official RMBK function.  */
int RemoveBlock(blockchain_t *blockchain)
{
    /* Usage Format: [BID] [NID]... */
    char *argv_bid = blockchain->user.argv[0];
    char **argv_nid = blockchain->user.argv+1;
    if (!argv_bid) {
        return printf(ERROR_LACKINGARG);
    }

    /* [NID = NULL or *]: Remove [BID] from all nodes  */
    node_t *node = blockchain->head;
    if (!*argv_nid || IsMatch(*argv_nid, "*")) {
        while (node != NULL) {
            PullOutBlock(argv_bid, &node->bid);
            node = node->next;
        }
        return OK;
    }

    /* Find and remove [BID] inside [NID]...  */
    block_t *block;
    int error = 0;
    for ( ; *argv_nid != NULL; argv_nid++) {
        if (InvalidNid(*argv_nid)) {
            error += printf(ERROR_NIDMISSING, *argv_nid);
            continue;
        }
        int nid = atoi(*argv_nid);
        node = PullNode(nid, blockchain->head);
        if (node == NULL) {
            error += printf(ERROR_NIDMISSING, *argv_nid);
            continue;
        }
        if ((block = PullOutBlock(argv_bid, &node->bid)) == NULL) {
            error += printf(ERROR_BIDMISSING, nid, argv_bid);
            continue;
        }
        free(block);
    }
    return error;
}

/*=============[2.5 BLOCKCHAIN SYSTEM: CHECK SYNC STATE]=================


=================================================*/

/* Count the number of blocks  */
int CountBlocks(block_t *block)
{
    int i = 0;
    while (block != NULL) block = block->next, i++;
    return i;
}

/* Return the sync state of blockchain  */
char UpdateSync(node_t *node)
{
    if (node == NULL) {
        return SYNC_TRUE;
    }
    block_t *firstnode = node->bid;
    block_t *curr;
    int length = CountBlocks(firstnode);
    
    while ((node = node->next) != NULL) {
        /* Test 1: Same length?  */
        if (length != CountBlocks(node->bid)) {
            return SYNC_FALSE;
        }
        /* Test 2: Same composition?  */
        for (curr = firstnode; curr != NULL; curr = curr->next) {
            if (PullBlock(curr->data, node->bid) == NULL) {
                return SYNC_FALSE;
            }
        }
    }
    return SYNC_TRUE;
}

/*=============[2.6 BLOCKCHAIN SYSTEM: SYNCING]=================


=================================================*/

/* Push a malloced copy of source to the end of target.  */
int PushBlockList(block_t *source, block_t **target)
{
    block_t *firstblock = malloc(sizeof(block_t));
    block_t *current = firstblock;
    if (firstblock == NULL) {
        return -1;
    }
    if (source == NULL) {
        free(firstblock);
        return 0;
    }
    while (source != NULL) {
        /* Copy 1: data  */
        if ((current->data = StrcpyMalloc(source->data)) == NULL) {
            FreeBlockList(firstblock);
            return -1;
        }
        /* Copy 2: next  */
        if ((source = source->next) != NULL) {
            if ((current = current->next = malloc(sizeof(block_t))) == NULL) {
                FreeBlockList(firstblock);
                return -1;
            }
        }
    }
    /* Push the resulting copy to the end of target  */
    current->next = NULL;
    StackBlock(firstblock, target);
    return 0;
}

/* Official SYNC function.  */
int Synchronize(blockchain_t *blockchain)
{
    for (char **argv = blockchain->user.argv; *argv != NULL; ) {
        printf(ERROR_UNKNOWNARG, *argv++);
    }

    node_t *node = blockchain->head;
    block_t *forward = NULL;    /* Previous nodes have; but current does not.  */ 
    block_t *stationary;        /* Both previous and current nodes have.  */ 
    block_t *backward;          /* Previous nodes doesn't have; but the current does.  */ 

    
    for ( ; node != NULL; node = node->next) {
        /* Segment 1: Classify blocks into the above category  */
        backward   = NULL;
        stationary = NULL;
        for (block_t *bid = node->bid ; bid != NULL; bid = bid->next) {
            block_t *tmp = PullOutBlock(bid->data, &forward);
            if (tmp == NULL) {
                if ((tmp = CreateBlock(bid->data)) == NULL) {
                    return printf(ERROR_MALLOCFAIL);
                }
                tmp->next = backward;
                backward = tmp;
            } else {
                tmp->next = stationary;
                stationary = tmp;
            }
        }
        /* Segment 2: Spread new blocks backward and forward.  */
        if (PushBlockList(forward, &node->bid) == -1) {
            return printf(ERROR_MALLOCFAIL);
        }
        for (node_t *i = blockchain->head; i != node; i = i->next) {
            if (PushBlockList(backward, &i->bid) == -1) {
                return printf(ERROR_MALLOCFAIL);
            }
        }
        /* Segment 3: Rejoin all the blocks.  */
        StackBlock(forward, &backward);
        StackBlock(stationary, &backward);
        forward = backward;
    }
    return OK;
}

/*=============[2.8 BLOCKCHAIN SYSTEM: ListING]=================


=================================================*/

/* Official List function.  */
int List(blockchain_t *blockchain)
{
    bool opt_l = false;
    char **argv = blockchain->user.argv;
    for ( ; *argv != NULL; argv++) {
        if (IsMatch(*argv, "-l")) {
            opt_l = true;
        } else {
            printf(ERROR_UNKNOWNARG, *argv);
        }
    }

    node_t *current = blockchain->head;
    block_t *block;
    while (current != NULL) {
        /* Print node  */
        printf("%d", current->nid);
        if (opt_l == true) {
            printf(":");
            /* Display node's blocks  */
            if ((block = current->bid) != NULL) {
                printf(" %s", block->data);
                while ((block = block->next)) {
                    printf(", %s", block->data);
                }
            }
        }
        printf("\n");
        current = current->next;
    }

    return SILENT_OK;
}