
import numpy as np
from PIL import Image
import time

img = Image.open('C:/Users/belyn/Downloads/vincent-van-gogh_road-with-cypresses-1890.jpg')
width, height = img.size
print(width, height)

print(img.getbands())

img_arr = np.asarray(img)

# вырезаем нечетные строки и столбцы
def cut2x(img_arr):
  height, width, channels = img_arr.shape
  mini_arr = np.zeros((height // 2, width // 2, channels), dtype=np.uint8)
  i = 0
  j = 0
  while i < height:
    k = 0
    s = 0
    while k < width:
      mini_arr[j][s] = img_arr[i][k]
      k += 2
      s += 1
    i += 2
    j += 1
  return mini_arr

mini_arr = cut2x(img_arr)
# Image.fromarray(mini_arr).save('C:/Users/belyn/Downloads/road_cut.jpg')


def u(x, xj, xj1, uj, uj1):
  return (uj1 - uj) * (x - xj) / (xj1 - xj) + uj

# увеличиваем в исходный размер

def enlarge2x(img):
  height, width, channels = img.shape
  new_arr = np.zeros((height * 2, width * 2, channels), dtype=np.uint8)
  i = 0   # восстанавливаем пиксели с четными номерами строк и столбцов(копируем из маленького изображения)
  j = 0
  while(i < height):
    k = 0
    s = 0
    while(k < width):
      new_arr[j][s] = img[i][k]
      k += 1
      s += 2
    i += 1
    j += 2
  for channel in range(channels):  # восстанавливаем нечетные строки
    j = 0
    while j < height * 2:
      s = 1
      while s < width * 2 - 1:
        res = abs(u(s, s-1, s+1, int(new_arr[j][s-1][channel]), int(new_arr[j][s+1][channel])))
        if res > 255:
          res = 255
        new_arr[j][s][channel] = np.uint8(res)
        s += 2
      j += 1  
  for channel in range(channels):  # восстанавливаем нечетные столбцы
    j = 0
    while j < width * 2:
      s = 1
      while s < height * 2 - 1:
        res = abs(u(s, s-1, s+1, int(new_arr[s-1][j][channel]), int(new_arr[s+1][j][channel])))
        if res > 255:
          res = 255
        new_arr[s][j][channel] = np.uint8(res)
        s += 2
      j += 1
  return new_arr

start = time.time()
new_arr = enlarge2x(mini_arr)
end = time.time()
# Image.fromarray(new_arr).save('C:/Users/belyn/Downloads/road_enl.jpg')
print(end - start) # 49.7, 50.3, 47.9, 47.5, 46.4 ~ 48.4
