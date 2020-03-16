#!/usr/bin/env python

from __future__ import division
import sys
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
#from subprocess import run
from timeit import timeit

T_CONVERSION=100

# Fetches data from preformatted files
def get_data(fname):
    times = []
    res = []
    req = []

    # Open the file
    f = open(fname)
    # For each line
    for line in f.readlines():
        # Split the comment-separated data into an array
        d = line.split(",")
        if len(d) != 3:
            print("Error: File Not Formatted Properly")
            print("Expecting: #req threads, #res threads, time")
            exit()
        # Append the split data to a list for plotting
        req.append(int(d[0]))
        res.append(int(d[1]))
        times.append(float(d[2]))

    return res, req, times

def generate_data(exe):
    req_list  = []
    res_list  = []
    time_list = []

    # Generate a range for each type of thread
    ### MODIFY HERE ###
    reps = 10
    for req in range(1, 10):
        for res in range(1, 10):
            # Declare parameters and the call arguments for the subprocess.call
            #input_dir = "input/"
            #name_files = "%snames1.txt %snames2.txt %snames3.txt %snames4.txt %snames5.txt" % (input_dir, input_dir, input_dir, input_dir, input_dir)
            #parameters = "%s %s" % (req, res)
            call_arguments = str("""["%s", "%s", "%s", "output/reqlog.txt", "output/reslog.txt", "input/names1.txt", "input/names2.txt", "input/names3.txt", "input/names4.txt", "input/names5.txt"]""" % ("./"+str(exe), req, res))
            #print(call_arguments)

            # Time the program using timeit
            time = timeit(stmt = "subprocess.call(%s)" % call_arguments, setup = "import subprocess", number=reps) * T_CONVERSION / reps

            #print("Time: %s\n" % time)

            # Store the data
            req_list.append(req)
            res_list.append(res)
            time_list.append(time)

    return req_list, res_list, time_list

# Takes the data input and plots it to a 3D graph
def plot(data):
    # Split the data into its components
    res, req, times = data

    # Format requestor and resolver data
    resset = set(res)
    reqset = set(req)
    req2d, res2d = np.meshgrid(sorted(resset), sorted(reqset))
    
    # Format time data
    Z = np.zeros(shape=(max(reqset)+1, max(resset)+1))
    for ind, (i, j) in enumerate(zip(req, res)):
        Z[i, j] = times[ind]
    Z = Z[min(reqset):, min(resset):]

    # Plot output
    f = plt.figure()
    ax2 = plt.axes(projection='3d')
    ax2.set_title('Time vs. Thread Count')
    surf2 = ax2.plot_surface(req2d, res2d, Z, cmap=plt.cm.coolwarm, linewidth=0)
    ax2.set_xlabel("# Requester Threads")
    ax2.set_ylabel('# Resolver Threads')
    ax2.set_zlabel('Time')
    f.colorbar(surf2, shrink=0.5, aspect=5)
    plt.savefig("performance.png")
    plt.show()

# An example of data that works with the 3d plot
def mock_data():
    x = []
    y = []
    z = []

    # Generate semi-arbitrary ranges and append data points
    for i in range(4, 19):
        for j in range(20, 30):
            x.append(i)
            y.append(j)
            z.append(np.random.rand())
    return x, y, z

# When called interactively, treat this as main()
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Error: Missing Arguments")
        exit()
    elif len(sys.argv) > 2:
        print("Error: Extra Arguments")
        exit()

    # Input arguments
    exe = sys.argv[1]
    print(exe)

    # Uncomment the following line to test with mock data
    #data = mock_data()
    data = generate_data(exe)

    plot(data)

