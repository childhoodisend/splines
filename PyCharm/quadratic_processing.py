import matplotlib.pyplot as plt
import multiprocessing as mp
import numpy as np

import show
import time

img = plt.imread("../img/mandrill.jpg")

# Ширина и длина img
n = img.shape[0]
m = img.shape[1]


def cut2x(data, x: int, y: int, n: int):
    width = x // 2
    height = y // 2
    cutdata = np.zeros(width * height * n, dtype=np.uint8)
    i = 0
    j = 0
    while i < x * y * n:
        k = 0
        s = 0
        while k < x * n:
            cutdata[j + s] = data[i + k]
            cutdata[j + s + 1] = data[i + k + 1]
            cutdata[j + s + 2] = data[i + k + 2]
            k += 6
            s += 3

        i += 2 * x * n
        j += width * n

    return cutdata


# Функция, с заранее известными коэффициентами
def u(u_j: np.array, u_j1: np.array):
    return abs(0.5 * u_j + 0.5 * u_j1)


def linear_parallel_enlarge_threads(cutdata: mp.Array, width: int, height: int, n: int, part: int, th: int,
                                    data: mp.Array):
    num = height // th
    if num % 2 == 1:
        num -= 1
    i = width * n * part * num
    end_i = i + width * n * num

    if part == th - 1:
        end_i = width * height * n
    j = i * 4

    while i < end_i:
        k = 0
        s = 0
        while k < width * n:
            data[j + s] = cutdata[i + k]
            data[j + s + 1] = cutdata[i + k + 1]
            data[j + s + 2] = cutdata[i + k + 2]
            k += 3
            s += 6

        i += width * n
        j += 4 * width * n

    for channel in range(n):
        i = width * n * 4 * part * num
        end_i = width * n * 4 * (part + 1) * num
        if part == th - 1:
            end_i = width * height * n * 4

        while i < end_i:
            j = n
            while j < 2 * width * n - 3:
                res = u(data[i + j - n + channel], data[i + j + n + channel])
                if res > 255:
                    res = 255
                data[i + j + channel] = np.uint8(res)  # int8_t
                j += n * 2

            if j == 2 * width * n - 3:
                res = u(data[i + j - n + channel], data[i + j - 2 * n + channel])
                if res > 255:
                    res = 255
                data[i + j + channel] = np.uint8(res)  # int8_t

            i += width * 4 * n

    for channel in range(n):
        actual_width = 2 * width * n
        i = 0
        while i <= actual_width - 1:
            j = num * part * 2 + 1
            end_j = num * 2 * (part + 1) - 1
            if part == th - 1:
                end_j = height * 2 - 1

            while j < end_j:
                res = u(data[i + actual_width * (j - 1) + channel], data[i + actual_width * (j + 1) + channel])
                if res > 255:
                    res = 255
                data[i + actual_width * j + channel] = np.uint8(res)
                j += 2

            if j == end_j:
                res = u(data[i + actual_width * (j - 1) + channel], data[i + actual_width * (j - 2) + channel])
                if res > 255:
                    res = 255
                data[i + actual_width * j + channel] = np.uint8(res)

            i += n


def process_quadratic(num_process: int):
    shared_img_cutted = mp.Array('i', (n // 2) * (m // 2) * 3)
    img_ar = np.array(img, dtype=np.uint8).reshape(-1)
    shared_img_cutted[:] = cut2x(img_ar, n, m, 3)

    shared_img = mp.Array('i', n * m * 3)

    # Замеряем только время выполнения процессов!
    # start_time = time.time()

    proc = []  # list of processes
    for i in range(num_process):
        p = mp.Process(target=linear_parallel_enlarge_threads,
                       args=(shared_img_cutted, n // 2, m // 2, 3, i, num_process, shared_img,))

        proc.append(p)
        p.start()

    for p in proc:
        p.join()

    # print("--- %s seconds ---" % (time.time() - start_time))

    # show.show_one(np.array(shared_img).reshape((n, m, 3)), 'quadratic_processing')
