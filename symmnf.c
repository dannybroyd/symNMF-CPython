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
void error_alloc_link(VECTOR *);
void print_matrix(double**, int, int);
double similarity_calc(double*, double*, int);
double **sym(double **, int, int);
void free_mat_alloc(double **);
double sum_of_vector(double*, int);
double **ddg(double **, int, int);
double **ddg_in_sequence(double **, int, int, double **);


int main()
{
    int ND[2];
    FILE *file;
    int n,d;
    double **data;
    double **sym_mat, **ddg_mat;

    file = fopen("B:/code/software_project_final/symNMF-CPython/test_input.txt", "r");
    if (file == NULL)
    {
        printf("wtf");
    }

    data = read_from_file(file, ND);
    n = ND[0];
    d = ND[1];

    sym_mat = sym(data, n, d);
    print_matrix(sym_mat, n, n);

    ddg_mat = ddg_in_sequence(data, n, d, sym_mat);
    print_matrix(ddg_mat, n, n);
}


double **sym(double **data, int n, int d){
    double **sym_mat;
    double *rows;
    int i,j;
    /* allocate memory for matrix */
    rows = (double *)calloc(n * n, sizeof(double));
    if (rows == NULL)
    {
        free_mat_alloc(data);
        printf("An Error Has Occurred\n");
        exit(1);    
    }
    sym_mat = (double **)calloc(n, sizeof(double *));
    if (sym_mat == NULL)
    {
        free(rows);
        free_mat_alloc(data);
        printf("An Error Has Occurred\n");
        exit(1);
    }
    for (i = 0; i < n; i++)
    {
        sym_mat[i] = rows + i * n;
    }
    
    for(i = 0; i < n; i++){
        for(j = 0; j < n; j++){
            sym_mat[i][j] = similarity_calc(data[i], data[j], d);
        }
    }
    return sym_mat;
}


double **ddg(double **data, int n, int d){
    double **ddg_mat;
    double *rows;
    int i,j;
    /* allocate memory for matrix */
    rows = (double *)calloc(n * n, sizeof(double));
    if (rows == NULL)
    {
        return NULL;
    }
    ddg_mat = (double **)calloc(n, sizeof(double *));
    if (ddg_mat == NULL)
    {
        free(rows);
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        ddg_mat[i] = rows + i * n;
    }
    
    for(i = 0; i < n; i++){
        for(j = 0; j < n; j++){
            if (i == j){
                ddg_mat[i][j] = sum_of_vector(data[i], d);
            }
            else{
                ddg_mat[i][j] = 0;
            }
        }
    }
    return ddg_mat;

}


double **ddg_in_sequence(double **data, int n, int d, double **sym_mat){
    double **ddg_mat;
    ddg_mat = ddg(data, n, d);
    if (ddg_mat == NULL){
        free_mat_alloc(sym_mat);
        free_mat_alloc(data);
        printf("An Error Has Occurred\n");
        exit(1);
    }
    return ddg_mat;
}


double **norm(double **data, int n, int d, double **sym_mat, double **ddg_mat){
    double **norm_mat;
    double *rows;
    int i,j;
    /* allocate memory for matrix */
    rows = (double *)calloc(n * d, sizeof(double));
    if (rows == NULL)
    {
        return NULL;
    }
    norm_mat = (double **)calloc(n, sizeof(double *));
    if (norm_mat == NULL)
    {
        free(rows);
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        norm_mat[i] = rows + i * n;
    }
    
    for(i = 0; i < n; i++){
        for(j = 0; j < d; j++){
            norm_mat[i][j];
        }
    }
    return norm_mat;
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
                error_alloc_link(head_vec);
            }
            prev_vec = curr_vec;
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            curr_vec->num_list = malloc(sizeof(NUM));
            if (curr_vec->num_list == NULL)
            {
                fclose(file);
                error_alloc_link(head_vec);
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
            error_alloc_link(head_vec);
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
        error_alloc_link(head_vec);
    }
    data = (double **)calloc(n, sizeof(double *));
    if (data == NULL)
    {
        free(rows);
        error_alloc_link(head_vec);
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


double similarity_calc(double *vector1, double *vector2, int d)
{
    double sum = 0;
    int i;
    for (i = 0; i < d; i++)
    {
        sum += pow((vector1[i] - vector2[i]), 2);
    }
    return exp(-sum/2);
}


void error_alloc_link(VECTOR *head_vec)
{
    free_linked_mem(head_vec);
    printf("An Error Has Occurred\n");
    exit(1);
}


void free_mat_alloc(double **data)
{
    free(data[0]);
    free(data);
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


double sum_of_vector(double *vector, int d){
    int i;
    double sum = 0;
    for (i = 0; i < d; i++){
        sum += vector[i];
    }
    return sum;
}


double **pow_diag_matrix(double** diag, int rows, int cols, int power){
    
}
