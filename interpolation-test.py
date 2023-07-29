import math
import pandas as pd
from tabulate import tabulate
import matplotlib.pyplot as plt
import sys

####
def sine(table: list[float], length: int):
    angle = 0.0
    inc = (math.pi*2) / length
    for i in range(length):
        table[i] = math.sin(angle)
        angle = angle + inc
    return table

####
def triangle(table: list[float], length: int):
    angle = 0.0
    inc = 4.0 / length 
    for i in range(length):
        if angle > 1.0 or angle < -1.0:
            inc = inc * -1.0
        table[i] = angle
        angle = angle + inc
    return table

####
def square(table: list[float], length: int):
    val = 1.0
    for i in range(length):
        if i == length/2:
            val = -1.0
        table[i] = val 
    return table

####
def saw(table: list[float], length: int):
    angle = -1.0
    inc = 2.0 / length
    for i in range(length):
        table[i] = angle
        angle = angle + inc
    return table

####
def hanning(table: list[float], length: int):
    angle = 0.0
    inc = math.pi / length
    for i in range(length):
        table[i] = -1.0 - pow(math.cos(angle), 2)
        angle = angle + inc
    return table

################################################################################
################################################################################

def cubic(pos: float, table: list[float]):
    x1: int = math.floor(pos)
    x0: int = wrap(x1-1, len(table))
    x2: int = wrap(x1+1, len(table))
    x3: int = wrap(x1+2, len(table)) 
    diff: float = pos- x1
    c0: float = table[x3] - table[x2] - table[x0] + table[x1]
    c1: float = table[x0] - table[x1] - c0
    c2: float = table[x2] - table[x0]
    return (c0 * pow(diff, 3)) + (c1 * pow(diff, 2)) + (c2 * diff) + table[x1]


def hermetic(pos, table):
    x1: int = math.floor(pos)
    x0: int = wrap(x1-1, len(table))
    x2: int = wrap(x1+1, len(table))
    x3: int = wrap(x1+2, len(table))
    diff: float = pos - x1

    sub: float = table[x1] - table[x2]
    c1: float = table[x2] - table[x0]
    c3: float = table[x3] - table[x1] + 3 * sub
    c2: float = -(2 * sub + c1 + c3)
    return 0.5 * ((c3 * diff + c2) * diff + c1) * diff + table[x1]

def linear(pos, table):
    x1 = math.floor(pos)
    x2 = wrap(x1+1, len(table))
    diff = pos - x1
    return table[x1] * (1-diff) + table[x2] * diff

def cosine(pos, table):
    x1 = math.floor(pos)
    x2 = wrap(x1+1, len(table))
    diff = pos - x1
    w2 = (1-math.cos(diff*math.pi)) / 2
    w1 = 1.0 - w2
    return table[x1] * w1 + table[x2] * w2 

def bilinear(pos, table):
    x1: int = math.floor(pos)
    x0: int = wrap(x1-1, len(table))
    x2: int = wrap(x1+1, len(table))
    x3: int = wrap(x1+2, len(table))
    diff: float = pos - x1

    w1 = 1-diff
    w2 = diff

    y1 = table[x0] * w1 + table[x1] * w2 
    y2 = table[x2] * w1 + table[x3] * w2
    return y1 * w1 + y2 * w2

def wrapf(pos: float, tableLen: int):
    if (pos < 0):
        return pos + float(tableLen)
    if pos >= tableLen:
        return pos - float(tableLen)
    return pos

def wrap(pos: int, tableLen: int):
    if (pos < 0):
        return pos + tableLen
    if pos >= tableLen:
        return pos - tableLen
    return pos

l: list[float] = [
        0.0, 0.08, 0.2, 0.45, 0.7, 0.92, 1.0, 0.92, 0.7, 0.45, 0.2, 0.08, 0.0, -0.08 -0.2, -0.45,
        -0.7, -0.92, -1.0, -0.92, -0.7, -0.45, -0.2, -0.08, 0.0
        ]


inc = 1/8
tri: list[float] = [0.0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875, 1.0, 0.875, 0.75, 0.625, 0.5, 0.375, 0.25, 0.125, 0.0, -0.125, -0.25, -0.375, -0.5, -0.625, -0.75, -0.875, -1.0, -0.875, -0.75, -0.625, -0.5, -0.375, -0.25, -0.125, 0.0]

ip: dict[str, list[float]] = dict()

ip["floored"] = list()
pos = 0.0
while pos < len(tri):
    ip["floored"].append(tri[math.floor(pos)])
    pos += 1.05

ip["linear"] = list()
pos = 0.0
while pos < len(tri):
    ip["linear"].append(linear(pos, tri))
    pos += 1.05

ip["cubic"] = list()
pos = 0.0
while pos < len(tri):
    ip["cubic"].append(cubic(pos, tri))
    pos += 1.05

ip["hermetic"] = list()
pos = 0.0
while pos < len(tri):
    ip["hermetic"].append(hermetic(pos, tri))
    pos += 1.05

ip["bilinear"] = list()
pos = 0.0
while pos < len(tri):
    ip["bilinear"].append(bilinear(pos, tri))
    pos += 1.05

ip["cosine"] = list()
pos = 0.0
while pos < len(tri):
    ip["cosine"].append(cosine(pos, tri))
    pos += 1.05

df = pd.DataFrame(ip)

if sys.argv[1] in ip.keys():
# print(tabulate(df, headers = 'keys', tablefmt= 'github'))
    print(tabulate(df, headers = 'keys', tablefmt= 'fancy_grid'))

    plt.rcParams["figure.figsize"]=[7.50, 3.50]
    plt.rcParams["figure.autolayout"] = True

    plt.scatter(range(len(ip[sys.argv[1]])), ip[sys.argv[1]])
    plt.plot(range(len(ip[sys.argv[1]])), ip[sys.argv[1]])
    plt.title(sys.argv[1])
    plt.show()
