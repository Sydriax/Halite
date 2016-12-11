Bots can be developed in any language that can read from `stdin` and write to `stdout` (pretty much every language ever).

## <a name="local_development_environment"></a> Local Development Environment

Starter Package Overview
See the Downloads page for a selection of Starter Packages to choose from. While using a Starter Package is not required, we recommend starting with a Starter Package and getting an idea how everything works.


Your starter package should contain these files in your language of choice:

| Filename     | Description |
| ------------ | ----------- |
| MyBot        | Your main file. Starts containing the code for a random bot. |
| RandomBot    | A random bot to test changes to your bot against.      |
| runGame.sh   | Script to run a game on Linux/macOS.      |
| runGame.sh   | Script to run a game on Linux/macOS.      |
| runGame.bat  | Script to run a game on Windows. Assumes halite.exe is in the same folder.    |
| hlt          | In some starter packages, this contains code describing the GameMap, Locations, Sites, etc.    |
| Networking   | In some starter packages, this contains code for receiving and parsing input from stdin and formatting and sending output to stdout.      |


You can (and should!) edit these files to fit your needs. You must include a `MyBot` file in your `zip` file submission and any other files needed to compile and run your `MyBot` file.

### The Halite Environment

You need the `halite` executable in order to run your bot. Follow the instructions for your OS on the [Downloads page](downloads.php). When you are finished, `halite` should exist in the same directory as `runGame.sh`.

The Halite environment is responsible for running games between bots and outputting appropriate data and files upon the end of a game. The downloadable version is the same version used on the servers.


It may be passed a number of flags, including:

 - `-d`: allows the automatic input of the dimensions of the map. The following argument is expected to be a string containing the width and height (space-separated).
 - `-t`: disables timeouts for the duration of the game.
 - `-q`: turns on quiet output. Output will take the form of:
   - A line containing the replay file name, a space, and the map seed.
   - For n players in the game, n lines like so: playerID rank
   - A line of space separated playerIDs of the players that timed out.
 - `-s`: provides the seed to the map generator. If this is not provided, it will use a time-based seed.

#### Examples

To run your bot against itself on a 40 by 40 map with no timeouts, run:

 - Linux/macOS: `./halite -d “40 40” -t “python3 MyBot.py” “python3 MyBot.py”`
 - Windows: `.\halite.exe -d “40 40” -t “python3 MyBot.py” “python3 MyBot.py”`

To run your python bot against a java bot (assuming it’s been compiled) on a 25 by 25 map with a predefined seed (2168), run:

 - Linux/macOS: `./halite -d “25 25” -s 2168 “python3 PythonBot.py” “java JavaBot”`
 - Windows: `.\halite.exe -d “25 25” -s 2168 “python3 PythonBot.py” “java JavaBot”`

### Testing Your Bot

To simulate a game, simply `runGame.sh` (Linux and macOS) or `runGame.bat` (Windows). This command will run a game between my `MyBot` and `RandomBot` (both are just copies of each other at this point) on a grid of size 30x30. You can edit this file to meet your needs.

The output should look like this and the details of the game will be stored in a file with the `hlt` extension (`35538-124984302.hlt` in the example below).

```text
$ ./runGame.sh 
python3 MyBot.py
python3 RandomBot.py
Init Message sent to player 2.
Init Message sent to player 1.
Init Message received from player 1, MyPythonBot.
Init Message received from player 2, RandomPythonBot.
Turn 1
Turn 2
Turn 3
Turn 4
...
Turn 299
Turn 300
Map seed was 124984302
Opening a file at 35538-124984302.hlt
Player #1, MyPythonBot, came in rank #1!
Player #2, RandomPythonBot, came in rank #2!
```

### Visualizing a Game

The console output from the game environment gives just the outcome of the game. To replay the game, drag and drop the file to the [visualizer](local_visualizer.php) to get a visualization like this one:

<div id="gameReplay" class="text-center"></div>

### Best Practices

#### Using a Log File

`stdout` and `stdin` are used to communicate with the game environment. As such, you cannot use functions like `System.out.println`, `print()`, or `std::cout`. Instead, print debugging information to a log file.

#### Local Bot Evaluation

Before submitting a new bot to the online [leaderboard](leaderboard.php), we recommend running some games against the version of your bot that is currently on the leaderboard. If your new bot consistently wins, then put it up!

#### Disabling the Timeout Flag

When debugging latency issues with your bot, it can be helpful to disable game environment timeouts. To do so, append the `-t` flag to your environment command (e.g. `./environment -d "30 30" "python3 MyBot.py" "python3 RandomBot.py" -t`).

#### Debugging with an IDE

There is a community contributed method for running a Halite bot from a custom debugger locally. More on this can be found [here on the forums](http://forums.halite.io/t/running-your-halite-bot-from-a-debugger/70).

<script>
window.onload = function() {
    $('table').addClass('table');
    var data = textFromURL("ar1478846062-2923329127.hlt", $("#gameReplay"), function(data) {
            console.log(data)
            if(data != null) {
                showGame(data, $("#gameReplay"), null, 500, true, false);
            }
        });
};
</script>
