# Halite Contributing Guide

If you find a bug or have a feature request, please [open an issue](https://github.com/HaliteChallenge/Halite/issues/new).

Want to help out? Have you implemented a patch or a new feature? Send us a pull request! If you are looking for things to do, check out [our open issues](https://github.com/HaliteChallenge/Halite/issues).

## Common Contributions 

### Writing a Starter Package

If you'd like to write a starter package for a new language, see this [guide](https://halite.io/advanced_writing_sp.php).

### Adding Your Company or University

Edit [this whitelist](https://github.com/HaliteChallenge/Halite/edit/master/website/organizationWhitelist.txt) and send us a pull request. If you need to change your email, head [here](https://halite.io/email.php). We'll make sure to tag all members of your organization who have already signed up.

## Folder Contents

- `admin/` - A collection of administrative resources (ex. a technical specification)
- `airesources/` - The language-specific starter kits for writing bots
- `environment/` - The halite game engine 
- `tests/` - All of the project's unit and integration tests
- `website/` - The website that hosts the competition. Includes the API that manages the game servers.
- `worker/` - The source for the worker servers that compile bots and run games safely

## Installation 

### Installing the website on Ubuntu

Clone the repo:

    git clone https://github.com/HaliteChallenge/Halite.git

Run the website install script with root user permissions. This will install php, apache, python (and some python modules), and composer (and some composer packages):

    cd website; sudo ./install.sh

Run the database install script with root user permissions. This will install mysql, insert our schema into a database titled Halite, and import some dummy users.

    cd sql; sudo ./install.sh

Create a `halite.ini` file using your favorite text editor in the root project directory. Place information about your local database setup in there. Your `halite.ini` file should look like this;
    
    [database]
    hostname = 127.0.0.1 
    username = YOUR_LOCAL_MYSQL_USERNAME
    password = YOUR_LOCAL_MYSQL_PASSWORD
    name = Halite

### Setting up the manager and worker on Ubuntu 

We assume that you have already followed "Installing the website on Ubuntu."

Run the worker install script. It will install some required libraries, switch your default compiler to g++4.9, build our docker sandbox, and enable swap memory:
    
    cd worker; sudo ./install.sh

Add these lines to your `halite.ini` file:

    [hce]
    managerURl = http://localhost/website/api/manager/
    apiKey = 1 
    secretFolder = blah_blah_blah

Reboot your system (because of the enabling of swap memory):
    
    sudo reboot

Start a worker:

    cd worker; sudo python3 worker.py
