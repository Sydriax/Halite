import Foundation

let tag = getInit()

sendInit(name: "RandomBot-Swift")

while true {
    let gameMap = getFrame()
    
    var moves = [Move]()
    
    for y in 0..<gameMap.height {
        for x in 0..<gameMap.width {
            let location = Location(x: x, y: y)
            if (gameMap[location].owner == tag) {
                let direction = Direction(rawValue: UInt8(arc4random_uniform(5)))!
                moves.append(Move(location: location, direction: direction))
            }
        }
    }
    
    sendFrame(moves: moves)
}
