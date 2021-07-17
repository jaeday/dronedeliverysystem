import os
from pathlib import Path

# put the code for c++ compilation here.
# this will run the code in routeplanning directory.
# the input for the c++ code (so the raw input of delivery locations) will be in routeplanning/inputfiles.
# the output for the c++ code (so the input for the drone movement instruction) will be in routeplanning/outputfiles.
# This script, along with the c++ compilation, will also delete the "used" input file (so deleting the order that's been processed. I guess you can delete the files once
# the file has been processed by the c++ code maybe after the c++ compilation code)
# This will then transfer the output from c++ into the outputfiles, and then drone.py will fetch the information from the same directory.
