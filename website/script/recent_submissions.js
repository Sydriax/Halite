$(function() {
    var submissionTable = {
        $alternateMessage: $("#noBotsMessage"),
        $panel: $("#submissionPanel"),
        $table: $("#submissionTable"),
        $tableBody: $("#submissionTableBody"),
        lastSubmissionTime: null,
        init: function() {
            var submissions = getRecentSubmissions();
            submissions = this.prepSubmissions(submissions);
            this.lastSubmissionTime = submissions[0].compileTime;

            var schedCutoff = submissions.length > 1 ? 1 : 0;
            var lastTime = submissions[0].date.valueOf();
            while (schedCutoff < 15 &&
                schedCutoff < submissions.length - 2 &&
                lastTime - submissions[schedCutoff].date.valueOf() < 60000) {
                schedCutoff += 1;
            }

            this.render(submissions.slice(schedCutoff));
            var delay = 45000;
            if(schedCutoff > 0) {
                delay = this.scheduleSubmissions(submissions.slice(0, schedCutoff));
            }
            window.setTimeout(this.loadMore.bind(this), delay);
        },
        prepSubmissions: function(submissions) {
            submissions.forEach(function(sub) {
                if(sub.compileTime) {
                    var dateComponents = sub.compileTime.split(/[- :]/);
                    sub.date = new Date(Date.UTC(dateComponents[0], dateComponents[1]-1, dateComponents[2], dateComponents[3], dateComponents[4], dateComponents[5]));
                } else {
                    sub.date = new Date(0);
                }
                if(sub.numSubmissions > 1) {
                    var history = getHistories(sub.userID);
                    if(history.length > 0) {
                        sub.lastRank = history[0].lastRank;
                    } else {
                        // This should only occur if there was a server error
                        // during the compilation posting.
                        sub.lastRank = 'Missing';
                    }
                } else {
                    sub.lastRank = 'None';
                }
            });
            return submissions;
        },
        render: function(submissions) {
            if(submissions.length == 0) {
                this.$alternateMessage.css("display", "block");
                this.$panel.css("display", "none");
            } else {
                this.$tableBody.empty();
                for(var a = 0; a < submissions.length; a++) {
                    this.$tableBody.append(this.getTableRow(submissions[a]));
                }
            }
        },
        getTableRow: function(user) {
            var $row = $("<tr><td>"+user.date.toLocaleTimeString()+"</td><td><a href='user.php?userID="+user.userID+"'><img src='https://avatars1.githubusercontent.com/u/"+user.oauthID+"?s=20' style='border-radius: 2px; width: 20px; height: 20px; margin: 2px'>"+user.username+"</a></td><td>"+user.lastRank+"</td><td>"+user.language+"</td><td>"+user.numSubmissions+"</td></tr>");
            return $row;
        },
        insertSubmission: function(submission) {
            var $newrow = this.getTableRow(submission);
            $newrow.children('td').css({"padding": "0"}).wrapInner('<div>');
            $newrow.find("td > div").css({"padding": "8px"}).hide();
            $newrow.prependTo(this.$tableBody)
            $newrow.find("td > div").slideDown(800);

            // Check for and remove submissions if there are more than 50
            var $subrows = this.$tableBody.children("tr");
            for (i = $subrows.length-1; i > 50; i--) {
                $subrows[i].remove();
            }
        },
        scheduleSubmissions: function(submissions) {
            var last = submissions[submissions.length-1].date;
            var nextSlot = 0;
            var timeMultiple = 1;
            var span = submissions[0].date.valueOf() - last.valueOf();
            if(span < 55000) {
                timeMultiple = 55000 / Math.max(span, 1);
            }
            for (i=submissions.length-1; i>=0; i--) {
                var iSFunc = this.insertSubmission.bind(this);
                function iSFactory(submission) {
                    return function() {
                        iSFunc(submission);
                    }
                }
                var callback = iSFactory(submissions[i]);
                var delay = (submissions[i].date.valueOf() - last.valueOf()) * timeMultiple;
                delay = Math.max(delay, nextSlot);
                nextSlot = delay + 2000;
                window.setTimeout(callback, delay);
            }
            console.log("Scheduled "+submissions.length+" subs for display in "+delay/1000);
            return delay;
        },
        loadMore: function() {
            var newsubs = getRecentSubmissions(this.lastSubmissionTime);
            if(newsubs.length == 0) {
                console.log("No more submissions available.");
                window.setTimeout(this.loadMore.bind(this), 60000);
                return
            }
            newsubs = this.prepSubmissions(newsubs);
            var nextGet = this.scheduleSubmissions(newsubs);
            nextGet = Math.max(nextGet, 60000);
            this.lastSubmissionTime = newsubs[0].compileTime;

            console.log("Next get in "+nextGet/1000);
            window.setTimeout(this.loadMore.bind(this), nextGet);
        }
    }

    submissionTable.init();
})
