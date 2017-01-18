#!/bin/bash

kotlinc *.kt -include-runtime -d mybot.jar
jar ufe mybot.jar MyBotKt
kotlinc *.kt -include-runtime -d randombot.jar
jar ufe randombot.jar RandomBotKt
./halite -d "30 30" "java -jar mybot.jar" "java -jar randombot.jar"
