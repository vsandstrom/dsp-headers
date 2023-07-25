import enum
import math
import pandas as pd
from tabulate import tabulate
import matplotlib.pyplot as plt
import sys

shapes: set[str] = set()
shapes.update(
    'sine',
    'triangle',
    'square,'
    'saw'
    'hanning'
    )

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

len = 16
ws: dict[str, list[float]] = dict()

t1 = list()
ws['sine'] = sine(t1, len)
t2 = list()
ws['square'] = square(t2, len)
t3 = list()
ws['triangle'] = triangle(t3, len)
t4 = list()
ws['saw'] = saw(t4, len)
t5 = list()
ws['hanning'] = hanning(t5, len)

df = pd.DataFrame(ws)
print(sys.argv[1])
print(ws[sys.argv[1]])

if sys.argv[1] in ws:
# print(tabulate(df, headers = 'keys', tablefmt= 'github'))
    print(tabulate(df, headers = 'keys', tablefmt= 'fancy_grid'))

    plt.rcParams["figure.figsize"]=[7.50, 3.50]
    plt.rcParams["figure.autolayout"] = True

    plt.scatter(range(len), ws[sys.argv[1]])
    plt.plot(range(len), ws[sys.argv[1]])
    plt.title(sys.argv[1])
    plt.show()
else:
    print("failed")
