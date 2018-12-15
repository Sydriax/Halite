<script type="text/javascript">
    var githubSubmitButton = {
        init: function(email, onSubmit) {
            email = "truell20@gmail.com";
            this.cacheDOM();

            this.onSubmit = onSubmit;

            this.$emailField.html(email);
            console.log(this.$emailField);
            this.$button.click(this, this.onPress.bind(this));         
        },
        cacheDOM: function() {
            this.$emailField = $("#githubEmail");
            this.$button = $("#githubEmailButton");
        },
        onPress: function() {
            console.log("press")
            messageBox.alert("Success", "You're all set!.", true);
            this.onSubmit();
        }
    }
</script>
<button class="btn btn-primary" id="githubEmailButton"><b>Use your github's email (<span id="githubEmail"></span>) without needing to verify</b></button>
