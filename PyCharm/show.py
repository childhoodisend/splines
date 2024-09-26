import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns

def show_one(img: np.array, s: str):
    figsize = 10, 10
    f = plt.figure(figsize=figsize)
    _ = plt.imshow(img)
    plt.savefig('../img/' + s + '.jpg')


def show(new_img: np.array, img: np.array, name1: str, name2='original image'):
    f, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 15))
    ax1.set_title(name1)
    _ = ax1.imshow(new_img)
    ax2.set_title(name2)
    _ = ax2.imshow(img)
    plt.savefig('../img/' + name1 + '_original' + '.jpg')


# Amdal's law
def amdal(ar: np.array, name: str):
    t1 = ar[0]
    t2 = np.zeros(ar.size)
    k = np.zeros(ar.size)

    for i in range(1, t2.size):
        t2[i] = ar[i] / (i+1)
        k[i] = t1/t2[i]

    # show Amdal
    # data = [[k[i], i] for i in range(1, k.size)]
    # print(data)

    plt.plot(k, 'o', color='black')
    plt.xlabel('N')
    plt.ylabel('K')

    plt.savefig('../img/' + name + '.jpg')
    # plt.show()

    return 0
