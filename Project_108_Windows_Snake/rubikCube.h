#include "shapes.h"
#include <thread>
#include <mutex>
#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <deque>
#include <cmath>

// #include "rubik-cube-solver/src/rubik_cube.hpp"
#include "rubik-cube-solver/src/rubik_cube_solver.hpp"

//#define M_PI 3.14159265358979323846

class RubikCube{

public:

    Cube cubes[3][3][3];
    mutex mtx;

    float TotalHeight;
    float padding = 0.08;

    bool animation = 0;
    bool solving = 0;
    int shufleNumber = 15;
    int fpsCounter = 0;
    int fpsXAnimation = 20;
    float radianXFps;
    string actualAnimation;
	string shufMovsCopy;

    // Crea un Cubo rubik en el centro
    RubikCube() {

        // Build Cube
        const float height = 0.5;
        const float translateFactor = height*2 + padding;

        TotalHeight = 6*height + 2*padding;
        

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

    ~RubikCube(){

    }
	char toPosition(string color)
	{
			if (color == "1.000000,1.000000,1.000000") return 'D';
			if (color == "1.000000,0.984000,0.000000") return 'U';
			if (color == "0.000000,0.447000,0.800000") return 'L';
			if (color == "0.200000,0.788000,0.000000") return 'R';
			if (color == "0.902000,0.000000,0.000000") return 'F';
			if (color == "0.902000,0.373000,0.000000") return 'B';
			
			cout << "COLOR NO RECONOCIDO: " << color << endl;
			return 'N';
	}

    string getColorString(){       

        string colorString = "";

        // Up
        for (int z = 0; z < 3; ++z){ // Z
            for (int x = 0; x < 3; ++x){ // X
                colorString += toPosition(cubes[2][x][z].getColor('U'));
            }
        }
        cout << "UP: \t" << colorString << endl;


        // Right
        for (int y = 0; y < 3; ++y){ // Y
            for (int z = 0; z < 3; ++z){ // Z
                colorString += toPosition(cubes[2-y][2][2-z].getColor('R'));
            }
        }
        cout << "Right: \t" << colorString << endl;


        // Front
        for (int y = 0; y < 3; ++y){ // Y
            for (int x = 0; x < 3; ++x){ // X
                colorString += toPosition(cubes[2-y][x][2].getColor('F'));
            }
        }
        cout << "Front: \t" << colorString << endl;


        // Down
        for (int z = 0; z < 3; ++z){ // Z
            for (int x = 0; x < 3; ++x){ // X
                colorString += toPosition(cubes[0][x][2-z].getColor('D'));
            }
        }
        cout << "Down: \t" << colorString << endl;


        // Left
        for (int y = 0; y < 3; ++y){ // Y
            for (int z = 0; z < 3; ++z){ // Z
                colorString += toPosition(cubes[2-y][0][z].getColor('L'));
            }
        }
        cout << "Left: \t" << colorString << endl;


        // Back
        for (int y = 0; y < 3; ++y){ // Y
            for (int x = 0; x < 3; ++x){ // X
                colorString += toPosition(cubes[2-y][2-x][0].getColor('B'));
            }
        }
        cout << "Back: \t" << colorString << endl;

        cout << endl <<endl;

        return colorString;
    }

    string shufle(){
        int r;
        string shufleSTR = "";
        for (int i = 0; i < shufleNumber; ++i){
            r = rand() % 6;
            if (r == 0) shufleSTR += "L ";
            if (r == 1) shufleSTR += "R ";
            if (r == 2) shufleSTR += "U ";
            if (r == 3) shufleSTR += "D ";
            if (r == 4) shufleSTR += "B ";
            if (r == 5) shufleSTR += "F ";
        }
        shufleSTR.pop_back();
        return shufleSTR;
    }

    string getSolution(){
        string pythonScript = R"(
import kociemba
print(kociemba.solve(')";
        pythonScript += getColorString();
        pythonScript += R"(')))";

        // Create a temporary file to save the Python script
        ofstream scriptFile("temp_script.py");
        if (!scriptFile.is_open()) {
            cerr << "Failed to create the script file." << endl;
            return "";
        }

        // Write the Python script string to the file
        scriptFile << pythonScript;
        scriptFile.close();

        // Open a pipe to run the Python script and capture its output
        FILE *pipe = popen("python3 temp_script.py", "r");

        if (!pipe) {
            cerr << "Failed to open the pipe." << endl;
            return "";
        }

        // Create a buffer to read the Python script's output
        char buffer[128];
        string result = "";

        // Read the output line by line
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }

        // Close the pipe
        pclose(pipe);

        // Print or process the captured output
        cout << "Python script output:\n" << result << endl;

        return result;
    }

    vector<string> splitString(const string& input) {
        vector<string> result;
        istringstream iss(input);
        string token;

        while (iss >> token) {
            if (token.size() == 2 && token[1] == '2'){
                token.pop_back();
                result.push_back(token);
                result.push_back(token);
            }
            else{
                result.push_back(token);
            }
        }

        return result;
    }

    void solveThread(bool shuf = 0){

        string solution;
        if (shuf) solution = shufle();
        else solution = getSolution();

        // Convert string to vect
        vector<string> sol = splitString(solution);

        // Ejecutar cada move de sol
        for (int i = 0; i < sol.size(); ++i){

            while(animation)this_thread::sleep_for(chrono::milliseconds(10));

            move(sol[i]);
        }

    }

    void solve(bool shuf = 0){
        thread(&RubikCube::solveThread, this, shuf).detach();
    }
	
	void solveThread2(){

        string solution;
        // cout << "GOOD" << endl;
		Rubik_Cube rb(3);
		// cout << "GOOD2" << endl;
		rb.Move(shufMovsCopy);   
		// cout << "GOOD3" << endl;
		
		Rubik_CubeSolver *solver = new Rubik_Cube3BasicSolver(rb);
		// cout << "GOOD4" << endl;
		solution = solver->Solve();
		
		
		cout << "Solution: " << solution << endl;

        // Convert string to vect
        vector<string> sol = splitString(solution);

        // Ejecutar cada move de sol
        for (int i = 0; i < sol.size(); ++i){

            while(animation)this_thread::sleep_for(chrono::milliseconds(10));

            move(sol[i]);
        }

    }

    void solve2(string shuf){
		shufMovsCopy = shuf;
        thread(&RubikCube::solveThread2, this).detach();
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

    bool move(string comand){


        if (animation) return 0; // Se esta animando (No iniciar otra)

        // Iniciar nueva animacion
        fpsCounter = 0;

        mtx.lock();
        animation = 1;
        mtx.unlock();

        actualAnimation = comand;
        radianXFps = (M_PI/2) / fpsXAnimation; // Numero de radianes que avanzara por animacion

        return 1;
    }

    void animate(){

        if (!animation) return;

        bool last = (++fpsCounter >= fpsXAnimation);


		//Movimientos horarios
        if (actualAnimation == "L") left(radianXFps, last);
        else if (actualAnimation == "R'") right(radianXFps, last);
        else if (actualAnimation == "U'") up(radianXFps, last);
        else if (actualAnimation == "D") down(radianXFps, last);
        else if (actualAnimation == "F'") front(radianXFps, last);
        else if (actualAnimation == "B") back(radianXFps, last);
        else if (actualAnimation == "V") vertical1(radianXFps, last);
        else if (actualAnimation == "H") horizontal(radianXFps, last);
		else if (actualAnimation == "N") vertical2(radianXFps, last);
		else if (actualAnimation == "L'") i_left(radianXFps, last);
        else if (actualAnimation == "R") i_right(radianXFps, last);
        else if (actualAnimation == "U") i_up(radianXFps, last);
        else if (actualAnimation == "D'") i_down(radianXFps, last);
        else if (actualAnimation == "F") i_front(radianXFps, last);
        else if (actualAnimation == "B'") i_back(radianXFps, last);
        else if (actualAnimation == "V'") i_vertical1(radianXFps, last);
        else if (actualAnimation == "H'") i_horizontal(radianXFps, last);
		else if (actualAnimation == "N'") i_vertical2(radianXFps, last);
		
        if (last){
            mtx.lock();
            animation = 0;
            mtx.unlock();

        }


    }

    void left(float radian, bool last = 1){
        // rotate cubes
        vector<float> center = cubes[1][0][1].getCenter();
        for (int y = 0; y < 3; ++y){
            for (int z = 0; z < 3; ++z) cubes[y][0][z].rotateAxis(radian,0.0f,0.0f,center);
        }
        if (last){
            // Rotate in mat
            Cube auxArr[3];

            for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][0][i]; // Save Top
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2-i][0][2]); // Move top to front and save front
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][0][2-i]); // Move front to down and save down
            for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][0][0]); // Move down to back and save back
            cubes[2][0][1] = auxArr[1];
        }
        
    }

    void right(float radian, bool last = 1){
        // rotate cubes
        vector<float> center = cubes[1][2][1].getCenter();
        for (int y = 0; y < 3; ++y){
            for (int z = 0; z < 3; ++z) cubes[y][2][z].rotateAxis(radian,0.0f,0.0f,center);
        }

        if (last){
            // Rotate in mat
            Cube auxArr[3];

            for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][2][i]; // Save Top
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2-i][2][2]); // Move top to front and save front
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][2][2-i]); // Move front to down and save down
            for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][2][0]); // Move down to back and save back
            cubes[2][2][1] = auxArr[1];
        }

    }

    void up(float radian, bool last = 1){

        // rotate cubes
        vector<float> center = cubes[2][1][1].getCenter();
        for (int x = 0; x < 3; ++x){
            for (int z = 0; z < 3; ++z) cubes[2][x][z].rotateAxis(0.0f,radian,0.0f,center);
        }

        if (last){

            // Rotate in mat
            Cube auxArr[3];

            for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][i][2]; // Save Front
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2][2][2 - i]); // Move front to right and save right
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2][2-i][0]); // Move right to back and save back
            for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[2][0][i]); // Move back to left and save left
            cubes[2][1][2] = auxArr[1];

        }
        
    }

    void down(float radian, bool last = 1){

        // rotate cubes
        vector<float> center = cubes[0][1][1].getCenter();
        for (int x = 0; x < 3; ++x){
            for (int z = 0; z < 3; ++z) cubes[0][x][z].rotateAxis(0.0f,radian,0.0f,center);
        }

        if (last){

            // Rotate in mat
            Cube auxArr[3];

            for (int i = 0; i < 3; ++i) auxArr[i] = cubes[0][i][2]; // Save Front
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][2][2 - i]); // Move front to right and save right
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][2-i][0]); // Move right to back and save back
            for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[0][0][i]); // Move back to left and save left
            cubes[0][1][2] = auxArr[1];
        }
        
    }

    void front(float radian, bool last = 1){

        // rotate cubes
        vector<float> center = cubes[1][1][2].getCenter();
        for (int y = 0; y < 3; ++y){
            for (int x = 0; x < 3; ++x) cubes[y][x][2].rotateAxis(0.0f,0.0f,radian,center);
        }

        if (last){

            // Rotate in mat
            Cube auxArr[3];
 
            for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][2-i][2]; // Save Front
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2-i][0][2]); // Move front to left and save left
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][i][2]); // Move left to bottom and save bottom
            for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][2][2]); // Move bottom to right and save right
            cubes[2][1][2] = auxArr[1];
            
        }

    }

    void back(float radian, bool last = 1){

        // rotate cubes
        vector<float> center = cubes[1][1][0].getCenter();
        for (int y = 0; y < 3; ++y){
            for (int x = 0; x < 3; ++x) cubes[y][x][0].rotateAxis(0.0f,0.0f,radian,center);
        }

        if (last){

            // Rotate in mat
            Cube auxArr[3];
 
            for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][2-i][0]; // Save Front
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2-i][0][0]); // Move front to left and save left
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][i][0]); // Move left to bottom and save bottom
            for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][2][0]); // Move bottom to right and save right
            cubes[2][1][0] = auxArr[1];

        }
    }

    void vertical1(float radian, bool last = 1){
        // rotate cubes
        vector<float> center = cubes[1][1][1].getCenter();
        for (int y = 0; y < 3; ++y){
            for (int z = 0; z < 3; ++z) cubes[y][1][z].rotateAxis(radian,0.0f,0.0f,center);
        }

        if (last){
            // Rotate in mat
            Cube auxArr[3];

            for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][1][i]; // Save Top
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2-i][1][2]); // Move top to front and save front
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][1][2-i]); // Move front to down and save down
            for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][1][0]); // Move down to back and save back
            cubes[2][1][1] = auxArr[1];
        }

        
    }

    void horizontal(float radian, bool last = 1){

        // rotate cubes
        vector<float> center = cubes[1][1][1].getCenter();
        for (int x = 0; x < 3; ++x){
            for (int z = 0; z < 3; ++z) cubes[1][x][z].rotateAxis(0.0f,radian,0.0f,center);
        }

        if (last){

            // Rotate in mat
            Cube auxArr[3];

            for (int i = 0; i < 3; ++i) auxArr[i] = cubes[1][i][2]; // Save Front
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[1][2][2 - i]); // Move front to right and save right
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[1][2-i][0]); // Move right to back and save back
            for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[1][0][i]); // Move back to left and save left
            cubes[1][1][2] = auxArr[1];

        }
        
    }

    void vertical2(float radian, bool last = 1){

        // rotate cubes
        vector<float> center = cubes[1][1][1].getCenter();
        for (int y = 0; y < 3; ++y){
            for (int x = 0; x < 3; ++x) cubes[y][x][1].rotateAxis(0.0f,0.0f,radian,center);
        }

        if (last){

            // Rotate in mat
            Cube auxArr[3];

            for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][2-i][1]; // Save Front ;
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2-i][0][1]); // Move front to right and save right
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][i][1]); // Move right to back and save back
            for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][2][1]); // Move back to left and save left
            cubes[2][1][1] = auxArr[1];
        }
    }
	
	//SENTIDO ANTIHORARIO
	void i_left(float radian, bool last = true) {
		// rotate cubes
		vector<float> center = cubes[1][0][1].getCenter();
		for (int y = 0; y < 3; ++y) {
			for (int z = 0; z < 3; ++z) cubes[y][0][z].rotateAxis(-radian, 0.0f, 0.0f, center);  // Cambio de radian a -radian
		}
		if (last) {
			// Rotate in mat
			Cube auxArr[3];

			for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][0][2-i];// Save Top
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2-i][0][0]); // Move top to front and save front
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][0][i]); // Move front to down and save down
            for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][0][2]); // Move down to back and save back
            cubes[2][0][1] = auxArr[1];
		}
	}
	
	void i_right(float radian, bool last = true) {
		// rotate cubes
		vector<float> center = cubes[1][2][1].getCenter();
		for (int y = 0; y < 3; ++y) {
			for (int z = 0; z < 3; ++z) cubes[y][2][z].rotateAxis(-radian, 0.0f, 0.0f, center);  // Cambio de radian a -radian
		}

		if (last) {
			// Rotate in mat
			Cube auxArr[3];

			for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][2][2-i];// Save Top
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2-i][2][0]); // Move top to front and save front
            for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][2][i]); // Move front to down and save down
            for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][2][2]); // Move down to back and save back
            cubes[2][2][1] = auxArr[1];
		}
	}

	void i_up(float radian, bool last = true) {
		// rotate cubes
		vector<float> center = cubes[2][1][1].getCenter();
		for (int x = 0; x < 3; ++x) {
			for (int z = 0; z < 3; ++z) cubes[2][x][z].rotateAxis(0.0f, -radian, 0.0f, center);  // Cambio de radian a -radian
		}

		if (last) {
			// Rotate in mat
			Cube auxArr[3];

			for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][2-i][2];// Save Front
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2][0][2 - i]);// Move front to left and save left
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2][i][0]);// Move left to back and save back
			for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[2][2][i]);// Move back to right and save right
			cubes[2][1][2] = auxArr[1];
		}	
	}

	void i_down(float radian, bool last = true) {
		// rotate cubes
		vector<float> center = cubes[0][1][1].getCenter();
		for (int x = 0; x < 3; ++x) {
			for (int z = 0; z < 3; ++z) cubes[0][x][z].rotateAxis(0.0f, -radian, 0.0f, center);  // Cambio de radian a -radian
		}

		if (last) {
			// Rotate in mat
			Cube auxArr[3];

			
			for (int i = 0; i < 3; ++i) auxArr[i] = cubes[0][2 - i][2];// Save Front
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][0][2 - i]);// Move front to left and save left
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][i][0]);// Move left to back and save back
			for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[0][2][i]);// Move back to right and save right
			cubes[0][1][2] = auxArr[1];
		}
	}

	void i_front(float radian, bool last = true) {
		// rotate cubes
		vector<float> center = cubes[1][1][2].getCenter();
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) cubes[y][x][2].rotateAxis(0.0f, 0.0f, -radian, center);  // Cambio de radian a -radian
		}

		if (last) {
			// Rotate in mat
			Cube auxArr[3];

			for (int i = 0; i < 3; ++i) auxArr[i] = cubes[0][2 - i][2];  // Save Front
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[i][0][2]);  // Move front to left and save left
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2][i][2]);  // Move left to bottom and save bottom
			for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[2 - i][2][2]);  // Move bottom to right and save right
			cubes[0][1][2] = auxArr[1];
		}
	}

	void i_back(float radian, bool last = true) {
		// rotate cubes
		vector<float> center = cubes[1][1][0].getCenter();
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) cubes[y][x][0].rotateAxis(0.0f, 0.0f, -radian, center);  // Cambio de radian a -radian
		}

		if (last) {
			// Rotate in mat
			Cube auxArr[3];

			for (int i = 0; i < 3; ++i) auxArr[i] = cubes[0][2 - i][0];  // Save Front
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[i][0][0]);  // Move front to left and save left
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2][i][0]);  // Move left to bottom and save bottom
			for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[2 - i][2][0]);  // Move bottom to right and save right
			cubes[0][1][0] = auxArr[1];
		}
	}

	void i_vertical1(float radian, bool last = true) {
		// rotate cubes
		vector<float> center = cubes[1][1][1].getCenter();
		for (int y = 0; y < 3; ++y) {
			for (int z = 0; z < 3; ++z) cubes[y][1][z].rotateAxis(-radian, 0.0f, 0.0f, center);  // Cambio de radian a -radian
		}

		if (last) {
			// Rotate in mat
			Cube auxArr[3];

			for (int i = 0; i < 3; ++i) auxArr[i] = cubes[2][1][2 - i];  // Save Top
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2 - i][1][0]);  // Move top to front and save front
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[0][1][i]);  // Move front to down and save down
			for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[i][1][2]);  // Move down to back and save back
			cubes[2][1][1] = auxArr[1];
		}
	}

	void i_horizontal(float radian, bool last = true) {
		// rotate cubes
		vector<float> center = cubes[1][1][1].getCenter();
		for (int x = 0; x < 3; ++x) {
			for (int z = 0; z < 3; ++z) cubes[1][x][z].rotateAxis(0.0f, -radian, 0.0f, center);  // Cambio de radian a -radian
		}

		if (last) {
			// Rotate in mat
			Cube auxArr[3];

			for (int i = 0; i < 3; ++i) auxArr[i] = cubes[1][2 - i][2];  // Save Front
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[1][0][2 - i]);  // Move front to right and save right
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[1][i][0]);  // Move right to back and save back
			for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[1][2][i]);  // Move back to left and save left
			cubes[1][1][2] = auxArr[1];
		}
	}

	void i_vertical2(float radian, bool last = true) {
		// rotate cubes
		vector<float> center = cubes[1][1][1].getCenter();
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) cubes[y][x][1].rotateAxis(0.0f, 0.0f, -radian, center);  // Cambio de radian a -radian
		}

		if (last) {
			// Rotate in mat
			Cube auxArr[3];

			for (int i = 0; i < 3; ++i) auxArr[i] = cubes[0][2 - i][1];  // Save Front
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[i][0][1]);  // Move front to right and save right
			for (int i = 0; i < 3; ++i) swap(auxArr[i], cubes[2][i][1]);  // Move right to back and save back
			for (int i = 1; i < 3; ++i) swap(auxArr[i], cubes[2 - i][2][1]);  // Move back to left and save left
			cubes[0][1][1] = auxArr[1];
		}
	}

	
	//OTRAS FUNCIONES
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