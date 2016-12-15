kotlinc.bat *.kt -include-runtime -d mybot.jar
jar ufe mybot.jar MyBotKt
kotlinc.bat *.kt -include-runtime -d randombot.jar
jar ufe randombot.jar RandomBotKt
.\halite.exe -d "30 30" "java -jar mybot.jar" "java -jar randombot.jar"
