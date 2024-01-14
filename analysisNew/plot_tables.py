import matplotlib.pyplot as plt
from math import sqrt
import numpy as np
from matplotlib.lines import Line2D


# function to parse the folowing file format:
#run	zone	gain
#		Xpitch	Xinter	Xclsize	XampF	Xres
#		Ypitch	Yinter	Yclsize	YampF	Yres

def parse_file(file_path, msc):
    Gdata = []
    Xdata = []
    Ydata = []
    
    with open(file_path, 'r') as file:
        lines = file.readlines()
        i = 0
        while i < len(lines):
            line = lines[i].strip()
            
            if line and not line.startswith('#'):
                values = line.split('\t')
                Gdata.append((values[0], values[1], values[2]))
                i += 1
                line = lines[i].strip()
                values = line.split('\t')
                rescorr = sqrt(float(values[4])**2 - msc**2)
                Xdata.append((float(values[0]), float(values[1]), float(values[2]), float(values[3]), rescorr))
                i += 1
                line = lines[i].strip()
                values = line.split('\t')
                rescorr = sqrt(float(values[4])**2 - msc**2)
                Ydata.append((float(values[0]), float(values[1]), float(values[2]), float(values[3]), rescorr))
            i += 1
    return Gdata, Xdata, Ydata

plt.rcParams.update({'font.size': 16})
# listmsc = [0.101, 0.212, 0.364, 0.549, 0.766]
listmsc = [0.101, 0.197, 0.323, 0.461, 0.600]

file_pathStrip = './stripFEU1Inv/stripFEU1_table.txt'
GdataStrip, XdataStrip, YdataStrip = parse_file(file_pathStrip, listmsc[0])

file_pathInter = './interFEU1Inv/interFEU1_table.txt'
GdataInter, XdataInter, YdataInter = parse_file(file_pathInter, listmsc[1])


def plotPitch(XdataStrip, YdataStrip, XdataInter, YdataInter, val, yTitle, graphName):
    fig = plt.figure( figsize=(15, 10) )
    fig.subplots_adjust(left=0.1, right=0.9, bottom=0.1, top=0.9)
    ax1 = fig.add_subplot(111)
    ax2 = ax1.twiny()

    Xtick_locations = [0.4, 0.5, 0.6, 0.9, 1., 1.1, 1.4, 1.5, 1.6]
    Xtick_labels = [0.5, 0.5, 0.5, 1., 1., 1., 1.5, 1.5, 1.5 ]

    Ytick_locations = [0.4, 0.5, 0.6, 0.9, 1., 1.1, 1.4, 1.5, 1.6]
    Ytick_labels = [0.5, 1, 1.5, 0.5, 1, 1.5, 0.5, 1, 1.5 ]

    ax1.set_xticks(Xtick_locations)
    ax1.set_xticklabels(Xtick_labels)
    ax2.set_xticks(Ytick_locations)
    ax2.set_xticklabels(Ytick_labels)

    # plt.plot([x[0] for x in XdataStrip], [x[4] for x in XdataStrip], 'b.', label='stripFEU1 Xstrips, inter = 0.1 ', markersize=15)
    # plt.annotate(f'{[x[0] for x in YdataStrip]}', ([x[0] for x in XdataStrip], [x[4] for x in XdataStrip]))
    # plt.plot([x[0] for x in YdataStrip], [x[4] for x in YdataStrip], 'r.', label='stripFEU1 Ystrips, inter = 0.75', markersize=15)
    # plt.plot([x[0] for x in YdataInter], [x[4] for x in XdataInter], 'b^', label='interFEU1 Xstrips, inter = 0.1 ', markersize=15)
    # plt.plot([x[0] for x in YdataInter], [x[4] for x in YdataInter], 'r^', label='interFEU1 Ystrips, inter = 0.5 ', markersize=15)
    for i, x in enumerate(XdataStrip):
        xD = x[0]
        if(YdataStrip[i][0]==0.5):
            xD = xD - 0.1
        elif(YdataStrip[i][0]==1.5):
            xD = xD + 0.1
        plt.plot(xD, x[val], 'b.', markersize=15, label='stripFEU1 Xstrips, inter = 0.1 ')
        # plt.annotate(f'{YdataStrip[i][0]}', (x[0], x[4]))

    for i, x in enumerate(YdataStrip):
        xD = x[0]
        if(XdataStrip[i][0]==0.5):
            xD = xD - 0.1
        elif(XdataStrip[i][0]==1.5):
            xD = xD + 0.1
        plt.plot(xD, x[val], 'r.', markersize=15, label='stripFEU1 Ystrips, inter = 0.75')

    for i, x in enumerate(XdataInter):
        plt.plot(x[0], x[val], 'b^', markersize=15, label='interFEU1 Xstrips, inter = 0.1 ')
        # plt.annotate(f'{YdataInter[i][0]}', (x[0], x[4]))

    for i, x in enumerate(YdataInter):
        plt.plot(x[0], x[val], 'r^', markersize=15, label='interFEU1 Ystrips, inter = 0.5 ')
        # plt.annotate(f'{XdataInter[i][0]}', (x[0], x[4]))

    # Define the line styles
    line_styles = [Line2D([0], [0], color='b', lw=0, linestyle='-', marker='.', markersize=15),
                Line2D([0], [0], color='r', lw=0, linestyle='-', marker='.', markersize=15),
                Line2D([0], [0], color='b', lw=0, linestyle='-', marker='^', markersize=15),
                Line2D([0], [0], color='r', lw=0, linestyle='-', marker='^', markersize=15)]

    # Define the labels
    labels = ['stripFEU1 Xstrips', 'stripFEU1 Ystrips',
            'interFEU1 Xstrips', 'interFEU1 Ystrips']

    # Create the legend
    plt.legend(handles=line_styles, labels=labels)

    ax1.set_xlim(0.3, 1.7)
    ax2.set_xlim(ax1.get_xlim())
    ax1.set_xlabel('Pitch [mm]')
    # ax2.set_xlabel('Pitch [mm]')

    ax1.set_ylabel(yTitle)
    # fig.legend()
    plt.title('Resolution vs Pitch')
    plt.grid()
    # plt.show()
    fig.savefig(graphName)


plotPitch(XdataStrip, YdataStrip, XdataInter, YdataInter, 4, 'Resolution [mm]', 'Resolution_vs_Pitch.png')
plotPitch(XdataStrip, YdataStrip, XdataInter, YdataInter, 2, 'Cluster size [mm]', 'clsize_vs_Pitch.png')
plotPitch(XdataStrip, YdataStrip, XdataInter, YdataInter, 3, 'Amplitude fraction [mm]', 'amp_vs_Pitch.png')