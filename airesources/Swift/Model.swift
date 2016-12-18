import Foundation

public enum Direction: UInt8 {
    case still = 0, north, east, south, west
}

public struct Location {
    let x: Int
    let y: Int
}

public struct Move {
    public let location: Location
    public let direction: Direction
}

public struct Site {
    public var owner: UInt8
    public var strength: UInt8
    public var production: UInt8
}

public class GameMap {
    public let width: Int
    public let height: Int
    public var contents: [[Site]]
    
    public init(width: Int = 0, height: Int = 0) {
        self.width = width
        self.height = height
        self.contents = [[Site]](repeating: [Site](repeating: Site(owner: 0, strength: 0, production: 0), count: width), count: height)
    }
    
    public func isInBounds(location: Location) -> Bool {
        return location.x < width && location.y < height
    }
    
    public func getDistance(from location1: Location, to location2: Location) -> Int {
        var dx = abs(location1.x - location2.x)
        var dy = abs(location1.y - location2.y)
        
        if dx > width / 2 {
            dx = width - dx
        }
        
        if dy > height / 2 {
            dy = height - dy
        }
        
        return dx + dy;
    }
    
    public func getAngle(from location1: Location, to location2: Location) -> Float {
        var dx = location1.x - location2.x;
        var dy = location2.y - location1.y; // reverse the y-axis.
        
        if dx > width - dx {
            dx -= width
        }
        
        if -dx > width + dx {
            dx += width
        }
        
        if dy > height - dy {
            dy -= height
        }
        
        if -dy > height + dy {
            dy += height
        }
        
        return atan2(Float(dy), Float(dx))
    }
    
    // TODO: implement with modulo
    public func getLocation(location: Location, direction: Direction) -> Location {
        var y = location.y
        var x = location.x
        
        switch direction {
        case .still:
            break;
            
        case .north:
            y = location.y == 0 ? height - 1 : location.y - 1
            
        case .east:
            x = location.x == width - 1 ? 0 : location.y + 1
            
        case .south:
            y = location.y == height - 1 ? 0 : location.y + 1
            
        case .west:
            x = location.x == 0 ? width - 1 : location.x - 1
        }
        
        return Location(x: x, y: y)
    }
    
    public func getSite(location: Location, direction: Direction) -> Site {
        let l = getLocation(location: location, direction: direction)
        return contents[l.y][l.x]
    }
    
    public func getSite(location: Location) -> Site {
        return contents[location.y][location.x]
    }
    
    public subscript(location: Location) -> Site {
        get {
            return contents[location.y][location.x]
        }
    }
    
    public subscript(y: Int, x: Int) -> Site {
        get {
            return contents[y][x]
        }
    }
}
