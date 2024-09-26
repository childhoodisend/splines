import numpy as np
import time
from PyCharm.show import show, show_one
import matplotlib.pyplot as plt

def u_known(u_j: np.array, u_j1: np.array):
    array = np.abs(0.5 * u_j + 0.5 * u_j1)
    return np.where(array > 255, 255, array)


def quadratic_spline():
    img = plt.imread("../img/mandrill.jpg")
    step = 2
    img_array_cutted = np.array(img[::step, ::step].copy(), dtype=np.uint8)

    # start_time = time.time()
    n = img_array_cutted.shape[0]
    m = img_array_cutted.shape[1]
    new_img = np.zeros(img.shape, dtype=np.uint8)

    #   копируем то что было
    for i in range(n):
        for j in range(m):
            new_img[2 * i][2 * j] = img_array_cutted[i][j]

    #   восстанавливаем строки
    for i in range(0, 2 * n):
        for j in range(1, 2 * m - 1, 2):
            new_img[i][j] = u_known(new_img[i][j - 1], new_img[i][j + 1])

    #   восстанавливаем столбцы
    for j in range(0, 2 * m):
        for i in range(1, 2 * n - 1, 2):
            new_img[i][j] = u_known(new_img[i - 1][j], new_img[i + 1][j])

    # print("--- %s seconds ---" % (time.time() - start_time))
    # show_one(new_img, 'quadratic')
    # show(new_img, img, 'quadratic spline')

