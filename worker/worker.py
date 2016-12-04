import archive
import backend

import random

import os
import os.path
import stat
import glob

import platform
import tempfile

from time import sleep

from compiler import *

import smtplib
from email.mime.text import MIMEText

import configparser

import copy

import traceback

import shutil
import gzip

parser = configparser.ConfigParser()
parser.read("../halite.ini")

RUN_GAME_FILE_NAME = "runGame.sh"
HALITE_EMAIL = parser["email"]["email"]
HALITE_EMAIL_PASSWORD = parser["email"]["password"]
SECRET_FOLDER = parser["hce"]["secretFolder"]

def makePath(path):
    """Deletes anything residing at path, creates path, and chmods the directory"""
    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path)
    os.chmod(path, 0o777)

def sendEmail(subject, body, recipient):
    print("Sending email")

    msg = MIMEText(body, "html")
    msg['Subject'] = subject
    msg['From'] = HALITE_EMAIL
    msg['To'] = recipient

    s = smtplib.SMTP('smtp.gmail.com:587')
    s.ehlo()
    s.starttls();
    s.login(HALITE_EMAIL, HALITE_EMAIL_PASSWORD)
    s.sendmail(HALITE_EMAIL, [recipient], msg.as_string())
    s.quit()


def executeCompileTask(user, backend):
    """Downloads and compiles a bot. Posts the compiled bot files to the manager."""
    print("Compiling a bot with userID %s" % (user["userID"]))

    try:
        workingPath = "workingPath"
        makePath(workingPath)

        botPath = backend.storeBotLocally(int(user["userID"]), workingPath, isCompile=True)
        archive.unpack(botPath)

        while len([name for name in os.listdir(workingPath) if os.path.isfile(os.path.join(workingPath, name))]) == 0 and len(glob.glob(os.path.join(workingPath, "*"))) == 1:
            singleFolder = glob.glob(os.path.join(workingPath, "*"))[0]
            bufferFolder = os.path.join(workingPath, SECRET_FOLDER)
            os.mkdir(bufferFolder)

            for filename in os.listdir(singleFolder):
                shutil.move(os.path.join(singleFolder, filename), os.path.join(bufferFolder, filename))
            os.rmdir(singleFolder)

            for filename in os.listdir(bufferFolder):
                shutil.move(os.path.join(bufferFolder, filename), os.path.join(workingPath, filename))
            os.rmdir(bufferFolder)

        language, errors = compile_anything(workingPath)
        didCompile = True if errors == None else False
    except Exception as e:
        language = "Other"
        errors = ["Your bot caused unexpected behavior in our servers. If you cannot figure out why this happened, please email us at halite@halite.io. We can help.", "For our reference, here is the trace of the error: " + traceback.format_exc()]
        didCompile = False

    if didCompile:
        print("Bot did compile")
        archive.zipFolder(workingPath, os.path.join(workingPath, user["userID"]+".zip"))
        backend.storeBotRemotely(int(user["userID"]), os.path.join(workingPath, user["userID"]+".zip"))
    else:
        print("Bot did not compile")
        print(str(errors))

    backend.compileResult(int(user["userID"]), didCompile, language, errors=(None if didCompile else "\n".join(errors)))
    if os.path.isdir(workingPath):
        shutil.rmtree(workingPath)

def downloadUsers(users):
    for user in users:
        userDir = str(user["userID"])
        if os.path.isdir(userDir):
            shutil.rmtree(userDir)
        os.mkdir(userDir)
        archive.unpack(backend.storeBotLocally(user["userID"], userDir))

def runGame(width, height, users):
    runGameCommand = " ".join([RUN_GAME_FILE_NAME, str(width), str(height), str(len(users))]+[a["userID"] for a in users]+["\""+a["username"]+" v"+a["numSubmissions"]+"\"" for a in users])

    print("Run game command: " + runGameCommand)
    print("Game output:")
    lines =  subprocess.Popen("bash "+runGameCommand, shell=True, stdout=subprocess.PIPE).stdout.read().decode('utf-8').split('\n')
    print("\n".join(lines))
    return lines

def parseGameOutput(output, users):
    users = copy.deepcopy(users)

    replayPath = output[len(output) - (len(users)+3)].split(" ")[0]

    # Get player ranks and scores by parsing shellOutput
    for lineIndex in range(len(output)-(len(users)+2), len(output)-2):
        components = output[lineIndex].split(" ")
        for cIndex in range(len(components)):
            if components[cIndex] == "nan" or components[cIndex] == "-nan":
                components[cIndex] = 0
        playerTag = int(components[0])
        users[playerTag-1]["playerTag"] = playerTag
        users[playerTag-1]["rank"] = int(components[1])

    for user in users:
        user["didTimeout"] = False
        user["errorLogName"] = None

    errorLine = output[len(output)-1]
    errorPaths = []
    if errorLine.isspace() == False:
        errorPaths = errorLine.strip().split(" ")

    timeoutLine = output[len(output)-2]
    if timeoutLine.isspace() == False:
        timeoutTags = [int(a) for a in timeoutLine.strip().split(" ")]
        for index in range(len(timeoutTags)):
            playerTag = timeoutTags[index]
            users[playerTag-1]["didTimeout"] = True
            users[playerTag-1]["errorLogName"] = os.path.basename(errorPaths[index])

    print(errorPaths)

    return users, replayPath, errorPaths

def executeGameTask(width, height, users, backend):
    """Downloads compiled bots, runs a game, and posts the results of the game"""
    print("Running game with width %d, height %d, and users %s" % (width, height, str(users)))

    downloadUsers(users)
    users, replayPath, errorPaths = parseGameOutput(runGame(width, height, users), users)

    replayArchivePath = "ar"+replayPath
    fIn = open(replayPath, 'rb')
    fOut = gzip.open(replayArchivePath, 'wb')
    shutil.copyfileobj(fIn, fOut)
    fIn.close()
    fOut.close()

    backend.gameResult(width, height, users, replayArchivePath, errorPaths)
    filelist = glob.glob("*.log")
    for f in filelist:
        os.remove(f)

    os.remove(replayPath)
    os.remove(replayArchivePath)

if __name__ == "__main__":
    print("Starting up worker...")
    while True:
        try:
            task = backend.getTask()
            if "type" in task and (task["type"] == "compile" or task["type"] == "game"):
                print("Got new task: " + str(task))
                if task["type"] == "compile":
                    executeCompileTask(task["user"], backend)
                else:
                    executeGameTask(int(task["width"]), int(task["height"]), task["users"], backend)
            else:
                print("No task available. Sleeping...")
                sleep(2)
        except Exception as e:
            print("Error on get task:")
            print(e)
            print("Sleeping...")
            sleep(2)

