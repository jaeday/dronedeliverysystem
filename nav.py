from dataclasses import dataclass
from scipy.spatial import distance
import math

# TODO: Change this to only perform clockwise rotations
# TODO: Have drone rotate to an absolute north position before making any rotations

# This is like a C struct / C++ POD struct
@dataclass
class Point:
    x: float
    y: float

class Nav:

    def __init__(self) -> None:
        self.coords = []
        self.rotations = []
        self.travelDist = []


    def readCoords(self) -> None:
        # TODO: Decide on a filename
        filename = "coords.txt"
        coordFile = open(filename, "r")

        points = coordFile.readlines()

        for point in points:
            coordPair = point.split()
            p = Point(float(coordPair[0]), float(coordPair[1]))
            self.coords.append(p)

    # The drone will always begin at point 0 facing north (0°)
    # It will always rotate to create a straight-line trajectory before
    # travelling anywhere

    """
    Returns the following number depending on the location of
    nextCoord relative to currentCoord

    Returns -1 if nextCoord is at the same position as currentCoord

    Note: c is the position of currentCoord
    
          5
      4   |   1
    8 --- c --- 6
      3   |   2
          7
    """
    def getQuadrant(self, currentCoord, nextCoord) -> int:

        if currentCoord.x < nextCoord.x:
            if currentCoord.y < nextCoord.y:
                return 1
            elif currentCoord.y > nextCoord.y:
                return 2
            # currentCoord.y == nextCoord.y
            else:
                return 6
        elif currentCoord.x > nextCoord.x:
            if currentCoord.y < nextCoord.y:
                return 4
            elif currentCoord.y > nextCoord.y:
                return 3
            # currentCoord.y == nextCoord.y
            else:
                return 8
        # currentCoord.x == nextCoord.x
        else:
            if currentCoord.y < nextCoord.y:
                return 5
            elif currentCoord.y > nextCoord.y:
                return 7
            else:
                return -1


    """
    currentCoord and nextCoords are pairs (2-tuples) of ints
    
    calculateRot works on a relative system; i.e. the rotations are performed
    relative to the current direction the drone is facing
    
    Note: Negative angles indicate a counterclockewise rotation

    Note: The drone will rotate clockwise unless a counterclockwise rotation
    is faster
    """
    def calculateRot(self, currentCoord, nextCoord) -> float:

        quadrant = self.getQuadrant(currentCoord, nextCoord)

        # Cases where rotations can be made in multiples of 90°
        # Straight ahead
        if quadrant == 5 or quadrant == -1:
            return 0
        elif quadrant == 6:
            return 90
        elif quadrant == 7:
            return 180
        elif quadrant == 8:
            return -90

        # fabs(a) returns the absolute value of a
        xDist = math.fabs(nextCoord.x - currentCoord.x)
        yDist = math.fabs(nextCoord.y - currentCoord.y)
        offset = 0

        if quadrant == 1:
            pass
        elif quadrant == 2:
            offset = 90
        elif quadrant == 3:
            offset = 180
        # quadrant == 4
        else:
            offset = -90
        
        return offset + math.degrees(math.atan(xDist/ yDist))

    def createPath(self) -> None:

        currentCoord = self.coords[0]

        for i in range(0, len(self.coords) - 1):
            nextCoord = self.coords[i + 1]
            rotation = self.calculateRot(currentCoord, nextCoord)
            currentCoordPair = (currentCoord.x, currentCoord.y)
            nextCoordPair = (nextCoord.x, nextCoord.y)
            dist = distance.euclidean(currentCoordPair, nextCoordPair)
            print(rotation, " ", dist)

        # The last link
        # TODO: Find a way to include this in the for loop above
        currentCoord = self.coords[-1]
        nextCoord = self.coords[0]
        rotation = self.calculateRot(currentCoord, nextCoord)
        currentCoordPair = (currentCoord.x, currentCoord.y)
        nextCoordPair = (nextCoord.x, nextCoord.y)
        dist = distance.euclidean(currentCoordPair, nextCoordPair)
        print(rotation, " ", dist)

n = Nav()
n.readCoords()
n.createPath()

