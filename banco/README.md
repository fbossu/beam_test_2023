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

## Type of Dataframe (VERY IMPORTANT)

Data: It's a dataframe representing all the hits in the (x,y,z) space (be sure to add the Z value which is not there by default) and on which ladder

Tracks : It's all the computation of four hits (cluster) on four different planes, the information given are the slope and intersect in X and Y, and the different residuals. To be changed, is that for the moment there are two type of tracks :

• One having the global information : 'trgNum':[],'Mean_Residuals_X':[],'Mean_Residuals_Y':[],'slope_X':[],'intercept_X':[],'slope_Y':[],'intercept_Y':[]; 

• The other being more specific to a ladder 'trgNum':[],'Mean_Residuals_X':[],'Mean_Residuals_Y':[],'slope_X':[],'intercept_X':[],'slope_Y':[],'intercept_Y':[],'ldr':[],'Residual_X_'+str(ldr['id'][0]):[],'Residual_Y_'+str(ldr['id'][0]):[],'Residual_Z_'+str(ldr['id'][0]):[],'z_inter':[],'Residual_tot'+ldr['id'][0]):[], with the intersection of the track with the plane and the specific residual. 


Ladder : This object define the ladder with it's given coordinates (x,y,z) of the origin and rotation
({'id':[],'theta':[],'phi':[],'psi':[],'x':[],'y':[],'z':[]})


