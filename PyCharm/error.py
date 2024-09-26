import numpy as np


# Метрика Евклида
def euclidean_dist(arr1: np.array, arr2: np.array):
    return np.linalg.norm(arr1 - arr2)


# Ошибка с метрикой Евклида
def error_euclidean(new_img: np.array, img: np.array):
    n = img.shape[0]
    m = img.shape[1]
    error = np.zeros((n, m, 3), dtype=np.uint8)

    for i in range(n):
        for j in range(m):
            error[i][j] = euclidean_dist(new_img[i][j], img[i][j])

    return error
