from hlt import *
from networking import *


from keras.models import Sequential, model_from_json
from keras.layers import Dense, Activation
from keras.optimizers import SGD, Adam, RMSprop

import numpy as np
import random

model = model_from_json(open('my_model_architecture.json').read())
model.load_weights('my_model_weights.h5')
#model.compile(loss='categorical_crossentropy', optimizer='adam')

myID, gameMap = getInit()
maxProduction = 0
for y in range(gameMap.height):
    for x in range(gameMap.width):
        prod = gameMap.getSite(Location(x, y)).production
        if prod > maxProduction:
            maxProduction = prod

with open('net-info.txt', 'w') as f:
    sendInit("rustyBot")
    while True:
        moves = []
        gameMap = getFrame()
        boardSize = len(range(gameMap.height)) * len(range(gameMap.width))
        
        for y in range(gameMap.height):
            for x in range(gameMap.width):
                loc = Location(x, y)
                here = gameMap.getSite(loc)
                    
                if here.owner == myID:
                    box = [
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,SOUTH,EAST,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,SOUTH,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,SOUTH,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,SOUTH,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,SOUTH)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,SOUTH,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,SOUTH,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,SOUTH,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,SOUTH,WEST,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,EAST,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,SOUTH,WEST,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,EAST,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,SOUTH,WEST,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,EAST,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, SOUTH,WEST,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, EAST,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, WEST,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,EAST,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,WEST,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,EAST,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,WEST,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,EAST,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,WEST,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,NORTH,EAST,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,NORTH,EAST,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,NORTH,EAST,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,NORTH,EAST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,NORTH)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,NORTH,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,NORTH,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,NORTH,WEST,WEST,WEST)),
                        gameMap.getSite(gameMap.getLocation(loc, NORTH,NORTH,NORTH,NORTH,WEST,WEST,WEST,WEST))
                    ]
                    nnInput = [
                        -1 if here.strength > 0 else 1
                    ]
                    for site in box:
                        cell_value = site.production * 4
                        if site.owner != myID:
                            cell_value = cell_value - here.strength - site.strength
                        else:
                            cell_value = 0

                        cell_value = cell_value / (255 * 4)
                        nnInput += [1 if site.owner == myID else -1,
                                    float(site.strength / 255),
                                    float(site.production / maxProduction),
                                    1 if site.owner != myID and site.strength <= here.strength else -1,
                                    float(math.log(site.strength / (here.strength+0.001)+0.01)),
                                    float(cell_value)
                            ]
                    nnInput = np.asarray(nnInput).reshape((1, len(nnInput)))

                    f.write("{},{}\n".format(x, y))
                    f.write("Net results\n")

                    
                    output = model.predict(nnInput)[0]
                    biggest = -222
                    direction = STILL
                    check = range(len(output))
                    output[0] = output[0] * 1.2
                    f.write("Strength {}\n".format(gameMap.getSite(loc).strength));
                    for d in check:
                        f.write("output {} {}\n".format(d, output[d]))
                        if output[d] > biggest:
                            biggest = output[d]
                            direction = d
                            
                    f.write("Picked direction {}\n".format(direction))
                    f.write("other cell strength: {}\n".format(gameMap.getSite(gameMap.getLocation(loc, direction)).strength))
                    moves.append(Move(loc, direction))
        sendFrame(moves)

