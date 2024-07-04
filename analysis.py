import symnmf
import kmeans
import sys
from sklearn.metrics import silhouette_score


def main():
    """
    Calculate list of most compatible clusters for each vector using both kmeans and symnmf
    :return: Prints both silhouette scores
    """
    file_name, k = get_args()
    kmeans_labels = kmeans.kmeans(k, file_name)
    symnmf_labels, data_points = symnmf.analysis(k, file_name)
    print("nmf: %.4f" %silhouette_score(data_points, symnmf_labels))
    print("kmeans: %.4f" %silhouette_score(data_points, kmeans_labels))
    

def get_args():
    """
    get args (k, filename) from system
    :return: filename, k
    """
    if len(sys.argv) != 3:
        print("An Error Has Occurred")
        quit()
    file_name = sys.argv[2]
    k = int(sys.argv[1])
    return file_name, k


if __name__ == "__main__":
    main()