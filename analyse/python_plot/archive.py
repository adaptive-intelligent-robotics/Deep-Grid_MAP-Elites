########## Import #########
import sys
import math
import numpy as np
import matplotlib as mpl
import pandas as pd
from matplotlib.patches import Circle, Wedge, Polygon, Rectangle
from matplotlib.collections import PatchCollection
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap



######## Archive file analysis function #########

def load_archive_text_file(path, D, R):
    archive = np.genfromtxt(path, usecols = range(1,4))

    grid = np.empty((D,D,))
    grid[:] = np.nan

    for i in range(archive.shape[0]):
        bd = get_bd(archive[i,0], archive[i,1], D, R)

        if not np.isnan(bd).any():
            grid[bd[0],bd[1]] = archive[i,2]
    return grid


############ Polar coordinate calculation functions #############

def get_nb_points(D, D1, i):
    return int(round( D * (i+0.5)/(D1+0.5)))

def get_lseg_and_D1(D, R):
    D1 = round(R/(2*np.pi*R/D)); # number of rings
    lseg = R/(D1+0.5);# Length of the arc between the cells of the outer ring
    return lseg, D1

def get_bd(x, y, D=350, R=0.5):
    r = math.sqrt((x-0.5)**2+(y-0.5)**2)
    t = math.atan2(y-0.5,x-0.5)
    if t<0:
        t = t+2*np.pi
    lseg, D1 = get_lseg_and_D1(D,R)

    if(r<=R):
        d1=min(D1,round(r/lseg)); # Index of the ring containing the point x,y 
        nb_points=get_nb_points(D, D1, d1)
        d2=round(t * nb_points/(2*np.pi)); # Index of the cell on the ring.
        if(d2 == nb_points): # cell of index 0 or "nb_points" are actually the same (they are both half cells).
            d2 = 0;
        bd=[int(d1),int(d2)]; # normalisation of the BD to match the usual MAP-Elites discretisation
    else:
        bd = [np.nan, np.nan]; # Invalid individuals
    return bd


########## Plot function ############

def grid_plot(ax, data, D=350, R=0.5, no_border = False,cmap=None, norm=None, vmin =0, vmax=1):
    linewidth = 0.2

    theta = np.linspace(0, 2 * np.pi, 768)
    lseg, D1 = get_lseg_and_D1(D,R)

    # Create the bound for the segment 17
    res = 30 # angular resolution per cell, ie. how many points are used to plot each arcs of the cell
    segs = [] # table to add the different segs
    colors = [] # and the colors of the segs

    # plot central cell
    r = np.array(( 0 ,  0.5*lseg ))
    r0 = np.repeat(r[:, np.newaxis], res*1 + 1 , axis=1).T
    theta_offset = 2*np.pi/1
    theta0 = np.linspace( -0.5, 1-0.5, res*1 + 1) * theta_offset
    theta0 = np.repeat(theta0[:, np.newaxis], 2, axis=1)
    z =  np.array([np.repeat(data[0,0],res)]).T
    ax.pcolormesh(theta0, r0, z, cmap=cmap, norm=norm, vmin =vmin, vmax=vmax)

    # plot all other cells
    for i in range(1, int(D1)+1):
        r = np.array(( (i-0.5)*lseg,  (i+0.5)*lseg ))
        r[0] = r[0] if r[0]< R else R
        r[1] = r[1] if r[1]< R else R
        
        if not no_border: ax.plot(theta, np.repeat(r[0] , theta.shape), '-k', lw=linewidth) # plots the circles of each ring
        nb_points=get_nb_points(D, D1, i)
        r0 = np.repeat(r[:, np.newaxis], res*nb_points + 1 , axis=1).T
        theta_offset = 2*np.pi/nb_points
        theta0 = np.linspace( -0.5, nb_points-0.5, res*nb_points + 1) * theta_offset
        theta0 = np.repeat(theta0[:, np.newaxis], 2, axis=1)

        z =  np.array([np.repeat(data[i,0:nb_points],res)]).T
        
        ax.pcolormesh(theta0, r0, z, cmap=cmap, norm=norm, vmin =vmin, vmax=vmax)
        if not no_border: 
            for j in range(int(nb_points)): # defines the lines for the different cell borders
                theta_j = (j+0.5)*2*np.pi/nb_points
                colors.append((0,0,0))
                segs.append(((theta_j, r[0]), ( theta_j, r[1])))

    if not no_border: 
        ln_coll = mpl.collections.LineCollection(segs, colors=colors, linewidths=linewidth)
        ax.add_collection(ln_coll)
    ax.plot(theta, np.repeat(R , theta.shape), '-k', lw=linewidth) # plots last circle


def plot_archive_graph(name, path, dim, radius, picname, vmin, vmax):
    data = load_archive_text_file(path, dim, radius)
    plt.figure(figsize=(10, 10))

    # Cut subplots
    gs = mpl.gridspec.GridSpec(1, 2, width_ratios=[13, 1]) 

    # First subplot is the archive
    ax = plt.subplot(gs[0], projection='polar')
    grid_plot(ax,data, no_border=False, cmap = 'viridis', vmin=vmin, vmax=vmax)
    ax.set_rmax(radius)
    ax.grid(False)
    ax.axis('off')
    ax.set_title(name, va='bottom', fontsize = '18')

    # Second subplot is the colormap
    ax2 = plt.subplot(gs[1], projection=None)
    ax2.imshow(np.outer(np.arange(vmin,vmax,0.01),np.ones(10)), aspect='auto',
            cmap=plt.get_cmap('viridis'), origin="lower", extent=[0,1,vmin,vmax])
    ax2.tick_params(
                axis='x',          # changes apply to the x-axis
                which='both',      # both major and minor ticks are affected
                bottom=False,      # ticks along the bottom edge are off
                top=False,         # ticks along the top edge are off
                labelbottom=False) # labels along the bottom edge are off
    ax2.yaxis.tick_right()
    plt.savefig(picname)
    plt.close()



######## Main ##########

print("\n \nPrinting archives \n")

# Path declaration
path = sys.argv[1]

# Open the archives file
frame = pd.read_csv(path + "/archives.dat", delimiter = ',')

# For each archive in the archives file
for i in range (0, frame.shape[0]):
    name = frame['name'].iloc[i]
    plot_archive_graph(name.replace("_", " "), frame['path'].iloc[i], 
                       frame['dim'].iloc[i], frame['radius'].iloc[i],
                       path + "/" + name + ".png", 
                       frame['min'].iloc[i], frame['max'].iloc[i])

print("Finish plotting. Printed: ", frame.shape[0], " archives \n")

