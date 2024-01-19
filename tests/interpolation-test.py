import math
import pandas as pd
from tabulate import tabulate
import matplotlib.pyplot as plt
import sys

TRI = "triangle"
SAW = "sawtooth"
SQR = "square"
SIN = "sine"
HAN = "hanning"


def sine(length: int) -> list[float]:
    table = list()
    angle = 0.0
    inc = (math.pi*2) / length
    for i in range(length):
        table.append(math.sin(angle))
        angle = angle + inc
    return table


def triangle(length: int):
    table = list()
    angle = 0.0
    inc = 4.0 / length
    for i in range(length):
        if angle > 1.0 or angle < -1.0:
            inc = inc * -1.0
        table.append(angle)
        angle = angle + inc
    return table


def square(length: int):
    table = list()
    val = 1.0
    for i in range(length):
        if i == length/2:
            val = -1.0
        table.append(val)
    return table


def saw(length: int):
    table = list()
    angle = -1.0
    inc = 2.0 / length
    for i in range(length):
        table.append(angle)
        angle = angle + inc
    return table


def hanning(length: int):
    table = list()
    angle = 0.0
    inc = math.pi / length
    for i in range(length):
        table.append(-1.0 - pow(math.cos(angle), 2))
        angle = angle + inc
    return table

###############################################################################
###############################################################################


def cubic(pos: float, table: list[float]):
    x1: int = math.floor(pos)
    x0: int = wrap(x1-1, len(table))
    x2: int = wrap(x1+1, len(table))
    x3: int = wrap(x1+2, len(table))
    diff: float = pos - x1
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


def waveshape(shape: str, wavelength: int) -> list[float]:
    if shape == TRI:
        return triangle(wavelength)
    elif shape == SAW:
        return saw(wavelength)
    elif shape == SQR:
        return square(wavelength)
    elif shape == SIN:
        return sine(wavelength)
    elif shape == HAN:
        return hanning(wavelength)


if __name__ == "__main__":
    if sys.argv[1] == "-h" or sys.argv[1] == "--help":
        print("""
    NTERPOLATION TESTING:
      interpolation: [ none, linear, cubic, bilinear, cosine ]
      waveshape:     [ sine, triangle, square, sawtooth, hanning ]
      buffer size:   integer
              """)
        sys.exit(0)

    inc = 1/8
    wavelength = 512
    shape = TRI
    wave = waveshape(TRI, wavelength)
    print(len(wave))
    ip: dict[str, list[float]] = dict()

    if len(sys.argv) >= 4:
        wavelength = int(sys.argv[3])

    if len(sys.argv) >= 3:
        shape = sys.argv[2]
        wave = waveshape(shape, wavelength)

    ip["none"] = list()
    pos = 0.0
    while pos < wavelength:
        i = math.floor(pos)
        ip["none"].append(wave[i])
        pos += 0.5

    ip["linear"] = list()
    pos = 0.0
    while pos < wavelength:
        ip["linear"].append(linear(pos, wave))
        pos += 0.5

    ip["cubic"] = list()
    pos = 0.0
    while pos < wavelength:
        ip["cubic"].append(cubic(pos, wave))
        pos += 0.5

    ip["hermetic"] = list()
    pos = 0.0
    while pos < wavelength:
        ip["hermetic"].append(hermetic(pos, wave))
        pos += 0.5

    ip["bilinear"] = list()
    pos = 0.0
    while pos < wavelength:
        ip["bilinear"].append(bilinear(pos, wave))
        pos += 0.5

    ip["cosine"] = list()
    pos = 0.0
    while pos < wavelength:
        ip["cosine"].append(cosine(pos, wave))
        pos += 0.5

    df = pd.DataFrame(ip)

    if sys.argv[1] in ip.keys():
        print(tabulate(df, headers = 'keys', tablefmt= 'fancy_grid'))
        plt.rcParams["figure.figsize"]=[7.50, 3.50]
        plt.rcParams["figure.autolayout"] = True
        title = " Interpolation: {}  |  Waveshape: {}  |  Buffersize: {} ".format(
                sys.argv[1].capitalize(),
                shape.capitalize(),
                wavelength
                )

        plt.scatter(range(len(ip[sys.argv[1]])), ip[sys.argv[1]])
        plt.plot(range(len(ip[sys.argv[1]])), ip[sys.argv[1]])
        plt.title(title)
        plt.show()
