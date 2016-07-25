// Google analytics
(function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
(i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
})(window,document,'script','https://www.google-analytics.com/analytics.js','ga');

ga('create', 'UA-80656237-1', 'auto');
ga('send', 'pageview');

function getGET(name) {
	name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
	var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
	results = regex.exec(location.search);
	return results === null ? null : decodeURIComponent(results[1].replace(/\+/g, " "));
}

// The message box should be accessible to all pages
var messageBox = {
	$messageBox: $("#messageBox"),
	alert: function(title, message, isSuccess, onClose) {
		this.clear()
		this.$messageBox.append($("<div class='alert "+(isSuccess ? "alert-success" : "alert-danger")+" alert-dismissible' role='alert'><button type='button' class='close' id='messageCloseButton' data-dismiss='alert' aria-label='Close'><span aria-hidden='true'>&times;</span></button><strong>"+title+"</strong>&nbsp;&nbsp;"+message+"</div>"))
		if(onClose != null) $("#messageCloseButton").click(onClose);
	},
	clear: function() {
		this.$messageBox.empty()
	}
};

function SmartForm($submitButton, $form, onSubmit) {
	$submitButton.click(function() {
		console.log("CLICK");
		onSubmit();
	});
	$form.keypress(function(event) {
		if (event.which == 13) {
			event.preventDefault();
			onSubmit();
		}
	});
};

$(function() {
	var navbar = {
		loggedIn: false,
		$logInUsername: $("#login_user"),
		$logInPassword: $("#login_pass"),
		$logInButton: $("#login_button"),
		$logInForm: $("#login_form"),
		$registerUsername: $("#register_user"),
		$registerEmail: $("#register_email"),
		$registerPassword: $("#register_pass"),
		$registerConfirmPassword: $("#register_confirm_pass"),
		$registerButton: $("#register_button"),
		$registerForm: $("#register_form"),
		$logInNav: $("#loginNav"),
		$logOutNav: $("#logoutNav"),
		$logOutButton: $("#logoutButton"),

		uploadButton: {
			$button: $("#submitButton"),
			$form: $("#submitForm"),
			$fileInput: $("#myFile"),
			init: function() {
				this.$button.click(this, this.buttonClicked.bind(this));
				this.$fileInput.change(this, this.fileChanged.bind(this));
			},
			setCredentials: function(userID, password) {
				this.$form.append("<input type='hidden' name='userID' value='"+userID+"'>");
				this.$form.append("<input type='hidden' name='password' value='"+password+"'>");
			},
			buttonClicked: function() { this.$fileInput.click(); },
			fileChanged: function() {
				$.get("http://ipinfo.io", function(response) {
					if(response.ip == "208.77.212.129") {
						messageBox.alert("Restricted Access", "You are not allowed to submit code to Halite from a Two Sigma Desktop. Please use a personal computer on Two Sigma Wifi instead.", false);
					} else {
						var uploadOutput = storeBotFile("submitForm");
						if(uploadOutput.indexOf("error") != -1) {
							messageBox.alert("File Upload Error", "An error occurred while uploading your file. <b>Your file may have been too big</b>. Check to make sure that your file is under <b>5 megabytes</b>. Make sure that you haven't packaged some unnecessary, big binaries. If this persists, post of the forums or email us at halite@halite.io.", false);
						} else if(uploadOutput.indexOf("desktop") != -1) {
							messageBox.alert("Restricted Access", "You are not allowed to submit code to Halite from a Two Sigma Desktop. Please use a personal computer on Two Sigma Wifi instead.", false);
						} else if(uploadOutput.indexOf("large") != -1) {
							messageBox.alert("File Size Error", "Your bot file was too big. We only allow submissions less than 5 megabytes. Make sure you aren't packaging unnecessary binaries.", false);
						} else {
							messageBox.alert("Bot Submitted", "Your bot was successfully uploaded to our servers. <b>If your bot does not compile, you will receive an email in a couple of minutes.</b> Otherwise, you will show up on the leaderboard very soon.", true)
						}
					}
				}, "jsonp");
			}
		},

		init: function(session) {
			new SmartForm(this.$logInButton, this.$logInForm, this.logIn.bind(this));
			new SmartForm(this.$registerButton, this.$registerForm, this.register.bind(this));

			this.uploadButton.init();
			this.$logOutButton.click(this.logOut.bind(this));

			if(session != null && session.userID != null) {
				this.user = session;
				this.loggedIn = true;
			}

			this.render();
		},
		logIn: function() {
			messageBox.clear();

			var user = getUser(null, this.$logInUsername.val(), this.$logInPassword.val());
			if(user == null) {
				messageBox.alert("Login failed", "That username/password combo does not exist", false);
			} else if(user['isVerified'] == false) {
				messageBox.alert("Login failed", "Your email needs to be verified", false);
			} else {
				messageBox.alert("Login successful", "You are now logged in as <b>"+user.username+"</b>.", true);
				storeUserSession(this.$logInUsername.val(), this.$logInPassword.val(), false);
				this.loggedIn = true;
				this.user = user;
				this.render();
			}
		},
		register: function() {
			messageBox.clear();

			var username = this.$registerUsername.val();
			var email = this.$registerEmail.val();
			var password = this.$registerPassword.val();
			var confirmPassword = this.$registerConfirmPassword.val();

			if(password != confirmPassword) {
				messageBox.alert("Passwords Don't Match", "Please type in your password correctly.", false);
			} else {
				var resp = storeUserDatabase(email, username, password, false);
				console.log(resp)
				if (resp === "Success") {
					messageBox.alert("Verify Your Email", "You may not log in until you verify your email.", true);
					storeUserSession(username, password, false);
				} else  {
					if(resp.toLowerCase().indexOf("username") > -1) {
						messageBox.alert("Registration failed", "That username is already taken", false);
					} else if (resp.toLowerCase().indexOf("exists") > -1) {
						messageBox.alert("Registration failed", "That email is already taken", false);
					} else if (resp.toLowerCase().indexOf("sigma") > -1) {
						messageBox.alert("Registration failed", "That email does not end in twosigma.com", false);
					} else {
						messageBox.alert("Registration failed", "That email is invalid", false);
					}
				}
			}
		},
		logOut: function() {
			destroySession(false);
			this.loggedIn = false;
			this.render();
		},
		render: function() {
			if(this.loggedIn) {
				this.$logInNav.css("display", "none");
				this.$logOutNav.css("display", "inline");

				this.uploadButton.setCredentials(this.user.userID, this.user.password);
			} else {
				this.$logInNav.css("display", "inline");
				this.$logOutNav.css("display", "none");
			}
		}
	}

	var doLogOff = getGET("forumsLogOut");
	if(doLogOff != null && doLogOff != undefined) {
		messageBox.alert("Logged Out", "You have been logged out of forums.halite.io and halite.io", true);
		destroySession(false);
	}

	var session = getSession();
	navbar.init(session);
	if(session != null) {
		var announcement = getLatestAnnouncement(session.userID);
		console.log(announcement)
		if(announcement != null) {
			messageBox.alert(announcement.header, announcement.body, true, function() {
				closedAnnouncement(announcement.announcementID, session.userID, session.password);	
			});
		}
	}
})
