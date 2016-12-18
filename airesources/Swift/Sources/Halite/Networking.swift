import Foundation

fileprivate var gameMap: GameMap!

@discardableResult
func deserializeGameMap(input: String) -> GameMap {
    let inputComponents = input.components(separatedBy: " ")
    
    var y = 0
    var x = 0

    var index = 0
    
    while y != gameMap.height {
        let counter = UInt(inputComponents[index])!
        let owner = UInt8(inputComponents[index + 1])!
        
        for _ in 1...counter {
            gameMap.contents[y][x].owner = owner;
            x += 1
        
            if x == gameMap.width {
                x = 0
                y += 1
            }
        }

        index += 2
    }
    
    for y in 0..<gameMap.height {
        for x in 0..<gameMap.width {
            let strength = UInt8(inputComponents[index])!
            gameMap.contents[y][x].strength = strength;
            index += 1
        }
    }
    
    return gameMap
}

func getString() -> String {
    return readLine()!
}

func sendString(_ string: String) {
    FileHandle.standardOutput.write("\(string)\n".data(using: .utf8)!)
}

public func getInit() -> UInt8 {
    let tag = UInt8(getString())!

    var inputComponents = getString().components(separatedBy: " ")
    let width = Int(inputComponents[0])!
    let height = Int(inputComponents[1])!
    
    gameMap = GameMap(width: width, height: height)

    inputComponents = getString().components(separatedBy: " ")
    var index = 0
    for y in 0..<height {
        for x in 0..<width {
            gameMap.contents[y][x].production = UInt8(inputComponents[index])!
            index += 1
        }
    }

    deserializeGameMap(input: getString())

    return tag
}

public func sendInit(name: String) {
    sendString(name)
}

public func getFrame() -> GameMap {
    deserializeGameMap(input: getString())
    return gameMap
}

public func sendFrame(moves: [Move]) {
    let serialized = moves.map({ "\($0.location.x) \($0.location.y) \($0.direction.rawValue) " })
                          .joined()

    sendString(serialized)
}
