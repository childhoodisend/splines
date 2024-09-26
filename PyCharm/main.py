import matplotlib.pyplot as plt
import time
import numpy as np

import PyCharm.error
from PyCharm.show import show, show_one
from clocktime import clocktime

from PyCharm.quadratic_processing import process_quadratic
from PyCharm.quadratic_spline import quadratic_spline

from PyCharm.cubic_spline import cubic_spline
from PyCharm.cubic_processing import process_cubic

if __name__ == '__main__':
    # # Quadratic spline
    # quadratic_spline()

    # #Cubic spline
    # cubic_spline()

    # #-----Multiprocessing-----
    # # Quadratic spline + processing
    # process_quadratic(num_process=2)
    #
    # # # Cubic spline + threading
    # process_cubic(num_process=2)
    # #~~~~~Multiprocessing~~~~~

    # time all functions
    clocktime(proc=20)
