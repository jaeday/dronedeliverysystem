#/usr/bin/python3

import time
from scipy.spatial import distance # distance.euclidean

class TSP:

    def __init__(self, debug) -> None:
                        
        # coords is a list of tuples (ordered containers) containing
        # (distance (float), parentNode (int), visited (bool))
        self.coords = []
        self.coordVisited = []
        
        # prim is a list of lists containing [distance (float), parent (int), visited (bool)]
        self.prim = []

        # path is a containing indices of coordinates in the order they
        # are to be traversed
        # since the 0th point must always be visited first, these are included
        # in the constructor
        self.path = [0]
        self.optimalPath = [0]

        self.distmat = []

        self.mstDist = 0.0
        self.totalDist = 0.0
        self.optimalDist = 0.0
        self.distSoFar = 0.0
        self.numPoints = 0
        self.debug = bool(debug)


    """ 
    Reads the input file into the coords list
    """
    def readInput(self) -> None:

        # filename = input("Enter coordinate filename: ")
        filename = "sample-e.txt" # temporary hardcoded filename
        coordFile = open(filename, "r")
        # this reads the first line of the input file (the number of points)
        self.numPoints = int(coordFile.readline())

        points = coordFile.readlines()

        # reading in points
        for point in points:
            # split() splits strings based on whitespace by default
            coordPair = point.split()
            x = int(coordPair[0])
            y = int(coordPair[1])
            self.coords.append((x, y))
            self.coordVisited.append(False)

        # printing in read-in coordinates (for debugging)
        if self.debug:
            for coord in self.coords:
                print(coord)

        self.initDistmat()


    def initDistmat(self) -> None:

        # This is a really inefficient way of filling a matrix
        # but it seems to work so I shall keep it for now
        for i in range(0, self.numPoints):
            tempList = []
            for j in range(0, self.numPoints):
                tempList.append(0)
            self.distmat.append(tempList)

        for i in range(0, len(self.distmat)):
            for j in range(0, len(self.distmat[i])):
                # Ensures that no loops can be created (where an edge connects a point to itself)
                self.distmat[i][j] = float('inf') if i == j else distance.euclidean(self.coords[i], self.coords[j])


    """
    Finds the closest other point to a given point.
    Note: 'point' is an index in the coords list
    """
    def findClosestPoint(self, point) -> int:

        minDist = float('inf')
        closest = -1

        for i in range(0, len(self.coords)):
            temp = distance.euclidean(self.coords[i], self.coords[point])

            if i != point and temp < minDist:
                minDist = temp
                closest = i
            
        return closest


    """
    Attempts to find an MST from self.coords[start]
    using Prim's algorithm
    """
    def mst(self, start) -> None:

        self.prim.clear()

        # if there's a better way to fill lists, I will replace this code
        for i in range(0, self.numPoints):
            # mixing types in lists is apparently not a good thing to do
            # but elements in tuples are immutable and thus hard to work with
            # so this will have to do

            # prim is a list of lists containing [distance (float), parent (int), visited (bool)]
            self.prim.append([float('inf'), -1, False])

        self.mstDist = 0.0
        current = start
        numVisited = start

        # Using a while loop to avoid accidentally using the outer loop variable
        while numVisited < self.numPoints:
            numVisited += 1

            # The next node is initally unknown
            next = -1

            for i in range(start, len(self.prim)):
                
                # Default initializing next
                if not self.prim[i][2]:
                    if next == -1:
                        next = i
                
                    dist = self.distmat[self.path[current]][self.path[i]]
                    
                    # If the distance between current and i is less that the
                    # distance already recorded in i, set the distance at i
                    # to be that of the parent and the parent to be current
                    if dist < self.prim[i][0]:
                        self.prim[i][0] = dist
                        self.prim[i][1] = current

                    # If prim[i][0] has the shortest distance from
                    # current, make next = i
                    if self.prim[i][0] < self.prim[next][0]:
                        next = i
            
            self.prim[next][2] = True
            # Start from the point that was closest to the previous current
            current = next

        # Updating mstDist
        for i in range(start, len(self.prim)):
            self.mstDist += self.prim[i][0]


    """
    Returns the index in self.path[] where inserting the point
    coords[toAdd] results in the shortest increase in the overall distance
    """
    def findMinInsertionIdx(self, toAdd) -> int:

        minDist = float('inf')
        insertIdx = -1

        for i in range(0, len(self.path) - 1):
            dist1 = distance.euclidean(self.coords[self.path[i]], self.coords[toAdd])
            dist2 = distance.euclidean(self.coords[self.path[i + 1]], self.coords[toAdd])
            dist3 = distance.euclidean(self.coords[self.path[i]], self.coords[self.path[i + 1]])
            distDiff = dist1 + dist2 - dist3

            if distDiff < minDist:
                minDist = distDiff
                insertIdx = i + 1

        return insertIdx


    """
    Calculates the distance of the Hamiltonian cycle created by the points
    in self.path[] and updates self.totalDist
    """
    def calcTotalDist(self) -> None:
        
        for i in range(0, len(self.path) - 1):
            self.totalDist += distance.euclidean(self.coords[self.path[i]], self.coords[self.path[i + 1]])

        # Adding the last link to create the Hamiltonian cycle
        self.totalDist += distance.euclidean(self.coords[self.path[0]], self.coords[self.path[-1]])


    """
    Attemps to find a solution to the TSP for the coordinates read in
    using the random insertion heuristic
    """
    def fastTSP(self) -> None:

        self.path.clear()
                
        # Adding the 0th point first (since this must always be visited first)
        self.coordVisited[0] = True
        toAdd = self.findClosestPoint(0)
        self.coordVisited[toAdd] = True
        # Adding the point closest to the 0th point
        self.path.append(0)
        self.path.append(toAdd)

        numVisited = 1
        # Other nodes are added sequentially (in tyhe order they were read in)
        while numVisited < self.numPoints:

            # toAdd is the index of the point to be added
            toAdd = numVisited
            numVisited += 1

            if self.coordVisited[toAdd]:
                continue
            
            insertIdx = self.findMinInsertionIdx(toAdd)
            self.path.insert(insertIdx, toAdd)
            self.coordVisited[toAdd] = True

        self.calcTotalDist()


    """
    Returns true if adding a certain point is "promising"
    i.e. if (sDist + eDist + self.distSoFar + self.mstDist < self.optimalDist)
    """
    def promising(self, permLength) -> bool:

        if self.distSoFar > self.optimalDist:
            return False

        # Creating an MST of all points after permLength
        self.mst(permLength)

        # Finding the points in the MST that are closest to the start and end
        # points of the optimal path found so far
        sDist = float('inf')
        eDist = float('inf')

        for i in range(permLength, len(self.path)):
            
            dist1 = self.distmat[self.path[0]][self.path[i]]
            dist2 = self.distmat[self.path[i]][self.path[permLength - 1]]

            if dist1 < sDist:
                sDist = dist1

            if dist2 < eDist:
                eDist = dist2

        return bool(sDist + eDist + self.distSoFar + self.mstDist < self.optimalDist)


    """
    Recursively generates and checks all possible paths from permLength to
    the last element of self.path[]
    """
    def genPerms(self, permLength) -> None:
        
        if permLength == len(self.path):

            # Adding the last link to create a Hamiltonian cycle
            # [-1] is the last element of a list
            lastLink = self.distmat[self.path[0]][self.path[-1]]

            if self.distSoFar + lastLink < self.optimalDist:

                self.optimalPath = self.path.copy()
                self.optimalDist = self.distSoFar + lastLink
            
            return

        # Return if not promising
        if not self.promising(permLength):
            return
        
        for i in range(permLength, len(self.path)):

            # Swapping self.path[permLength] and self.path[i]
            self.path[permLength], self.path[i] = self.path[i], self.path[permLength]
            # Generating permutations with the last distance added
            self.distSoFar += self.distmat[self.path[permLength -1]][self.path[permLength]]
            self.genPerms(permLength + 1)
            # Removing the last distance
            self.distSoFar -= self.distmat[self.path[permLength -1]][self.path[permLength]]
            # Swapping self.path[permLength] and self.path[i]
            self.path[permLength], self.path[i] = self.path[i], self.path[permLength]


    """
    Attempts to find the optimal TSP solution for the given coordinates
    """
    def optimalTSP(self) -> None:

        self.fastTSP()

        # Setting lower bounds
        self.optimalPath = self.path.copy()
        self.optimalDist = self.totalDist
        
        # The initial permLength is 1
        self.genPerms(1)

        # Printing the optimal distance and optimal path
        print(self.optimalDist)

        for i in self.optimalPath:
            print(i, end = " ")
        print("\n")
    

# End of TSP class

startTime = time.time()

salesperson = TSP(False)
salesperson.readInput()
salesperson.optimalTSP()

printTime = True

if printTime:
    print("--- %s seconds ---" % (time.time() - startTime))