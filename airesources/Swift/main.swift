import Foundation

let tag = getInit()

sendInit(name: "MyBot-Swift")

while true {
    let gameMap = getFrame()
    
    var moves = [Move]()
    
    for y in 0..<gameMap.height {
        for x in 0..<gameMap.width {
            let location = Location(x: x, y: y)
            if (gameMap[location].owner == tag) {
                let random = UInt8(arc4random_uniform(15))
                let direction = Direction(rawValue: random > 4 ? 0 : random)!
                moves.append(Move(location: location, direction: direction))
            }
        }
    }
    
    sendFrame(moves: moves)
}
