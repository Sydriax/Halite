## <a name="improving_the_random_bot"></a> Improving the Random Bot

In this tutorial, we'll go through the code that powers the `RandomBot` and add a couple heuristics to it. This will hopefully help you fully understand Halite and set you on your way to [leaderboard](leaderboard.php) domination.

The code in this tutorial can be found at the following links for [Python](https://gist.github.com/JCGrant/5ea767bae8a725f738d75649e5f2c91a), [Java](https://gist.github.com/Sydriax/a2b8b88c940abe8f346df62a77e23441), and [C++](https://gist.github.com/Sydriax/3aaabd3ecbc03ff997c720e7c5840a9a).

### Prerequisites

This tutorial assumes you have read the [game rules](game_overview.php#game_rules) and have your [local development environment](developing_a_bot.php#local_development_environment) setup.

Now open up the `MyBot` file in your favorite editor and let's get started!

### Import Considerations

When writing a halite bot, be sure to stay away from functions like `System.out.print`, `cout`, `print`, etc. Bots use `stdout` and `stdin` to communicate with the game environment. You will be ejected from a game of Halite if you print debugging info to `stdout`. Instead, print to a log file.

### A Look At the Random Bot

Now that you know how the game works, how do the two random starter bots work? How does one code a Halite bot? Here is the source from the main file of our python starter bot:

```python
# Goes in MyBot.py

from hlt import *
from networking import *

myID, gameMap = getInit()
sendInit("MyPythonBot")

while True:
    moves = []
    gameMap = getFrame()
    for y in range(gameMap.height):
        for x in range(gameMap.width):
            location = Location(x, y)
            if gameMap.getSite(location).owner == myID:
                moves.append(Move(location, random.choice(DIRECTIONS)))
    sendFrame(moves)
```

Let's walk through it line by line.

First we import a couple of helper files that are included in the starter packages:

```python
from hlt import *
from networking import *
```

Then we get our ID (each player has a unique identifier that is associated with their pieces) and the game initial map from the environment.

We send back the name of our bot. This is used in game replays.

```python
myID, gameMap = getInit()
sendInit("MyPythonBot")
```

Now we start our game loop. Each frame let's initialize a list of moves and get the current map:

```python
while True:
    moves = []
    gameMap = getFrame()
```

Let's cycle through all of the pieces on the map. If a piece is owned by us, let's order it to move in a random direction.

```python
    for y in range(gameMap.height):
        for x in range(gameMap.width):
            location = Location(x, y)
            if gameMap.getSite(location).owner == myID:
                moves.append(Move(location, random.choice(DIRECTIONS)))
```

Finally, let's send all of our moves to the environment:

```python
    sendFrame(moves)
```

And that's random bot!

### Utilizing Our Production

From the [game rules](game_overview.php#game_rules), we know that when a piece moves, it gains no strength and leaves behind a piece with zero strength. It easily follows from this that moving zero strength pieces is a terrible idea, since:

 - A zero strength piece that moves will necessarily stay at zero strength, because pieces don't gain strength for any turn that they move.
 - A zero strength piece won't ever conqueror any territory, because it has no strength with which to damage other pieces.
 
Let's wrap the movement logic inside a function of its own. This function will take the location of a piece and will return the piece's movement.

Now we can improve our bot by making sure that we tell all of our zero strength pieces to remain still.

```python
# Goes in MyBot.py

from hlt import *
from networking import *

myID, gameMap = getInit()
sendInit("PythonBot")

def move(location):
    site = gameMap.getSite(location)
    if site.strength == 0:
        return Move(location, STILL)
    return Move(location, random.choice(DIRECTIONS))

while True:
    moves = []
    gameMap = getFrame()
    for y in range(gameMap.height):
        for x in range(gameMap.width):
            location = Location(x, y)
            if gameMap.getSite(location).owner == myID:
                moves.append(move(location))
    sendFrame(moves)
```

Our bot still moves its pieces around a lot (only a bit over one out of five turns will a piece stay still). This costs us a lot of strength (since a piece doesn't gain any strength on turns that it moves).

To increase our utilization of our production, let's have pieces only move once their strength equals their production times some factor X. We're using 5 as the value of X in this example, but this is arbitrary.

```python
# Goes in MyBot.py

from hlt import *
from networking import *

myID, gameMap = getInit()
sendInit("PythonBot")

def move(location):
    site = gameMap.getSite(location)
    if site.strength < site.production * 5:
        return Move(location, STILL)
    return Move(location, random.choice(DIRECTIONS))

while True:
    moves = []
    gameMap = getFrame()
    for y in range(gameMap.height):
        for x in range(gameMap.width):
            location = Location(x, y)
            if gameMap.getSite(location).owner == myID:
                moves.append(move(location))
    sendFrame(moves)
```

### Moving to Our Borders

When building a Halite bot, one of our goals should be moving strength out of your territory quickly and with little production loss. Our current bot is terrible at this. Its pieces move randomly around our territory, going nowhere, costing us production, and often losing strength to the [strength cap](game_overview.php#strength_cap_details).

To improve this, let's just mandate that our pieces move only north and west. Since the map is wrap-around, we can still capture all of the board with this strategy!

```python
# Goes in MyBot.py

from hlt import *
from networking import *

myID, gameMap = getInit()
sendInit("PythonBot")

def move(location):
    site = gameMap.getSite(location)
    if site.strength < site.production * 5:
        return Move(location, STILL)
    return Move(location, NORTH if random.random() > 0.5 else WEST)

while True:
    moves = []
    gameMap = getFrame()
    for y in range(gameMap.height):
        for x in range(gameMap.width):
            location = Location(x, y)
            if gameMap.getSite(location).owner == myID:
                moves.append(move(location))
    sendFrame(moves)
```

### Improving our Attack

Once our pieces get to our borders, we don't want them to randomly attack just any square (or worse, move back into our territory), as we do now. One problem with this random strategy is that we may attack a map square that has more strength than us. This is unproductive (pun implied) since moving onto the map square costs us a turn of production and we don't actually gain anything. We just diminish the squares strength.

To improve on our current combat, if there is an enemy or map square that is adjacent to one of our pieces with less strength than our piece, let's take it.

```python
# Goes in MyBot.py

from hlt import *
from networking import *

myID, gameMap = getInit()
sendInit("PythonBot")

def move(location):
    site = gameMap.getSite(location)
    for d in CARDINALS:
        neighbour_site = gameMap.getSite(location, d)
        if neighbour_site.owner != myID and neighbour_site.strength < site.strength:
            return Move(location, d)
    if site.strength < site.production * 5:
        return Move(location, STILL)
    return Move(location, NORTH if random.random() > 0.5 else WEST)

while True:
    moves = []
    gameMap = getFrame()
    for y in range(gameMap.height):
        for x in range(gameMap.width):
            location = Location(x, y)
            if gameMap.getSite(location).owner == myID:
                moves.append(move(location))
    sendFrame(moves)
```

### What Next?

That's really up to you! How you improve your bot from here is where you step into the competition.

That said, if you're looking for more ideas or a stronger starting base, [nmalaguti](https://halite.io/user.php?userID=2697) wrote a tutorial [here](http://forums.halite.io/t/so-youve-improved-the-random-bot-now-what/482) that we highly recommend.

Good luck!
