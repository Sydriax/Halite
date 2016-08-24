import unittest
import sys
import os
import glob
import zipfile
import shutil

WORKER_PATH = os.path.join(os.getcwd(), '..', '..', 'worker')
OUR_PATH = os.getcwd()

sys.path.append(WORKER_PATH)
os.chdir(WORKER_PATH)
print(os.listdir(os.getcwd()))
import compiler
import worker
os.chdir(OUR_PATH)

class CompilerTests(unittest.TestCase):
    def testStarterPackages(self):
        '''Archive the starter packages like the website would'''
        SP_DIR = "starterpackages"
        if os.path.isdir(SP_DIR):
            shutil.rmtree(SP_DIR)
        os.system("cd ../../website/; ./archiveStarterPackages.sh");
        shutil.copytree("../../website/downloads/starterpackages/", SP_DIR)
        for f in glob.glob(os.path.join(SP_DIR, "*.zip")):
            zip_ref = zipfile.ZipFile(f, 'r')
            zip_ref.extractall(SP_DIR)
            zip_ref.close()

            folderName = os.path.splitext(os.path.basename(f))[0]
            folder = os.path.join(SP_DIR, os.path.splitext(os.path.basename(f))[0])
            expectedLanguage = folderName.split("-")[1]
            language, errors = compiler.compile_anything(folder)
            print(errors)
            print(language)

            assert language == expectedLanguage
            assert errors == None

class GameTests(unittest.TestCase):
	def testNormalGame(self):
		'''Test the parsing of the output of runGame.sh'''
		WIN_BOT_PATH = "winBot"
		LOSE_BOT_PATH = "loseBot"

		shutil.copytree(os.path.join(OUR_PATH, WIN_BOT_PATH), os.path.join(WORKER_PATH, WIN_BOT_PATH))
		shutil.copytree(os.path.join(OUR_PATH, LOSE_BOT_PATH), os.path.join(WORKER_PATH, LOSE_BOT_PATH))
		os.chdir(WORKER_PATH)
		output = worker.runGame(20, 20, [{"userID": WIN_BOT_PATH, "username": WIN_BOT_PATH, "numSubmissions": "1"}, {"userID": LOSE_BOT_PATH, "username": LOSE_BOT_PATH, "numSubmissions": "1"}])
		os.chdir(OUR_PATH)

		assert len(output) == 5
		assert int(output[1].split(" ")[0]) == 1
		assert int(output[2].split(" ")[0]) == 2

if __name__ == '__main__':
    unittest.main()
