import java.util.*

class Networking {

  val SIZE_OF_INTEGER_PREFIX = 4
  val CHAR_SIZE = 1
  var width : Int = 0
  var height : Int = 0

  val productions = ArrayList<ArrayList<Int>>()

  internal fun deserializeGameMapSize(inputString: String) {
    val map = inputString.split(" ")

    width = Integer.parseInt(map[0])
    height = Integer.parseInt(map[1])
  }


  internal fun deserializeProductions(inputString: String) {
    val input = inputString.split(" ")

    var index = 0
    for (a in 0..height - 1) {
      val row = ArrayList<Int>()
      for (b in 0..width - 1) {
        row.add(Integer.parseInt(input[index]))
        index++
      }
      productions.add(row)
    }
  }

  internal fun serializeMoveList(moves: ArrayList<Move>): String {
    val builder = StringBuilder()
    for ((loc, dir) in moves) builder.append(loc.x.toString() + " " + loc.y + " " + dir.ordinal + " ")
    return builder.toString()
  }

  internal fun deserializeGameMap(inputString: String): GameMap {
    val input = inputString.split(" ")

    val map = GameMap(width, height)

    // Run-length encode of owners
    var y = 0
    var x = 0
    var counter = 0
    var owner = 0
    var currentIndex = 0
    while (y != map.height) {
      counter = Integer.parseInt(input[currentIndex])
      owner = Integer.parseInt(input[currentIndex + 1])
      currentIndex += 2
      for (a in 0..counter - 1) {
        map.contents[y][x].owner = owner
        ++x
        if (x == map.width) {
          x = 0
          ++y
        }
      }
    }

    for (a in 0..map.contents.size - 1) {
      for (b in 0..map.contents[a].size - 1) {
        val strengthInt = Integer.parseInt(input[currentIndex])
        currentIndex++
        map.contents[a][b].strength = strengthInt
        map.contents[a][b].production = productions[a][b]
      }
    }

    return map
  }

  internal fun sendString(sendString: String) {
    print(sendString + '\n')
    System.out.flush()
  }

  internal fun getString(): String? {
    val builder = StringBuilder()
    var buffer = 0
    while (true) {
      buffer = System.`in`.read()
      if (buffer < 0 || buffer == '\n'.toInt()) {
        break
      }
      builder.append(buffer.toChar())
    }
    //Removes a carriage return if on windows for manual testing.
    if (builder[builder.length - 1] == '\r') {
      builder.setLength(builder.length - 1)
    }
    return builder.toString()
  }

  internal fun getInit(): InitPackage {
    val myID = Integer.parseInt(getString()!!).toInt()
    deserializeGameMapSize(getString()!!)
    deserializeProductions(getString()!!)
    val map = deserializeGameMap(getString()!!)

    return InitPackage(myID, map)
  }

  internal fun sendInit(name: String) {
    sendString(name)
  }

  internal fun getFrame(): GameMap {
    return deserializeGameMap(getString()!!)
  }

  internal fun sendFrame(moves: ArrayList<Move>) {
    sendString(serializeMoveList(moves))
  }
}

data class Site(var owner: Int, var strength : Int, var production: Int)
data class Location(var x: Int, var y: Int)
data class InitPackage(var myID: Int, var map: GameMap)
data class Move(var loc: Location, var dir: Direction)

enum class Direction {
  STILL, NORTH, EAST, SOUTH, WEST;

  companion object {
    val DIRECTIONS = arrayOf(STILL, NORTH, EAST, SOUTH, WEST)
    val CARDINALS = arrayOf(NORTH, EAST, SOUTH, WEST)

    fun random() : Direction {
      return DIRECTIONS[Random().nextInt(DIRECTIONS.size)]
    }
  }
}

class GameMap(val width: Int, val height: Int) {

  val contents = ArrayList<ArrayList<Site>>()

  init {
    for (y in 0..height - 1) {
      val row = ArrayList<Site>()
      for (x in 0..width - 1) {
        row.add(Site(0, 0, 0))
      }
      contents.add(row)
    }
  }

  fun inBounds(location: Location) : Boolean {
    return location.x < width && location.x >= 0 && location.y < height && location.y >= 0
  }

  fun getLocation(loc: Location, dir: Direction): Location {
    val l = loc.copy()
    when(dir) {
      Direction.STILL -> return l
      Direction.NORTH -> if (l.y == 0)          l.y = height - 1 else l.y--
      Direction.EAST ->  if (l.x == width - 1)  l.x = 0          else l.x++
      Direction.SOUTH -> if (l.y == height - 1) l.y = 0          else l.y++
      Direction.WEST ->  if (l.x == 0)          l.x = width - 1  else l.x--
    }
    return l
  }

  fun getSite(loc: Location, dir: Direction): Site {
    val (x, y) = getLocation(loc, dir)
    return contents[y][x]
  }

  fun getSite(loc: Location): Site {
    return contents[loc.y][loc.x]
  }
}