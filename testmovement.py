#/usr/bin/python3
from djitellopy import tello
from time import sleep
import numpy as np
import math

# Instantiating Tello object
te = tello.Tello()
te.connect()
print(te.get_battery())

# listOfCoordinates = np.array([[0,0], [1,2], [-4,-5], [0,0]])

te.takeoff()
# left/right, forward/backwards, up/down, yaw
#te.send_rc_control(50, 0, 0, 0)
sleep(5)    

#te.rotate_clockwise(10)
#te.send_rc_control(0, 0, 0, 30)
# te.send_rc_control(0, 0, 0, 0)
te.land()


