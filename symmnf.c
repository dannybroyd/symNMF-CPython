#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define EPSILON 0.0001

typedef struct t_num
{
    double value;
    struct t_num *next;
} NUM;

typedef struct vector
{
    struct vector *next;
    struct t_num *num_list;
} VECTOR;

double **read_from_file(FILE *, int *);
void free_linked_mem(VECTOR *);
double **mat_from_linked(VECTOR *, int, int);
void error_alloc(VECTOR *);
void print_matrix(double**, int, int);

int main()
{
    int ND[2];
    FILE *file;
    int n,d;
    double **data;
    file = fopen("B:/code/software_project_final/symNMF-CPython/input_1.txt", "r");
    if (file == NULL)
    {
        printf("wtf");
    }

    data = read_from_file(file, ND);
    n = ND[0];
    d = ND[1];
    print_matrix(data, n, d);
    printf("n is: %d\nd is: %d", ND[0], ND[1]);
}

double **read_from_file(FILE *file, int ND[2])
{
    VECTOR *head_vec, *curr_vec, *prev_vec;
    NUM *head_num, *curr_num;
    double **data;
    int rows = 0, elements = 0;
    double num;
    char c;

    // Memory allocation for the initial structures
    head_num = malloc(sizeof(NUM));
    if (head_num == NULL)
    {
        printf("An Error Has Occurred\n");
        fclose(file);
        exit(1);
    }

    head_vec = malloc(sizeof(VECTOR));
    if (head_vec == NULL)
    {
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
    while (fscanf(file, "%lf%c", &num, &c) == 2)
    {
        elements++;
        if (c == '\n')
        {
            rows++;
            // Create a new vector node
            curr_num->value = num;
            curr_vec->next = malloc(sizeof(VECTOR));
            if (curr_vec->next == NULL)
            {
                fclose(file);
                error_alloc(head_vec);
            }
            prev_vec = curr_vec;
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            curr_vec->num_list = malloc(sizeof(NUM));
            if (curr_vec->num_list == NULL)
            {
                fclose(file);
                error_alloc(head_vec);
            }
            curr_num = curr_vec->num_list;
            curr_num->next = NULL;
            continue;
        }

        // Create a new number node
        curr_num->value = num;
        curr_num->next = malloc(sizeof(NUM));
        if (curr_num->next == NULL)
        {
            fclose(file);
            error_alloc(head_vec);
        }
        curr_num = curr_num->next;
        curr_num->next = NULL;
    }

    /* take care of empty last line */
    prev_vec->next = NULL;
    free(curr_vec);
    free(curr_num);
    /* send n and d back */
    ND[0] = rows;
    ND[1] = elements / rows;

    // Close the file
    fclose(file);

    data = mat_from_linked(head_vec, ND[0], ND[1]);
    free_linked_mem(head_vec);

    return data;
}

double **mat_from_linked(VECTOR *head_vec, int n, int d)
{
    double **data;
    double *rows;
    int i, j;
    VECTOR *curr_vec;
    NUM *curr_num;

    curr_vec = head_vec;
    curr_num = curr_vec->num_list;
    rows = (double *)calloc(n * d, sizeof(double));
    if (rows == NULL)
    {
        error_alloc(head_vec);
    }
    data = (double **)calloc(n, sizeof(double *));
    if (data == NULL)
    {
        free(rows);
        error_alloc(head_vec);
    }
    for (i = 0; i < n; i++)
    {
        data[i] = rows + i * d;
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < d; j++)
        {
            data[i][j] = curr_num->value;
            curr_num = curr_num->next;
        }
        curr_vec = curr_vec->next;
        if (curr_vec != NULL){
            curr_num = curr_vec->num_list;
        }
    }
    return data;
}

void free_linked_mem(VECTOR *head_vec)
{
    NUM *curr_num;
    NUM *next_num;
    VECTOR *next_vec;
    VECTOR *curr_vec;

    curr_num = head_vec->num_list;
    next_num = curr_num->next;
    curr_vec = head_vec;
    next_vec = head_vec->next;

    while (curr_vec != NULL)
    {
        while (curr_num != NULL)
        {
            free(curr_num);
            curr_num = next_num;
            if (next_num != NULL)
            {
                next_num = curr_num->next;
            }
        }
        free(curr_vec);
        curr_vec = next_vec;
        if (curr_vec != NULL)
        {
            next_vec = curr_vec->next;
            curr_num = curr_vec->num_list;
            next_num = curr_num->next;
        }
    }
}


void error_alloc(VECTOR *head_vec)
{
    free_linked_mem(head_vec);
    printf("An Error Has Occurred\n");
    exit(1);
}


void print_matrix(double **mat, int rows, int cols){
    int i,j;
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            printf("%f,", mat[i][j]);
        }
        printf("\n");
    }
}
