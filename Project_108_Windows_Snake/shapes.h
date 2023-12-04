//! [code]

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <deque>

//This is something that stb image defines in its own include guard, so I should be able to test for it
#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include "learnopengl/shader_m.h"
#include "learnopengl/camera.h"
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;
GLFWwindow* window;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 70.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// Timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// Shader
Shader ourShader;

//TEXTURA
unsigned int texture1, texture2;


float randF(){   
    return (float) rand() / RAND_MAX;
}

void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.ProcessKeyboard(DOWN, deltaTime);
    
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


int initOpenGL(){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL){
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    gladLoadGL(glfwGetProcAddress);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
	fs::path p = fs::current_path();
	int levels_path = 1;
	fs::path p_current; 
	p_current = p.parent_path();	
	std::string vs_path , fs_path;
	std::stringstream ss;
	ss << std::quoted( p_current.string() );
	std::string out;
	ss >> std::quoted(out);
	vs_path = out + "\\OwnProjects\\Project_108_glfw\\learnopengl\\7.4.camera.vs";
	fs_path = out + "\\OwnProjects\\Project_108_glfw\\learnopengl\\7.4.camera.fs";
    ourShader = Shader(vs_path.c_str(),fs_path.c_str());
    ourShader.use();
    ourShader.setVec4("customColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red default
    return 1;

}



// Result stored in coords
void vectTransformation(vector<float> &coords, vector< vector <float> > &mat, int offset = 0){

    float v[4] = { coords[0 + offset], coords[1 + offset], coords[2 + offset], 1.0f } ;

    for (int i = 0; i < 3; ++i){ // We dont need to multiply last row
        coords[i + offset] = 0;
        for (int k = 0; k < 4; ++k) coords[i + offset] += mat[i][k] * v[k];
    }   

}

vector< vector <float> > translationMat(float tx, float ty, float tz){
    vector<vector<float>> mat(4, vector<float>(4, 0.0f)); 
    mat[0][3] = tx;
    mat[1][3] = ty;
    mat[2][3] = tz;
    mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = 1.0f;
    return mat;
}

vector< vector <float> > scaleMat(float factorX, float factorY, float factorZ){
    vector<vector<float>> mat(4, vector<float>(4, 0.0f)); 
    mat[0][0] = factorX;
    mat[1][1] = factorY;
    mat[2][2] = factorZ;
    mat[3][3] = 1.0f;
    return mat;
}

vector< vector <float> > rotationXMat(float radian){
    vector<vector<float>> mat(4, vector<float>(4, 0.0f)); 
    mat[0][0] = mat[3][3] = 1.0f;
    mat[1][1] = mat[2][2] = cos(radian);
    mat[1][2] = -sin(radian);
    mat[2][1] = sin(radian);
    return mat;
}

vector< vector <float> > rotationYMat(float radian){
    vector<vector<float>> mat(4, vector<float>(4, 0.0f)); 
    mat[0][0] = mat[2][2] = cos(radian);
    mat[1][1] = mat[3][3] = 1.0f;
    mat[0][2] = sin(radian);
    mat[2][0] = -sin(radian);
    return mat;
}

vector< vector <float> > rotationZMat(float radian){
    vector<vector<float>> mat(4, vector<float>(4, 0.0f)); 
    mat[0][0] = mat[1][1] = cos(radian);
    mat[2][2] = mat[3][3] = 1.0f;
    mat[0][1] = -sin(radian);
    mat[1][0] = sin(radian);
    return mat;
}



class Shape{
public:

    int PointSize = 8;
    int LineSize = 6;
    vector <float> coords;
    unsigned int VBO, VAO;
    float rgb[4];

    Shape() {}

    virtual ~Shape() {}

    virtual vector<float> getCenter() = 0;

    void setPointSize(int s){ PointSize = s; }
    void setLineSize(int s){ LineSize = s; }

    void setColor(float r, float g, float b, float a = 1.0f){
        rgb[0] = r; rgb[1] = g; rgb[2] = b; rgb[3] = a; 
    }

    void updateVertex(){
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * coords.size(), coords.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void setCoordsBuffer(){
        // Generate and bind a new VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Generate and bind a new VBO for the point's vertices
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Upload the vertex data to the VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * coords.size(), coords.data(), GL_STATIC_DRAW);

        // Specify the layout of the vertex data
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // Unbind the VAO
        glBindVertexArray(0);
    }


    void translate(float tx, float ty, float tz){
        vector< vector <float> > translate_mat = translationMat(tx,ty,tz);
        for (int i = 0; i+2 < (int) coords.size(); i += 3) vectTransformation(coords, translate_mat, i);
        updateVertex();
    }

    void scale(float factorX, float factorY, float factorZ){
        vector<float> center = getCenter();
        vector< vector <float> > scale_mat = scaleMat(factorX, factorY, factorZ);

        translate(-center[0], -center[1], -center[2]);

        for (int i = 0; i+2 < (int) coords.size(); i += 3) vectTransformation(coords, scale_mat, i);

        translate(+center[0], +center[1], +center[2]);
        updateVertex();
    }

    void scaleAxis(float factorX, float factorY, float factorZ, vector<float> center){
        vector< vector <float> > scaleMatrix = scaleMat(factorX, factorY, factorZ);

        translate(-center[0], -center[1], -center[2]);
        for (int i = 0; i+2 < (int) coords.size(); i += 3) vectTransformation(coords, scaleMatrix, i);
        translate(+center[0], +center[1], +center[2]);
    
        updateVertex();
    }

    void rotateX(float radian){
        vector< vector <float> > rotateMat = rotationXMat(radian);
        vector<float> center = getCenter();
        translate(-center[0], -center[1], -center[2]);
        for (int i = 0; i+2 < (int) coords.size(); i += 3) vectTransformation(coords, rotateMat, i);
        translate(+center[0], +center[1], +center[2]);
        updateVertex();
    }

    void rotateY(float radian){
        vector< vector <float> > rotateMat = rotationYMat(radian);
        vector<float> center = getCenter();
        translate(-center[0], -center[1], -center[2]);
        for (int i = 0; i+2 < (int) coords.size(); i += 3) vectTransformation(coords, rotateMat, i);
        translate(+center[0], +center[1], +center[2]);
        updateVertex();
    }

    void rotateZ(float radian){
        vector< vector <float> > rotateMat = rotationZMat(radian);
        vector<float> center = getCenter();
        translate(-center[0], -center[1], -center[2]);
        for (int i = 0; i+2 < (int) coords.size(); i += 3) vectTransformation(coords, rotateMat, i);
        translate(+center[0], +center[1], +center[2]);
        updateVertex();
    }

    void rotate(float radianX, float radianY, float radianZ){
        vector< vector <float> > rotateMat;
        vector<float> center = getCenter();

        translate(-center[0], -center[1], -center[2]);

        rotateMat = rotationXMat(radianX);
        for (int i = 0; i+2 < (int) coords.size(); i += 3) vectTransformation(coords, rotateMat, i);

        rotateMat = rotationYMat(radianY);
        for (int i = 0; i+2 < (int) coords.size(); i += 3) vectTransformation(coords, rotateMat, i);

        rotateMat = rotationZMat(radianZ);
        for (int i = 0; i+2 < (int) coords.size(); i += 3) vectTransformation(coords, rotateMat, i);

        translate(+center[0], +center[1], +center[2]);
        updateVertex();
    }

    void rotateAxis(float radianX, float radianY, float radianZ, vector<float> center){
        vector< vector <float> > rotateMat;
        

        translate(-center[0], -center[1], -center[2]);

        rotateMat = rotationXMat(radianX);
        for (int i = 0; i+2 < (int) coords.size(); i += 3) vectTransformation(coords, rotateMat, i);

        rotateMat = rotationYMat(radianY);
        for (int i = 0; i+2 < (int) coords.size(); i += 3) vectTransformation(coords, rotateMat, i);

        rotateMat = rotationZMat(radianZ);
        for (int i = 0; i+2 < (int) coords.size(); i += 3) vectTransformation(coords, rotateMat, i);

        translate(+center[0], +center[1], +center[2]);
        updateVertex();
    }

};



class Point : public Shape {
public:

    Point(){}

    Point(float x, float y, float z) : Shape() {
        coords.assign(3, 0.0f);
        coords[0] = x;
        coords[1] = y;
        coords[2] = z;
        setCoordsBuffer();
    }

    vector<float> getCenter(){ return coords; }

    void render() {
        glBindVertexArray(VAO);
        ourShader.setVec4("customColor", glm::vec4(rgb[0], rgb[1], rgb[2], rgb[3]));
        glPointSize(PointSize);
        glLineWidth(LineSize);
        glDrawArrays(GL_POINTS, 0, 1);
        glBindVertexArray(0);
    }   

};



class Triangle : public Shape{
public:

    Triangle(){}

    Triangle(float x1, float y1, float z1, 
             float x2, float y2, float z2, 
             float x3, float y3, float z3) {
        
        coords.assign(9, 0.0f);
        coords[0] = x1; coords[1] = y1; coords[2] = z1;
        coords[3] = x2; coords[4] = y2; coords[5] = z2;
        coords[6] = x3; coords[7] = y3; coords[8] = z3;

        setCoordsBuffer();

    }

    vector<float> getCenter(){
        vector<float> center;
        for (int i = 0; i < 3; ++i) center.push_back( (float) (coords[i] + coords[i+3] + coords[i+6]) / 3);
        return center;
    }

    // 0 = Fill , 1 = Lines, // 2 = Points
    void render(int type = 0) {
        glBindVertexArray(VAO);
        ourShader.setVec4("customColor", glm::vec4(rgb[0], rgb[1], rgb[2], rgb[3]));
        glPointSize(PointSize);
        glLineWidth(LineSize);

        if (type == 0) glDrawArrays(GL_TRIANGLES, 0, 3);
        else if (type == 1) glDrawArrays(GL_LINE_LOOP, 0, 3);
        else if (type == 2) glDrawArrays(GL_POINTS, 0, 3);

        glBindVertexArray(0);
    }

};


class Rectangle : public Shape {
public:

    Rectangle() {}

    Rectangle(float x1, float y1, float z1, 
              float x2, float y2, float z2,
              float x3, float y3, float z3,
              float x4, float y4, float z4) {

        coords.assign(18, 0.0f);
        coords[0] = x1; coords[1] = y1; coords[2] = z1;
        coords[3] = x3; coords[4] = y3; coords[5] = z3;
        coords[6] = x2; coords[7] = y2; coords[8] = z2;

        coords[9] = x1; coords[10] = y1; coords[11] = z1;
        coords[12] = x3; coords[13] = y3; coords[14] = z3;
        coords[15] = x4; coords[16] = y4; coords[17] = z4;

        setCoordsBuffer();
        
    }

    vector<float> getCenter(){
        vector<float> center;
        center.push_back((float) (coords[0] + coords[3]) / 2);
        center.push_back((float) (coords[1] + coords[4]) / 2);
        center.push_back((float) (coords[2] + coords[5]) / 2);
        return center;
    }

    // 0 = Fill , 1 = Lines, // 2 = Points
    void render(int type = 0) {
        glBindVertexArray(VAO);
        ourShader.setVec4("customColor", glm::vec4(rgb[0], rgb[1], rgb[2], rgb[3]));
        glPointSize(PointSize);
        glLineWidth(LineSize);

        if (type == 0) glDrawArrays(GL_TRIANGLES, 0, 6);
        else if (type == 1) glDrawArrays(GL_LINE_LOOP, 0, 6);
        else if (type == 2) glDrawArrays(GL_POINTS, 0, 6);

        glBindVertexArray(0);

    }

};



class Circle : public Shape{
public:

    int num_triangles = 3;

    Circle() {}

    Circle(float x, float y, float z, float radio, int triangles = 20) {

        num_triangles = max(triangles, 3); // No menos de 3

        for (int i = 0; i < num_triangles; ++i){
            coords.push_back(sin((float) 2*M_PI * i / num_triangles) * radio + x);
            coords.push_back(cos((float) 2*M_PI * i / num_triangles) * radio + y);
            coords.push_back(z);

            coords.push_back(sin((float) 2*M_PI * (i+1) / num_triangles) * radio + x);
            coords.push_back(cos((float) 2*M_PI * (i+1) / num_triangles) * radio + y);
            coords.push_back(z);

            coords.push_back(x);
            coords.push_back(y);
            coords.push_back(z);
        }

        setCoordsBuffer();
    }

    vector<float> getCenter(){
        vector<float> r = {coords[6], coords[7], coords[8]};
        return r;        
    }

    // 0 = Fill , 1 = Lines, // 2 = Points
    void render(int type = 0) {
        glBindVertexArray(VAO);
        ourShader.setVec4("customColor", glm::vec4(rgb[0], rgb[1], rgb[2], rgb[3]));
        glPointSize(PointSize);
        glLineWidth(LineSize);
        
        if (type == 0){
            for (int i = 0; i < num_triangles; ++i) glDrawArrays(GL_TRIANGLES, i*3, 3);
        }
        else if (type == 1){
            for (int i = 0; i < num_triangles; ++i) glDrawArrays(GL_LINE_STRIP, i*3, 2);
        }
        else if (type == 2){
            for (int i = 0; i < num_triangles; ++i) glDrawArrays(GL_POINTS, i*3, 2);
        }

        glBindVertexArray(0);
    }

};


// 3D
class Cube{
public:

    Rectangle rects[6];

    Cube(){ }

    Cube(float x1, float y1, float z1, 
         float x2, float y2, float z2){
        
        // Front
        rects[0] = Rectangle(x1, y1, z1,
                             x1, y2, z1,
                             x2, y2, z1,
                             x2, y1, z1);

        // Left
        rects[1] = Rectangle(x1, y1, z1,
                             x1, y1, z2,
                             x1, y2, z2,
                             x1, y2, z1);

        // Bottom
        rects[2] = Rectangle(x1, y1, z1,
                             x2, y1, z1,
                             x2, y1, z2,
                             x1, y1, z2);
        
        // Up
        rects[3] = Rectangle(x2, y2, z2,
                             x2, y2, z1,
                             x1, y2, z1,
                             x1, y2, z2);

        // Right
        rects[4] = Rectangle(x2, y2, z2,
                             x2, y2, z1,
                             x2, y1, z1,
                             x2, y1, z2);
        
        // Behind
        rects[5] = Rectangle(x2, y2, z2,
                             x2, y1, z2,
                             x1, y1, z2,
                             x1, y2, z2);
    }

    vector<float> getCenter(){
        vector<float> center = {
            (float) (rects[0].coords[0] + rects[3].coords[0]) / 2, 
            (float) (rects[0].coords[1] + rects[3].coords[1]) / 2, 
            (float) (rects[0].coords[2] + rects[3].coords[2]) / 2
        };
        return center;
    }

    void render(int type = 0) {
        for (int i = 0; i < 6; ++i) rects[i].render(type);
    }

    void setColor(int index, float r, float g, float b, float a = 1.0f){
        rects[index].setColor(r,g,b,a);
    }

    void translate(float tx, float ty, float tz){
        for (int i = 0; i < 6; ++i) rects[i].translate(tx,ty,tz);
    }

    void scale(float factorX, float factorY, float factorZ){
        vector<float> center = getCenter();
        for (int i = 0; i < 6; ++i) rects[i].scaleAxis(factorX,factorY,factorZ, center);
    }

    void scaleAxis(float factorX, float factorY, float factorZ, vector<float> center){
        for (int i = 0; i < 6; ++i) rects[i].scaleAxis(factorX,factorY,factorZ, center);
    }

    // Indiv Rotations dont make sense
    void rotateX(float radian){
        for (int i = 0; i < 6; ++i) rects[i].rotateX(radian);
    }

    void rotateY(float radian){
        for (int i = 0; i < 6; ++i) rects[i].rotateY(radian);
    }

    void rotateZ(float radian){
        for (int i = 0; i < 6; ++i) rects[i].rotateZ(radian);
    }

    void rotate(float radianX, float radianY, float radianZ){
        vector<float> center = getCenter();
        for (int i = 0; i < 6; ++i) rects[i].rotateAxis(radianX, radianY, radianZ, center);
    }

    void rotateAxis(float radianX, float radianY, float radianZ, vector<float> center){
        for (int i = 0; i < 6; ++i) rects[i].rotateAxis(radianX, radianY, radianZ, center);
    }

    string getColor(char position){

        int index = 0;
        float coordFlag, aux;
        string colorStr = "";

        if (position == 'U'){ // Buscar cara mas arriba

            coordFlag = rects[0].getCenter()[1];
            for (int i = 1; i < 6; ++i){
                aux = rects[i].getCenter()[1];
                if (aux > coordFlag) {
                    coordFlag = aux;
                    index = i;
                }
            }

        }
        else if (position == 'R'){
            coordFlag = rects[0].getCenter()[0];
            for (int i = 1; i < 6; ++i){
                aux = rects[i].getCenter()[0];
                if (aux > coordFlag) {
                    coordFlag = aux;
                    index = i;
                }
            }
        }
        else if (position == 'F'){
            coordFlag = rects[0].getCenter()[2];
            for (int i = 1; i < 6; ++i){
                aux = rects[i].getCenter()[2];
                if (aux > coordFlag) {
                    coordFlag = aux;
                    index = i;
                }
            }
        }
        else if (position == 'D'){
            coordFlag = rects[0].getCenter()[1];
            for (int i = 1; i < 6; ++i){
                aux = rects[i].getCenter()[1];
                if (aux < coordFlag) {
                    coordFlag = aux;
                    index = i;
                }
            }
        }
        else if (position == 'L'){
            coordFlag = rects[0].getCenter()[0];
            for (int i = 1; i < 6; ++i){
                aux = rects[i].getCenter()[0];
                if (aux < coordFlag) {
                    coordFlag = aux;
                    index = i;
                }
            }
        }
        else if (position == 'B'){
            coordFlag = rects[0].getCenter()[2];
            for (int i = 1; i < 6; ++i){
                aux = rects[i].getCenter()[2];
                if (aux < coordFlag) {
                    coordFlag = aux;
                    index = i;
                }
            }
        }


        colorStr  = to_string(rects[index].rgb[0]) + ",";
        colorStr += to_string(rects[index].rgb[1]) + ",";
        colorStr += to_string(rects[index].rgb[2]);

        return colorStr;
    }

};