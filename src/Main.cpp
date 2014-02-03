
#include "Bathy.h"

int main()
{
	cout<<"Start\n";

	string inputFile = "himbsyn.bathytopo.1km.v19.grd",
		   inputFileType="netcdf",
		   seriesName="z";
	int startX = 500,
	    startY = 400,
	    XDist = 30,
	    YDist = 30;
	long timestamp = -1;
	bool debug = false;

	Grid tGrid = getBathy(inputFile, inputFileType, startX, startY,XDist, YDist, seriesName, timestamp, debug);
}
/*
   #include <unistd.h>
   #include <stdio.h>
   #include <errno.h>

   int main() {
       char cwd[1024];
       if (getcwd(cwd, sizeof(cwd)) != NULL)
           fprintf(stdout, "Current working dir: %s\n", cwd);
       else
           perror("getcwd() error");
       return 0;
   }*/
