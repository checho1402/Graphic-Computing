#include "shapes.h"

class RubikCube{

public:

    Cube cubes[3][3][3];

    // Crea un Cubo rubik en el centro
    RubikCube() {

        // Natural Coords (Center cube)
        const float height = 0.5;

        // Build Cube
        const float padding = 0.1;
        const float translateFactor = height*2 + padding;

        float x1, y1, z1;
        float x2, y2, z2;

        // Loop through the 3x3x3 grid
        for (int i = 0; i < 3; ++i) { // Y axis
            for (int j = 0; j < 3; ++j){ // X axis
                for (int k = 0; k < 3; ++k){ // Z axis

                    if (i == j && i == k && k == 1) continue; // Center Cube 

                    x1 = -height + (j-1) * translateFactor;
                    y1 = -height + (i-1) * translateFactor;
                    z1 = -height + (k-1) * translateFactor;

                    x2 = +height + (j-1) * translateFactor;
                    y2 = +height + (i-1) * translateFactor;
                    z2 = +height + (k-1) * translateFactor;

                    cubes[i][j][k] = Cube(x1, y1, z1, x2, y2, z2);


                    for (int h = 0; h < 6; ++h) cubes[i][j][k].setColor(h, randF(),randF(),randF());

                }
            }
        }        
    }

    void render(int type = 0){
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j){
                for (int k = 0; k < 3; ++k){
                    if (i == j && i == k && k == 1) continue; // Center Cube
                    cubes[i][j][k].render(type);
                }
            }
        }   
    }


};