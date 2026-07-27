# NibletteDiskMapper
A simple tool to map (external) disks, allowing you to parse through their content when unplugged.

This tool aims to an issue some people have professionally. 
Imagine a photographer, due to his occupation he will, over the years, accumulate a substantial quantity of data that he wants or has to keep.
This means dozens upon dozens of external disks, stacked on shelves. If our photographer needs to find a specific photo and doesn't remember exactly which disk it's in...

Niblette's disk mapper is here to solve exactly that!
With this tool, you can map your disks, creating a lightweight file that you can then use to view the filetree of the mapped disk and search for specific files/folders by name.


Build instructions windows:

1 - In a folder, place the "source" folder containing the .h .cpp and the CMakeLists.txt files of this project.

2 - In the same folder, create a "builddir" folder.

3 - Using the Qt installation tool, install a Qt6.11.1 version of Qt (my installation was at C:\Qt)

4 - In a cmd, add those 4 entries to you %PATH% variable:
      C:\Qt\Tools\CMake_64\bin
	    C:\Qt\Tools\Ninja
	    C:\Qt\Tools\mingw1310_64\bin
	    C:\Qt\6.11.1\mingw_64\bin

5 - With that cmd, go to the folder containg our "sources" and "builddir" folders, and execute the following commands:
      C:\Qt\6.11.1\mingw_64\bin\qt-cmake.bat -G Ninja -S sources -B builddir 
      cmake --build builddir

6 - In the builddir folder, grab NibletteDiskMapper.exe and place it in a folder where you want it installed (warning, many files will be created in that folder, we advise to be empty)

7 - Still with the same cmd, execute the following command:
      C:\Qt\6.11.1\mingw_64\bin\windeployqt6.exe PATH/TO/NibletteDiskMapper.exe

8 - Niblette Disk Mapper should now be ready to use !


FOR MAC:

1 - Download Qt6.11.1, download Cmake and Ninja

2 - in a folder containing the sources folder, create a builddir folder

3 - Execute the following commands:

	cmake -DCMAKE_PREFIX_PATH=/Path/To/Qt/Installation/6.11.1/macos/ -G Ninja -S sources -B builddir

	cmake --build buildir

NOTE: IN the future I plan to directly release statically linked  (ie ready to use) executables.
