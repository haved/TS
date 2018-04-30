#!/usr/bin/env python3

from tkinter import *
from sys import argv

exec_name = argv[1]

TILE_SIZE = 20
WIDTH = 10
HEIGHT = 10
MARGIN = 30

root = Tk()
DISP_WIDTH = TILE_SIZE*WIDTH+MARGIN*2
DISP_HEIGHT = TILE_SIZE*HEIGHT*4+MARGIN*5
root.geometry(str(DISP_WIDTH) + "x" + str(DISP_HEIGHT))
canvas = Canvas(root, width=DISP_WIDTH, height=DISP_HEIGHT)
canvas.pack()

def makeScreen(x_, y_):
    def makeTile(x, y):
        global canvas
        tile = canvas.create_rectangle(0, 0, TILE_SIZE, TILE_SIZE)
        canvas.move(tile, x_+x*TILE_SIZE, y_+y*TILE_SIZE)
        return tile
    return [[makeTile(x, y) for y in range(HEIGHT)] for x in range(WIDTH)]

screens = [makeScreen(MARGIN, MARGIN+(MARGIN+TILE_SIZE*HEIGHT)*i) for i in range(4)]

colors = [[[(0,0,0) for y in range(HEIGHT)] for x in range(WIDTH)] for i in range(4)]

def colorTupleToText(color):
    return "#"+"".join('{:02x}'.format(ch) for ch in color)

def updateColors():
    for i in range(4):
        for x in range(WIDTH):
            for y in range(HEIGHT):
                canvas.itemconfig(screens[i][x][y], fill=colorTupleToText(colors[i][x][y]))

def getInternalScreenIndex(player, attack):
    return (2 if attack else 3) if player == 1 else (1 if attack else 0)

def getInternalScreenCoord(screenIndex, x, y):
    if screenIndex in (0, 1): #The player two screens has x=0 to the right
        x = WIDTH-1-x;
    if screenIndex % 2 == 0: #y 0 is always between attack and defend
        y = HEIGHT-1-y;
    return (x,y)

from subprocess import Popen, PIPE
battleships = Popen([exec_name], stdout=PIPE, stdin=PIPE)

def listenThread():
    global colors

    def getByt():
        return battleships.stdout.read(1)
    def readXY():
        byt = getByt()[0]-ord('0')
        x = byt % WIDTH
        y = byt // WIDTH
        return (x, y)
    def readColor():
        return (getByt()[0], getByt()[0], getByt()[0])
    def readScreen():
        byt = getByt()[0]-ord('A')
        player = 2 if byt >= 2 else 1
        attack = byt % 2 == 0
        return (player, attack)

    while True:
        byt = getByt()
        if byt == b"": #Out of input
            break
        if byt != b">":
            continue
        byt = getByt()
        if byt == b"S": #Set single tile
            player, attack = readScreen()
            x, y = readXY()
            color = readColor()
            intScreenInx = getInternalScreenIndex(player, attack)
            x, y = getInternalScreenCoord(intScreenInx, x, y)
            colors[intScreenInx][x][y] = color
        elif byt == b"R": #Set rectangle
            player, attack = readScreen()
            x1, y1 = readXY()
            width, height = readXY()
            color = readColor()
            intScreenInx = getInternalScreenIndex(player, attack)
            for X in range(x1, x1+width):
                for Y in range(y1, y1+height):
                    x_, y_ = getInternalScreenCoord(intScreenInx, X, Y)
                    colors[intScreenInx][x_][y_] = color
        elif byt == b"F": #Fill all screens
            color = readColor()
            colors = [[[color for _ in range(HEIGHT)] for _ in range(WIDTH)] for _ in range(4)]
        root.after(0, updateColors)


def keyAction(key, release):
    if release:
        battleships.stdin.write(b">U");
    else:
        battleships.stdin.write(b">D");
    battleships.stdin.write(chr(ord('A')+key).encode('utf-8'));
    battleships.stdin.flush()

def bindKey(name, code):
    root.bind("<"+name+">", lambda x: keyAction(code, False))
    root.bind("<KeyRelease-"+name+">", lambda x: keyAction(code, True))

keys = ["Left", "Right", "Up", "Down", "space", "Return", "a", "d", "w", "s", "e", "t"]

for i in range(len(keys)):
    bindKey(keys[i], i)

from threading import Thread
Thread(target=listenThread, daemon=True).start()
#Thread(target=writeThread, daemon=True).start()

root.mainloop()

