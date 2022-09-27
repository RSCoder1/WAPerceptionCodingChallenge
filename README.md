# WAPerceptionCodingChallenge

Methodology
1. Generate image mask that isolates cone colored pixels
2. Further process image so any small artifacts are not detected using
   the area of each cone blob
3. Find centroid of each cone blob of pixels using moments
4. Locate the starting cone and ending cone using their x,y coordinates
5. Isolate each starting and ending cone based on their position on the left
   of the image or the right
6. Connect the two cones' centroids using a line
7. Scale the line so it extends beyond the cones in their direction
8. Save modified image

---------------------------------------------------------------------------

What did you try and why do you think it did not work.

I initially tried to use a set value for each of the HSV values. However, it did
not work because different angles of lighting and shadows produce different HSV 
values. Thus, I solved this issue by using a range of values for the cones. I 
also thought only one centroid per cone would be found. However, multiple
centroids were detected which resulted in misaligned points for the cones. To fix 
this, I placed a check that removed any centroids which were very close to existing 
ones.

---------------------------------------------------------------------------

Libraries Used:
OpenCV
