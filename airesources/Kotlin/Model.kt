import java.util.*

class Networking {

  val SIZE_OF_INTEGER_PREFIX = 4
  val CHAR_SIZE = 1
  var width : Int = 0
  var height : Int = 0

  val productions : MutableList<MutableList<Int>> = arrayListOf()

  fun deserializeGameMapSize(inputString: String) {
    inputString.split(" ")
        .map { Integer.parseInt(it) }
        .forEachIndexed { index, size ->
          if (index == 0) width = size else height = size}
  }

  fun deserializeProductions(input: String) {
    val inputs = input.split(" ").map { Integer.parseInt(it) }
    (0 until height).mapTo(productions) { inputs.subList(it * width, (it + 1) * width).toMutableList() }
  }

  fun serializeMoveList(moves: ArrayList<Move>): String {
    val builder = StringBuilder()
    for ((loc, dir) in moves) builder.append(loc.x.toString() + " " + loc.y + " " + dir.ordinal + " ")
    return builder.toString()
  }

  fun deserializeGameMap(input: String): GameMap {
    val inputs = input.split(" ").map { Integer.parseInt(it) }

    val map = GameMap(width, height)

    var y = 0
    var x = 0
    var currentIndex = 0
    while (y < map.height) {
      val counter = inputs[currentIndex++]
      val owner = inputs[currentIndex++]
      (0 until counter).forEach {
        map.contents[y][x].owner = owner
        ++x
        if (x == map.width) {
          x = 0
          ++y
        }
      }
    }

    for (x in 0 until map.contents.size) {
      for (y in 0 until map.contents[x].size) {
        map.contents[x][y].strength = inputs[currentIndex++]
        map.contents[x][y].production = productions[x][y]
      }
    }

    return map
  }

  fun sendString(sendString: String) {
    println(sendString)
    System.out.flush()
  }

  fun getInit(): InitPackage {
    val myID = Integer.parseInt(readLine()?.trim())
    deserializeGameMapSize(readLine()!!.trim())
    deserializeProductions(readLine()!!.trim())
    val map = deserializeGameMap(readLine()!!.trim())

    return InitPackage(myID, map)
  }

  fun sendInit(name: String) {
    sendString(name)
  }

  fun getFrame(): GameMap {
    return deserializeGameMap(readLine()!!.trim())
  }

  fun sendFrame(moves: ArrayList<Move>) {
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

  val contents : MutableList<MutableList<Site>> = arrayListOf()

  init {
    (0 until height).forEach {
      contents.add(Array(width, {Site(0, 0, 0)}).toMutableList())
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