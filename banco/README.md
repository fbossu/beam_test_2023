# Visualizing BANCO Data

## Files Required

To visualize the data from BANCO, make sure you have two types of files: a Noise file (examples available on alicemftsac (Test_Beam -> data -> Noise_files)) and a root file of the RUN for each ladder. These files will be useful for the rest of the code.

## Coordinates

In this implementation, the coordinates used are the pixels from the ladder, considering the (0,0) coordinate as the (0,0) of the first chip of the first ladder. We have also modified the coordinates from 3 coordinates (chipId, row, col) to row and col, where $col=col+(chipId-4) \times 1024$.

One has to add the Z coordinate to those (x,y) coordinates manually, it is better to had them in pixels.

## Clustering

The clustering is performed using the AgglomerativeClustering from the sklearn library.

## Alignment

This version of the code implements two alignment techniques. The first one uses residuals from a 3-point fitting, whereas the second compute the derivative according to the different rotations and translations. It computes it in a CSV and it has then to be converted to a mille file using th C++ repository then, one can try using pede to compute misalignments.

## Tracking

Tracking has not been implemented yet, but there are some basic code to do track but not with a good resolution.

## Type of Dataframe (VERY IMPORTANT)

Data: It's a dataframe representing all the hits in the (x,y,z) space (be sure to add the Z value which is not there by default) and on which ladder

Tracks : It's all the computation of four hits (cluster) on four different planes, the information given are the slope and intersect in X and Y, and the different residuals. To be changed, is that for the moment there are two type of tracks :

• One having the global information : 'trgNum','Mean_Residuals_X','Mean_Residuals_Y','slope_X','intercept_X','slope_Y','intercept_Y'

• The other being more specific to a ladder 'trgNum','Mean_Residuals_X','Mean_Residuals_Y','slope_X','intercept_X','slope_Y','intercept_Y','ldr','Residual_X_ldr_id','Residual_Y_ldr_id','Residual_Z_ldr_id','z_inter_ldr_id', with the intersection of the track with the plane and the specific residual. 


Ladder : This object define the ladder with it's given coordinates (x,y,z) of the origin and rotation
({'id','theta','phi','psi','x','y','z'})

# Step by step analysis

## Open a file 

To create a filtered data file please follow those operation within the notebook :

$Noise1=open_noise('../Noise1')  
Noise2=open_noise('../Noise2')
Noise3=open_noise('../Noise3')
Noise4=open_noise('../Noise4')$

Please be sure to enter the relative path of your Noise file in the parenteses. 

Then :

$data=open_file("../Data1")
data2=open_file("../Data2")
data3=open_file("../Data3")
data4=open_file("../Data4")

data=pd.concat([data,data2])
data=pd.concat([data,data3])
data=pd.concat([data,data4])$

It enables to create a big data file with all the hits. 

Then, one can filter the given data :

$data_filtered=remove_noise(Noise1,data[data.ldr==1])
data_filtered=pd.concat([data_filtered,remove_noise(Noise2,data[data.ldr==2])])
data_filtered=pd.concat([data_filtered,remove_noise(Noise2,data[data.ldr==3])])
data_filtered=pd.concat([data_filtered,remove_noise(Noise2,data[data.ldr==4])])$

This has to be upgrade to avoid those concatenation and make them inside the different functions. 

Be careful then to flip your data (the odd ladder are flipped comapred to the even one (recto/verso)) :

$data_filtered[(data_filtered.ldr==2)]=flip_data_row(data_filtered[(data_filtered.ldr==2)])
data_filtered[(data_filtered.ldr==4)]=flip_data_row(data_filtered[(data_filtered.ldr==4)])$

## Reducing 

Before, doing any clusterization and recution it is better to start by a manual focusing on the interest chip (with a given chipId) with :

$data_focused=data_filtered[data_filtered.chipId==chipId]$

And then clustering and reducing :

$do_clusters(data_focused)
data_reduced=reduce_by_barycenter_new (data_clusterized)$

## Analysing 

To compute the distribution of a cluster size within your data. You can run :

$cluster_size(data_clusterized)$

And it will retrieve the length of a each cluster in a list (it can then be plotted on an histogram) : there are few useful function in cell 13 and 14. and Efficiency in cell 16.

## Alignment 



