#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>

#include "hlt.hpp"
#include "Networking.hpp"

int main() {
	srand(time(NULL));

	std::cout.sync_with_stdio(0);

	unsigned char myTag;
	hlt::Map presentMap;
	getInit(myTag, presentMap);
	sendInitResponse("BasicC++Bot");

	std::set<hlt::Move> moves;
	while(true) {
		moves.clear();

		getFrame(presentMap);

		for(unsigned short y = 0; y < presentMap.height; y++) {
			for(unsigned short x = 0; x < presentMap.width; x++) {
				hlt::Site site = presentMap.contents[y][x];
				if (site.owner == myTag) {
					unsigned char moveDirection = (unsigned char)(rand() % 5);
					if(site.strength < site.production*5) {
						moveDirection = STILL;
					} else {
						for(int d : CARDINALS) {
							if(presentMap.getSite({x, y}, d).owner != myTag) {
								moveDirection = d;
							}
						}
					}
					moves.insert({{x, y}, moveDirection});
				}
			}
		}
		sendFrame(moves);
	}

	return 0;
}
