#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>  // For printf(), sscanf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool

#define ALLOCATION_ERROR_MASSAGE "Allocation failure: Failed to allocate new memory\n"


/***************************/
/*   insert typedefs here  */
/***************************/


/***************************/



/***************************/
/*        STRUCTS          */
/***************************/
typedef struct NextNodeCounter;

typedef struct MarkovNode {
    void *data; //pointer to word
    NextNodeCounter *counter_list; //point to all possible next words
    int counter_list_size ;
} MarkovNode;

typedef struct NextNodeCounter {
    struct MarkovNode *markov_node ; //point to the node with the next possible word
    int frequency;//counts the number of times that word2
    // appears right after word 1 in the text
} NextNodeCounter;

/* DO NOT ADD or CHANGE variable names in this struct */
typedef struct MarkovChain {
    LinkedList *database;
    //void *print_func(a) ;
    typedef int *comp_func(a, b) ;
    //void *free_data(a) ;
    //typedef char[] *copy_func(a) ;
    //typedef bool is_last(a) ;

    // pointer to a func that receives data from a generic type and prints it
    // returns void.
    /* <fill_type> */ print_func;

    // pointer to a func that gets 2 pointers of generic data type(same one) and compare between them */
    // returns: - a positive value if the first is bigger
    //          - a negative value if the second is bigger
    //          - 0 if equal
    /* <fill_type> */ comp_func;

    // a pointer to a function that gets a pointer of generic data type and frees it.
    // returns void.
    /*<fill_type>*/ free_data;

    // a pointer to a function that  gets a pointer of generic data type and returns a newly allocated copy of it
    // returns a generic pointer.
    /*<fill_type>*/ copy_func;

    //  a pointer to function that gets a pointer of generic data type and returns:
    //      - true if it's the last state.
    //      - false otherwise.
    /*<fill_type>*/ is_last;
} MarkovChain;

/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain);

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr);

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose a random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_random_sequence(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length);

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_markov_chain(MarkovChain **markov_chain);

/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_counter_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain);

/**
* Check if data_ptr is in database. If so, return the markov_node wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr);

/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return node wrapping given data_ptr in given chain's database
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr);

#endif /* MARKOV_CHAIN_H */
