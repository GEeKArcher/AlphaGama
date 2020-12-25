/*
l or L - to move the collector left
r or R - to move the collector right
if bomb id collected game will be over
S or s - to restart
ESC - To exit

*/

#include <windows.h>
#include <stdio.h>
#include <GL/glut.h>
#include <bits/stdc++.h>
#define PI 3.14159265358
using namespace std;

int cnt, gameover, point, speed = 300;
int dp[700][700];       // screen
int L = 0, R = 0;       // bootom left coordinate of collector
const int length = 50, width = 10;   // length and width of the collector
const int radii = 20;   // radius of ball

struct ball_coor{
    int hr, hy;   // x and y coordinate of ball
    int ok;       // Ball or Bomb
    int col;      // color of ball

    ball_coor(){
        hy = 600;
    }
};

template <class T>
struct node{
    T val;
    node *next;
    node *prev;

    node(){
        next = nullptr;
        prev = nullptr;
    }
};


template <class T>
class LinkedList{
public:

    node <T> *root;
    node <T> *last;
    int capacity;

    LinkedList(){
        root = nullptr;
        last = nullptr;
        capacity = 0;
    }

    int Size(void){
        return capacity;
    }

    void Clear(void){
        root = last = nullptr;
        capacity = 0;
    }

    void PushBack(T n){
        node <T> *new_node = new node <T>;
        new_node->val = n;
        if(!capacity){
            root = new_node;
            last = new_node;
        }
        else{
            last->next = new_node;
            new_node->prev = last;
            last = new_node;
        }
        capacity += 1;
    }

    void PushFront(T n){
        node <T> *new_node = new node <T>;
        new_node->val = n;
        if(!capacity){
            root = new_node;
            last = new_node;
        }
        else{
            root->prev = new_node;
            new_node->next = root;
            root = new_node;
        }
        capacity += 1;
    }

    void PopBack(void){
        if(capacity == 0)
            return;
        if(capacity == 1)
            root = last = nullptr;
        else
            last = last->prev;
        capacity -= 1;
    }

    void PopFront(void){
        if(capacity == 0)
            return;
        if(capacity == 1)
            root = last = nullptr;
        else
            root = root->next;
        capacity -= 1;
    }
};


class PrintText{
public:

    string over, points_str, info1, info2;

    PrintText(void){                        // constructor to initialize
        over = "GAME OVER!!";
        points_str = "";
        info1 = "RESART - S";
        info2 = "EXIT - ESC";
    }

    void output(int x, int y, string &str){ // function to display text on the screen
        glColor3f( 0, 0, 0);
        glRasterPos2f(x, y);
        int len, i;
        len = str.size();
        for (i = 0; i < len; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
        }
    }

    string convert_to_string(int n){       // function to convert the integer point to string
        string s = "";
        while(n){
            s.push_back(n%10 + '0');
            n /= 10;
        }
        reverse(s.begin(), s.end());
        if(s.size() == 0)
            return "0";
        else
            return s;
    }

    void endpage(void){                   // function for end page
        output(235, 300, over);
        if(points_str == "")
            points_str = "TOTAL - " + convert_to_string(point);
        output(235, 280, points_str);
        output(235, 260, info1);
        output(235, 240, info2);
    }

};

class Falling{
public:

    LinkedList <ball_coor> v;                                             // deque to store the current balls
    float color[5][3] = {{0.8627, 0.07843, 0.23529},                 // different color of balls (r, g, b)
                         {1,      0.07843, 0.5764 },
                         {1,      0.2705,  0      },
                         {1,      0.84313, 0      },
                         {0.0274, 0.5098,  0.70588}};


    void DrawBalls(GLfloat x, GLfloat y, GLfloat radius){            // function to draw balls on screen of different colors
        int i;
        int triangleAmount = 20;
        GLfloat twicePi = 2.0f * PI;
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, y); // center of circle
            for(i = 0; i <= triangleAmount; i++) {
                glVertex2f(
                        x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount))
                );
            }
        glEnd();
    }

    void cracker(int x, int y){                                      // function for cracker over the bomb
        glPointSize(3);
        glColor3f(1, 0.6470, 0);
        int c;
        if(cnt%2 == 0)
            c = 5;
        else
            c = 7;
        glBegin(GL_POINTS);
            for(int i = -1; i <= 1; i++){
                for(int j = -1; j <= 1; j++){
                    glVertex2i(x + i * c, y + j * c);
                }
            }
        glEnd();
    }


    void Bomb(GLfloat x, GLfloat y, GLfloat radius){                // function for bomb
        int triangleAmount = 20;
        GLfloat twicePi = 2.0f * PI;
        glColor3f(0.20392, 0.29019, 0.3656);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, y); // center of circle
            for(int i = 0; i <= triangleAmount; i++) {
                glVertex2f(
                        x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount))
                );
            }
        glEnd();
        glBegin(GL_POLYGON);
            glVertex2i(x - 10, y + 10);
            glVertex2i(x + 10, y + 10);
            glVertex2i(x + 10, y + 25);
            glVertex2i(x - 10, y + 25);
        glEnd();

        glLineWidth(3);
        glBegin(GL_LINE_LOOP);
            glVertex2i(x, y + 25);
            glVertex2i(x, y + 35);
        glEnd();
        cracker(x, y + 35);
    }

    void AllBalls(void){                                            // function to print all the balls
        while(v.Size() > 0){
            if(v.root->val.hy - radii < 0)
                v.PopFront();
            else if(dp[v.root->val.hr][v.root->val.hy - radii]){
                if(v.root->val.ok == 0){
                    PrintText obj;
                    obj.endpage();
                    gameover = 1;
                    return;
                }
                point += 1;
                v.PopFront();
            }
            else
                break;
        }

        node <ball_coor> *temp = v.root;
        while(temp != nullptr){
            if(!gameover)
                temp->val.hy -= 1;
            if(temp->val.ok == 1){
                glColor3f(color[temp->val.col][0], color[temp->val.col][1], color[temp->val.col][2]);
                DrawBalls(temp->val.hr, temp->val.hy, radii);
            }
            else{
                Bomb(temp->val.hr, temp->val.hy, radii);
            }
            temp =   temp->next;
        }
    }

};

class BackGround{
public:

    void Circle(GLfloat x, GLfloat y, GLfloat radius){         // function for circle
        int i;
        int triangleAmount = 20;
        GLfloat twicePi = 2.0f * PI;
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, y); // center of circle
            for(i = 0; i <= triangleAmount; i++) {
                glVertex2f(
                        x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount))
                );
            }
        glEnd();
    }

    void cloud_model(){                                       // function to print cloud
        glColor3f(1, 1, 1);

        Circle(200, 200, 20);
        Circle(230, 200, 20);
        Circle(260, 200, 20);
        Circle(290, 200, 20);
        Circle(215, 220, 20);
        Circle(245, 230, 30);
        Circle(275, 230, 25);

        Circle(500, 230, 10);

        Circle(500, 500, 20);
        Circle(530, 500, 20);
        Circle(560, 500, 20);
        Circle(590, 500, 20);
        Circle(515, 520, 20);
        Circle(545, 530, 30);
        Circle(575, 530, 25);

        Circle(0, 400, 20);
        Circle(30, 400, 20);
        Circle(60, 400, 20);
        Circle(90, 400, 20);
        Circle(15, 420, 20);
        Circle(45, 430, 30);
        Circle(75, 430, 25);

        Circle(0, 0, 20);
    }

    void DrawCollector(void){                                           // function to draw collector at the bottom
        glColor3f(0, 0, 0);
        glBegin(GL_POLYGON);
            glVertex2i(L, R);
            glVertex2i(L + length, R);
            glVertex2i(L + length, R + width);
            glVertex2i(L, R + width);
        glEnd();
    }

};

Falling obj1;
BackGround obj;

void display(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	gluOrtho2D(0.0, 600.0, 0.0, 600.0);
	obj.cloud_model();
	obj.DrawCollector();
	obj1.AllBalls();
	if(cnt%200 == 0){
        ball_coor ny;
        ny.col = rand()%5;
        if(cnt%1000 == 0){
            ny.ok = 0;
        }
        else{
            ny.ok = 1;
        }
        ny.hr = rand()%(600 - radii);
        if(ny.hr < radii)
            ny.hr = radii;
        obj1.v.PushBack(ny);
	}
	cnt += 1;
    glutSwapBuffers();
}

void keyboard(unsigned char key, int, int){
    if((key == 'S' || key == 's') && gameover){
        memset(dp, 0, sizeof(dp));
        obj1.v.Clear();
        speed = 300;
        gameover = 0;
        point = 0;
        cnt = 0;
        L = 0;
    }
    if(key == 27 && gameover)
        exit(0);
    if(gameover)
        return;
	for(int i = L; i <= L + length; i++){
        dp[i][R + width] = 0;
	}
    if(key == 'r' || key == 'R'){
        if(L + 50 + length <= 600)
            L += 50;
    }
    else if(key == 'L' || key == 'l'){
        if(L - 50 >= 0)
            L -= 50;
    }
	for(int i = L; i <= L + length; i++){
        dp[i][R + width] = 1;
	}
}

void myInit(void){
	glClearColor(0.81960, 0.9490, 0.92156, 0.0);
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(4.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 600.0, 0.0, 600.0);
}

void timer(int){
    glutPostRedisplay();
    glutTimerFunc(1000/speed, timer, 0);
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Ball Game");
    glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutTimerFunc(1000, timer, 0);
	myInit();
	glutMainLoop();
	return 0;
}
