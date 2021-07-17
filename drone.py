#/usr/bin/python3
from djitellopy import tello
from time import sleep
import numpy as np
import math

# Instantiating Tello object
te = tello.Tello()
te.connect()
print(te.get_battery())

listOfCoordinates = np.array([[0,0], [1,2], [-4,-5], [0,0]])

te.takeoff()
# left/right, forward/backwards, up/down, yaw
#te.send_rc_control(0, 50, 0, 0)
sleep(2)    



def rotateTowardsNext(current: np.array, next: np.array, angle:float):
    angleRequired = 180 - math.degrees(math.atan((current[0] - next[0])/(current[1]-next[1])))
    angleRequired = int(angleRequired)
    if angle == 0 or angleRequired == angle:
        te.send_rc_control(0,0,0,angleRequired)
        return angleRequired
    
    elif angleRequired < angle:
        te.send_rc_control(0,0,0,angleRequired - angle)
        return angleRequired

    else:
        te.send_rc_control(0,0,0,angle - angleRequired)
        return angleRequired

listIt = 0
tempIt = 0
tempAngle = 0
print(listOfCoordinates)
while listOfCoordinates.size != 1:
    rotatingAngle = rotateTowardsNext(listOfCoordinates[0], listOfCoordinates[1], tempAngle)
    te.send_rc_control(0,20,0,0)
    listOfCoordinates = np.delete(listOfCoordinates, 0, 0)
    tempAngle = rotatingAngle

# to prevent moving before landing
te.send_rc_control(0, 0, 0, 0)
te.land()


