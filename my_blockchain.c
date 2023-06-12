#include "my_blockchain.h"

/*=============[MAIN]=================


=======================================*/

/* Execute the requested operation.  */
int ExecuteCommand(blockchain_t *blockchain)
{
    switch (blockchain->user.command) {
        case ADND: return AddNode      (blockchain);
        case ADBK: return AddBlock     (blockchain);
        case RMND: return RemoveNode   (blockchain);
        case RMBK: return RemoveBlock  (blockchain);
        case LIST: return List          (blockchain);
        case SYNC: return Synchronize   (blockchain);
    }
    return printf(ERROR_NOCOMMAND);
}

/* Create and manage blockchain.  */
int main(int argc, char** argv)
{   
    if (argc == 1 && argv)
    {
        blockchain_t blockchain = {
            .head = NULL, 
            .size = 0, 
            .sync = SYNC_TRUE, 
            .user.command = NONE,
            .user.excess = NULL
        };
        RestoreBackup(&blockchain);

        while (GetCommand(&blockchain)) {
            if (ExecuteCommand(&blockchain) == OK) {
                printf(SUCCESS);
            }
            blockchain.sync = UpdateSync(blockchain.head);
            free(blockchain.user.argv);
        }
        Quit(&blockchain);
    }
}