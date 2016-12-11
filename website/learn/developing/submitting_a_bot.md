## <a name="submitting_a_bot"></a> Submitting A Bot

You submit a `zip` file containing your `MyBot` file and any other files needed to compile and run your `MyBot` file.

### Installing Dependencies

Instead of packaging up your dependencies inside your `zip` file, you may include a bash file named `install.sh` that will be run before your bot is compiled.

It will be run with internet access and write access to only its current directory. It may run for a maximum of 10 minutes and will not be able to make global installation changes (i.e. `apt-get` will not work).

#### Package Managers

The following package managers are already installed on the server and can be used to install dependencies locally:

 - `pip3`
 - `bundler`
 - `npm`

`curl` is also available and can be used to download additional runtimes, tools, and environments.

If your library isn't on a package manager that supports local installation and you can’t download it with `curl`, you are going to have to compile it on our game servers. Include the source of you library in your bot's `zip` file and put compilation instructions in the `install.sh` file.

### Compilation

Bot compilation is done using [this autocompile script](https://github.com/HaliteChallenge/Halite/blob/master/worker/compiler.py). Many languages will be properly autodetected and compiled if needed without the need for an `install.sh` script.

Your main file must be called `MyBot`. Your language is recognized using the file extension of your `MyBot` file. The appropriate file extensions for each language are:

 - Java - `.java`
 - Python - `.py`
 - C++ - `.cpp` and `.h(pp)`
 - C# - `.cs`
 - Rust - `.toml` (for your `Cargo.toml`) and `.rs` (for your Rust source)
 - Scala - `.scala`
 - Ruby - `.rb`
 - Go - `.go`
 - PHP - `.php`
 - JavaScript - `.js`
 - OCaml - `.ml`
 - Clojure - `.clj`
 - C - `.c`

See the [Server Overview](server_overview.php#software) for details about compiler and runtime versions.

#### Customizing your language name

If you are using a language that is generic or that does not have first class support on the server, you can include a file named `LANGUAGE` containing the name of the language you are using. This will be used only for display on the rankings and in your profile.

#### JVM Languages

For JVM languages, you can submit a `jar` file inside of your `zip` file instead of source files. The `jar` will be executed `java -jar MyBot.jar` so you need to [define a Main-Class header in the manifest](https://docs.oracle.com/javase/tutorial/deployment/jar/appman.html).

### Running

You may supply a `run.sh` script to control how your bot is run. Many languages will be properly autodetected and run without the need for an `install.sh` script. You should only include a custom `run.sh` script if you have a real need for one.

#### Custom Runtime

You could use a `run.sh` file to use a custom runtime such as PyPy instead of the default Python 3.

### Understanding Game Logs

When your bot times out or errors on our game servers, we save and display a log file with debugging information including the time your bot took each turn, its output each turn, and its final output from stdout and stderr.

To find these log files, visit your [homepage](user.php). Just click the download log button to grab your error log for a game.

