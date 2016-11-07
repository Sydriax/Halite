<html lang="en">
<head>
    <?php include 'includes/header.php'; ?>

    <title>Downloads</title>

    <link href="lib/bootstrap.min.css" rel="stylesheet">
    <link href="style/general.css" rel="stylesheet">
</head>
<body>
    <div class="container">
        <?php include 'includes/navbar.php'; ?>
        <div class="row">
            <div class="col-sm-12">
                <h1>Downloads</h1>

                <h2>Starter Packages</h2>
                <p>
                    This set of starter packages was uploaded on November 6nd. We fixed a minor bug with the getDistance() method in the Python starter package.  These packages also contain the Game Environment.
                </p>
                <p>
                    <ul>
                        <li><a href="downloads/starterpackages/Halite-Python-Starter-Package.zip">Python 3</a></li>
                        <li><a href="downloads/starterpackages/Halite-Java-Starter-Package.zip">Java 7</a></li>
                        <li><a href="downloads/starterpackages/Halite-C++-Starter-Package.zip">C++ 11</a></li>
                        <li><a href="downloads/starterpackages/Halite-C%23-Starter-Package.zip">C# 6.0</a></li>
                        <li><a href="downloads/starterpackages/Halite-Rust-Starter-Package.zip">Rust 1.10</a></li>
                        <li><a href="downloads/starterpackages/Halite-Scala-Starter-Package.zip">Scala 2.10.4</a></li>
                    </ul>
                </p>

                <h2>Running the Environment</h2>
                <h3>Linux/macOS</h3>
                <p>After unpacking the Started Package, open your terminal and cd into the unzipped directory.  Run the install.sh script to download and install the environment.  Then you can execute the environment with ./bin/halite </p>
                <pre><code>cd Halite-Python-Starter-Package
                ./install.sh
                ./bin/halite -[some example arguments]</code></pre>

                <h3>Windows</h3>
        I'm not sure how to run it on windows?!
                

                <h2>Game Environment</h2>
                <p>If you'd rather download the Game Environment separately you can.</p>
                <p>The environment is on version <b>1.0</b>. This version of the environment was posted on November 2nd.</p>

                <h3>Linux/macOS</h3>
                <p>Execute:</p>
                <pre><code>sudo sh -c "$(curl -fsSL https://raw.githubusercontent.com/HaliteChallenge/Halite/master/environment/install.sh)"</code></pre>
                <p>Now, the binary will be installed into ./bin/ and <code>./bin/halite</code> should work. </p>

                <h3>Windows</h3>
                <p>Download <a href="downloads/environment/halite.exe">halite.exe</a>.</p>
                
            </div>
        </div>
    </div>

    <script src="https://ajax.googleapis.com/ajax/libs/jquery/2.2.4/jquery.min.js"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js"></script>
    <script src="script/backend.js"></script>
    <script src="script/general.js"></script>
</body>
</html>
