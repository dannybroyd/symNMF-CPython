# ifndef LINKER_H_
# define LINKER_H_

double **symmnf(double **init_H, double **norm_mat, int n, int k);
double **sym(double **data, int n, int d);
double **ddg(double **sym_mat, int n);
double **norm(int n, double **sym_mat, double **ddg_mat);
void free_mat_alloc(double **data);
double **alloc_mat(int m, int n);
void free_all_mat(double **mat_1, double **mat_2, double **mat_3, double **mat_4, int n, int error_flag);

# endif