# What is Millenium-Cell

Millenium-Cell is a cell morphogenesis simulation. It's a student project.

# Build instruction

Make sure you have installed all the dependencies :

* CMake
* Boost
* VTK
* HDF5
* ODBC
* OpenMPI
* GDAL
* LibPQ

There may be other dependencies. If you have build error with undefined reference, search the web for the missing libraries and install them. Please add them to the list of dependencies above.

```shell
git clone https://github.com/kwon-young/Millenium-Cell.git
cd Millenium-Cell
mkdir build
cd build
cmake ..
make
./Millenium-Cell
```

