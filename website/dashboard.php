<!DOCTYPE html>
<html>

<head>
	<title>Halite4</title>
	<link rel="stylesheet" type="text/css" href="style/index.css">
	<script src="lib/jquery.min.js"></script>
	<script src="script/backend.js"></script>
	<script src="script/app.js"></script>

	<script type="text/javascript">
		var user = getSession();
		if(user == null) {
			goToHomepage();
		}

		console.log(user)

		$(document).ready(function() {
			// Set the welcome text to include the user's name
			$("#welcome").text("Welcome " + user.username + "!");

			
			$("#signOut").click(function() {
				destroySession(false);
				goToHomepage();
			});

			$("#uploadBot").click(function() {
				$("#uploadBotForm").find("input[name='userID']").val(user.userID+"")
				storeBotFiles("uploadBotForm");
				$("#welcome").text("Uploaded!");
			})
		});
	</script>
</head>

<body>
	<h1>Halite</h1>
	
	<p id="welcome">Welcome!</p>
	<a href="#" id="signOut">Sign out</a>
	<hr>

	<h3>Rankings</h3>
	<p>You rank __ out of __. <a href="#">Full Rankings</a></p>

	<h3>Update Bot</h3>
	<form enctype="multipart/form-data" id="uploadBotForm">
   		Bot Code:<br>
   		<input type="hidden" name="userID" value="">
    	<input name="files[]" type="file" multiple="multiple"><br>
    	<input type="button" value="Update Bot" id="uploadBot">
	</form>

</body>

</html>
