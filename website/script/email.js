$(function() {
    function render() {
        if(parseInt(user.isEmailGood) == 0) {
            if(user.email != null && user.email != undefined) { // Waiting for verification
                $("#wait").css("display", "block");

                $("#change").css("display", "none");
                $("#verify").css("display", "none");
            } else { // Need to pick an email
                $("#verify").css("display", "block");

                $("#change").css("display", "none");
                $("#wait").css("display", "none");
            }
        } else { // Email change
                $("#change").css("display", "block");

                $("#wait").css("display", "none");
                $("#verify").css("display", "none");
        }
    }

    var session = getSession(); session = {"userID": 4431};
    if(session == null) window.location.href = "index.php";
    var user = getUser(session['userID']);
    if(user == null) window.location.href = "index.php";

    render(user);

    customEmailForm.init(function(email) {
        newEmail(email);

        user.email = email;
        render(user);
    });
    githubSubmitButton.init(user.githubEmail, function() {
        validateEmail();
        window.location.href = "index.php";
    });
});
