kotlinc.bat *.kt -d mybot.jar
kotlinc.bat *.kt -d randombot.jar
.\halite.exe -d "30 30" "kotlin -classpath mybot.jar MyBotKt" "kotlin -classpath randombot.jar RandomBotKt"
