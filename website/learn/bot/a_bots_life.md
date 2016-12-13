## <a name="a_bots_life"></a> A Bot’s Life

### Communicating with the Environment

All bots receive their input from `stdin` and send their output to `stdout`. When writing a Halite bot, you will not be able to write debugging information using functions like `System.out.print`, `cout`, `print`, etc. You will be ejected from a game of Halite if the environment receives output it does not understand. Instead, write debugging information to a log file.

### Resources

Bots are given [limited resources](https://docs.docker.com/engine/reference/run/#/runtime-constraints-on-resources) by the server. Each bot may use up to `250 MB` of memory and an equal share of the computing resources.

### Time Limits

Time limits are enforced as [wall-clock time](https://en.wikipedia.org/wiki/Wall-clock_time). Be aware that the computing resources of the [game servers](server_overview.php) may be different than your personal machine. Your bot may take more time per turn than when you run it locally.

Bots are given 15 seconds to initialize and 1 second for every subsequent turn. Every bot's clock starts ticking once the environment sends its message (be it initialization or frame) to the bot and resets once the environment receives the newline character marking the end of the bot's response. If a bot's clock hits zero, it is ejected from the game and deemed to have lost. It's pieces become part of the map.

Many players have their bot monitor their time usage and will send whatever moves they have ready before time is up to avoid timing out.

### Initialization

At the start of the game, each bot is sent some information (accessed using getInit in the starter packages):

 - Their player ID. IDs can have values of 1-6. **Do not hardcode your player ID**.
 - The initial map state.


Bots are permitted to use time at the beginning of the game to initialize. This initialization might include (but is in no way limited to):

 - getting the initial map and player tag
 - identifying important, high-production regions on the map
 - identifying the locations of neighboring players
 - planning the bot's initial expansion strategy
 - compiling a model


Once bots are done initializing (before their time is up), they should send a response (sendInit in the starter packages) with their own player name. When running locally, your player name will be used in the replay file and in the terminal output. When running on the server, your username will automatically be used, regardless of the name the bot sends to the server.

### Turns

After all bots have finished setting up, the environment will do the following until endgame conditions are met:

1. Send the present gamestate - map and messages - to all players.
2. Receive moves from the players.
3. Kill bots whose responses take longer than their remaining allotted time.
4. Add strength to pieces which choose to remain where they are.
5. Simultaneously move (and combine if necessary) all player's pieces. The capping of strengths to 255 occurs here. See the [strength cap documentation](game_overview.php#strength_cap_details) for more details.
6. Simultaneously damage (and remove if damage equals or exceeds strength) all player's pieces. All pieces will output damage equivalent to their strength when starting this phase, and the damage will apply to all coinciding or adjacent enemy squares. See the [combat documentation](game_overview.php#combat) for more details.
7. Check if endgame conditions have been met.
