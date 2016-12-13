<style>
    /* sidebar */
    .bs-docs-sidebar {
        padding-left: 20px;
        margin-bottom: 20px;
    }

    .bs-docs-sidebar .nav>li>span {
        color: #63ceca;
    }

    .bs-docs-sidebar .nav>li>span,
    .bs-docs-sidebar .nav>li>a { border-left: 2px solid transparent;
        padding: 4px 20px;
        font-size: 18px;
        font-weight: 400;
    }

    /* nested links */
    .bs-docs-sidebar .nav .nav>li>a {
        padding-top: 1px;
        padding-bottom: 1px;
        padding-left: 30px;
        font-size: 12px;
    }

    /* active & hover links */
    .bs-docs-sidebar .nav>.active>a,
    .bs-docs-sidebar .nav>li>a:hover,
    .bs-docs-sidebar .nav>li>a:focus {
        text-decoration: none;
        background-color: transparent;
        border-left-color: #63ceca;
    }

    /* nested active links */
    .bs-docs-sidebar .nav .nav>.active>a,
    .bs-docs-sidebar .nav .nav>.active:hover>a,
    .bs-docs-sidebar .nav .nav>.active:focus>a {
        font-weight: 700;
        border-left-color: #63ceca;
        font-weight: 500;
    }

</style>

<nav class="col-sm-3 bs-docs-sidebar">
    <ul id="sidebar" class="nav nav-stacked">
        <li id="quickstart">
            <a href="quickstart.php">Quickstart</a>
            <ul class="nav nav-stacked">
                <li id="quickstart_dive_in">
                    <a href="quickstart.php#dive_in">Dive In</a>
                </li>
            </ul>
        </li>
        <li id="game_overview">
            <a href="game_overview.php">Game Overview</a>
            <ul class="nav nav-stacked">
                <li id="game_overview_what_is_halite">
                    <a href="game_overview.php#what_is_halite">What is Halite?</a>
                </li>
                <li id="game_overview_game_rules">
                    <a href="game_overview.php#game_rules">Game Rules</a>
                </li>
            </ul>
        </li>
        <li id="bot_overview">
            <a href="bot_overview.php">Bot Overview</a>
            <ul class="nav nav-stacked">
                <li id="bot_overview_a_bots_life">
                    <a href="bot_overview.php#a_bots_life">A Bot's Life</a>
                </li>
            </ul>
        </li>
        <li id="developing_a_bot">
            <a href="developing_a_bot.php">Developing A Bot</a>
            <ul class="nav nav-stacked">
                <li id="developing_a_bot_local_development_environment">
                    <a href="developing_a_bot.php#local_development_environment">Local Development Environment</a>
                </li>
                <li id="developing_a_bot_submitting_a_bot">
                    <a href="developing_a_bot.php#submitting_a_bot">Submitting A Bot</a>
                </li>
            </ul>
        </li>
        <li id="tournament_overview">
            <a href="tournament_overview.php">Tournament Overview</a>
            <ul class="nav nav-stacked">
                <li id="tournament_overview_about_the_tournament">
                    <a href="tournament_overview.php#about_the_tournament">About the Tournament</a>
                </li>
            </ul>
        </li>
        <li id="server_overview">
            <a href="server_overview.php">Server Overview</a>
            <ul class="nav nav-stacked">
                <li id="server_overview_hardware">
                    <a href="server_overview.php#hardware">Hardware</a>
                </li>
                <li id="server_overview_software">
                    <a href="server_overview.php#software">Software</a>
                </li>
            </ul>
        </li>
        <li id="tutorials">
            <a href="tutorials.php">Tutorials</a>
            <ul class="nav nav-stacked">
                <li id="tutorials_improving_the_random_bot">
                    <a href="tutorials.php#improving_the_random_bot">Improving The Random Bot</a>
                </li>
            </ul>
        </li>
        <li id="reference">
            <span>Reference</span>
            <ul class="nav nav-stacked">
                <li id="advanced_replay_file">
                    <a href="advanced_replay_file.php">Replay File Format</a>
                </li>
                <li id="advanced_writing_sp">
                    <a href="advanced_writing_sp.php">Writing Your Own Starter Package</a>
                </li>
            </ul>
        </li>
        <li id="faqs">
            <a href="faqs.php">FAQs</a>
        </li>
    </ul>
    <hr>
    <p style="line-height: 1.5em; font-size: 13px;">You can <a id="githubLink" href="">edit this content on GitHub</a> and send us a pull request!</p>
</nav>

<script>
    var fileName = location.pathname.substring(location.pathname.lastIndexOf("/") + 1);
    document.getElementById("githubLink").href = "https://github.com/HaliteChallenge/Halite/blob/master/website/"+fileName;

    function markNavActive(event) {
        var name = fileName.split(".")[0];

        if (event) {
            var l = document.createElement("a");
            l.href = event.oldURL;
            if (l.hash) {
                document.getElementById(name + "_" + l.hash.substr(1)).removeAttribute("class")
            } else {
                document.getElementById(name).removeAttribute("class")
            }
        }

        if (location.hash) {
            document.getElementById(name).removeAttribute("class")
            document.getElementById(name + "_" + location.hash.substr(1)).className = "active"
        } else {
            document.getElementById(name).className = "active"
        }
    }

    markNavActive();

    window.addEventListener("hashchange", markNavActive, false);
</script>
