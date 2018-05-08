#!/usr/bin/env python3

from tkinter import *
from sys import argv
from time import sleep

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
transitionFrom = [None for _ in range(4)]
transitionTo = [None for _ in range(4)]

transitionProgress = [0 for _ in range(4)]
transitionGoal = [0 for _ in range(4)]

def getBufferForScreen(internScreenIndex):
    if transitionProgress[internScreenIndex] < transitionGoal[internScreenIndex]:
        return transitionTo[internScreenIndex]
    else:
        return colors[internScreenIndex]

def overrideBufferForScreen(internScreenIndex, newBuffer):
    if transitionProgress[internScreenIndex] < transitionGoal[internScreenIndex]:
        transitionTo[internScreenIndex] = newBuffer
    else:
        colors[internScreenIndex] = newBuffer

def colorTupleToText(color):
    return "#"+"".join('{:02x}'.format(ch) for ch in color)

def interpolate(color1, color2, x):
    y = 1-x
    return (int(color1[0]*y+color2[0]*x), int(color1[1]*y+color2[1]*x), int(color1[2]*y+color2[2]*x))

def updateColors():
    for i in range(4):
        for x in range(WIDTH):
            for y in range(HEIGHT):
                canvas.itemconfig(screens[i][x][y], fill=colorTupleToText(colors[i][x][y]))

def getInternalScreenIndex(player, attack):
    return (2 if attack else 3) if player == 1 else (1 if attack else 0)

def getInternalScreenCoord(screenIndex, x, y):
    if screenIndex in (0, 1): #The player two screens has x=0 to the right
        x = WIDTH-1-x
    if screenIndex % 2 == 0: #y 0 is always between attack and defend
        y = HEIGHT-1-y
    return (x,y)

from subprocess import Popen, PIPE
battleships = Popen([exec_name], stdout=PIPE, stdin=PIPE)

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

def handleInput():
    while True:
        cmd = getByt()
        if cmd == b'':
            exit(0)
        elif cmd == b"S": #Set single tile
            player, attack = readScreen()
            x, y = readXY()
            color = readColor()
            intScreenInx = getInternalScreenIndex(player, attack)
            x, y = getInternalScreenCoord(intScreenInx, x, y)
            getBufferForScreen(intScreenInx)[x][y] = color
        elif cmd == b"R": #Set rectangle
            player, attack = readScreen()
            x1, y1 = readXY()
            width, height = readXY()
            color = readColor()
            intScreenInx = getInternalScreenIndex(player, attack)
            for X in range(x1, x1+width):
                for Y in range(y1, y1+height):
                    x_, y_ = getInternalScreenCoord(intScreenInx, X, Y)
                    getBufferForScreen(intScreenInx)[x_][y_] = color
        elif cmd == b"F": #Fill all screens
            color = readColor()
            for i in range(4):
                overrideBufferForScreen(i, [[color for _ in range(HEIGHT)] for _ in range(WIDTH)])
        elif cmd == b"T": #Start transition
            player, attack = readScreen()
            inx = getInternalScreenIndex(player, attack)
            transitionFrom[inx] = colors[inx]
            transitionTo[inx] = colors[inx]
            transitionProgress[inx] = 0
            transitionGoal[inx] = getByt()[0]
        elif cmd == b'\n':
            break


def listenThread():
    global exiting
    exiting = False
    def renderLoop():
        global exiting
        while True:
            sleep(1/60)
            if exiting:
                handleInput()
                exiting = False #handleInput goes until the '\n' signal
            elif sum(transitionGoal) == 0:
                continue #Nothing exiting going on

            for inx in range(4):
                goal = transitionGoal[inx]
                if goal != 0:
                    transitionProgress[inx]+=1
                    frac = transitionProgress[inx]/goal
                    colors[inx] = [[interpolate(transitionFrom[inx][x][y], transitionTo[inx][x][y], frac) for y in range(HEIGHT)] for x in range(WIDTH)]
                    if transitionProgress[inx] == goal:
                        transitionGoal[inx] = 0
                        if sum(transitionGoal) == 0:
                            sendAllTransitionDone()

            root.after(0, updateColors)

    Thread(target=renderLoop, daemon=True).start()
    while True:
        if exiting:
            sleep(1/60)
            continue
        if getByt() == b'>':
            exiting = True
            sendReadyForCommands()



def sendReadyForCommands():
    battleships.stdin.write(b">>")
    battleships.stdin.flush()

def sendAllTransitionDone():
    battleships.stdin.write(b">T")
    battleships.stdin.flush()

def keyAction(key, release):
    if release:
        battleships.stdin.write(b">U")
    else:
        battleships.stdin.write(b">D")
    battleships.stdin.write(chr(ord('A')+key).encode('utf-8'))
    battleships.stdin.flush()

def bindKey(name, code):
    root.bind("<"+name+">", lambda x: keyAction(code, False))
    root.bind("<KeyRelease-"+name+">", lambda x: keyAction(code, True))

keys = ["Left", "Right", "Up", "Down", "space", "Return", "a", "d", "w", "s", "e", "t"]

for i in range(len(keys)):
    bindKey(keys[i], i)

from threading import Thread
Thread(target=listenThread, daemon=True).start()

root.mainloop()

