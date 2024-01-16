import matplotlib.pyplot as plt
from math import sqrt
import numpy as np
from matplotlib.lines import Line2D


# function to parse the folowing file format:
#run	zone	gain
#		Xpitch	Xinter	Xclsize	XampF	Xres
#		Ypitch	Yinter	Yclsize	YampF	Yres

def resErr(resTot, msc):
    errTot = 0.05*resTot;
    errmsc = 0.2*msc;
    err = resTot**2/(resTot**2 - msc**2)*errTot**2 + msc**2/(resTot**2 - msc**2)*errmsc**2
    return sqrt(err)

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
                Xdata.append((float(values[0]), float(values[1]), float(values[2]), float(values[3]), rescorr, resErr(float(values[4]), msc)))
                i += 1
                line = lines[i].strip()
                values = line.split('\t')
                rescorr = sqrt(float(values[4])**2 - msc**2)
                Ydata.append((float(values[0]), float(values[1]), float(values[2]), float(values[3]), rescorr, resErr(float(values[4]), msc)))
            i += 1
    return Gdata, Xdata, Ydata

plt.rcParams.update({'font.size': 18})
# listmsc = [0.101, 0.212, 0.364, 0.549, 0.766]
listmsc = [0.101, 0.197, 0.323, 0.461, 0.600]

file_pathStrip = './stripFEU1Inv/stripFEU1_table.txt'
GdataStrip, XdataStrip, YdataStrip = parse_file(file_pathStrip, 0)

file_pathInter = './interFEU1Inv/interFEU1_table.txt'
GdataInter, XdataInter, YdataInter = parse_file(file_pathInter, 0)

file_pathPlein = './asaFEU2Inv/asaFEU2_table.txt'
GdataPlein, XdataPlein, YdataPlein = parse_file(file_pathPlein, 0)

file_pathAsa = './asaFEU4Inv/asaFEU4_table.txt'
GdataAsa, XdataAsa, YdataAsa = parse_file(file_pathAsa, 0)


def plotPitch(XdataStrip, YdataStrip, XdataInter, YdataInter, val, yTitle, graphName):
    fig = plt.figure( figsize=(10, 8) )
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

    for i, x in enumerate(XdataStrip):
        xD = x[0]
        if(YdataStrip[i][0]==0.5):
            xD = xD - 0.1
        elif(YdataStrip[i][0]==1.5):
            xD = xD + 0.1
        if val==4:
            plt.errorbar(xD, x[val], xerr=0.0, yerr=x[5], fmt='b.', markersize=15, capsize=5,    label='stripFEU1 Xstrips, inter = 0.1 ')
        else :
            plt.plot(xD, x[val], 'b.', markersize=15, label='stripFEU1 Xstrips, inter = 0.1 ')
        # plt.annotate(f'{YdataStrip[i][0]}', (x[0], x[4]))

    for i, x in enumerate(YdataStrip):
        xD = x[0]
        if(XdataStrip[i][0]==0.5):
            xD = xD - 0.1
        elif(XdataStrip[i][0]==1.5):
            xD = xD + 0.1
        if val==4:
            plt.errorbar(xD, x[val], xerr=0.0, yerr=x[5], fmt='r.', markersize=15, capsize=5,    label='stripFEU1 Ystrips, inter = 0.75')
        else :
            plt.plot(xD, x[val], 'r.', markersize=15, label='stripFEU1 Ystrips, inter = 0.75')

    for i, x in enumerate(XdataInter):
        if val==4:
            plt.errorbar(x[0], x[val], xerr=0.0, yerr=x[5], fmt='b^', markersize=15, capsize=5,  label='interFEU1 Xstrips, inter = 0.1 ')
        else :
            plt.plot(x[0], x[val], 'b^', markersize=15, label='interFEU1 Xstrips, inter = 0.1 ')
        # plt.annotate(f'{YdataInter[i][0]}', (x[0], x[4]))

    for i, x in enumerate(YdataInter):
        if val==4:
            plt.errorbar(x[0], x[val], xerr=0.0, yerr=x[5], fmt='r^', markersize=15, capsize=5,  label='interFEU1 Ystrips, inter = 0.5 ')
        else :
            plt.plot(x[0], x[val], 'r^', markersize=15, label='interFEU1 Ystrips, inter = 0.5 ')
        # plt.annotate(f'{XdataInter[i][0]}', (x[0], x[4]))

    # Define the line styles
    line_styles = [Line2D([0], [0], color='b', lw=0, linestyle='-', marker='.', markersize=15),
                Line2D([0], [0], color='r', lw=0, linestyle='-', marker='.', markersize=15),
                Line2D([0], [0], color='b', lw=0, linestyle='-', marker='^', markersize=15),
                Line2D([0], [0], color='r', lw=0, linestyle='-', marker='^', markersize=15)]

    # Define the labels
    labels = ['D1 X, strip=100% pitch', 'D1 Y, strip=25% pitch',
              'D2 X, strip=100% pitch', 'D2 Y, strip=50% pitch']

    # Create the legend
    plt.legend(handles=line_styles, labels=labels, fontsize=16)

    ax1.set_xlim(0.3, 1.7)
    ax2.set_xlim(ax1.get_xlim())
    ax1.set_xlabel('Pitch [mm]')

    if(val==2) : ax1.set_ylim(1.15, 3.1)
    if(val==3) : ax1.set_ylim(0.3, 0.7)

    if val == 4:
        ax1.set_ylabel(yTitle+' [mm]')
    else :
        ax1.set_ylabel(yTitle)

    ax1.xaxis.grid(True)
    ax2.xaxis.grid(True)
    for side in ['bottom', 'right', 'top', 'left']:
        ax1.spines[side].set_linewidth(2)
        ax2.spines[side].set_linewidth(2)

    plt.title(yTitle+' vs Pitch')
    # plt.show()
    fig.savefig(graphName)


def plotPitchAsa(XdataStrip, YdataStrip, XdataPlein, YdataPlein, val, yTitle, graphName):
    fig = plt.figure( figsize=(10, 8) )
    fig.subplots_adjust(left=0.1, right=0.9, bottom=0.1, top=0.9)
    ax1 = fig.add_subplot(111)

    Xtick_locations = [0.8, 1., 1.5, 2.]
    Xtick_labels = [0.8, 1., 1.5, 2.]

    ax1.set_xticks(Xtick_locations)
    ax1.set_xticklabels(Xtick_labels)

    # plt.plot([x[0] for x in XdataStrip], [x[4] for x in XdataStrip], 'b.', label='stripFEU1 Xstrips, inter = 0.1 ', markersize=15)
    # plt.annotate(f'{[x[0] for x in YdataStrip]}', ([x[0] for x in XdataStrip], [x[4] for x in XdataStrip]))
    # plt.plot([x[0] for x in YdataStrip], [x[4] for x in YdataStrip], 'r.', label='stripFEU1 Ystrips, inter = 0.75', markersize=15)
    # plt.plot([x[0] for x in YdataInter], [x[4] for x in XdataInter], 'b^', label='interFEU1 Xstrips, inter = 0.1 ', markersize=15)
    # plt.plot([x[0] for x in YdataInter], [x[4] for x in YdataInter], 'r^', label='interFEU1 Ystrips, inter = 0.5 ', markersize=15)
    for i, x in enumerate(XdataStrip):
        if val==4:
            plt.errorbar(x[0], x[val], xerr=0.0, yerr=x[5], fmt='k.', markersize=15, capsize=5)
        else :
            plt.plot(x[0], x[val], 'k.', markersize=15)
        # plt.annotate(f'{YdataStrip[i][0]}', (x[0], x[4]))

    for i, x in enumerate(YdataStrip):
        if val==4:
            plt.errorbar(x[0], x[val], xerr=0.0, yerr=x[5], fmt='m.', markersize=15, capsize=5)
        else :
            plt.plot(x[0], x[val], 'm.', markersize=15)

    for i, x in enumerate(XdataPlein):
        if val==4:
            plt.errorbar(x[0], x[val], xerr=0.0, yerr=x[5], fmt='k^', markersize=15, capsize=5)
        else :
            plt.plot(x[0], x[val], 'k^', markersize=15)
        # plt.annotate(f'{YdataInter[i][0]}', (x[0], x[4]))

    for i, x in enumerate(YdataPlein):
        if val==4:
            plt.errorbar(x[0], x[val], xerr=0.0, yerr=x[5], fmt='m^', markersize=15, capsize=5)
        else :
            plt.plot(x[0], x[val], 'm^', markersize=15)
        # plt.annotate(f'{XdataInter[i][0]}', (x[0], x[4]))

    # Define the line styles
    line_styles = [Line2D([0], [0], color='k', lw=0, linestyle='-', marker='.', markersize=15),
                Line2D([0], [0], color='m', lw=0, linestyle='-', marker='.', markersize=15),
                Line2D([0], [0], color='k', lw=0, linestyle='-', marker='^', markersize=15),
                Line2D([0], [0], color='m', lw=0, linestyle='-', marker='^', markersize=15)]

    # Define the labels
    labels = ['D3 X, resist strips', 'D3 Y, resist strips',
            'D4 X, resist full', 'D4 Y, resist full']

    # Create the legend
    plt.legend(handles=line_styles, labels=labels, fontsize=16)

    ax1.set_xlim(0.5, 2.2)
    ax1.set_xlabel('Pitch [mm]')
    ax1.xaxis.grid(True)
    for side in ['bottom', 'right', 'top', 'left']:
        ax1.spines[side].set_linewidth(2)
    
    if(val==2) : ax1.set_ylim(1.15, 3.1)
    if(val==3) : ax1.set_ylim(0.3, 0.7)

    if val == 4:
        ax1.set_ylabel(yTitle+' [mm]')
    else :
        ax1.set_ylabel(yTitle)

    # fig.legend()
    plt.title(yTitle+' vs Pitch')
    # plt.show()
    fig.savefig(graphName)

plotPitch(XdataStrip, YdataStrip, XdataInter, YdataInter, 4, 'Residues', 'Residues_vs_Pitch.png')
plotPitch(XdataStrip, YdataStrip, XdataInter, YdataInter, 2, 'Cluster size', 'clsize_vs_Pitch.png')
plotPitch(XdataStrip, YdataStrip, XdataInter, YdataInter, 3, 'Amplitude fraction', 'amp_vs_Pitch.png')

plotPitchAsa(XdataAsa, YdataAsa, XdataPlein, YdataPlein, 4, 'Residues', 'Residues_vs_Pitch_Asa.png')
plotPitchAsa(XdataAsa, YdataAsa, XdataPlein, YdataPlein, 2, 'Cluster size', 'clsize_vs_Pitch_Asa.png')
plotPitchAsa(XdataAsa, YdataAsa, XdataPlein, YdataPlein, 3, 'Amplitude fraction', 'amp_vs_Pitch_Asa.png')
