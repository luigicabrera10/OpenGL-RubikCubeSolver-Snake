#include "rubikCube.h"
#include <cmath>

float maxWidth = 50;
float maxHeight = 30; 

RubikCube* generateRubikCube(string &shufMovs){

    RubikCube* fruit = new RubikCube();

    float randX = (  ( (float) rand() / RAND_MAX) * maxWidth) -   ( (float) maxWidth / 2);
    float randY = (  ( (float) rand() / RAND_MAX) * maxHeight) -  ( (float) maxHeight / 2);

    fruit->translate(randX, randY, 0.0f);

    cout << "Pos: " << randX << " , " << randY << endl;
	
	shufMovs = fruit->shufle();
    vector<string> shufleVect = fruit->splitString(shufMovs);

    for (int i = 0; i < shufleVect.size(); ++i){
        if (shufleVect[i] == "L") fruit->left((M_PI/2), 1);
        else if (shufleVect[i] == "R") fruit->i_right((M_PI/2), 1);
        else if (shufleVect[i] == "U") fruit->i_up((M_PI/2), 1);
        else if (shufleVect[i] == "D") fruit->down((M_PI/2), 1);
        else if (shufleVect[i] == "F") fruit->i_front((M_PI/2), 1);
        else if (shufleVect[i] == "B") fruit->back((M_PI/2), 1);
    }

    return fruit;

}



class Snake{
public:
    float velo = 0.01;

    char lastMove = 'R';
    vector< int > curveIndex;
    deque< pair<float, float> > curves;
    vector<RubikCube *> body;

    Snake(){
        curveIndex.push_back(0);
        curves.push_back({0.0,0.0});
        lastMove = 'R';
        body.push_back(new RubikCube());
        srand(time(NULL));
    }

    void render(int fill = 0){
        for (int i = 0; i < body.size(); ++i){
            body[i]->render(fill);
            body[i]->animate();
            
        }
    }

    void addCube(RubikCube *fruit, string shufMovs){
        vector<float> cube0Center = body.back()->getCenter();
        vector<float> cube1Center = fruit->getCenter();

        fruit->translate(cube0Center[0] - cube1Center[0], cube0Center[1] - cube1Center[1], 0.0f);
        float translateFactor = body.back()->TotalHeight + velo;

        if (lastMove == 'R') fruit->translate(-translateFactor, 0.0f, 0.0f);
        else if (lastMove == 'L') fruit->translate(translateFactor, 0.0f, 0.0f);
        else if (lastMove == 'U') fruit->translate(0.0f, -translateFactor, 0.0f);
        else fruit->translate(0.0f, translateFactor, 0.0f);

        body.push_back(fruit);
        curveIndex.push_back(curves.size()-1);
        body.back()->solve2(shufMovs);
        return;
    }

    bool translate(RubikCube* fruit){
        vector<float> actualCenter;
        vector<float> objectiveCenter = fruit->getCenter();

        bool newFruit = 0;
        float offsetX, offsetY;
        float diffX, diffY;

        // Cabeza persigue fruta (Proxima Curva)
        curves[0] = {objectiveCenter[0] , objectiveCenter[1]};

        // Check if a curve need to be deleted
        if (curveIndex.back() < curves.size()-1) curves.erase(curves.end()-1);


        for (int i = 0; i < body.size(); ++i){

            actualCenter = body[i]->getCenter();

            diffX = curves[curveIndex[i]].first  - actualCenter[0];
            diffY = curves[curveIndex[i]].second - actualCenter[1];

            // cout << "\nObjective "<< i<<": " << curves[curveIndex[i]].first << " , " << curves[curveIndex[i]].second << endl;
            // cout << "Actual: " << actualCenter[0] << " , " << actualCenter[1] << endl;
            // cout << "Curve index: " << curveIndex[i] << endl;

            if (abs(diffX) < velo && abs(diffY) < velo){ // Arrive to curve
                offsetX = diffX;
                offsetY = diffY;

                if (i == 0) newFruit = 1;
                else --curveIndex[i]; // Next Curve

                // cout << "ARRIVE!!" << endl;
            }
            else if (abs(diffX) < velo){ // No X translation
                offsetX = diffX;
                offsetY = diffY > 0.0f ? velo : -velo;
                lastMove = diffY > 0.0f ? 'U' : 'D';
                // cout << "Y only: " << diffY<< " -> " << offsetY << endl;

            }
            else if (abs(diffY) < velo){ // No Y translation
                offsetY = diffY;
                offsetX = diffX > 0.0f ? velo : -velo;
                lastMove = diffX > 0.0f ? 'R' : 'L';
                // cout << "X only: " << diffX<< " -> " << offsetX << endl;

            }
            else{ // X and Y translation 
                offsetY = 0.0f;
                offsetX = diffX > 0.0f ? velo : -velo;
                lastMove = diffX > 0.0f ? 'R' : 'L';
                // cout << "X FIRST: " << diffX << " -> " << offsetX << endl;

            }

            body[i]->translate(offsetX, offsetY, 0.0f);

        }

        if (newFruit){ // Update curves Indexes
            curves.push_front({0.0f,0.0f});
            for (int i = 1; i < curveIndex.size(); ++i) ++curveIndex[i];
        }

        return newFruit;

    }

    /*

    bool translate(RubikCube* fruit){
        vector<float> cube0Center;
        vector<float> cube1Center;

        float offsetX, offsetY;
        float diffX, diffY;
        
        for (int i = body.size()-1; i > 0; --i){
            cube1Center = body[i-1]->getCenter();
            cube0Center = body[i]->getCenter();

            diffX = cube0Center[0] - cube1Center[0];
            diffY = cube0Center[1] - cube1Center[1];

            cout << "EUCLIDEAN " << i << ": " << sqrt(pow(diffX, 2) + pow(diffY, 2)) << " <? " << body.back()->TotalHeight << endl;
            if (sqrt(pow(diffX, 2) + pow(diffY, 2)) < body.back()->TotalHeight) continue;

            offsetX = 0.0f;
            offsetY = 0.0f;

            if (abs(diffX) < err){ // NO X translation
                if (abs(diffY) < err)  offsetY = 0.0f; // NO Y translation
                else if (diffY < 0.0f){
                    offsetY = velo;
                    lastMove[i] = 'U';
                }
                else if (diffY > 0.0f){
                    offsetY = -velo;
                    lastMove[i] = 'D';
                }
                cout << "CUBE " << i << " only move Y" << endl;
            }
            else if (abs(diffY) < err){ // NO Y translation
                if (abs(diffX) < err)  offsetX = 0.0f;
                else if (diffX < 0.0f){
                    offsetX = velo;
                    lastMove[i] = 'R';
                }
                else if (diffX > 0.0f){
                    offsetX = -velo;
                    lastMove[i] = 'L';
                }
                cout << "CUBE " << i << " only move X" << endl;
            }
            else{
                if (lastMove[i] == 'R' || lastMove[i] == 'L'){
                    if (diffX < 0.0f) offsetX = velo;
                    else offsetX = -velo;
                    cout << "CUBE " << i << " only move X (last)" << endl;
                }
                else{
                    if (diffY < 0.0f) offsetY = velo;
                    else offsetY = -velo;
                    cout << "CUBE " << i << " only move Y (last)" << endl;
                }
                
            }

            cout << "OFFSET " << i << ": " << offsetX << " " << offsetY << endl;
            body[i]->translate(offsetX, offsetY, 0.0f);

            
        }

        offsetX = 0.0f;
        offsetY = 0.0f;

        cube0Center = body[0]->getCenter();
        cube1Center = fruit->getCenter();

        diffX = cube0Center[0] - cube1Center[0];
        diffY = cube0Center[1] - cube1Center[1];

        if (abs(diffX) < err){ // NO X translation
            offsetX = 0.0f;
            if (abs(diffY) < err)  offsetY = 0.0f; // NO Y translation
            else if (diffY < 0.0f){
                offsetY = velo;
                lastMove[0] = 'U';
            }
            else if (diffY > 0.0f){
                offsetY = -velo;
                lastMove[0] = 'D';
            }
        }
        else if (abs(diffY) < err){ // NO Y translation
            offsetY = 0.0f;
            if (abs(diffX) < err)  offsetX = 0.0f;
            else if (diffX < 0.0f){
                offsetX = velo;
                lastMove[0] = 'R';
            }
            else if (diffX > 0.0f){
                offsetX = -velo;
                lastMove[0] = 'L';
            }
        }
        else{
            
            
            if (diffX < 0.0f){

                if (lastMove[0] = 'L'){
                    if (diffY < 0.0f){
                        offsetY = velo;
                        lastMove[0] = 'U';
                    }
                    else{
                        offsetY = -velo;
                        lastMove[0] = 'D';
                    }  
                }
                else{
                    offsetX = velo;
                    lastMove[0] = 'R';
                }
                
            }
            else{
                if (lastMove[0] = 'R'){
                    if (diffY < 0.0f){
                        offsetY = velo;
                        lastMove[0] = 'U';
                    }
                    else{
                        offsetY = -velo;
                        lastMove[0] = 'D';
                    }  
                }
                else{
                    offsetX = -velo;
                    lastMove[0] = 'L';
                }
                
            }   

            // if (diffY < 0.0f){
            //     offsetY = velo;
            //     lastMove[0] = 'U';
            // }
            // else{
            //     offsetY = -velo;
            //     lastMove[0] = 'D';
            // }  

            // if (lastMove[0] == 'U' || lastMove[0] == 'D'){
            //     if (diffX < 0.0f){
            //         offsetX = velo;
            //         lastMove[0] = 'R';
            //     }
            //     else{
            //         offsetX = -velo;
            //         lastMove[0] = 'L';
            //     }    
            // }
            // else{
            //     if (diffY < 0.0f){
            //         offsetY = velo;
            //         lastMove[0] = 'U';
            //     }
            //     else{
            //         offsetY = -velo;
            //         lastMove[0] = 'D';
            //     }  
            // }

                    
        }
        
        if (offsetX == 0.0f && offsetY == 0.0f){
            offsetX = 0.0f;
            offsetY = 0.0f;
            addCube(fruit);
            return 1;
        }

        body[0]->translate(offsetX, offsetY, 0.0f);
        return 0;
        
    }

    */


};



