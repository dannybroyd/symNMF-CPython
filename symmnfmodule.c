#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <symmnf.h>
#include <Python.h>

static PyObject *sym_wrapper(PyObject *self, PyObject *args)
{
    PyObject *data_pyobj, *sym_mat_pyobj;
    int rows, cols, i, j;
    double **sym_mat, **data;
    char *goal;

    if (!PyArg_ParseTuple(args, "O", &data_pyobj))
    {
        printf("An Error Has Occurred\n");
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

    rows = PyObject_Length(data_pyobj);
    cols = PyOnject_length(PyList_GetItem(data_pyobj, 0));

    data = pyobj_to_mat(data_pyobj, rows, cols);
    if (data == NULL)
    {
        printf("An Error Has Occurred\n");
        return NULL;
    }

    goal = "sym";
    sym_mat = get_mat_by_goal(goal, data, rows, cols);
    if (sym_mat == NULL)
    {
        free_mat_alloc(data);
        printf("An Error Has Occurred\n");
        return NULL;
    }

    sym_mat_pyobj = mat_to_pyobj(sym_mat, rows, rows);
    free_all_mat(data, sym_mat, NULL, NULL, 2, 0);

    return sym_mat_pyobj;
}

static PyObject *ddg_wrapper(PyObject *self, PyObject *args)
{
    PyObject *data_pyobj, *ddg_mat_pyobj;
    int rows, cols, i, j;
    double **data, **ddg_mat;
    char *goal;

    if (!PyArg_ParseTuple(args, "O", &data_pyobj))
    {
        printf("An Error Has Occurred\n");
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

    rows = PyObject_Length(data_pyobj);
    cols = PyOnject_length(PyList_GetItem(data_pyobj, 0));

    data = pyobj_to_mat(data_pyobj, rows, cols);
    if (data == NULL)
    {
        printf("An Error Has Occurred\n");
        return NULL;
    }

    goal = "ddg";
    ddg_mat = get_mat_by_goal(goal, data, rows, cols);
    if (ddg_mat == NULL)
    {
        free_mat_alloc(data);
        printf("An Error Has Occurred\n");
        return NULL;
    }

    ddg_mat_pyobj = mat_to_pyobj(ddg_mat, rows, rows);
    free_all_mat(data, ddg_mat, NULL, NULL, 2, 0);
    return ddg_mat_pyobj;
}

static PyObject *norm_wrapper(PyObject *self, PyObject *args)
{
    PyObject *data_pyobj, *norm_mat_pyobj;
    int rows, cols, i, j;
    double **data, **norm_mat;
    char *goal;

    if (!PyArg_ParseTuple(args, "O", &data_pyobj))
    {
        printf("An Error Has Occurred\n");
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

    rows = PyObject_Length(data_pyobj);
    cols = PyOnject_length(PyList_GetItem(data_pyobj, 0));

    data = pyobj_to_mat(data_pyobj, rows, cols);
    if (data == NULL)
    {
        printf("An Error Has Occurred\n");
        return NULL;
    }

    goal = "norm";
    norm_mat = get_mat_by_goal(goal, data, rows, cols);
    if (norm_mat == NULL)
    {
        free_mat_alloc(data);
        printf("An Error Has Occurred\n");
        return NULL;
    }

    norm_mat_pyobj = mat_to_pyobj(norm_mat, rows, rows);
    free_all_mat(data, norm_mat, NULL, NULL, 2, 0);
    return norm_mat_pyobj;
}

static PyObject *symmnf_wrapper(PyObject *self, PyObject *args)
{
    PyObject *initial_H_pyobj, *final_H_pyobj, *norm_mat_pyobj;
    double **initial_H, **norm_mat, **final_H;
    int rows, k, i, j;

    if (!PyArg_ParseTuple(args, "OO", &initial_H_pyobj, &norm_mat_pyobj))
    {
        printf("An Error Has Occurred\n");
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

    rows = PyObject_Length(initial_H_pyobj);
    k = PyOnject_length(PyList_GetItem(initial_H_pyobj, 0));

    initial_H = pyobj_to_mat(initial_H_pyobj, rows, k);
    if (initial_H == NULL)
    {
        printf("An Error Has Occurred\n");
        return NULL;
    }

    norm_mat = pyobj_to_mat(norm_mat_pyobj, rows, rows);
    if (norm_mat == NULL)
    {
        free_mat_alloc(initial_H);
        printf("An Error Has Occurred\n");
        return NULL;
    }

    final_H = symmnf(initial_H, norm_mat, rows, k);
    if (final_H == NULL)
    {
        free_all_mat(initial_H, norm_mat, NULL, NULL, 2, 0);
        printf("An Error Has Occurred\n");
        return NULL;
    }

    final_H_pyobj = mat_to_pyobj(final_H, rows, k);
    free_all_mat(initial_H, norm_mat, final_H, NULL, 3, 0);

    return final_H_pyobj;
}
static PyMethodDef symmnf_FunctionsTable[] = {
    {
        "sym",                  // name exposed to Python
        sym_wrapper,            // C wrapper function
        METH_VARARGS,           // received variable args (but really just 1)
        "Excecute sym function" // documentation
    },
    {
        "ddg",                  // name exposed to Python
        ddg_wrapper,            // C wrapper function
        METH_VARARGS,           // received variable args (but really just 1)
        "Excecute ddg function" // documentation
    },
    {
        "norm",                  // name exposed to Python
        norm_wrapper,            // C wrapper function
        METH_VARARGS,            // received variable args (but really just 1)
        "Excecute norm function" // documentation
    },
    {
        "symmnf",               // name exposed to Python
        symmnf_wrapper,         // C wrapper function
        METH_VARARGS,           // received variable args (but really just 1)
        "Excecute ddg function" // documentation
    },

    {NULL, NULL, 0, NULL}};

static struct PyModuleDef symmnf_Module = {
    PyModuleDef_HEAD_INIT,
    "symmnf",                                    // name of module exposed to Python
    "C implementation of the symmnf algorithm.", // module documentation
    -1,
    symmnf_FunctionsTable};

PyMODINIT_FUNC PyInit_symmnf(void)
{
    return PyModule_Create(&symmnf_Module);
}

static double **pyobj_to_mat(PyObject *data_pyobj, int rows, int cols)
{
    double **mat;
    int i, j;
    PyObject *data_pyobj_row, *item_in_data_pyobj;

    mat = alloc_mat(rows, cols);
    if (mat == NULL)
    {
        return NULL;
    }

    /* this parses pyobject to 2d array(matrix) */
    for (i = 0; i < rows; i++)
    {
        data_pyobj_row = PyList_GetItem(data_pyobj, i);
        for (j = 0; j < cols; j++)
        {
            item_in_data_pyobj = PyList_GetItem(data_pyobj_row, j);
            mat[i][j] = PyFloat_AsDouble(item_in_data_pyobj);
        }
    }

    return mat;
}

static PyObject *mat_to_pyobj(double **mat, int rows, int cols)
{
    PyObject *outer_list, *inner_list, list_value;
    int i, j;

    /* parse a 2d array to 2d list pyobject */
    outer_list = PyList_New(rows);
    for (i = 0; i < rows; i++)
    {
        inner_list = PyList_New(cols);
        for (j = 0; j < cols; j++)
        {
            list_value = PyFloat_FromDouble(mat[i][j]);
            PyList_Set_Item(inner_list, j, list_value);
        }
        PyList_Set_Item(outer_list, i, inner_list);
    }
}

static double **get_mat_by_goal(char *goal, double **data, int n, int d)
{
    double **sym_mat, **ddg_mat, **norm_mat;

    sym_mat = sym(data, n, d);
    if (sym_mat == NULL)
    {
        return NULL;
    }

    if (strcmp(goal, "sym") == 0)
    {
        return sym_mat;
    }
    else if (strcmp(goal, "ddg") == 0 || strcmp(goal, "norm") == 0)
    {
        ddg_mat = ddg(sym_mat, n);
        if (ddg_mat == NULL)
        {
            free_all_mat(sym_mat, NULL, NULL, NULL, 1, 0);
            return NULL;
        }
        if (strcmp(goal, "ddg") == 0)
        {
            free_all_mat(sym_mat, NULL, NULL, NULL, 1, 0);
            return ddg_mat;
        }
        else
        {
            norm_mat = norm(n, sym_mat, ddg_mat);
            free_all_mat(sym_mat, ddg_mat, NULL, NULL, 2, 0);
            return norm_mat;
        }
    }
}
