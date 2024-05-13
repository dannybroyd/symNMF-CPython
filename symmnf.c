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


double **read_from_file(char *, int *);
void free_linked_mem(VECTOR *);
double **mat_from_linked(VECTOR *, int, int);
void error_alloc_link(VECTOR *);
void print_matrix(double**, int, int);
double similarity_calc(double*, double*, int);
double **sym(double **, int, int);
void free_mat_alloc(double **);
double sum_of_vector(double*, int);
double **ddg(double **, int);
double **norm(int, double **, double **);
double **pow_diag_matrix(double**, int);
void free_all_mat(double **, double **, double **, double **, int);
double **matrix_multi(double **, double **, int);

int main(int argc, char *argv[])
{
    int ND[2];
    int n,d;
    char *goal;
    double **data;
    double **sym_mat, **ddg_mat, **norm_mat;
    if(argc != 3){
        printf("An Error Has Occurred\n");
        exit(1);
    }
    data = read_from_file(argv[2], ND);
    n = ND[0];
    d = ND[1];
    goal = argv[1];
    sym_mat = sym(data, n, d);
    if(sym_mat == NULL){
        free_all_mat(data, NULL, NULL, NULL, 1);
    }
    if(strcmp(goal, "sym") == 0){ /* prints sym matrix*/
        print_matrix(sym_mat, n, n);
    }
    else if (strcmp(goal, "ddg") == 0 || strcmp(goal, "norm") == 0)
    {
        ddg_mat = ddg(sym_mat, n);
        if(ddg_mat == NULL){
            free_all_mat(data, sym_mat, NULL, NULL, 2);
        }
        if(strcmp(goal, "ddg") == 0){
            print_matrix(ddg_mat, n, n);
        }
        else{
            norm_mat = norm(n, sym_mat, ddg_mat);
            if (norm_mat == NULL){
                free_all_mat(data, sym_mat, ddg_mat, NULL, 3);
            }
            print_matrix(norm_mat, n, n);
        }
    }
    return 0;
}


double **sym(double **data, int n, int d){
    double **sym_mat;
    double *rows;
    int i,j;
    /* allocate memory for matrix */
    rows = (double *)calloc(n * n, sizeof(double));
    if (rows == NULL)
    {
        return NULL;
    }
    sym_mat = (double **)calloc(n, sizeof(double *));
    if (sym_mat == NULL)
    {
        free(rows);
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        sym_mat[i] = rows + i * n;
    }
    
    for(i = 0; i < n; i++){
        for(j = 0; j < n; j++){
            if(i==j){
                sym_mat[i][j] = 0;
            }
            else{
            sym_mat[i][j] = similarity_calc(data[i], data[j], d);
            }
        }
    }
    return sym_mat;
}


double **ddg(double **sym_mat, int n){
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
                ddg_mat[i][j] = sum_of_vector(sym_mat[i], n);
            }
            else{
                ddg_mat[i][j] = 0;
            }
        }
    }
    return ddg_mat;

}


double **norm( int n, double **sym_mat, double **ddg_mat){
    double **norm_mat, **new_d, **final_norm_mat;
    /* calculate the D^-0.5 */
    new_d = pow_diag_matrix(ddg_mat, n);
    if(new_d == NULL){
        return NULL;
        }
    /* calculate the D^-0.5 * W * D^-0.5 */
    norm_mat = matrix_multi(new_d, sym_mat, n);
    if(norm_mat == NULL){
        free_mat_alloc(new_d);
        return NULL;
    }
    final_norm_mat = matrix_multi(norm_mat, new_d, n);
    if(final_norm_mat == NULL){
        free_mat_alloc(new_d);
        free_mat_alloc(norm_mat);
        return NULL;
    }
    return final_norm_mat;
}

double **read_from_file(char *file_name, int ND[2])
{
    VECTOR *head_vec, *curr_vec, *prev_vec;
    NUM *head_num, *curr_num;
    FILE *file;
    double **data;
    int rows = 0, elements = 0;
    double num;
    char c;
    file = fopen(file_name, "r");
    if (file == NULL)
    {
        printf("An Error Has Occurred\n");
        exit(1);
    }
    /* Memory allocation for the initial structures*/
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

    /* Initialize the linked lists*/
    curr_num->next = NULL;
    curr_vec->num_list = head_num;
    curr_vec->next = NULL;
    /* Read from the file*/
    while (fscanf(file, "%lf%c", &num, &c) == 2)
    {
        elements++;
        if ((int)c == 13 || (int)c == 10) /* check if its the end of the line*/
        {
            rows++;
            /* Create a new vector node*/
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

        /* Create a new number node*/
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
    /* Close the file*/
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
        for (j = 0; j < cols-1; j++){
            printf("%.4f,", mat[i][j]);
        }
        printf("%.4f", mat[i][j]);
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


double **pow_diag_matrix(double** diag, int n){
    int i,j;
    double **result;
    double *rows;
    result = (double **)calloc(n, sizeof(double *));
    if (result == NULL)
    {
        return NULL;
    }
    rows = (double *)calloc(n * n, sizeof(double));
    if (rows == NULL)
    {
        free(result);
        return NULL;
    }
    for (i = 0; i < n; i++){
        result[i] = rows + i * n;
    }
    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            if (i == j){
                result[i][j] = 1 / (sqrt(diag[i][j]));
            }
            else{
                result[i][j] = 0;
            }
        }
    }
    return result;
}

void free_all_mat(double ** mat_1, double ** mat_2, double ** mat_3, double ** mat_4, int n){
    if(n > 0){
        free_mat_alloc(mat_1);
    }  
    if(n > 1){
        free_mat_alloc(mat_2);
    }       
    if (n > 2){
        free_mat_alloc(mat_3);
    }
    if (n > 3){
        free_mat_alloc(mat_4);
    }
    printf("An Error Has Occurred\n");
    exit(1);
}

double **matrix_multi(double **mat_1, double **mat_2, int n){
    double **result;
    double *rows;
    int i,j,k;
    /* allocate memory for matrix */
    rows = (double *)calloc(n * n, sizeof(double));
    if (rows == NULL)
    {
        return NULL;
    }
    result = (double **)calloc(n, sizeof(double *));
    if (result == NULL)
    {
        free(rows);
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        result[i] = rows + i * n;
    }
    for(i = 0; i < n; i++){
        for(j = 0; j < n; j++){
            result[i][j] = 0;
            for(k = 0; k < n; k++){
                result[i][j] += mat_1[i][k] * mat_2[k][j];
            }
        }
    }
    return result;
}
