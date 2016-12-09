import java.util.*

fun main(args : Array<String>) {
  MyBot().start()
}

class MyBot {

  fun start() {
    val network = Networking()
    val iPackage = network.getInit()
    val myID = iPackage.myID
    var gameMap = iPackage.map

    network.sendInit("MyKotlinBot")

    while (true) {
      val moves = ArrayList<Move>()

      gameMap = network.getFrame()

      for (y in 0..gameMap.height - 1) {
        for (x in 0..gameMap.width - 1) {
          val site = gameMap.getSite(Location(x, y))
          if (site.owner == myID) {
            val dir = Direction.random()
            moves.add(Move(Location(x, y), dir))
          }
        }
      }
      network.sendFrame(moves)
    }
  }
}