from . import symnmf
import kmeans
import sys
from sklearn.metrics import silhouette_score


def main():
    file_name, k = get_args()
    kmeans_labels = kmeans.kmeans(k, file_name)
    symnmf_labels, data_points = symnmf.analysis(k, file_name)
    print(f"nmf: {silhouette_score(data_points, symnmf_labels)}")
    print(f"kmeans: {silhouette_score(data_points, kmeans_labels)}")
    

def get_args():
    if len(sys.argv) != 3:
        print("An Error Has Occurred")
        quit()
    file_name = sys.argv[1]
    k = int(sys.argv[2])
    return file_name, k


if __name__ == "__main__":
    main()