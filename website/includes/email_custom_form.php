<script type="text/javascript">
    var customEmailForm = {
        init: function(submitCallback) {
            this.cacheDOM();

            this.submitCallback = submitCallback;
            this.$submitButton.click(this, this.onClick.bind(this));         
            this.$firstField.keypress(this, this.keypress.bind(this));         
            this.$secondField.keypress(this, this.keypress.bind(this));         
        },
        cacheDOM: function() {
            this.$firstField = $("#firstField");
            this.$secondField = $("#secondField");
            this.$messageBox = $("#messageBox");
            this.$submitButton = $("#customSubmitButton");
        },
        render: function() {
            this.$emailLoc.html(this.email);
        },
        onClick: function() {
            if(this.$firstField.val() != this.$secondField.val()) {
                messageBox.alert("Email Mismatch", "The two emails that you entered do not match.", false);
            } else if(this.$firstField.val() == "" || this.$secondField.val() == "") {
                messageBox.alert("Empty Fields", "Please fill your email twice in the boxes below.", false);
            } else {
                messageBox.alert("Success", "We've sent a verification email to "+this.$firstField.val()+".", true);
                this.submitCallback(this.$firstField.val());
            }
        },
        keypress: function(e) {
            if(e.which == 13) this.onClick();
        }
    }
</script>
<input class="form-control" type="email" placeholder="Email" style="margin-top: 20px; max-width: 400px;" id="firstField">
<input class="form-control" type="email" placeholder="Confirm Email" style="margin-bottom: 10.5px; max-width: 400px;" id="secondField">
<button id="customSubmitButton" class="btn btn-primary">Submit</button>
