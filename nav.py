from dataclasses import dataclass
from scipy.spatial import distance
import math
import os

inputPath = "./routeplanning/inputfiles"

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
        self.lastRot = 0


    def readCoords(self, i) -> None:
        filename = inputPath + "/input" + str(i) + ".txt"
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
            return 270

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
            offset = 270
        
        return (math.degrees(math.atan(xDist/ yDist)) + offset - self.lastRot) % 360


    def createPath(self) -> None:

        currentCoord = self.coords[0]

        for i in range(0, len(self.coords) - 1):
            nextCoord = self.coords[i + 1]
            rotation = self.calculateRot(currentCoord, nextCoord)
            self.lastRot = rotation
            currentCoordPair = (currentCoord.x, currentCoord.y)
            nextCoordPair = (nextCoord.x, nextCoord.y)
            dist = distance.euclidean(currentCoordPair, nextCoordPair)
            self.rotations.append([round(rotation, 2), round(dist, 2)])

        # The last link
        # TODO: Find a way to include this in the for loop above
        currentCoord = self.coords[-1]
        nextCoord = self.coords[0]
        rotation = self.calculateRot(currentCoord, nextCoord)
        currentCoordPair = (currentCoord.x, currentCoord.y)
        nextCoordPair = (nextCoord.x, nextCoord.y)
        dist = distance.euclidean(currentCoordPair, nextCoordPair)
        self.rotations.append([round(rotation, 2), round(dist, 2)])

        for i in self.rotations:
            print(i)
        print("")

i = 1

for file in os.listdir(inputPath):
    n = Nav()
    currentFile = os.path.join(inputPath, file)
    n.readCoords(i)
    i += 1
    n.createPath()
