from PyCharm.quadratic_processing import process_quadratic
from PyCharm.quadratic_spline import quadratic_spline

from PyCharm.cubic_spline import cubic_spline
from PyCharm.cubic_processing import process_cubic

import timeit
import numpy as np
from show import amdal


def clocktime(num=1, proc=2):
    time_q = np.zeros(proc + 1)
    time_quadratic = timeit.timeit(quadratic_spline, number=num)/num
    time_quadratic_proc = np.zeros(proc + 1)
    for i in range(1, proc + 1):
        time_quadratic_proc[i] = timeit.Timer(lambda: process_quadratic(i + 1)).timeit(number=num) / num
    time_q[0] = time_quadratic  # 1 thread script
    time_q += time_quadratic_proc  # 2, 3, 4 ... process script
    print(time_q)
    amdal(time_q, 'amdal_qua')

    time_c = np.zeros(proc + 1)
    time_cubic = timeit.timeit(cubic_spline, number=num) / num
    time_cubic_proc = np.zeros(proc + 1)
    for i in range(1, proc + 1):
        time_cubic_proc[i] = timeit.Timer(lambda: process_cubic(i + 1)).timeit(number=num) / num
    time_c[0] = time_cubic
    time_c += time_cubic_proc
    print(time_c)
    amdal(time_c, 'amdal_cub')
