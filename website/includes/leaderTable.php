<script type="text/snooze" data-pipe="http://halite.io/website/php/user?active=1" data-guard="sortLeaderboard">
    <table class="table well well-sm" id="leaderTable">
    <thead>
        <tr>
            <th>#</th>
            <th>Username</th>
            <th>Language</th>
            <th>Organization</th>
            <th>Submissions</th>
            <th>Games Played</th>
            <th>Points</th>
        </tr>
    </thead>
    <~ for  (var i in this.data) { ~>
        <tbody id='user<~ this.data[i].userID ~>'>
            <tr>
                <th scope='row'><~ this.data[i].rank ~></th>
                <td><a href='this.data[i].php?userID=<~ this.data[i].userID ~>'><~ this.data[i].username ~></a></td>
                <td><a href='leaderboard.php?field=language&value=<~ this.data[i].language ~>&heading=<~ this.data[i].language ~>'><~ this.data[i].language ~></a></td>
                <td><a href='leaderboard.php?field=organization&value=<~ this.data[i].organization ~>&heading=<~ this.data[i].organization ~>'><~ this.data[i].organization ~></a></td>
                <td><~ this.data[i].numSubmissions ~></td>
                <td><~ this.data[i].numGames ~></td>
                <td><~ this.data[i].score ~></td>
            </tr>
        </tbody>
    <~ } ~>
    </table>
</script>
