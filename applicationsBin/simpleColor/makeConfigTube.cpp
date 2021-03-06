#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main(int argc,char **argv) {
    int radius=10,radiusInterne=0,height=20;
    int maxTimeSimulation=3000;

    if (argc>1) {
        radius = atoi(argv[1]);
        cout << "radius=" << radius << endl;
    }
    if (argc>2) {
        radiusInterne = atoi(argv[2]);
        cout << "radiusInterne=" << radiusInterne << endl;
    }
    if (argc>3) {
        height = atoi(argv[3]);
        cout << "height=" << height << endl;
    }
    if (argc>4) {
        maxTimeSimulation = atoi(argv[4]);
        cout << "maxTimeSimulation=" << maxTimeSimulation << endl;
    }

    int areaLx = 2*radius,
        areaLy = 2*radius,
        areaLz = ceil(M_SQRT2*height);
    int cameraDist = 35.0*radius/tan(50.0*M_PI/180.0);
    int srcDist = 35.0*radius/tan(45.0*M_PI/180.0);

    radius*=10.0;
    radiusInterne*=10.0;

    char titre[1024];
    sprintf(titre,"configMakeTube%d_%d_%d.xml",radius,radiusInterne,height);

    ofstream fout;
    fout.open(titre);

    fout << "<?xml version=\"1.0\" standalone=\"no\" ?>"<< endl;
    fout << "<world gridSize=\""<< areaLx+1 << "," << areaLy+1 << "," << areaLz+1 << "\" windowSize=\"1800,900\" maxSimulationTime=\"" << maxTimeSimulation << "mn\">" << endl;
  	fout << "<camera target=\""<< radius << "," << radius << "," << height/2 <<"\" directionSpherical=\"0,38," << cameraDist << "\" angle=\"50\" far=" << cameraDist*2 << "/>" << endl;
  	fout << "<spotlight target=\""<< radius << "," << radius << "," << height/2 << "\" directionSpherical=\"-30,50," << srcDist << "\" angle=\"45\"/>" << endl;
  	fout << "<blockList color=\"128,128,128\" blocksize=\"10.0,10.0,10.0\">" << endl;

    double x,y,z,dx,dy,d;
    int n=0;
    for (size_t iz = 0; iz < areaLz; iz++) {
        z = 2.5*M_SQRT2+5.0*M_SQRT2*iz;
        for (size_t iy = 0; iy < areaLy; iy++) {
            y = (iz%2)?10.0+iy*10.0:5.0+iy*10.0;
            for (size_t ix = 0; ix < areaLx; ix++) {
                x = (iz%2)?10.0+ix*10.0:5.0+ix*10.0;
                dx = x-radius;
                dy = y-radius;
                d = sqrt(dx*dx+dy*dy);
                //cout << "(" << ix << "," << iy << "," << iz << ") -> (" << x << "," << y << "," << z << ")" << endl;
                if (d<radius && d>=radiusInterne) {
                  fout << "<block position=\"" << ix << "," << iy << ","<< iz << "\" orientation=\"0\" />" << endl;
                  n++;
                }
            }
        }
    }
    cout << "nbre catoms:" << n << endl;

    fout << "</blockList>" << endl;
    fout << "</world>\n";
    fout.close();
    return 0;
}
