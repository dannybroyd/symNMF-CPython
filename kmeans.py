import math
import sys

EPSILON = 0.001


def kmeans(k, file_name, max_iter=200):
    """
    Implement the kmeans algorithm as specified

    :param k: how many clusters
    :param file_name: name of file
    :param max_iter: how many iterations until we stop
    :return: list of most compatible clusters for each vector
    """
    file = open(file_name)
    vector_list = read_from_input(file)
    file.close()
    cluster_list = init_k_clusters(k, vector_list)
    new_cluster_id = -1
    old_cluster_id = 0
    old_mean = []
    new_mean = []
    labels = []  # for analysis
    max_mean_delta = 0
    for i in range(max_iter):
        for vector in vector_list:
            # calculate means and update centroids accordingly
            old_cluster_id = vector[1]
            new_cluster_id = find_min_distance_cluster(vector[0], cluster_list)
            assign_vector_to_cluster(vector, new_cluster_id, cluster_list)
            old_mean = cluster_list[new_cluster_id][2]
            new_mean = get_new_centroid(cluster_list[new_cluster_id])
            max_mean_delta = max(vector_dist(old_mean, new_mean), max_mean_delta)
        for cluster in cluster_list:
            cluster[2] = get_new_centroid(cluster)

        if max_mean_delta < EPSILON:
            break
        max_mean_delta = 0
    for vector in vector_list:
        labels.append(vector[1])
    return labels


def main():
    """
    Get system arguments and run kmeans() with them
    """
    if len(sys.argv) != 6:
        if len(sys.argv) == 5:
            max_iter = '200'
        else:
            print("An Error Has Occurred")
            quit()
    else:
        max_iter = sys.argv[4]
        file_path = sys.argv[5]
    k = sys.argv[1]
    n = sys.argv[2]
    d = sys.argv[3]
    file_path = sys.argv[len(sys.argv) - 1]
    check_input(k, n, d, max_iter)
    kmeans(int(k), file_path, int(max_iter))  # changed for analysis


def read_from_input(input_data):
    """
    Read vectors from input data

    :param input_data: data containing all vectors
    :return: list of all vectors
    """
    # each vector is represented by a list with 2 values: [vector, cluster]
    input_lines = input_data.readlines()
    vector_list = []
    for line in input_lines:
        if line != "":
            vector_list.append([[float(x) for x in line.split(',')], -1])
    return vector_list


def init_k_clusters(k, vector_list):
    """
    Initialize k clusters with vectors

    :param k: the given k
    :param vector_list: list of the vectors
    :return: list of initialized clusters
    """
    # each cluster is represented by a list with 3 values: [cluster_size, cluster_sum, mean_vector]
    cluster_list = []
    for i in range(k):
        cluster_list.append([1, vector_list[i][0], vector_list[i][0]])
        vector_list[i][1] = i
    return cluster_list


def find_min_distance_cluster(x_vector, cluster_list):
    """
    Find the cluster with the minimum distance to a given vector

    :param x_vector: the given vector
    :param cluster_list: list of clusters
    :return: index of the cluster with the minimum distance
    """
    min_distance = vector_dist(x_vector, cluster_list[0][2])
    min_cluster_id = 0
    curr_distance = min_distance
    for i in range(len(cluster_list)):
        curr_distance = vector_dist(x_vector, cluster_list[i][2])
        if curr_distance < min_distance:
            min_distance = curr_distance
            min_cluster_id = i
    return min_cluster_id


def assign_vector_to_cluster(vector, new_cluster_id, cluster_list):
    """
    Assign a vector to a new cluster

    :param vector: the vector to be assigned
    :param new_cluster_id: the index of the new cluster
    :param cluster_list: list of clusters
    """
    old_cluster_id = vector[1]
    if old_cluster_id == new_cluster_id:
        return
    vector_value = vector[0]
    if old_cluster_id != -1:
        # updates old cluster's size, sum and mean, don't need to if vector wasn't in cluster
        cluster_list[old_cluster_id][0] -= 1
        cluster_list[old_cluster_id][1] = vector_calculation(cluster_list[old_cluster_id][1], vector_value, False)
    # updates new cluster
    cluster_list[new_cluster_id][0] += 1
    cluster_list[new_cluster_id][1] = vector_calculation(cluster_list[new_cluster_id][1], vector_value, True)
    # update vector
    vector[1] = new_cluster_id


def vector_calculation(vector1, vector2, add):
    """
    Perform vector calculation (addition or subtraction)

    :param vector1: the first vector
    :param vector2: the second vector
    :param add: boolean value indicating whether to add or subtract
    :return: the resulting vector
    """
    product_vector = []
    if add:
        for i in range(len(vector1)):
            product_vector.append(vector1[i] + vector2[i])
    else:
        for i in range(len(vector1)):
            product_vector.append(vector1[i] - vector2[i])
    return product_vector


def get_new_centroid(cluster):
    """
    Calculate the new centroid of a cluster

    :param cluster: the cluster
    :return: the new centroid
    """
    new_centroid = []
    size = cluster[0]
    for curr_sum in cluster[1]:
        new_centroid.append(curr_sum / size)
    return new_centroid


def check_input(k, n, d, max_iter):
    """
    Check the validity of input parameters

    :param k: number of clusters
    :param n: number of points
    :param d: dimension of point
    :param max_iter: maximum iteration
    """
    if int(k) <= 1 or int(k) >= int(n) or not k.isdigit():
        print("Invalid number of clusters!")
        quit()
    if int(n) <= 1 or not n.isdigit():
        print("Invalid number of points!")
        quit()
    if not d.isdigit():
        print("Invalid dimension of point!")
        quit()
    if int(max_iter) <= 1 or int(max_iter) >= 1000 or not max_iter.isdigit():
        print("Invalid maximum iteration!")
        quit()


def vector_dist(vector1, vector2):
    """
    Calculate the Euclidean distance between two vectors

    :param vector1: the first vector
    :param vector2: the second vector
    :return: the Euclidean distance
    """
    curr_sum = 0
    for number1, number2 in zip(vector1, vector2):
        curr_sum += math.pow((number1 - number2), 2)
    return math.sqrt(curr_sum)


if __name__ == '__main__':
    main()
