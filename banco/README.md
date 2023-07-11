# Visualizing BANCO Data

## Files Required

To visualize the data from BANCO, make sure you have two types of files: a Noise file (examples available on alicemftsac (Test_Beam -> data -> Noise_files)) and a root file of the RUN for each ladder. These files will be useful for the rest of the code.

## Coordinates

In this implementation, the coordinates used are the pixels from the ladder, considering the (0,0) coordinate as the (0,0) of the first chip of the first ladder. We have also modified the coordinates from 3 coordinates (chipId, row, col) to row and col, where $col=col+(chipId-4) \times 1024$.


## Clustering

The clustering is performed using the AgglomerativeClustering from the sklearn library.

## Alignment

This version of the code implements two alignment techniques. The first one uses residuals from a 3-point fitting, whereas the second compute the derivative according to the different rotations and translations. It computes it in a CSV and it has then to be converted to a mille file using th C++ repository then, one can try using pede to compute misalignments.

## Tracking

Tracking has not been implemented yet, but there are some basic code to do track but not with a good resolution.
