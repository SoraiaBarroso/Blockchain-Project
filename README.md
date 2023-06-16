# Welcome to My Blockchain
***
Group project -- Adrian Sardinata and Soraia Lima
## Task
my_blockchain is a project that creates a program that allows users to execute commands for creating and managing a blockchain.

Blockchain is a distributed ledger technology that enables the secure sharing of information, it is a secure database which records information in a way that makes it difficult or impossible to change or tamper the system. <br>
Blockchain allows for the permanent, immutable, and transparent recording of data and transactions.

Our task is to make a small implementation of this technology, through commands we will make changes to the Blockchain.<br>
This involves working with Linked List to add data or remove it from the blockchain and files to save the changes done to it.<br>

## Description
**Design Architecture:** This project is divided into three major segments, and *'my_blockchain.c'* is the main file which heads the most major control-flow. *Interface* is responsible for informing the user on the current state of the blockchain, and requesting for further command on what to do. Upon receiving an intelligible command, *System* executes the requested operation and reports any error which occurred during it. This *interface-system* looping relationship is what forms the core activity of the program. *Backup* meanwhile focuses on the inter-program: when the program ends, it deposits the blockchain into a storage file, and when the program restarts it re-pickup the blockchain. These three separate sub-systems are altogether integrated under *'my_blockchain.c'*.

**Implementation:** <br>
*'blockchain_backup.c':* The backup system is itself composed of two parts - the deposit and the recover. Whilst both fairly simple by themselves, the guiding central concept which allows them to work in unison is the standard adopted in the storage file. In contrast to other approaches which deposits dynamic-length lines and recovers them using readline, our implementation prefer the much shorter and performance efficient approach of depositing in units of fixed-sized lines.

*'blockchain_interface.c':* Our approach to solving this issue is fairly mundane string manipulations; however what may be quirky are the small innovations - all of which seeks the very maximal use of a user buffer. Firstly, rather than reinitialize the buffer each time, it is embedded into a very mobile and reusable struct. Secondly, rather than having to strcpy its arguments into new strings, our approach cleans the buffer in-situ and have a single array (which are also saved into the same struct) pointing to its various arguments. This saves from a lot of processes from strcpy, malloc and free.

*'blockchain_system.c':* By far the biggest file in the project and manages every command except quit, which includes: <br>
1. Add nodes and blocks. The parallel concepts developed here are push and pull. Before pushing new NIDs or BIDs, the function tries to pull from the existing blockchain, to ensure that they do not yet exist and to ensure the node to add the block onto exists. <br>
2. Remove nodes and blocks. The concept here is of pullout. Unlike the regular pull, pullout also detaches the identified NID or BID from the linked-list from where they are found. Thus, they could be easily freed without disrupting the singly linked-list. <br>
3. Sync and list nodes. While list works fairly straightforward, the synchronization method we've chosen is deliberately more complex. Rather than separate the process of collecting all unique blocks from the blockchain away from its redistribution, our approach combines them together in order to avoid double-inspection. Additionally multiple blocks could be created and pushed in large batches than individually, which also saves resources. <br>

Throughout these processes, there were many instances of successful cosharing of functions, among which are the push, pull and pullout concepts previously mentioned. In between files, *backup* particularly reused many functions from *blockchain* (e.g. AddNode, AddBlock) and *interface* (e.g. CollectArguments).

## Installation
Only a gcc compiler is required. <br/>
There is no other installation needed.

## Usage
```
Usage: 
$> ./my_blockchain 
$> RUNTIME INFORMATION> [RUNTIME COMMAND]...

Manage and secure information stored inside of blocks by 
decentralizing it across multiple nodes. The nodes will
decide through rudimentary consensus algorithm the 
correct information if one or more node are compromised.

RUNTIME INFORMATION:
  state of synchronization      an 's' is indicated when all nodes have a consensus
                                of the correct set of information, and '-' for otherwise.
  number of nodes               a number shows the number of nodes active in circulation.

RUNTIME COMMAND:
  'add node [NID]'              NID identified node is added to the circulation.
  'rm node [NID]...'            NID identified node(s) are removed from the circulation.
                                If NID is '*', then all nodes will be removed.
  'add block [BID] [NID]...'    BID identified block is added to NID identified node(s).
                                If NID is '*', then all nodes will be added to.
  'rm block [BID] [NID]...'     BID identifed block is removed from NID identified nodes.
                                If NID is '*' or empty, then all nodes will be seized from.
  'ls [OPTION]'                 List all nodes in circulation by their NID identifiers.
                                OPTION '-l' shows the nodes' blocks by their BID identifiers.
  'sync'                        Every node will have the same set of BID identified blocks.
  'quit'                        The blockchain is saved before leaving.
  
```

### The Core Team


<span><i>Made at <a href='https://qwasar.io'>Qwasar SV -- Software Engineering School</a></i></span>
<span><img alt='Qwasar SV -- Software Engineering School's Logo' src='https://storage.googleapis.com/qwasar-public/qwasar-logo_50x50.png' width='20px'></span>
