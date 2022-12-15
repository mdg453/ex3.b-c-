#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60
#define WRONG_INPUT "Usage: int int"
#define ARGC3 3
#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder in case there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case there is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

void print_fun(const void * to_print) {
    const Cell *c = (Cell*)to_print ;
    printf("[%d]",c->number) ;
    if (c->ladder_to != EMPTY){
        printf("-ladder to %d",c->ladder_to) ;
    }
    if (c->snake_to != EMPTY){
        printf("-snake to %d",c->snake_to) ;
    }
    printf(" -> ") ;
}

static int comp_fun (const void* a , const void * b) {
    const Cell* a1 = a ;
    const Cell * b1 = b ;
    return(a1->number == b1->number) ;
}

static void free_data_fun (void * dp) {
    free(dp) ;
}

static void* copy_fun (const void * cp){
    Cell * cell_to_copy = (Cell *) cp ;
    Cell * coppied_cell = malloc(sizeof(Cell)) ;
    coppied_cell->snake_to = cell_to_copy->snake_to ;
    coppied_cell->ladder_to = cell_to_copy->ladder_to ;
    coppied_cell->number = cell_to_copy->number ;
    return coppied_cell  ;
}
static bool is_last_func(const void* last){
    Cell * last_cell = (Cell *) (last) ;
    if(last_cell->number == 100) {
        return  1 ;
    }
    return  0 ;
}

/** Error handler **/
static int handle_error(char *error_msg, MarkovChain **database) {
    printf("%s", error_msg);
    if (database != NULL)
    {
        free_markov_chain(database);
    }
    return EXIT_FAILURE;
}


static int create_board(Cell *cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++) {
                free(cells[j]);
            }
            handle_error(ALLOCATION_ERROR_MASSAGE,NULL);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        }
        else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database(MarkovChain *markov_chain)
{
    Cell* cells[BOARD_SIZE];
    if(create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        add_to_database(markov_chain, cells[i]);
    }

    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        from_node = get_node_from_database(markov_chain,cells[i])->data;

        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            index_to = MAX(cells[i]->snake_to,cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain, cells[index_to])
                    ->data;
            add_node_to_counter_list(from_node, to_node, markov_chain);
        }
        else
        {
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ((Cell*) (from_node->data))->number + j - 1;
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }
                to_node = get_node_from_database(markov_chain, cells[index_to])
                        ->data;
                add_node_to_counter_list(from_node, to_node, markov_chain);
            }
        }
    }
    // free temp arr
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[]) {
    if(argc != ARGC3){
        fprintf(stderr, WRONG_INPUT) ;
        return EXIT_FAILURE ;
    }
    int seed = (int)strtol(argv[1], NULL, 10);
    srand(seed);

    int players_num = (int) strtol(argv[2], NULL, 10);
    if (players_num < 1){
        fprintf(stderr, WRONG_INPUT) ;
        return EXIT_FAILURE ;
    }
    Cell *cell = malloc(sizeof (Cell)) ;
    if (cell == NULL){
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE) ;
        return EXIT_FAILURE ;
    }
    *cell = (Cell ){0,-1,-1} ;
    MarkovChain * base_root = calloc(1, sizeof (MarkovChain));
    if(base_root == NULL) {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE) ;
        return EXIT_FAILURE;
    }
    LinkedList * linked_list = malloc(sizeof(LinkedList)) ;
    if(linked_list == NULL) {
        fprintf(stderr, ALLOCATION_ERROR_MASSAGE) ;
        return EXIT_FAILURE;
    }
    base_root->database = linked_list ;
    *base_root = (MarkovChain){linked_list, &print_fun, &comp_fun,
                               &free_data_fun,copy_fun,is_last_func} ;
    fill_database(base_root) ;
    for (int i = 0 ; i < players_num; i++){
        printf("Random Walk %d: ", i) ;
        generate_random_sequence(base_root,base_root->database->first->data,MAX_GENERATION_LENGTH) ;
    }
    MarkovChain **point_to_base = &base_root ;
    free_markov_chain(point_to_base) ;
}
