import uproot as ur
import numpy as np
import matplotlib.pyplot as plt
import sys


fin = ur.open(sys.argv[-1])
events = fin['events']
samples = events.arrays('hits/hits.samples', library='np')['hits/hits.samples']
channel = events.arrays('hits/hits.channel', library='np')['hits/hits.channel']
axis = events.arrays('hits/hits.axis', library='np')['hits/hits.axis']
clusterId = events.arrays('hits/hits.clusterId', library='np')['hits/hits.clusterId']
eventId = events.arrays('eventId', library='np')['eventId']

x = np.linspace( 0, 15,16)

plt.ion()
plt.rcParams["figure.autolayout"] = True
fig = plt.figure( figsize=(10,6) )

def plot( evn ):
  global fig
  try:
    fig.clf()
  except:
    fig = plt.figure( figsize=(10,6) )
  ax = fig.add_subplot(121)
  ch = channel[ eventId == evn ][0] 
  aa = axis[ eventId == evn ][0]
  cc = clusterId[ eventId == evn ][0]
  ss = samples[ eventId == evn ][0]

  c = ch[ aa == 120 ]
  clid = cc[ aa == 120 ]
  for i,s in enumerate(ss[ aa == 120 ]):
      n = np.array(s)
      a = n[ n > 0 ]
      xx = x[ n > 0 ]
      ax.plot( xx, a,".-", label=str(c[i])+","+str(clid[i]) )
  ax.legend()
  ax.set_ylim(0, 4100)
  ax.set_xlim(0, 16)
  ax.set_title("X")
  ax.set_ylabel("amplitude")
  ax.set_xlabel("sample")

  ay = fig.add_subplot(122)

  c = ch[ aa == 121 ]
  clid = cc[ aa == 121 ]
  for i,s in enumerate(ss[ aa == 121 ]):
      n = np.array(s)
      a = n[ n > 0 ]
      xx = x[ n > 0 ]
      ay.plot( xx, a,".-", label=str(c[i])+","+str(clid[i]) )
  ay.legend()
  ay.set_ylim(0, 4100)
  ay.set_xlim(0, 16)
  ay.set_title("Y")
  ay.set_xlabel("sample")


 

