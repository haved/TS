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

def getScreenIndex(player, attack):
    return (1 if attack else 0) if player == 1 else (2 if attack else 3)

def getScreenCoord(screenIndex, x, y):
    if screenIndex in (0, 1):
        x = WIDTH-x;
    if screenIndex % 2 == 0: #y on the bottom
        y = HEIGHT-y;
    return (x,y)

def readColor(txt):
    return (txt[0], txt[1], txt[2])

def handleLine(line):
    global colors
    if line.startswith(b"BS+P"):
        playerNum = int(line[4])
        attack = line[5]==ord('A') #Screen 1 or 2
        if line[6] == ord('S'): #Single tile
            x = line[7]-ord('0')
            y = line[8]-ord('0')
            color = readColor(line[9:9+3])
            scrIndx = getScreenIndex(playerNum, attack)
            x,y=getScreenCoord(scrIndx, x, y)
            colors[scrIndx][x][y] = color
            #print("Set: colors", getScreenIndex(playerNum, attack), x, y, " to ", color)
        else:
            print("Garbo: ", line)
    elif line.startswith(b"BS+F"): #Fill
        color = readColor(line[4:4+3])
        colors = [[[color for y in range(HEIGHT)] for x in range(WIDTH)] for i in range(4)]
    else:
        print("Got a garbage line:", line)
        return
    root.after(0, updateColors)

from subprocess import Popen, PIPE
battleships = Popen([exec_name], stdout=PIPE, stdin=PIPE)

def listenThread():
    for line in battleships.stdout:
        handleLine(line)

def keyAction(key, release):
    if release:
        battleships.stdin.write(b"BS+U");
    else:
        battleships.stdin.write(b"BS+D");
    battleships.stdin.write(chr(ord('A')+key).encode('utf-8'));
    battleships.stdin.flush()
    print("Pressed:", keys[key])

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

