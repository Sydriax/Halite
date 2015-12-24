<?php
	function isDirEmpty($dir) {
		if (!is_readable($dir)) return NULL;

		$handle = opendir($dir);
		while (false !== ($entry = readdir($handle))) {
			if ($entry != "." && $entry != "..") {
				return FALSE;
			}
		}
		return TRUE;
	}

	if(!isset($_GET['userID'])) {
		echo "Incorrect HTTP parameters.";
		exit(0);
	}

	$userID = $_GET['userID'];
	$dir = "../../storage/bots/$userID/";

	if(!file_exists($dir) || isDirEmpty($dir)) {
		echo "Connot compile this bot";
		exit(0);
	}

	exec("python compiler.py ../../storage/bots/$userID");
?>