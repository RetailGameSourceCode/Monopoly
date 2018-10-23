#include <iostream.h>
#include <time.h>
#include "pc3d.h"
#include "xvrml.h"


int main(int argc, char **argv)
{
char stime[32], sdate[32];
if (argc < 2 || argc > 3) 
	{
	cout << "XVRML build: \t" << _strdate(sdate) << "...." << _strtime(stime) << "  (C) Artech Studios, 1997" << "\n";
	cout << "Extracts 3D scene information from a VRML file. \n\n";
	cout << "usage:" << "xvrml filename[.wrl] [nodename] [-v]\n";
	cout << "	nodename : extract info for a specified node only\n";
	cout << "	-v	 : verbose (not yet implemented)\n";

	return 0;
	}

	char *nodename = 0;
	if (argc == 3)
		nodename = argv[2];
	frame *f = VRMLextractScene( (char *) argv[1], nodename );
	if (!f)
		return -1;

return 0;
}
