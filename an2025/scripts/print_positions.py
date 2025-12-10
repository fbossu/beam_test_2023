import json

with open('run_config.json') as file:
  data = json.load(file)

print( "banco moveable position: ", data['bench_geometry']['banco_moveable_y_position'] )

dets = {}
for det in data['detectors']:
  dets[ det['name'] ] =  det["det_center_coords"]['z'] - 500.

indets = data['included_detectors']

for idet in indets:
  print( idet, " ", dets[idet] )
