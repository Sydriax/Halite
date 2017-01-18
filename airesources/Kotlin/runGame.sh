#!/bin/bash

kotlinc *.kt -d mybot.jar
kotlinc *.kt -d randombot.jar
./halite -d "30 30" "kotlin -classpath mybot.jar MyBotKt" "kotlin -classpath randombot.jar RandomBotKt"