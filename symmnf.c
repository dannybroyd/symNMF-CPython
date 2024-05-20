#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symmnf.h" /*TODO: check with nadav*/
#define EPSILON 0.0001
#define MAX_ITER 300

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

double **read_from_file(char *, int[2]);
void free_linked_mem(VECTOR *);
double **mat_from_linked(VECTOR *, int, int);
void error_alloc_link(VECTOR *);
void print_matrix(double **, int, int);
double similarity_calc(double *, double *, int);
double **sym(double **, int, int);
void free_mat_alloc(double **);
double sum_of_vector(double *, int);
double **ddg(double **, int);
double **norm(int, double **, double **);
double **pow_diag_matrix(double **, int);
void free_all_mat(double **, double **, double **, double **, int, int);
double **matrix_multi(double **, double **, int, int, int);
double **final_H(double **, double **, int, int);
double **update_H(double **, double **, int, int);
double **transpose_mat(double **, int, int);
double forb_norm(double **, int, int);
double **alloc_mat(int, int);
double **subtract_mat(double **, double **, int, int);
double **symmnf(double **, double **, int, int);
/* TODO: DELETE TESTER!!!!!*/
void tester(double **, int, int);

int main(int argc, char *argv[])
{
    int ND[2];
    int n, d;
    char *goal;
    double **data, **sym_mat, **ddg_mat, **norm_mat;;
    if (argc != 3)
    {
        printf("An Error Has Occurred\n");
        exit(1);
    }
    data = read_from_file(argv[2], ND);
    n = ND[0];
    d = ND[1];
    goal = argv[1];
    sym_mat = sym(data, n, d);
    if (sym_mat == NULL)
    {
        free_all_mat(data, NULL, NULL, NULL, 1, 1);
    }

    /* TODO: THIS IS A TEST, DELETE LATER */
    if (strcmp(goal, "test") == 0)
    {
        tester(data, n, d);
    }

    if (strcmp(goal, "sym") == 0)
    { /* prints sym matrix*/
        print_matrix(sym_mat, n, n);
        free_all_mat(data, sym_mat, NULL, NULL, 2, 0);
    }
    else if (strcmp(goal, "ddg") == 0 || strcmp(goal, "norm") == 0)
    {
        ddg_mat = ddg(sym_mat, n);
        if (ddg_mat == NULL)
        {
            free_all_mat(data, sym_mat, NULL, NULL, 2, 1);
        }
        if (strcmp(goal, "ddg") == 0)
        {
            print_matrix(ddg_mat, n, n);
            free_all_mat(data, sym_mat, ddg_mat, NULL, 3, 0);
        }
        else
        {
            norm_mat = norm(n, sym_mat, ddg_mat);
            if (norm_mat == NULL)
            {
                free_all_mat(data, sym_mat, ddg_mat, NULL, 3, 1);
            }
            print_matrix(norm_mat, n, n);
            free_all_mat(data, sym_mat, ddg_mat, norm_mat, 4, 0);
        }
    }
    /* free everything */
    return 0;
}

double **symmnf(double **init_H, double **norm_mat, int n, int k)
{
    double **result;

    result = final_H(init_H, norm_mat, n, k);

    return result;
}

double **final_H(double **init_H, double **norm_mat, int n, int k)
{
    int curr_iter;
    double **new_H, **curr_H, forb_dist, **sub_H;
    curr_H = init_H;
    for (curr_iter = 0; curr_iter < MAX_ITER; curr_iter++)
    {
        /* update H and free memeory, break if convergence */
        new_H = update_H(curr_H, norm_mat, n, k);
        if (new_H == NULL)
        {
            if (curr_iter != 0){
                /* edge case, we dont want to free init_H */
                free_mat_alloc(curr_H);
            }
            return NULL;
        }

        sub_H = subtract_mat(new_H, curr_H, n, k);
        if (sub_H == NULL)
        {
            if (curr_iter != 0){
                /* edge case, we dont want to free init_H */
                free_mat_alloc(curr_H);
            }
            free_mat_alloc(new_H);
            return NULL;
        }

        forb_dist = forb_norm(sub_H, n, k);

        /* every iteration, free old H, and free subtraction matrix */
        if (curr_iter == 0){
            /* edge case, we dont want to free init_H */
            free_mat_alloc(sub_H);
        }
        else{
            free_all_mat(curr_H, sub_H, NULL, NULL, 2, 0);
        }
        
        curr_H = new_H;

        if (forb_dist < EPSILON)
        {
            return curr_H;
        }
    }
    return curr_H;
}

double **update_H(double **curr_H, double **norm_mat, int n, int k)
{
    double **numerator;   /* this is W*H(t) */
    double **denominator; /* this is H*(H^T)*H */
    double **H_transpose, **temp_mat;
    int i, j;
    double **updated_H;

    /* memory allocation for updated H */
    updated_H = alloc_mat(n, k);
    if (updated_H == NULL)
    {
        return NULL;
    }

    numerator = matrix_multi(norm_mat, curr_H, n, n, k);
    if (numerator == NULL)
    {
        free_all_mat(updated_H, NULL, NULL, NULL, 1, 0);
        return NULL;
    }

    H_transpose = transpose_mat(curr_H, n, k);
    if (H_transpose == NULL)
    {
        free_all_mat(updated_H, numerator, NULL, NULL, 2, 0);
        return NULL;
    }

    temp_mat = matrix_multi(curr_H, H_transpose, n, k, n);
    if (temp_mat == NULL)
    {
        free_all_mat(updated_H, numerator, H_transpose, NULL, 3, 0);
        return NULL;
    }

    denominator = matrix_multi(temp_mat, curr_H, n, n, k);
    if (denominator == NULL)
    {
        free_all_mat(updated_H, numerator, H_transpose, temp_mat, 4, 0);
        return NULL;
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < k; j++)
        {
            updated_H[i][j] = curr_H[i][j] * (0.5 + (0.5 * (numerator[i][j] / denominator[i][j])));
        }
    }

    /* free all extra memory*/
    free_all_mat(denominator, numerator, H_transpose, temp_mat, 4, 0);

    return updated_H;
}

double **transpose_mat(double **mat, int m, int n)
{
    double **transposed_mat;
    int i, j;

    transposed_mat = alloc_mat(n, m);
    if (transposed_mat == NULL)
    {
        return NULL;
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            transposed_mat[i][j] = mat[j][i];
        }
    }
    return transposed_mat;
}

double forb_norm(double **mat, int m, int n)
{
    double result;
    int i, j;

    result = 0;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            result += (mat[i][j] * mat[i][j]);
        }
    }

    return result;
}

double **subtract_mat(double **mat_1, double **mat_2, int m, int n)
{
    double **result;
    int i, j;

    result = alloc_mat(m, n);
    if (result == NULL)
    {
        return NULL;
    }

    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            result[i][j] = mat_1[i][j] - mat_2[i][j];
        }
    }

    return result;
}

double **sym(double **data, int n, int d)
{
    double **sym_mat;
    int i, j;

    /* allocate memory for matrix */
    sym_mat = alloc_mat(n, n);
    if (sym_mat == NULL)
    {
        return NULL;
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (i == j)
            {
                sym_mat[i][j] = 0;
            }
            else
            {
                sym_mat[i][j] = similarity_calc(data[i], data[j], d);
            }
        }
    }
    return sym_mat;
}

double **ddg(double **sym_mat, int n)
{
    double **ddg_mat;
    int i, j;

    /* allocate memory for matrix */
    ddg_mat = alloc_mat(n, n);
    if (ddg_mat == NULL)
    {
        return NULL;
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (i == j)
            {
                ddg_mat[i][j] = sum_of_vector(sym_mat[i], n);
            }
            else
            {
                ddg_mat[i][j] = 0;
            }
        }
    }
    return ddg_mat;
}

double **norm(int n, double **sym_mat, double **ddg_mat)
{
    double **norm_mat, **new_d, **final_norm_mat;
    /* calculate the D^-0.5 */
    new_d = pow_diag_matrix(ddg_mat, n);
    if (new_d == NULL)
    {
        return NULL;
    }
    /* calculate the D^-0.5 * W * D^-0.5 */
    norm_mat = matrix_multi(new_d, sym_mat, n, n, n);
    if (norm_mat == NULL)
    {
        free_mat_alloc(new_d);
        return NULL;
    }
    final_norm_mat = matrix_multi(norm_mat, new_d, n, n, n);
    free_mat_alloc(new_d);
    free_mat_alloc(norm_mat);
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
    prev_vec = NULL;
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
        if (curr_vec != NULL)
        {
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
    return exp(-sum / 2);
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

void print_matrix(double **mat, int rows, int cols)
{
    int i, j;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols - 1; j++)
        {
            printf("%.4f,", mat[i][j]);
        }
        printf("%.4f", mat[i][j]);
        printf("\n");
    }
}

double sum_of_vector(double *vector, int d)
{
    int i;
    double sum = 0;
    for (i = 0; i < d; i++)
    {
        sum += vector[i];
    }
    return sum;
}

double **pow_diag_matrix(double **diag, int n)
{
    int i, j;
    double **result;

    /* allocate memory for matrix */
    result = alloc_mat(n, n);
    if (result == NULL)
    {
        return NULL;
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (i == j)
            {
                result[i][j] = 1 / (sqrt(diag[i][j]));
            }
            else
            {
                result[i][j] = 0;
            }
        }
    }
    return result;
}

void free_all_mat(double **mat_1, double **mat_2, double **mat_3, double **mat_4, int n, int error_flag)
{
    if (n > 0)
    {
        free_mat_alloc(mat_1);
    }
    if (n > 1)
    {
        free_mat_alloc(mat_2);
    }
    if (n > 2)
    {
        free_mat_alloc(mat_3);
    }
    if (n > 3)
    {
        free_mat_alloc(mat_4);
    }
    if (error_flag)
    {
        printf("An Error Has Occurred\n");
        exit(1);
    }
}

double **matrix_multi(double **mat_1, double **mat_2, int m, int n_1, int n)
{
    /* m is rows of mat_1, n_1 is cols of mat_1(and rows of mat_2), n is cols of mat_2*/
    double **result;
    int i, j, k;

    /* allocate memory for matrix */
    result = alloc_mat(m, n);
    if (result == NULL)
    {
        return NULL;
    }

    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            result[i][j] = 0;
            for (k = 0; k < n_1; k++)
            {
                result[i][j] += mat_1[i][k] * mat_2[k][j];
            }
        }
    }
    return result;
}

double **alloc_mat(int m, int n)
{
    double **mat, *rows;
    int i;

    /* allocate memory for matrix */
    rows = (double *)calloc(m * n, sizeof(double));
    if (rows == NULL)
    {
        return NULL;
    }
    mat = (double **)calloc(m, sizeof(double *));
    if (mat == NULL)
    {
        free(rows);
        return NULL;
    }
    for (i = 0; i < m; i++)
    {
        mat[i] = rows + i * n;
    }

    return mat;
}

void tester(double **data, int n, int d)
{
    double **transposed_mat, **sub_mat, **multi_mat, norm;
    printf("Original Matrix: \n");
    print_matrix(data, n, d);
    printf("\n");

    transposed_mat = transpose_mat(data, n, d);
    printf("Transpose Matrix: \n");
    print_matrix(transposed_mat, d, n);
    printf("\n");

    multi_mat = matrix_multi(data, transposed_mat, n, d, n);
    printf("Multi Matrix: \n");
    print_matrix(multi_mat, n, n);
    printf("\n");

    sub_mat = subtract_mat(data, data, n, d);
    printf("Sub Matrix: \n");
    print_matrix(sub_mat, n, n);
    printf("\n");

    norm = forb_norm(data, n, d);
    printf("norm: %f\n", norm);
}
