import tkinter as tk                    
from tkinter import ttk

from matplotlib.figure import Figure
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, 
NavigationToolbar2Tk)

import uproot as up
import awkward as awk
import numpy as np
import sys

# ================================================================
""" 
Read the data file in arrays
Prepare the histograms for
- Amplitude vs channel
- Amplitude vs sample
- ...
"""

fname = sys.argv[-1]
fin = up.open( fname )
nt = fin.get('nt')

ampl  = nt.arrays()[ 'amplitude' ]
ch    = nt.arrays()[ 'channel']
sample= nt.arrays()[ 'sample' ]

dream = np.floor(ch/64)
drmax = np.max(dream)
maxsample = np.max(sample)

# --------
figAvsS = Figure(figsize=(15,8))
for i in range(8):
    idream  = dream == i
    iampl   = awk.flatten(ampl[ idream ]).to_numpy()
    isample = awk.flatten(sample[ idream ]).to_numpy()

    ax = figAvsS.add_subplot(241+i)
    h = ax.hist2d( isample, iampl, bins=[maxsample+1,1000], range=[[0,maxsample+1],[0,4000]], cmin=1, norm = mpl.colors.LogNorm()  )
    ax.set_ylabel("amplitude")
    ax.set_xlabel("sample")
    ax.set_title( "Dream {}".format(1+i), fontdict={ 'color' : '#f01010'} )
figAvsS.tight_layout()

# --------
figAvsC = Figure(figsize=(15,8))
for i in range(8):
    idream  = dream == i
    iampl   = awk.flatten(ampl[ idream ]).to_numpy()
    ich     = awk.flatten(ch[ idream ]).to_numpy()%64

    ax = figAvsC.add_subplot(241+i)
    htest, xedges, yedges, v = ax.hist2d( ich, iampl, bins=[64,200], range=[[0,64],[0,4000]], cmin=1, norm = mpl.colors.LogNorm() )
    ax.set_ylabel("amplitude")
    ax.set_xlabel("channel")
    ax.set_title( "Dream {}".format(1+i), fontdict={ 'color' : '#f01010'} )
figAvsC.tight_layout()




root = tk.Tk()
root.title("Tab Widget")
#root.resizable(False,False)

frm = ttk.Frame(root,  padding=10 )

def update():
  print( "update" )
bt = ttk.Button( frm, text="Update", command=update )
bt.pack(side=tk.LEFT,pady=20)
frm.pack()
  
frm2 = ttk.Frame(root,  padding=10 )
frm2.pack()
tabControl = ttk.Notebook(frm2)

tab1 = ttk.Frame(tabControl)
tab2 = ttk.Frame(tabControl)

tabControl.add(tab1, text ='Tab 1')
tabControl.add(tab2, text ='Tab 2')
tabControl.pack(expand = 1, fill ="both")


canvas1 = FigureCanvasTkAgg(figAvsS,
                            master = tab1)  
canvas1.draw()
canvas1.get_tk_widget().pack(expand=True, fill="both")



canvas2 = FigureCanvasTkAgg(figAvsC,
                            master = tab2)
canvas2.draw()
canvas2.get_tk_widget().pack(expand=True, fill="both")

root.mainloop()  
