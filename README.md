Polar F6/F11 Exercise Extraction Tool
=====================================
The f6-split-tool is a little helper tool to extract the exercise data in RAW data files from Polar F6 and F11 heartrate monitors on Linux. The RAW data can be captured by using the F6's SonicLink and the [rs200-decoder](http://sourceforge.net/projects/rs200-decoder) tool by Jani Hursti. The split tool is neccessary to make the gathered data usable for [SportsTracker / PolarViewer](http://www.saring.de/sportstracker/index.html) by Stefan Saring.


Download and Installation
-------------------------
The tool has no dependencies. The installation follows the standard installation procedure. The program is free software and licensed under the GNU GPL v2. Download the tarball below.

Current version:

* [Version 0.1.0](https://github.com/rhostettler/f6-split-tool/archive/v0.1.0.zip)

Older versions:

* [Version 0.0.3](https://github.com/rhostettler/f6-split-tool/archive/v0.0.3.zip)
* [Version 0.0.2](https://github.com/rhostettler/f6-split-tool/archive/v0.0.2.zip)
* [Version 0.0.1](https://github.com/rhostettler/f6-split-tool/archive/v0.0.1.zip)

Unpack the tarball and compile the code:

    $ tar xf f6-split-tool-x.y.z.tar.gz
    $ cd f6-split-tool-x.y.z/
    $ ./configure
    $ make
    # make install


Usage
-----
First you need to transmit the data from your Polar F6 heartrate monitor to your computer using rs200-decode:

    $ rs200_decode -m -b -o dump.bin

After that, use the f6-split-tool to extract the exercises from the previously gathered dump:

	$ f6-split-tool **watch** dump.bin

where **watch** is either --f6 or --f11.

This will generate up to 12 individual files for each exercise. The files are called *exercise_X.frd* where X is a number between 0 and 11. 0 is the most recent exercise whereas exercise 11 is the oldest in the watch's memory. The exercise files will also contain the total cumulative exercise duration as well as the cumulative calories burned (this is added by the f6-split-tool). The frd-files can then be imported into SportsTracker/PolarViewer.


Bugs and Comments
-----------------
There are no known bugs. But please contact me if you have comments or suggestions.


Changes
-------
* 2012-01-10, Version 0.1.0:

    Improved the naming scheme of output files, these are now named according to the exercise date.

* 2008-09-16, Version 0.0.3:

    Support for Polar F11 watches (thanks to Dominik Schmidt).

