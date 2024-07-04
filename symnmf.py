import sys
import numpy as np
import pandas as pd
import math
import symnmfmodule


np.random.seed(0)


def main():
    k, goal, filename = get_sys_args()
    action_by_goal(k, goal, filename)


def init_H(norm_mat, k):
    """ Calculates initial H
    :return: initial H matrix
    """
    average = np.mean(np.array(norm_mat))
    top_interval = 2 * math.sqrt(average/k)
    h_mat = np.random.uniform(0, top_interval, (len(norm_mat), k))
    return h_mat.tolist()


def get_sys_args():
    """
    Gets sys arguments
    :return: k, goal, file_name
    """
    if len(sys.argv) != 4:
        print("An Error Has Occurred")
        quit()
    k = int(sys.argv[1])
    goal = sys.argv[2]
    file_name = sys.argv[3]
    return k, goal, file_name


def get_datapoints(filename):
    """
    Reads data from file
    :param filename: name of file
    :return: data from file as matrix
    """
    df = pd.read_csv(filename, header=None)
    data = df.values
    return data.tolist()


def action_by_goal(k, goal, filename):
    """
    Given k, goal, filename perform goal on data from filename with k
    :param k: given k
    :param goal: goal from {"sym", "ddg", "norm", "symnmf"}
    :param filename: name of file
    :return: Prints relevant matrix
    """
    datapoints = get_datapoints(filename)
    if goal == "sym":
        sym_mat = symnmfmodule.sym(datapoints)
        print_mat(sym_mat)
    elif goal == "ddg":
        ddg_mat = symnmfmodule.ddg(datapoints)
        print_mat(ddg_mat)
    elif goal == "norm":
        norm_mat = symnmfmodule.norm(datapoints)
        print_mat(norm_mat)
    elif goal == "symnmf":
        norm_mat = symnmfmodule.norm(datapoints)
        h_mat = init_H(norm_mat, k)
        final_h_mat = symnmfmodule.symnmf(h_mat, norm_mat)
        print_mat(final_h_mat)


def analysis(k, filename):
    """
    A helper function for analysis.py. Calculates the most compatible cluster from the final_h matrix (as instructed)
    :param k: given k
    :param filename: the name of the file
    :return: list of most compatible cluster for each vector in given data, and original matrix of all data
    (for silhouette_score we need original data points)
    """
    datapoints = get_datapoints(filename)
    norm_mat = symnmfmodule.norm(datapoints)
    h_mat = init_H(norm_mat, k)
    final_h_mat = symnmfmodule.symnmf(h_mat, norm_mat)
    matrix_array = np.array(final_h_mat)
    max_indices = np.argmax(matrix_array, axis=1)   # Use argmax along axis 1 to get the index of the maximum value in each row
    return max_indices.tolist(), datapoints


def print_mat(mat):
    """
    :param mat: a matrix
    :return: Prints the matrix with the valid formatting
    """
    for vector in mat:
        for i in range(len(vector)-1):
            print('%.4f' % vector[i], end=",")
        print('%.4f' % vector[len(vector) - 1])


if __name__ == "__main__":
    main()

