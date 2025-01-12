#include "shapes.h"
#include <mutex>

class RubikCube{

public:

    Cube cubes[3][3][3];

    // Crea un Cubo rubik en el centro
    RubikCube() {

        // Natural Coords (Center cube)
        const float height = 0.5;

        // Build Cube
        const float padding = 0.08;
        const float translateFactor = height*2 + padding;

        float x1, y1, z1;
        float x2, y2, z2;

        // Loop through the 3x3x3 grid
        for (int i = 0; i < 3; ++i) { // Y axis
            for (int j = 0; j < 3; ++j){ // X axis
                for (int k = 0; k < 3; ++k){ // Z axis

                    x1 = -height + (j-1) * translateFactor;
                    y1 = -height + (i-1) * translateFactor;
                    z1 = -height + (k-1) * translateFactor;

                    x2 = +height + (j-1) * translateFactor;
                    y2 = +height + (i-1) * translateFactor;
                    z2 = +height + (k-1) * translateFactor;

                    cubes[i][j][k] = Cube(x1, y1, z1, x2, y2, z2);

                    for (int h = 0; h < 6; ++h) cubes[i][j][k].setColor(h, 0.0f,0.0f,0.0f);

                    if (i == 0) cubes[i][j][k].setColor(2, 1.0,1.0,1.0);    // White (Bottom)
                    if (i == 2) cubes[i][j][k].setColor(3, 1, 0.984, 0);    // Yellow (Up)
                    if (j == 0) cubes[i][j][k].setColor(1, 0, 0.447, 0.8);  // Blue (Left)
                    if (j == 2) cubes[i][j][k].setColor(4, 0.2, 0.788, 0);  // Green (Right)
                    if (k == 2) cubes[i][j][k].setColor(5, 0.902, 0, 0);    // Red (Front)
                    if (k == 0) cubes[i][j][k].setColor(0, 0.902, 0.373, 0);// Orange (Behind)

                }
            }
        }        
    }

    void render(int type = 0){
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j){
                for (int k = 0; k < 3; ++k){
                    if (i == j && i == k && k == 1) continue; // Center Cube (Not render)
                    cubes[i][j][k].render(type);
                }
            }
        }   
    }


    void left(){

        // rotate cubes
        vector<float> center = cubes[1][0][1].getCenter();
        for (int y = 0; y < 3; ++y){
            for (int z = 0; z < 3; ++z) cubes[y][0][z].rotateAxis(M_PI/2,0.0f,0.0f,center);
        }

        // Rotate in mat
        Cube auxArr[3];

        for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][0][i]; // Save Top
        for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2-i][0][2]); // Move top to front and save front
        for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][0][2-i]); // Move front to down and save down
        for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][0][0]); // Move down to back and save back
        cubes[2][0][1] = auxArr[1];

    }

    void right(){

        // rotate cubes
        vector<float> center = cubes[1][2][1].getCenter();
        for (int y = 0; y < 3; ++y){
            for (int z = 0; z < 3; ++z) cubes[y][2][z].rotateAxis(M_PI/2,0.0f,0.0f,center);
        }

        // Rotate in mat
        Cube auxArr[3];

        for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][2][i]; // Save Top
        for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2-i][2][2]); // Move top to front and save front
        for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][2][2-i]); // Move front to down and save down
        for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][2][0]); // Move down to back and save back
        cubes[2][2][1] = auxArr[1];

    }

    void up(){

        // rotate cubes
        vector<float> center = cubes[2][1][1].getCenter();
        for (int x = 0; x < 3; ++x){
            for (int z = 0; z < 3; ++z) cubes[2][x][z].rotateAxis(0.0f,M_PI/2,0.0f,center);
        }

        // Rotate in mat
        Cube auxArr[3];

        for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][i][2]; // Save Front
        for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2][2][2 - i]); // Move front to right and save right
        for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2][2-i][0]); // Move right to back and save back
        for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[2][0][i]); // Move back to left and save left
        cubes[2][1][2] = auxArr[1];
        
    }

    void down(){

        // rotate cubes
        vector<float> center = cubes[0][1][1].getCenter();
        for (int x = 0; x < 3; ++x){
            for (int z = 0; z < 3; ++z) cubes[0][x][z].rotateAxis(0.0f,M_PI/2,0.0f,center);
        }

        // Rotate in mat
        Cube auxArr[3];

        for (int i = 0; i < 3; ++i) auxArr[i] = cubes[0][i][2]; // Save Front
        for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][2][2 - i]); // Move front to right and save right
        for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][2-i][0]); // Move right to back and save back
        for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[0][0][i]); // Move back to left and save left
        cubes[0][1][2] = auxArr[1];
        
    }

    void front(){

        // rotate cubes
        vector<float> center = cubes[1][1][2].getCenter();
        for (int y = 0; y < 3; ++y){
            for (int x = 0; x < 3; ++x) cubes[y][x][2].rotateAxis(0.0f,0.0f,M_PI/2,center);
        }

        // Rotate in mat
        Cube auxArr[3];

        for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][i][2]; // Save Front
        for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2-i][2][2]); // Move front to right and save right
        for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][2-i][2]); // Move right to bottom and save bottom
        for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][0][2]); // Move bottom to left and save left
        cubes[2][1][2] = auxArr[1];

    }

    void back(){

        // rotate cubes
        vector<float> center = cubes[1][1][0].getCenter();
        for (int y = 0; y < 3; ++y){
            for (int x = 0; x < 3; ++x) cubes[y][x][0].rotateAxis(0.0f,0.0f,M_PI/2,center);
        }

        // Rotate in mat
        Cube auxArr[3];

        for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][i][0]; // Save Front
        for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2-i][2][0]); // Move front to right and save right
        for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][2-i][0]); // Move right to bottom and save bottom
        for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][0][0]); // Move bottom to left and save left
        cubes[2][1][0] = auxArr[1];

    }


    vector<float> getCenter(){
        vector<float> center = {
            (float) (cubes[1][1][1].rects[0].coords[0] + cubes[1][1][1].rects[3].coords[0]) / 2, 
            (float) (cubes[1][1][1].rects[0].coords[1] + cubes[1][1][1].rects[3].coords[1]) / 2, 
            (float) (cubes[1][1][1].rects[0].coords[2] + cubes[1][1][1].rects[3].coords[2]) / 2
        };
        return center;
    }

    void translate(float tx, float ty, float tz){
        for (int i = 0; i < 3; ++i) { 
            for (int j = 0; j < 3; ++j){ 
                for (int k = 0; k < 3; ++k) cubes[i][j][k].translate(tx,ty,tz);
            }
        }
    }

    void scale(float factorX, float factorY, float factorZ){
        vector<float> center = getCenter();
        for (int i = 0; i < 3; ++i) { 
            for (int j = 0; j < 3; ++j){ 
                for (int k = 0; k < 3; ++k) cubes[i][j][k].scaleAxis(factorX,factorY,factorZ, center);
            }
        }
    }

    void scaleAxis(float factorX, float factorY, float factorZ, vector<float> center){
        for (int i = 0; i < 3; ++i) { 
            for (int j = 0; j < 3; ++j){ 
                for (int k = 0; k < 3; ++k) cubes[i][j][k].scaleAxis(factorX,factorY,factorZ, center);
            }
        }
    }

    // Indiv Rotations dont make sense
    void rotateX(float radian){
        for (int i = 0; i < 3; ++i) { 
            for (int j = 0; j < 3; ++j){ 
                for (int k = 0; k < 3; ++k) cubes[i][j][k].rotateX(radian);
            }
        }
    }

    void rotateY(float radian){
        for (int i = 0; i < 3; ++i) { 
            for (int j = 0; j < 3; ++j){ 
                for (int k = 0; k < 3; ++k) cubes[i][j][k].rotateY(radian);
            }
        }
    }

    void rotateZ(float radian){
        for (int i = 0; i < 3; ++i) { 
            for (int j = 0; j < 3; ++j){ 
                for (int k = 0; k < 3; ++k) cubes[i][j][k].rotateZ(radian);
            }
        }
    }

    void rotate(float radianX, float radianY, float radianZ){
        vector<float> center = getCenter();
        for (int i = 0; i < 3; ++i) { 
            for (int j = 0; j < 3; ++j){ 
                for (int k = 0; k < 3; ++k) cubes[i][j][k].rotateAxis(radianX, radianY, radianZ, center);
            }
        }
    }

    void rotateAxis(float radianX, float radianY, float radianZ, vector<float> center){
        for (int i = 0; i < 3; ++i) { 
            for (int j = 0; j < 3; ++j){ 
                for (int k = 0; k < 3; ++k) cubes[i][j][k].rotateAxis(radianX, radianY, radianZ, center);
            }
        }
    }


};