<?php

require_once '../API.class.php';
class ManagerAPI extends API
{

	// The database
	private $mysqli = NULL;

	public function __construct($request, $origin) {
		$this->initDB();

		$this->sanitizeHTTPParameters();

		if($this->isValidWorker() == false) {
			exit(1);
		}

		parent::__construct($request);
	}

	private function isValidWorker() {
		// Get apiKey
		$apiKey = NULL;
		if(isset($_GET['apiKey'])) $apiKey = $_GET['apiKey'];
		else if(isset($_POST['apiKey'])) $apiKey = $_POST['apiKey'];
		else if(isset($_PUT['apiKey'])) $apiKey = $_PUT['apiKey'];
		else if(isset($_DELETE['apiKey'])) $apiKey = $_DELETE['apiKey'];

		if($apiKey == NULL) return false;

		// Get ip
		$ipAddress = $_SERVER['REMOTE_ADDR'];
		if(count($this->select("SELECT ipAddress FROM Worker WHERE ipAddress = '$ipAddress' and apiKey = $apiKey")) > 0) return true;
		else return false;
	}

	private function sanitizeHTTPParameters() {
		foreach ($_GET as $key => $value) {
			$_GET[$key] = $this->mysqli->real_escape_string($value);
		}
		foreach ($_POST as $key => $value) {
			$_POST[$key] = $this->mysqli->real_escape_string($value);
		}
	}

	private function botDirectory($userID) {
		return "../../../storage/bots/$userID";
	}

	private function botFile($userID) {
		$botDirectory = $this->botDirectory($userID);
		$file = NULL;
		
		if (file_exists("{$botDirectory}/bot.zip")) $file = "{$botDirectory}/bot.zip";
		else if (file_exists("{$botDirectory}/bot.tgz")) $file = "{$botDirectory}/bot.tgz";
		else if (file_exists("{$botDirectory}/bot.tar.gz")) $file = "{$botDirectory}/bot.tar.gz";
		else if (file_exists("{$botDirectory}/bot.tar.xz")) $file = "{$botDirectory}/bot.tar.xz";
		else if (file_exists("{$botDirectory}/bot.txz")) $file = "{$botDirectory}/bot.txz";
		else if (file_exists("{$botDirectory}/bot.tar.bz2")) $file = "{$botDirectory}/bot.tar.bz2";
		else if (file_exists("{$botDirectory}/bot.tbz")) $file = "{$botDirectory}/bot.tbz";

		return $file;
	}

	// Initializes and returns a mysqli object that represents our mysql database
	private function initDB() {
		$config = include("../config.php");
		$this->mysqli = new mysqli($config['hostname'], 
			$config['username'], 
			$config['password'], 
			$config['databaseName']);
		
		if (mysqli_connect_errno()) { 
			echo "<br><br>There seems to be a problem with our database. Reload the page or try again later.";
			exit(); 
		}
	}

	private function select($sql) {
		try {
			$res = mysqli_query($this->mysqli, $sql);
			$array = mysqli_fetch_array($res, MYSQLI_ASSOC);
			return $array;
		} catch(Exception $e) {
			return array();
		}
	}

	private function selectMultiple($sql) {
		$res = mysqli_query($this->mysqli, $sql);
		$finalArray = array();

		while($temp = mysqli_fetch_array($res, MYSQLI_ASSOC)) {
			array_push($finalArray, $temp);
		}

		return $finalArray;
	}

	private function insert($sql) {
		mysqli_query($this->mysqli, $sql);
	}

	// API ENDPOINTS
	protected function task() {
		if($this->method == 'GET') {
			// Check for compile tasks
			$needToBeCompiled = $this->select("SELECT userID FROM User WHERE status = 0 ORDER BY userID ASC");
			if(count($needToBeCompiled > 0)) {
				return array(
					"type" => "compile",
					"userID" => $needToBeCompiled['userID']);
			}

			// TODO: Run a game

		}
		return NULL;
	}

	protected function bot() {
		if(isset($_GET['userID'])) {
			$userID = $_GET['userID'];
			$botDirectory = $this->botDirectory($userID);

			if (file_exists("{$botDirectory}/bot.zip")) {
				header("Content-disposition: attachment; filename=bot.zip");
				header("Content-type: application/zip");
			} else if (file_exists("{$botDirectory}/bot.tgz")) {
				header("Content-disposition: attachment; filename=bot.tgz");
				header("Content-type: application/x-gtar");
			} else if (file_exists("{$botDirectory}/bot.tar.gz")) {
				header("Content-disposition: attachment; filename=bot.tgz");
				header("Content-type: application/x-gtar");
			} else if (file_exists("{$botDirectory}/bot.tar.xz")) {
				header("Content-disposition: attachment; filename=bot.txz");
				header("Content-type: application/x-gtar");
			} else if (file_exists("{$botDirectory}/bot.txz")) {
				header("Content-disposition: attachment; filename=bot.txz");
				header("Content-type: application/x-gtar");
			} else if (file_exists("{$botDirectory}/bot.tar.bz2")) {
				header("Content-disposition: attachment; filename=bot.tbz");
				header("Content-type: application/x-gtar");
			} else if (file_exists("{$botDirectory}/bot.tbz")) {
				header("Content-disposition: attachment; filename=bot.tbz");
				header("Content-type: application/x-gtar");
			} else {
				header("HTTP/1.0 404 Not Found");
				die();
			}

			ob_clean();
			flush();
			readfile($this->botFile($userID));
			exit;
		} else {
			return NULL;
		}

		return "Success";
	}

	protected function botHash() {
		if(isset($_GET['userID'])) {
			$userID = $_GET['userID'];
			return array("hash" => md5_file($this->botFile($userID)));
		}
	}
 }

 ?>