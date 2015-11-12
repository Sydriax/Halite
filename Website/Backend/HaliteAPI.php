<?php

require_once 'API.class.php';
class HaliteAPI extends API
{

	// The database
	private $mysqli = NULL;

	public function __construct($request, $origin) {
		$this->initDB();
		parent::__construct($request);
	}

	// Initializes and returns a mysqli object that represents our mysql database
	private function initDB() {
		$this->mysqli = new mysqli("Halite.db.12061709.hostedresource.com", 
			"Halite", 
			"Fustercluck2!", 
			"Halite");
		
		if (mysqli_connect_errno()) { 
			echo "<br><br>There seems to be a problem with our database. Reload the page or try again later.";
			exit(); 
		}
	}

	private function select($sql) {
		$res = mysqli_query($this->mysqli, $sql);
		return mysqli_fetch_array($res, MYSQLI_ASSOC);
	}

	private function insert($sql) {
		echo $sql;
		mysqli_query($this->mysqli, $sql);
	}

	// API ENDPOINTS
	// Endpoint associated with a users credentials (everything in the User table; i.e. name, email, firstname, etc.)
	protected function user() {
		if($this->method == 'GET') {
			if(isset($_GET["username"]) && isset($_GET["password"])) {
				$username = $_GET["username"];
				$password = $_GET["password"];

				return $this->select("SELECT * FROM User WHERE username = '$username' AND password = '$password'");
			}
			if(isset($_GET["userID"]) && isset($_GET["password"])) {
				$userID = $_GET["userID"];
				$password = $_GET["password"];
				return $this->select("SELECT * FROM User WHERE userID = $userID AND password = '$password'");
			}
		}

		if($this->method == 'POST') {
			if(isset($_POST["username"]) && isset($_POST["password"])) {
				$username = $_POST["username"];
				$password = $_POST["password"];

				$this->insert("INSERT INTO User (username, password) VALUES ('$username', '$password')");
				return "success";
			}
		}

		return "ERROR";
	}

	protected function session() {
		session_start();
		if($this->method == 'GET') {
			return $_SESSION;
		} else if($this->method == 'POST' && (isset($_POST['username']) || isset($_POST['password']))) {
			if(isset($_POST['username'])) $_SESSION['username'] = $_POST['username'];
			if(isset($_POST['password'])) $_SESSION['password'] = $_POST['password'];
		} else if($this->method == 'DELETE') {
			session_destroy();
		} else {
			return "error";
		}

		return "success";
	}
 }

 ?>