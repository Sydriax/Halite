# Overview

Halite is an online programming challenge. Users write bots to play a simple, original game with the following rule set:

* Bots may move their pieces either north, south, east, or west every turn or choose to remain still.
* If a piece stays still, it gains strength equal to the production value of the current tile that it is on.
* If a piece moves onto a piece with the same owner, their strengths combine. Strength values are cut off at 255.
* A piece inflicts damage equal to its strength onto all adjacent pieces that are not maps squares but have a different owner and onto to all coinciding pieces that have a different owner (this includes map squares).
* When a piece has a strength < 0, it dies.

Users develop their bots locally using our game engine, zip and submit their source to our website when they are ready to test out their bot, and watch as their bot plays against others and is ranked on our leaderboard.

**Note:** This spec details how the project currently fuctions. Changes will be made before the public launch.

# Project Components

## Environment

The environment is written in C++ with no dependencies. The environment starts bot processes using the start commands given to it through the command line. It then communicates with bots over stdin and stdout, sending them the map and recieving their moves. A switch to using sockets for bot communication is planned. The environment outputs a replay file, with the `hlt` extension, which may be visualized [here](http://halite.io/website/game.php).

## Website

The frontend of halite.io is written in HTML, CSS, and Javascript. The Bootstrap 3 CSS library is used for styling. The Pixi javascript library is used for our game visualizer. The JQuery library is used for DOM manipulation and for AJAX calls. HTML files are classified as PHP files to allow the easy including of repeating HTML elements (i.e. the navigation bar). No templating is used at all on the frontend. All interactions with our backend are done through REST calls made through the JQuery AJAX library.

The backend is written in PHP. Apache is used as its webserver.

The server on which the website is hosted also hosts the manager and is used for the storage of error logs, replays, and the source code of contestants.

## HCE

The "HCE"(Halite Competition Environment) is what we call the system of servers that compiles the source code of each contestant, runs games between bots, and ranks each submission. The system consists of many worker servers and one manager server. 

Worker servers query the manager server for tasks, either a compile task or a game task. If there is any bot that needs to be compiled, the manager will respond with a compile task. If there are no compile tasks, the manager will respond with a game task, which is chosen like so:

* A seed plager is chosen by picking the bot with the highest `rand()*(sigma^2)`. "Sigma" is the level of uncertainty in the Trueskill score of a bot
* An allowed rank difference `d` is computed as: `5 / rand()^0.65`
* The number of players `n` is picked at random from a range of 2-6
* The other `n-1` players are chosen at random from the players within `d`
* The width and height of the map is chosen from a range of 20-50

Once given the ID of the bot(s) that they are compiling/running, workers query the manager for the executables and source of each bot. After compilation, the resulting binary+source mix is posted to the manager. These are removed from the disk on the worker server on completion of a task.

During both compilation and runtime, bots are run within their own Docker container. Networking, RAM, CPU, and disk access is limited.

## Database

A MySQL server is used as the database for the project. 

## Disk Storage

Replays, bot source and executables, and error logs are all currently stored on the server that hosts the website and the manager.

# Backups

Backups are made hourly from the website/manager server and the database server to a server using rsync and cron. Only the most recent data from the website/manager server data is kept, while all versions of the database data are stored.

# Admin Tools

A simple python script (`manager/commandRunner.py`) is used to run arbitrary commands on all of the workers listed in the `halite.ini` file.

A status page located at `halite.io/website/status.php` includes the time since every worker has queried the manager, the throughput of the HCE, and general stats about our user base. Google analytics is included on the site.

We plan on using cron jobs to alert us if one of the workers hasn't responded for a number of minutes and to alert us if there are any broken links on the site.

We plan on setting up pingdom to alert us of downtime.

# Configuration

An INI file, titled `halite.ini` and located in the root directory of the project, is used for all of our project configurations. Here is a sample halite.ini file: 

```
[hce]
managerURl = http://localhost/manager/
apiKey = 1234 
secretFolder = FAKE_FOLDER_NAME

[workerIPs]
FAKE_WORKER_NAME = 123.456.789.000

[email]
email = FAKE_EMAIL
password = FAKE_PASSWORD

[database]
hostname = localhost
username = root
password = pass123
name = MAIN_DB_NAME

[sso]
secret = SECRET_KEY_FORUMS
url = SINGLE_SIGN_ON_FORUMS

[forums]
apiUsername = FAKE_USERNAME
apiKey = 1234567890

[encrypt]
salt = abc123456789
```

# Project Component Diagram

![Server Architecture Diagram](https://github.com/HaliteChallenge/Halite/raw/master/spec/components.png)

# Installation

If you have not already:

```git clone https://github.com/HaliteChallenge/Halite.git```

## Website/Manager Server Setup

* Execute: ```cd website && ./install.sh```
* Check that you are on php >= 5.6 and mysql >= 5.6.5: ```php -v; mysql -V```
* Symlink the repo to /var/www: ```ln -s ~/Halite /var/www```
* Create and write a halite.ini file in the root directory of the project
* Apache setup
 * [Allow the following of symlinks by apache](http://superuser.com/questions/244245/how-do-i-get-apache-to-follow-symlinks)
 * [Allow .htaccess override](http://stackoverflow.com/a/22526144)
 * [Redirect root directory to website directory](http://serverfault.com/questions/9992/how-to-get-apache2-to-redirect-to-a-subdirectory)
 * Tell apache to forbid access to the storage/errors and storage/bots folders and to the halite.ini file

## Database server setup

* Execute: ```cd website/sql && ./install.sh```
* Add a superuser by wildcarding its host and allowing remote login
 * ```CREATE USER 'superuser'@'%' IDENTIFIED BY 'SOME_RANDOM_PASSWORD';```
 * ```GRANT ALL PRIVILEGES ON *.* TO 'superuser'@'%' IDENTIFIED BY 'SOME_RANDOM_PASSWORD' WITH GRANT OPTION;```
 * ```FLUSH PRIVILEGES;```
 * Comment out `bind-address = 127.0.0.1` in `/etc/mysql/my.cnf`
 * ```sudo service mysql restart```

## Worker server setup

* Execute: ```cd worker && ./install.sh```
* Create a `halite.ini` file in the root directory of the project
* Add the `apiKey` and the ip address of the worker to the `Worker` table
* [Enable swap](https://docs.docker.com/engine/installation/linux/ubuntulinux/#/adjust-memory-and-swap-accounting)
* Execute ```sudo python3 worker.py```
