#!/usr/bin/env python
# coding: utf-8

import uproot as up
import matplotlib as mpl
import matplotlib.pyplot as plt
import awkward as awk
import numpy as np
import sys

plt.ion()

fname = sys.argv[-1]
fin = up.open( fname )
nt = fin.get('events')
print(nt.keys())



''' data as awkward arrays'''
nt.keys()


ampl  = nt.arrays()[ 'hits.maxamp' ]
ch    = nt.arrays()[ 'hits.channel']
sample= nt.arrays()[ 'hits.samplemax']



dream = np.floor(ch/64)
drmax = np.max(dream)
maxsample = np.max(sample)



#fig = plt.figure(figsize=(15,8))

#for i in range(8):
    #idream  = dream == i
    #iampl   = awk.flatten(ampl[ idream ]).to_numpy()
    #isample = awk.flatten(sample[ idream ]).to_numpy()
    
    #ax = fig.add_subplot(241+i)
    #h = ax.hist2d( isample, iampl, bins=[maxsample+1,1000], range=[[0,maxsample+1],[0,4000]], cmin=1 )
    #ax.set_ylabel("amplitude")
    #ax.set_xlabel("sample")
    #ax.set_title( "Dream {}".format(1+i), fontdict={ 'color' : '#f01010'} )
#plt.tight_layout()

#plt.show()




fig = plt.figure(figsize=(15,8))

for i in range(8):
    idream  = dream == i
    iampl   = awk.flatten(ampl[ idream ]).to_numpy()
    ich    = awk.flatten(ch[ idream ]).to_numpy()%64
    
    ax = fig.add_subplot(241+i)
    # h = ax.hist2d( ich, iampl, bins=[64,40], range=[[(int)(i)*64,(int)(i+1)*64],[0,2000]], cmin=1 )
    htest, xedges, yedges, v = ax.hist2d( ich, iampl, bins=[64,200], range=[[0,64],[0,4000]], cmin=1, norm = mpl.colors.LogNorm() )
    ax.set_ylabel("amplitude")
    ax.set_xlabel("channel")
    ax.set_title( "Dream #{}".format(1+i), fontdict={ 'color' : '#f01010'} )
#fig.colorbar(v)
plt.tight_layout()
fig.savefig( fname[:-4] + "png" )

#plt.show()





