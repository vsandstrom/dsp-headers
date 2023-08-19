import enum
import math
import pathlib
import pandas as pd
from tabulate import tabulate
import matplotlib.pyplot as plt
import sys
import random

shapes: set[str] = set()
shapes.update(
    'sine',
    'triangle',
    'square,'
    'saw'
    'hanning'
    )

def scale(table: list[float]):
    min = 0.0
    max = 0.0

    for i in range(len(table)):
        if table[i] < min:
            min = table[i] 
        if table[i] > max:
            max = table[i] 
    for i in range(len(table)):
        table[i] = (1.0 - 0.0) * (table[i]- min) / (max - min) + 1
    return table

####
def sine(table: list[float], length: int) -> list[float]:
    angle = 0.0
    inc = (math.pi*2) / length
    for _ in range(length):
        table.append(math.sin(angle))
        angle = angle + inc
    return table

####
def triangle(table: list[float], length: int) -> list[float]:
    angle = 0.0
    inc = 4.0 / length 
    for _ in range(length):
        if angle >= 1.0 or angle <= -1.0:
            inc = inc * -1.0
        table.append(angle)
        angle = angle + inc
    return table

####
def square(table: list[float], length: int) -> list[float]:
    val = 1.0
    for i in range(length):
        if i == length/2:
            val = -1.0
        table.append(val)
    return table

####
def saw(table: list[float], length: int) -> list[float]:
    angle = -1.0
    inc = 2.0 / length
    for _ in range(length):
        table.append(angle)
        angle = angle + inc
    return table

####
def hanning(table: list[float], length: int) -> list[float]:
    angle = 0.0
    inc = math.pi / length
    for _ in range(length):
        table.append(-1.0 - pow(math.cos(angle), 2))
        angle = angle + inc
    return table

def noise(table: list[float], length: int) -> list[float]:
    for _ in range(length):
        table.append(random.random())
    return table

def complex(table: list[float], length: int, ampl: list[float], phase: list[float], paramlength:
            int):
    for n in range(1, paramlength+1):
        inc = (math.pi * 2 * n) / float(length)
        angle = inc * float(length) * phase[n-1]
        for i in range(length):
            if len(table) == i:
                table.append(math.sin(angle)*ampl[n-1])
            else:
                table[i] = table[i] + (math.sin(angle)*ampl[n-1])
            angle = inc + angle

    return scale(table)

length = 512
if len(sys.argv) > 2:
    length = int(sys.argv[2])

ws: dict[str, list[float]] = dict()

t1 = list()
ws['sine'] = sine(t1, length)
t2 = list()
ws['square'] = square(t2, length)
t3 = list()
ws['triangle'] = triangle(t3, length)
t4 = list()
ws['saw'] = saw(t4, length)
t5 = list()
ws['hanning'] = hanning(t5, length)
t6 = list()
ws['noise'] = noise(t6, length)
t7 = list()
ws['complex'] = complex(t7, length, [1, 0.8, 0.6, 0.4, 0.2], [0, 0, 0, 0, 0], 5)

df = pd.DataFrame(ws)

if sys.argv[1] in ws:
# print(tabulate(df, headers = 'keys', tablefmt= 'github'))
    print(tabulate(df, headers = 'keys', tablefmt= 'fancy_grid'))

    plt.rcParams["figure.figsize"]=[7.50, 3.50]
    plt.rcParams["figure.autolayout"] = True

    plt.scatter(range(length), ws[sys.argv[1]])
    plt.plot(range(length), ws[sys.argv[1]])
    plt.title(sys.argv[1])
    plt.show()
else:
    print("waveshape unrecognized")
