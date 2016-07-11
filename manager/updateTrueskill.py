import sys
import trueskill

sys.argv.pop(0)

teams = [[trueskill.Rating(mu=float(sys.argv[index]), sigma=float(sys.argv[index+1]))] for a in range(0, len(sys.argv), 2)]

newRatings = trueskill.rate(teams)
for rating in newRatings:
    print(str(rating[0].mu) + " " + str(rating[0].sigma))
