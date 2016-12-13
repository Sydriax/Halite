## <a name="hardware"></a> Hardware

All compilation and game execution is done on [AWS EC2 m3.medium](https://aws.amazon.com/ec2/instance-types/) servers running Ubuntu 16.04. They have the following specs:
 - 3.5 GB of RAM
 - ~10 GB of disk space
 - 1 CPU
 
### Sandbox

During compilation and games, players are run as unprivileged users in a Docker sandbox.

For compilation, players are given internet access to fetch dependencies and are limited to 150 MB of disk space.

For games, players are limited to:
 - 250 MB of RAM
 - An equal share of CPU resources
 - No internet access


The sandbox is a docker container defined by this [Dockerfile](https://github.com/HaliteChallenge/Halite/blob/master/worker/Dockerfile), running on top of Docker 1.6.2.
If you think that there is an issue with the sandbox, please email us at <halite@halite.io>.
