# Introduction

This defines an [Ansible](http://www.ansible.com/) that will deploy the Halite
website and database onto a single server. The playbook assumes a basically
fresh Ubuntu 16.04 OS install and that it will be the single use for the server.

WARNING: Do not deploy this to a server you are using for other purposes. This
will probably interfere with whatever you are already doing on the server.

# Prerequisites

On the server you need a user that the Halite code will live under, can be
accessed with SSH and has sudo permissions. Python must also be installed, `sudo apt install python` will do that.

Locally you need ansible installed, `pip install ansible`.

An `inventory` file in this directory with the line:
    MY.SERVER.HOSTNAME remote_user="USERNAME"

Where MY.SERVER.HOSTNAME is either the domain or ip address to your server and
USERNAME is the user on the server for the Halite code.

Also a `settings.yml` file filled in with the settings shown in
`settings_example.yml`.

If you will be using ssh password authentication instead of key based
authentication, you also need `sshpass` installed locally, `sudo apt install
sshpass`.

# Running the playbook

After getting the prerequisites setup above, to deploy to the server simply run:
    ansible-playbook -i inventory primary.yml

If you need to specify an ssh password, use the -k option. Similarly if a sudo password is need use -K. For example if you need both ssh and sudo passwords it would look something like:
    ansible-playbook -i inventory primary.yml -k -K
