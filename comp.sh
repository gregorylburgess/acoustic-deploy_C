g++ -std=c++0x -I/home/epy00n/Desktop/workspace/acoustic-deploy_C/src/Eigen -I/usr/include/netcdf.h -O3 -Wall -c -fmessage-length=0 -o src/Test/TestGoodness.o src/Test/TestGoodness.cpp 
g++ -std=c++0x -I/home/epy00n/Desktop/workspace/acoustic-deploy_C/src/Eigen -I/usr/include/netcdf.h -O3 -Wall -c -fmessage-length=0 -o src/Utility.o src/Utility.cpp 
g++ -std=c++0x -I/home/epy00n/Desktop/workspace/acoustic-deploy_C/src/Eigen -I/usr/include/netcdf.h -O3 -Wall -c -fmessage-length=0 -o src/Bathy.o src/Bathy.cpp 
g++ -std=c++0x -I/home/epy00n/Desktop/workspace/acoustic-deploy_C/src/Eigen -I/usr/include/netcdf.h -O3 -Wall -c -fmessage-length=0 -o src/GlobalVars.o src/GlobalVars.cpp 
g++ -std=c++0x -I/home/epy00n/Desktop/workspace/acoustic-deploy_C/src/Eigen -I/usr/include/netcdf.h -O3 -Wall -c -fmessage-length=0 -o src/Graph.o src/Graph.cpp 
g++ -std=c++0x -I/home/epy00n/Desktop/workspace/acoustic-deploy_C/src/Eigen -I/usr/include/netcdf.h -O3 -Wall -c -fmessage-length=0 -o src/Grid.o src/Grid.cpp 
g++ -std=c++0x -I/home/epy00n/Desktop/workspace/acoustic-deploy_C/src/Eigen -I/usr/include/netcdf.h -O3 -Wall -c -fmessage-length=0 -o src/Sensors.o src/Sensors.cpp 
g++ -std=c++0x -I/home/epy00n/Desktop/workspace/acoustic-deploy_C/src/Eigen -I/usr/include/netcdf.h -O3 -Wall -c -fmessage-length=0 -o src/FishModel.o src/FishModel.cpp 
g++ -std=c++0x -I/home/epy00n/Desktop/workspace/acoustic-deploy_C/src/Eigen -I/usr/include/netcdf.h -O3 -Wall -c -fmessage-length=0 -o src/Goodness.o src/Goodness.cpp 
g++ -std=c++0x -I/home/epy00n/Desktop/workspace/acoustic-deploy_C/src/Eigen -I/usr/include/netcdf.h -O3 -Wall -c -fmessage-length=0 -o src/Test/Test.o src/Test/Test.cpp 
g++ -std=c++0x -I/home/epy00n/Desktop/workspace/acoustic-deploy_C/src/Eigen -I/usr/include/netcdf.h -O3 -Wall -c -fmessage-length=0 -o src/Main.o src/Main.cpp 
g++ -L/usr/include -o acoustic-deploy_C src/Utility.o src/Test/TestGoodness.o src/Test/Test.o src/Sensors.o src/Main.o src/Grid.o src/Graph.o src/Goodness.o src/GlobalVars.o src/FishModel.o src/Bathy.o -lnetcdf 

