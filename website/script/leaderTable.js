snooze.guards['sortLeaderboard'] = function(data) {
    data.forEach(function(user) {
        user.score = (user.mu-(3*user.sigma)).toFixed(2);
    }); return data.sort((a, b) => a.rank-b.rank);
}
