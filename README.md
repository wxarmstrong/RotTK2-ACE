# RotTK2-ACE
This repository contains files pertaining to achieving tool-assisted arbitrary code execution in Romance of the Three Kingdoms II for NES.

# speedrun.bk2
This is the Bizhawk input file for an optimized tool-assisted speedrun of RotTK2 in 2:33.0. This does not constitute total control, but simply a small segment of arbitrary code to call the ending sequence. Further details are available from the movie's TASVideos page: http://tasvideos.org/6612S.html

# trollface.bk2
A demonstration of total-control, which writes the necessary code and data to print a greyscale photomosaic.
This is very unoptimized in every sense: the ACE setup, the writing of the graphics, and the graphic quality itself. This particular image uses only 25% of the available CHR RAM and could be vastly improved.

https://github.com/codebox/mosaic was used to produce the photomosaic in this case.

# mosaic.cpp
Given a directory of source frames, this program will output instructions of how to draw them as nametable graphics (in the form of a photomosaic) in Romance of the Three Kingdoms for NES.

All the game's CHR tiles are extracted (as chr.png). I used ImageMagick tools to apply the following palette set and split them up:

0F 0F 30 30 (black black white white)

0F 30 0F 30 (black white black white)

0F 30 30 0F (black white white black)

0F 30 30 30 (black white white white)

Then, I used a code to analyze them, sorting them by brightness (i.e. # of white pixels) and eliminating all duplicates. The data is then exported to root.txt (included here). This contains not only the pattern and tile IDs for each tile, but a hex hash that is used to perform the pixel matching.

Before using, the source frames should be shrunk down to the dimensions of the visible NES screen (256x232) and consist of only black and white pixels. For each 8x8 frame, the program will find the best pixel match among the game tiles. 

The code I used to actually produce root.txt from the original tile set (including sorting their brightnesses and eliminating duplicates) is included at the top but commented out.

To-do:
Use this program to produce an animation demonstration within Arbitrary Code Execution.
