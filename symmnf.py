import sys
import numpy as np
import pandas as pd
import math
import symmnf


np.random.seed(0)


def main():
    k, goal, filename = get_sys_args()
    action_by_goal(k, goal, filename)


def init_H(norm_mat, k):
    average = np.mean(np.array(norm_mat))
    top_interval = 2 * math.sqrt(average/k)
    h_mat = np.random.uniform(0, top_interval, (len(norm_mat), k))
    return h_mat.tolist()


def get_sys_args():
    if len(sys.argv) != 4:
        print("An Error Has Occurred")
        quit()
    k = int(sys.argv[1])
    goal = sys.argv[2]
    file_name = sys.argv[3]
    return k, goal, file_name


def get_datapoints(filename):
    df = pd.read_csv(filename, header=None)
    data = df.values
    return data.tolist()


def action_by_goal(k, goal, filename):
    datapoints = get_datapoints(filename)
    if goal == "sym":
        sym_mat = symmnf.sym(datapoints)
        print_mat(sym_mat)
    elif goal == "ddg":
        ddg_mat = symmnf.ddg(datapoints)
        print_mat(ddg_mat)
    elif goal == "norm":
        norm_mat = symmnf.norm(datapoints)
        print_mat(norm_mat)
    elif goal == "symmnf":
        norm_mat = symmnf.norm(datapoints)
        h_mat = init_H(norm_mat, k)
        final_h_mat = symmnf.symmnf(h_mat, norm_mat)
        print_mat(final_h_mat)


def print_mat(mat):
    for vector in mat:
        for i in range(len(vector)-1):
            print('%.4f' % vector[i], end=",")
        print('%.4f' % vector[len(vector) - 1])


if __name__ == "__main__":
    main()

