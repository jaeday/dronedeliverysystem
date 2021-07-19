from dataclasses import dataclass
from scipy.spatial import distance
import math
import os

inputPath = "./routeplanning/inputfiles"

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


    def readCoords(self, i: int) -> None:
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
    def getQuadrant(self, currentCoord: Point, nextCoord: Point) -> int:

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


    # TODO: Refactor this code:
    # either remove getQuadrant or use it fully
    def calculateRot(self, currentCoord: Point, nextCoord: Point) -> float:

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

        """
        if both positive: use tan
        positive x diff, negative y diff: + 90
        negative x diff, positive y diff: flip sign
        both negative: - 90
        """

        xDist = nextCoord.x - currentCoord.x
        yDist = nextCoord.y - currentCoord.y

        xDistPos = bool(xDist > 0)
        yDistPos = bool(yDist > 0)

        rotAngle = math.degrees(math.atan(math.fabs(xDist) / math.fabs(yDist)))

        if xDistPos and yDistPos:
            pass
        elif xDistPos and not yDistPos:
            rotAngle += 90
        elif not xDistPos and yDistPos:
            rotAngle *= -1
        # not xDistPos and yDistPos
        else:
            rotAngle -= 90

        return rotAngle


    def createPath(self) -> None:

        for i in range(0, len(self.coords) - 1):
            currentCoord = self.coords[i]
            nextCoord = self.coords[i + 1]
            rotation = self.calculateRot(currentCoord, nextCoord)
            self.lastRot = rotation

            currentCoordPair = (currentCoord.x, currentCoord.y)
            nextCoordPair = (nextCoord.x, nextCoord.y)

            dist = distance.euclidean(currentCoordPair, nextCoordPair)

            self.rotations.append([rotation, dist])
            self.rotations.append([self.lastRot, 0.0])


        # The last link
        currentCoord = self.coords[-1]
        nextCoord = self.coords[0]
        rotation = self.calculateRot(currentCoord, nextCoord)
        currentCoordPair = (currentCoord.x, currentCoord.y)
        nextCoordPair = (nextCoord.x, nextCoord.y)
        dist = distance.euclidean(currentCoordPair, nextCoordPair)
        self.rotations.append([rotation, dist])
        self.rotations.append([self.lastRot, 0.0])

        count = 0
        for i in self.rotations:
            print(count, (round(i[0], 2), round(i[1], 2)))
            count += 1
        print("")


i = 1

for file in os.listdir(inputPath):
    n = Nav()
    currentFile = os.path.join(inputPath, file)
    n.readCoords(i)
    i += 1
    n.createPath()
