import matplotlib.pyplot as plt
import threading as th
import numpy as np

import show

img = plt.imread("../img/mandrill.jpg")

# Ширина и длина
n = img.shape[0]
m = img.shape[1]


def cut(img: np.array):
    img1 = img.copy()
    for i in range(n):
        for j in range(m):
            if (i % 2 != 0) or (j % 2 != 0):
                img1[i][j] = 0
    return img1


img_cutted = cut(img)



# Функция, с заранее известными коэффициентами
def u(u_j: np.array, u_j1: np.array):
    array = np.abs(0.5 * u_j + 0.5 * u_j1)
    return np.where(array > 255, 255, array)



#   восстанавливаем строки
def quadratic_spline_row():
        for i in range(0, n):
            for j in range(1, m - 1, 2):
                img_cutted[i, j] = u(img_cutted[i][j - 1], img_cutted[i][j + 1])


#   восстанавливаем столбцы
def quadratic_spline_column():
        for j in range(0, m):
            for i in range(1, n - 1, 2):
                img_cutted[i][j] = u(img_cutted[i - 1][j], img_cutted[i + 1][j])


def threading_quadratic():
    p1 = th.Thread(target=quadratic_spline_row, args=[])
    p2 = th.Thread(target=quadratic_spline_column, args=[])

    p1.start()
    p1.join()

    p2.start()
    p2.join()

    show.show_one(img_cutted, 'quadratic_threading')