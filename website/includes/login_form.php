<script src="https://apis.google.com/js/platform.js" async defer></script>
<div id="fb-root"></div>
<script>(function(d, s, id) {
  var js, fjs = d.getElementsByTagName(s)[0];
  if (d.getElementById(id)) return;
  js = d.createElement(s); js.id = id;
  js.src = "//connect.facebook.net/en_US/sdk.js#xfbml=1&version=v2.7&appId=1500568723534820";
  fjs.parentNode.insertBefore(js, fjs);
}(document, 'script', 'facebook-jssdk'));</script>
<div id="login_form">
	<div class="g-signin2" data-onsuccess="googleLogin"></div>
	<fb:login-button scope="public_profile,email" onlogin="facebookLogin();">
	</fb:login-button>
</div>
