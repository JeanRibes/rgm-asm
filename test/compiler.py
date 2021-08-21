from time import sleep
import os, io

outer_circle = [0, 1, 2, 3, 8, 14, 21, 27, 32, 36, 35, 34, 33, 28, 22, 15, 9, 4]
mid_circle = [5, 6, 7, 13, 20, 26, 31, 30, 29, 23, 16, 10]
inner_circle = [11, 12, 19, 25, 24, 17, 18]
fill = lambda r, g, b: f"""STORE 0,0
STORE 1,{r}
STORE 2,{g}
STORE 3,{b}
LED 0,1,2,3i
ADD 0,1
CMP 0,37
JMP -3c
SHOW _
"""


def send_serial(lines):
    with open('/dev/ttyUSB0', 'ab', buffering=0) as f:
        for line in lines:
            f.write(line.encode('ascii') + b'\n')
            sleep(0.002)

def calc_jump(j):
    lj = []
    while j > 255:
        lj.append(254)
        j -= 254
    lj.append(j)
    return lj

def circle(write):
    write("RESET")
    write(fill(0, 0, 0))
    label_1 = len(prgm)
    write("STORE 0,0\nRAND 1\n RAND 2\nRAND 3")
    for led in outer_circle + mid_circle + inner_circle:
        write(f"STORE 0,{led}\nLED 0,1,2,3i\nSHOW _\nSLEEP 5")
    for led in reversed(outer_circle + mid_circle + inner_circle):
        write(f"LED {led},0,0,0\nSLEEP 3\nSHOW _")
    #write(f"LNJMP -{len(prgm) - label_1}") # pour tourner en boucle

if __name__ == '__main__':
    prgm = []
    def write(x):
        prgm.extend(x.split('\n'))

    
    #write("RUN")
    #write("STORE 7,1")#décalage initial
    #write("STORE 8,10")#adresse de départ
    #i=0
    #for led in outer_circle + mid_circle + inner_circle:
    #    write(f"STORE {10+i},{led}") # liste de pointeurs
    #    i+=1
    #write(f"STORE 9,{10+i}")# adresse de fin
    #write("RAND 1\nRAND 2\nRAND 3") # couleurs
    #write("STORE 100,9i")
    #write("ADD 100,1")
    circle(write)
    send_serial(prgm)
