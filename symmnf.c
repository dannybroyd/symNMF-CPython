#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define EPSILON 0.0001


typedef struct t_num
{
    double value;
    struct t_num *next;
}NUM;


typedef struct vector
{
    struct vector *next;
    struct t_num *num_list;
}VECTOR;

double** read_from_file(FILE*);
void free_linked_mem(VECTOR*);

int main(){
    FILE *file;
    file = fopen("B:/code/software_project_final/symNMF-CPython/test_input.txt", "r");
    if (file == NULL){
        printf("wtf");
    }
    read_from_file(file);
}


double** read_from_file(FILE *file){
    VECTOR *head_vec, *curr_vec;
    NUM *head_num, *curr_num;
    int rows = 0, cols;
    double num;
    char c;

    // Memory allocation for the initial structures
    head_num = malloc(sizeof(NUM));
    if (head_num == NULL) {
        printf("An Error Has Occurred\n");
        fclose(file);
        exit(1);
    }

    head_vec = malloc(sizeof(VECTOR));
    if (head_vec == NULL) {
        free(head_num);
        printf("An Error Has Occurred\n");
        fclose(file);
        exit(1);
    }

    curr_num = head_num;
    curr_vec = head_vec;

    // Initialize the linked lists
    curr_num->next = NULL;
    curr_vec->num_list = head_num;
    curr_vec->next = NULL;

    // Read from the file
    while (fscanf(file, "%lf%c", &num, &c) == 2) {
        if (c == '\n') {
            // Create a new vector node
            curr_num->value = num;
            curr_vec->next = malloc(sizeof(VECTOR));
            if (curr_vec->next == NULL) {
                free_linked_mem(head_vec);
                printf("An Error Has Occurred\n");;
                fclose(file);
                exit(1);
            }
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            curr_vec->num_list = malloc(sizeof(NUM));
            if (curr_vec->num_list == NULL) {
                free_linked_mem(head_vec);
                printf("An Error Has Occurred\n");
                fclose(file);
                exit(1);
            }
            curr_num = curr_vec->num_list;
            curr_num->next = NULL;
            continue;
        }

        // Create a new number node
        curr_num->value = num;
        curr_num->next = malloc(sizeof(NUM));
        if (curr_num->next == NULL) {
            free_linked_mem(head_vec);
            printf("An Error Has Occurred\n");
            fclose(file);
            exit(1);
        }
        curr_num = curr_num->next;
        curr_num->next = NULL;
    }

    /* takes care of last empty line */
    free(curr_vec->num_list);
    free(curr_vec);

    // Close the file
    fclose(file);
    free_linked_mem(head_vec);
}


void free_linked_mem(VECTOR *head_vec){
    NUM* curr_num;
    NUM* next_num;
    VECTOR* next_vec;
    VECTOR* curr_vec;


    curr_num = head_vec -> num_list;
    next_num = curr_num -> next;
    curr_vec = head_vec;
    next_vec = head_vec -> next;

    while(curr_vec != NULL){
        while (curr_num != NULL)
        {
            printf("%f,", curr_num->value);
            free(curr_num);
            curr_num = next_num;
            if (next_num != NULL)
            {
                next_num = curr_num -> next;
            }
        }
        printf("\n");
        free(curr_vec);
        curr_vec = next_vec;
        if (curr_vec != NULL){
            next_vec = curr_vec -> next;
            curr_num = curr_vec -> num_list;
            next_num = curr_num -> next;
        }
    }
}

