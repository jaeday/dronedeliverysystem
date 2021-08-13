from dataclasses import dataclass
from scipy.spatial import distance
import math
import os

# The input path can be hardcoded as a relative path
# ./routeplanning/outputfiles contains the outputs from the TSP algorithm
# implemented in C++
inputPath = "./routeplanning/outputfiles"

# This is like a C struct / C++ POD struct
@dataclass
class Point:
    x: float
    y: float


class Nav:

    def __init__(self) -> None:
        self.coords = []
        self.rotations = []
        self.lastRot = 0.0


    """
    Reads the coordinates of every file in the inputPath that follows the
    naming convention: "coordsn.txt" (e.g. input0.txt, input10.txt et)
    """
    def readCoords(self, i: int) -> None:

        filename = inputPath + "/coords" + str(i) + ".txt"

        # Opens the input file in read-only mode
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
    Returns the rotation required to get the drone from currentCoord to
    nextCoord in a straight line.
    """
    def calculateRot(self, currentCoord: Point, nextCoord: Point) -> float:

        # Cases where rotations can be made in multiples of 90°
        # Straight ahead or at the same position
        if currentCoord.x == nextCoord.x and currentCoord.y == nextCoord.y or \
            currentCoord.x == nextCoord.x and currentCoord.y < nextCoord.y:
            return 0
        # 90° Clockwise
        elif currentCoord.x < nextCoord.x and currentCoord.y == nextCoord.y:
            return 90
        # 180°
        elif currentCoord.x == nextCoord.x and currentCoord.y > nextCoord.y:
            return 180
        # 90° Counterclockwise4
        elif currentCoord.x > nextCoord.x and currentCoord.y == nextCoord.y:
            return -90

        # Offsets:
        # both positive -> 0
        # positive x diff, negative y diff -> 90
        # negative x diff, positive y diff -> flip sign
        # both negative -> -90
        xDist = nextCoord.x - currentCoord.x
        yDist = nextCoord.y - currentCoord.y

        xDistPos = bool(xDist > 0)
        yDistPos = bool(yDist > 0)

        # The angle of rotation is:
        # arctan(|xDist| / |yDist|)

        # Note: There is no possibility of dividing by 0 as any
        # 0 yDist would have returned as a rotation that is a multiple of 90°

        rotAngle = math.degrees(math.atan(math.fabs(xDist) / math.fabs(yDist)))

        # Adding the required offset
        if xDistPos and yDistPos:
            pass
        elif xDistPos and not yDistPos:
            rotAngle = 180 - rotAngle
        elif not xDistPos and yDistPos:
            rotAngle *= -1
        # not xDistPos and not yDistPos
        else:
            rotAngle += 180

        return rotAngle

    """
    Calculates all of the rotations and distances required
    to link all of the points in a given input file
    """
    def createPath(self) -> None:

        for i in range(0, len(self.coords) - 1):
            currentCoord = self.coords[i]
            nextCoord = self.coords[i + 1]
            rotation = self.calculateRot(currentCoord, nextCoord)
            self.lastRot = rotation

            # Converting Point objects into 2-tuples to be used
            # in the distance.euclidean() function
            currentCoordPair = (currentCoord.x, currentCoord.y)
            nextCoordPair = (nextCoord.x, nextCoord.y)

            dist = distance.euclidean(currentCoordPair, nextCoordPair)

            self.rotations.append([rotation, dist])
            # Appending the opposite of the last rotation to ensure the drone
            # points to (relative) 0°N
            self.rotations.append([-self.lastRot, 0.0])


        # The last link
        currentCoord = self.coords[-1]
        nextCoord = self.coords[0]
        rotation = self.calculateRot(currentCoord, nextCoord)
        self.lastRot = rotation
        currentCoordPair = (currentCoord.x, currentCoord.y)
        nextCoordPair = (nextCoord.x, nextCoord.y)
        dist = distance.euclidean(currentCoordPair, nextCoordPair)
        self.rotations.append([rotation, dist])
        self.rotations.append([-self.lastRot, 0.0])

        # Printing the rotations and distances to stdout
        # rounded to 2 decimal places
        count = 0
        for i in self.rotations:
            print(count, (round(i[0], 2), round(i[1], 2)))
            count += 1
        print("")

# Counter variable
i = 1

# Wrapper function for the Nav class
for file in os.listdir(inputPath):
    n = Nav()
    currentFile = os.path.join(inputPath, file)
    n.readCoords(i)
    i += 1
    n.createPath()
