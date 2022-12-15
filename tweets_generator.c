#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "linked_list.h"
#include "markov_chain.h"
#define WRONG_INPUT "Usage: int int filepath int"
#define ARGC5 5
#define ARGC4 4
#define FILE_FAIL "Error:The given file is invalid.\n"
#define NUM_OF_CHARS  100000000
#define NUM1000 1000
#define MAX_TWEET_LEN 20
//int *p = void* bsearch(&to_find, const void pinter, count, sizeofint, *(int compare(void, void))
//if p == NULL return Exit
//printf("%d", *p)
//qsort(arr, nmemb, sizeof(char*),cmp)
#define LAST_INDX(s) (strlen(s) - 1)


void print_fun(const void * to_print) {
    const char *s = to_print ;
    printf("%s",s) ;
}

static int comp_fun (const void* a , const void * b) {
    const char* a1 = a ;
    const char* b1 = b ;
    return(strcmp(a,b)) ;
}

static void free_data_fun (void * dp) {
    free_markov_chain(dp) ;
}

static void* copy_fun (const void * cp){
    char * str_to_cp =(char *) cp ;
    char * coppied_str = malloc(strlen((char *)str_to_cp)+1) ;
    strcpy(coppied_str, str_to_cp) ;
    return coppied_str  ;
}
static bool is_last_func(const void* last){

    char * last_str = (char *) (last) ;
    return strcmp((char *) (last_str + LAST_INDX(last_str)) , ".") == 0 ;
}

static int fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain){
    char text[NUM1000 + 1];
    char *token;
    MarkovNode * previus_word = NULL;
    Node * current_word ;
    while(fgets(text ,NUM_OF_CHARS,fp ) != NULL && words_to_read) {
        text[strcspn(text, "\n")] = '\0' ;
        token = strtok(text, " \r\n");
        while(token != NULL && words_to_read > 0) {
            current_word = add_to_database(markov_chain, token) ;
            add_node_to_counter_list(previus_word, current_word->data, markov_chain);
//            previus_word = copy_fun(current_word->data) ;
            previus_word = current_word->data;
            token = strtok(NULL, " \r\n");
            words_to_read--  ;
        }
    }
    return EXIT_SUCCESS ;
}

int main(int argc ,char* argv[]){
    if(argc < ARGC4) {
        fprintf(stderr, WRONG_INPUT) ;
        return EXIT_FAILURE ;
    }
    unsigned int seed = strtol(argv[1], NULL, 10);
    int tweets_num = (int) strtol(argv[2], NULL, 10);
    int num_of_chars_to_read = NUM_OF_CHARS ;
    srand(seed);
    if(argc == ARGC5) {
        num_of_chars_to_read = (int) strtol(argv[4], NULL, 10);;
    }
    char* input_path = argv[3] ;
    MarkovChain * base_root = calloc(1, sizeof (MarkovChain));
    if(base_root == NULL) {
        return EXIT_FAILURE;
    }
    LinkedList *linked_list = malloc(sizeof(LinkedList)) ;
    if(linked_list == NULL){
        fprintf(stderr,ALLOCATION_ERROR_MASSAGE) ;
        return EXIT_FAILURE ;
    }
    *base_root = (MarkovChain){linked_list, &print_fun, &comp_fun,
                               &free_data_fun,copy_fun,is_last_func} ;

    base_root->database = linked_list ;
    FILE* in = fopen ( input_path, "r") ;
    if (in == NULL) {
        fprintf(stderr,FILE_FAIL ) ;
        return EXIT_FAILURE ;
    }
    fill_database(in, num_of_chars_to_read,base_root);
    for (int i = 0 ; i<tweets_num; i ++) {
        printf("Tweet %d: ",i+1) ;
        generate_random_sequence(base_root,
                                 base_root->database->first->data, MAX_TWEET_LEN);
    }
    MarkovChain **point_to_base = &base_root ;
    //printf("\n%d",base_root->database->size);
    free_markov_chain(point_to_base) ;
    fclose(in) ;
}
