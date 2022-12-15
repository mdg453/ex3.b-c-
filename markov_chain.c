#include "markov_chain.h"
#include "linked_list.h"
#include <string.h>
#define NUMS "not enough words for sentence"

/**
* Get random number between 0 and max_number [0, max_number).
* @param max_number maximal number to return (not including)
* @return Random number
*/

int get_random_number(int max_number)
{
    return rand() % max_number;
}

/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
*/
MarkovNode* get_first_random_node(MarkovChain *markov_chain){
    if(markov_chain->database->first == NULL){
        fprintf(stderr, BAD_CHAIN);
        return NULL ;
    }
    Node *rand_node = markov_chain->database->first ;
    int ran_num = get_random_number(markov_chain->database->size) ;
    for (int i = 0 ; i < ran_num  ; ++i) {
        rand_node = rand_node->next ;
    }
    while(markov_chain->is_last(rand_node->data->data)){
        ran_num = get_random_number(markov_chain->database->size) ;
        rand_node = markov_chain->database->first ;
        for (int i = 0 ; i < ran_num  ; ++i) {
            rand_node = rand_node->next ;
        }
    }
    return rand_node->data ;
}


/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr){
    if(state_struct_ptr == NULL){
        fprintf(stderr, BAD_CHAIN);
        return NULL ;
    }
    if(state_struct_ptr->counter_list == NULL){
        fprintf(stderr, BAD_CHAIN);
        return NULL ;
    }
    int sum = 0 ;
    for (int j = 0 ; j < state_struct_ptr->counter_list_size; j++) {
        sum += state_struct_ptr->counter_list[j].frequency ;
    }
    int ran_num = get_random_number(sum);
    for (int i = 0 ; i < state_struct_ptr->counter_list_size; i++) {
        ran_num -= state_struct_ptr->counter_list[i].frequency ;
        if (ran_num < 0){
            return state_struct_ptr->counter_list[i].markov_node ;
        }
    }
    return state_struct_ptr->counter_list
    [state_struct_ptr->counter_list_size].markov_node ;
}

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with,
 *                   if NULL- choose a random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_random_sequence(MarkovChain *markov_chain,
                              MarkovNode *first_node, int max_length){
    if(max_length < 1) {
        fprintf(stderr, NUMS) ;
        return;
    }
    first_node = get_first_random_node(markov_chain) ;
    markov_chain->print_func(first_node->data) ;
    int j = 1 ;
    while (!markov_chain->is_last(first_node->data) &&
           j < max_length) {
        first_node = get_next_random_node(first_node);
        if (!markov_chain->is_last(first_node->data)){
            markov_chain->print_func(first_node->data) ;
        }
        else {
            markov_chain->print_func(first_node->data) ;
        }
        j++ ;
    }

    printf("\n") ;
}

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free

*/

void free_markov_chain(MarkovChain ** ptr_chain){
    if (ptr_chain == NULL){
        return;
    }
    MarkovChain markov_chain = **ptr_chain ;
    if (markov_chain.database == NULL) {
        free(*ptr_chain) ;
        *ptr_chain = NULL ;
        return;
    }
    if(markov_chain.database->first == NULL){
        free(markov_chain.database) ;
        free(*ptr_chain) ;
        *ptr_chain = NULL ;
        return;
    }
    for(int i = 0 ; i < markov_chain.database->size ; i++) {
        free(markov_chain.database->first->data->data) ;
        free(markov_chain.database->first->data->counter_list) ;
        free(markov_chain.database->first->data) ;
        markov_chain.database->first = markov_chain.database->first->next ;
    }
    free(markov_chain.database) ;
    *ptr_chain = NULL ;

}


/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_counter_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain){
    if (first_node == NULL || second_node == NULL)
    {
        return false;
    }
    if(markov_chain->is_last(first_node->data)) {
        return EXIT_SUCCESS ;
    }
    if (first_node->counter_list_size == 0)
    {
        first_node->counter_list = calloc(1, sizeof(NextNodeCounter));
        if(first_node->counter_list == NULL) {
            fprintf(stderr, ALLOCATION_ERROR_MASSAGE) ;
            return EXIT_FAILURE ;
        }
        first_node->counter_list_size++;
        first_node->counter_list[0] = (NextNodeCounter)
                {second_node,1};
        return true;
    }
    for(int i = 0; i < first_node->counter_list_size; i++)
    {
        char* first_node_data = first_node->counter_list[i].markov_node->data;
        if(strcmp(first_node_data, second_node->data) == 0)
        {
            first_node->counter_list[i].frequency++;
            return true;
        }
    }
    first_node->counter_list_size++ ;
    NextNodeCounter *temp =
            realloc(first_node->counter_list,
                    (first_node->counter_list_size)*sizeof(NextNodeCounter));
    if(temp == NULL) {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE) ;
        return EXIT_FAILURE ;
    }
    first_node->counter_list = temp ;
    first_node->counter_list[first_node->counter_list_size-1] =
            (NextNodeCounter){second_node,1};
    return true;


}
/**
* Check if data_ptr is in database. If so, return the markov_node wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr){
    if(markov_chain == NULL){
        fprintf(stderr,"NO DATA IN MARKOV CHAIN") ;
        return NULL ;
    }
    Node *traveler = markov_chain->database->first;
    for (int i = 0; i < markov_chain->database->size; i++) {
        if (markov_chain->comp_func(traveler->data->data, data_ptr) == 0){
            return traveler;
        }
        traveler = traveler->next ;
    }
    return NULL ;
}

/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return markov_node wrapping given data_ptr in given chain's database,
 * returns NULL in case of memory allocation failure.
 */

Node* add_to_database(MarkovChain *markov_chain, void *data_ptr){
    if(markov_chain == NULL){
        fprintf(stderr,"NO DATA IN MARKOV CHAIN") ;
        return NULL ;
    }
    if(markov_chain->database == NULL) {
        fprintf(stderr, "NO DATA IN MARKOV CHAIN");
        return NULL;
    }
    Node *traveler = get_node_from_database(markov_chain,data_ptr) ;
    if (traveler != NULL){
        return traveler;
    }
    MarkovNode *markov_node = calloc(1, sizeof(MarkovNode)) ;
    if(markov_node == NULL){
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE) ;
        return NULL ;
    }
    markov_node->data = malloc(strlen(data_ptr)+1);
    if(markov_node->data == NULL){
        free(markov_node) ;
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE) ;
        return NULL ;
    }
    markov_node->data = markov_chain->copy_func(data_ptr);
    markov_node->counter_list = NULL;
    markov_node->counter_list_size = 0;
    add(markov_chain->database, markov_node);
    return markov_chain->database->last;
}






