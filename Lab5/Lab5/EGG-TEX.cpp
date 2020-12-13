// Copyright 2020 Jakub Tolsciuk
#define _USE_MATH_DEFINES

#include<time.h>
#include<stdlib.h>
#include<windows.h>
#include<gl/gl.h>
#include<gl/glut.h>
#include<math.h>
#include<cmath>
#include<iostream>
#include<vector>
#include <chrono>  // NOLINT
#include <thread>  // NOLINT
#include <functional>
#include<string>

using std::vector;
using std::cout;
using std::cin;

typedef float point3[3];
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };

struct Point {
    float x;
    float y;
    float z;
};

vector<vector<Point>> coords;
vector<vector<Point>> normalVectors;

int N;
int model = 3;


static GLfloat theta = 0.0;
static GLfloat omega = 0.0;
static GLfloat pix2angleHorizontal;
static GLfloat pix2angleVertical;
static GLint status = 0;
static int x_pos_old = 0;
static int y_pos_old = 0;
static int delta_x = 0;
static int delta_y = 0;

static GLfloat lightTheta = 0.0;

GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth,
    GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat) {
#pragma pack(1)
    typedef struct {
        GLbyte    idlength;
        GLbyte    colormaptype;
        GLbyte    datatypecode;
        unsigned short    colormapstart;  // NOLINT
        unsigned short    colormaplength;  // NOLINT
        unsigned char     colormapdepth;  // NOLINT
        unsigned short    x_orgin;  // NOLINT
        unsigned short    y_orgin;  // NOLINT
        unsigned short    width;  // NOLINT
        unsigned short    height;  // NOLINT
        GLbyte    bitsperpixel;
        GLbyte    descriptor;
    }TGAHEADER;
#pragma pack(8)

    FILE* pFile;
    TGAHEADER tgaHeader;
    unsigned long lImageSize;  // NOLINT
    short sDepth;  // NOLINT
    GLbyte* pbitsperpixel = NULL;

    *ImWidth = 0;
    *ImHeight = 0;
    *ImFormat = GL_BGR_EXT;
    *ImComponents = GL_RGB8;
    #pragma warning(suppress : 4996)
    pFile = fopen(FileName, "rb");
    if (pFile == NULL)
        return NULL;


    fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

    *ImWidth = tgaHeader.width;
    *ImHeight = tgaHeader.height;
    sDepth = tgaHeader.bitsperpixel / 8;

    if (tgaHeader.bitsperpixel != 8 &&
        tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
        return NULL;

    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

    pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));   // NOLINT

    if (pbitsperpixel == NULL)
        return NULL;

    if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1) {
        free(pbitsperpixel);
        return NULL;
    }

    switch (sDepth) {
    case 3:
        *ImFormat = GL_BGR_EXT;
        *ImComponents = GL_RGB8;
        break;
    case 4:
        *ImFormat = GL_BGRA_EXT;
        *ImComponents = GL_RGBA8;
        break;
    case 1:
        *ImFormat = GL_LUMINANCE;
        *ImComponents = GL_LUMINANCE8;
        break;
    }  // !!! ;
    fclose(pFile);
    return pbitsperpixel;
}


const char* texNames[4] = { "tekstury/CP2077.tga", "tekstury/eggOffer.tga",
"tekstury/mask.tga", "tekstury/rwby.tga" };
int index = 0;

void spinLight() {
    lightTheta -= 0.1;
    if (lightTheta > 360.0) lightTheta -= 360.0;

    float R = 10.0;
    GLfloat light_position_1[] = { R * cos(lightTheta) * cos(45.0),
        R * sin(45.0), R * sin(lightTheta) * cos(45.0), 1.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, light_position_1);

    glutPostRedisplay();
}

void Axes(void) {
    point3  x_min = { -5.0, 0.0, 0.0 };
    point3  x_max = { 5.0, 0.0, 0.0 };


    point3  y_min = { 0.0, -5.0, 0.0 };
    point3  y_max = { 0.0,  5.0, 0.0 };


    point3  z_min = { 0.0, 0.0, -5.0 };
    point3  z_max = { 0.0, 0.0,  5.0 };

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3fv(x_min);
    glVertex3fv(x_max);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);

    glVertex3fv(y_min);
    glVertex3fv(y_max);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);

    glVertex3fv(z_min);
    glVertex3fv(z_max);
    glEnd();
}

void getCoords() {
    float x, y, z, u, v;
    vector<Point> vec;
    Point point;

    vector<Point> vec2;
    Point normalVec;
    float normalVecLen;

    float xU, xV, yU, yV, zU, zV;


    for (int i = 0; i <= N; i++) {
        for (int j = 0; j <= N; j++) {
            u = static_cast<float>(j) / static_cast<float>(N);
            v = static_cast<float>(i) / static_cast<float>(N);

            point.x = ((-90) * pow(u, 5) + 225 * pow(u, 4)
                + (-270) * pow(u, 3)
                + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v);
            point.y = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2);
            point.z = ((-90) * pow(u, 5) + 225 * pow(u, 4)
                + (-270) * pow(u, 3)
                + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v);

            vec.push_back(point);

            // equations checked
            xU = ((-450) * pow(u, 4) + 900 * pow(u, 3)
                - 810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v);
            xV = M_PI * (90 * pow(u, 5) - 225 * pow(u, 4)
                + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(M_PI * v);
            yU = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
            yV = 0;
            zU = (-450 * pow(u, 4) + 900 * pow(u, 3)
                - 810 * pow(u, 2) + 360 * u - 45) * sin(M_PI * v);
            zV = (-M_PI) * (90 * pow(u, 5) - 225 * pow(u, 4)
                + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(M_PI * v);

            normalVec.x = yU * zV - zU * yV;
            normalVec.y = zU * xV - xU * zV;
            normalVec.z = xU * yV - yU * xV;

            normalVecLen = sqrt(pow(normalVec.x, 2)
                + pow(normalVec.y, 2)
                + pow(normalVec.z, 2));

            // normalize
            if (normalVecLen != 0.0) {
                normalVec.x = normalVec.x / normalVecLen;
                normalVec.y = normalVec.y / normalVecLen;
                normalVec.z = normalVec.z / normalVecLen;
            }

            vec2.push_back(normalVec);
        }
        coords.push_back(vec);
        normalVectors.push_back(vec2);
        vec.resize(0);
        vec2.resize(0);
    }
}

void drawTriangle(int x1, int y1, int x2, int y2,
    int x3, int y3, int i_limit, int j_limit) {
    glBegin(GL_TRIANGLES);

    for (int i = 0; i <= N; i++) {
        for (int j = 0; j <= N; j++) {
            if (i != i_limit && j != j_limit) {
                // glColor3f(genFloatRGB(), genFloatRGB(), genFloatRGB());
                if (j < N / 2) {
                    glNormal3f(normalVectors[i + x1][j + y1].x,
                        normalVectors[i + x1][j + y1].y,
                        normalVectors[i + x1][j + y1].z);
                    glTexCoord2f(coords[i + x1][j + y1].x,
                        coords[i + x1][j + y1].y);
                    glVertex3f(coords[i + x1][j + y1].x,
                        coords[i + x1][j + y1].y,
                        coords[i + x1][j + y1].z);

                    glNormal3f(normalVectors[i + x2][j + y2].x,
                        normalVectors[i + x2][j + y2].y,
                        normalVectors[i + x2][j + y2].z);
                   glTexCoord2f(coords[i + x2][j + y2].x,
                       coords[i + x2][j + y2].y);
                    glVertex3f(coords[i + x2][j + y2].x,
                        coords[i + x2][j + y2].y,
                        coords[i + x2][j + y2].z);

                    glNormal3f(normalVectors[i + x3][j + y3].x,
                        normalVectors[i + x3][j + y3].y,
                        normalVectors[i + x3][j + y3].z);
                    glTexCoord2f(coords[i + x3][j + y3].x,
                        coords[i + x3][j + y3].y);
                    glVertex3f(coords[i + x3][j + y3].x,
                        coords[i + x3][j + y3].y,
                        coords[i + x3][j + y3].z);
                } else {
                    glNormal3f(normalVectors[i + x1][j + y1].x * (-1),
                        normalVectors[i + x1][j + y1].y * (-1),
                        normalVectors[i + x1][j + y1].z * (-1));
                    glTexCoord2f(coords[i + x1][j + y1].x ,
                        coords[i + x1][j + y1].y);
                    glVertex3f(coords[i + x1][j + y1].x,
                        coords[i + x1][j + y1].y,
                        coords[i + x1][j + y1].z);

                    glNormal3f(normalVectors[i + x2][j + y2].x * (-1),
                        normalVectors[i + x2][j + y2].y * (-1),
                        normalVectors[i + x2][j + y2].z * (-1));
                    glTexCoord2f(coords[i + x2][j + y2].x ,
                        coords[i + x2][j + y2].y);
                    glVertex3f(coords[i + x2][j + y2].x,
                        coords[i + x2][j + y2].y,
                        coords[i + x2][j + y2].z);

                    glNormal3f(normalVectors[i + x3][j + y3].x * (-1),
                        normalVectors[i + x3][j + y3].y * (-1),
                        normalVectors[i + x3][j + y3].z * (-1));
                   glTexCoord2f(coords[i + x3][j + y3].x ,
                       coords[i + x3][j + y3].y);
                    glVertex3f(coords[i + x3][j + y3].x,
                        coords[i + x3][j + y3].y,
                        coords[i + x3][j + y3].z);
                }
            }
        }
    }
    glEnd();
}


void canIOfferYouANiceEggInThisTryingTime() {
    point3 point;
    float x, y, z, u, v;

    glRotated(20.0, 1.0, 0.0, 0.0);     // (angle) [ vector ]
    glTranslatef(0.0f, -5.0f, 0.0f);

    drawTriangle(-1, 0, 0, 0, 0, 1, 0, N);    // 1
    drawTriangle(-1, 0, -1, 1, 0, 1, 0, N);  // 2

    drawTriangle(0, 0, 1, 0, 1, 1, N, N);   // 3
    drawTriangle(0, 0, 0, 1, 1, 1, N, N);     // 4

    drawTriangle(0, 0, -1, 0, -1, -1, 0, 0);    // 5
    drawTriangle(0, 0, -1, -1, 0, -1, 0, 0);    // 6

    drawTriangle(0, 0, 0, -1, 1, -1, N, 0);    // 7
    drawTriangle(0, 0, 1, 0, 1, -1, N, 0);     // 8

    drawTriangle(-1, 0, 0, 0, 0, 1, 0, N);    // 1
    drawTriangle(-1, 0, -1, 1, 0, 1, 0, N);  // 2

    drawTriangle(0, 0, 1, 0, 1, 1, N, N);   // 3
    drawTriangle(0, 0, 0, 1, 1, 1, N, N);     // 4

    drawTriangle(0, 0, -1, 0, -1, -1, 0, 0);    // 5
    drawTriangle(0, 0, -1, -1, 0, -1, 0, 0);    // 6

    drawTriangle(0, 0, 0, -1, 1, -1, N, 0);    // 7
    drawTriangle(0, 0, 1, 0, 1, -1, N, 0);     // 8
}


void RenderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


    Axes();

    if (status == 1) {
        theta += delta_x * pix2angleHorizontal;
        omega += delta_y * pix2angleVertical;
    }

    glRotatef(theta, 0.0, 1.0, 0.0);
    glRotatef(omega, 1.0, 0.0, 0.0);


    canIOfferYouANiceEggInThisTryingTime();

    glFlush();
    glutSwapBuffers();
}



void keysZoom(unsigned char key, int x, int y) {
    if (key == '+') viewer[2] -= 1;
    if (key == '-') viewer[2] += 1;

    RenderScene();
}

void keysModel(unsigned char key, int x, int y) {
    if (key == 'p') model = 1;
    if (key == 'l') model = 2;
    if (key == 't') model = 3;
    if (key == 'r') {
        theta = 0.0;
        omega = 0.0;
        viewer[0] = 0.0;
        viewer[1] = 0.0;
        viewer[2] = 10.0;
    }

    RenderScene();
}

void keys(unsigned char key, int x, int y) {
    keysModel(key, x, y);
    keysZoom(key, x, y);
}



GLuint* textures = new GLuint[4];

void MyInit(void) {
    GLbyte* pBytes;
    GLint ImWidth, ImHeight, ImComponents;
    GLenum ImFormat;

    glGenTextures(4, textures);

    pBytes = LoadTGAImage(texNames[0], &ImWidth,
        &ImHeight, &ImComponents, &ImFormat);

    // Zdefiniowanie tekstury 2-D
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents,
        ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    pBytes = LoadTGAImage(texNames[1], &ImWidth,
        &ImHeight, &ImComponents, &ImFormat);

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents,
        ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    pBytes = LoadTGAImage(texNames[2], &ImWidth,
        &ImHeight, &ImComponents, &ImFormat);

    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight,
        0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    pBytes = LoadTGAImage(texNames[3], &ImWidth,
        &ImHeight, &ImComponents, &ImFormat);

    glBindTexture(GL_TEXTURE_2D, textures[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents,
        ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


    free(pBytes);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess = { 20.0 };


    GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
    float R = 10.0;
    GLfloat light_position_1[] = { R * cos(lightTheta) * cos(45.0),
        R * sin(45.0), R * sin(lightTheta) * cos(45.0), 1.0 };


    // 0.823, 0.929, 0.070 - yellow
    // 0.070, 0.070, 0.929 - blue
    // 0.929, 0.082, 0.070 - red
    // 1, 1, 1 - white

    // Ia = [Iar,Iag,Iab]
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat light_ambient_1[] = { 0.1, 0.1, 0.1, 0.0 };

    // Id = [Idr,Idg,Idb]
    GLfloat light_diffuse[] = { 1, 1, 1, 1.0 };  //  0.070, 0.070, 0.929, 1.0
    GLfloat light_diffuse_1[] = { 1, 1, 1, 0.0 };  // 0.929, 0.082, 0.07, 0.0

    // Is = [Isr,Isg,Isb]
    GLfloat light_specular[] = { 1, 1, 1, 1.0 };  //  0.070, 0.070, 0.929, 1.0
    GLfloat light_specular_1[] = { 1, 1, 1, 1.0 };  // 0.929, 0.082, 0.07, 1.0

    GLfloat att_constant = { 1.0 };
    GLfloat att_constant_1 = { 1.0 };


    GLfloat att_linear = { 0.05 };
    GLfloat att_linear_1 = { 0.05 };

    GLfloat att_quadratic = { 0.001 };
    GLfloat att_quadratic_1 = { 0.001 };


    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient_1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse_1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular_1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position_1);

    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant_1);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear_1);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic_1);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    cout << "Initialized";
}

void ChangeSize(GLsizei horizontal, GLsizei vertical) {
    pix2angleHorizontal = 360.0 / static_cast<float>(horizontal);
    pix2angleVertical = 360.0 / static_cast<float>(vertical);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 1.0, 1.0, 30.0);

    if (horizontal <= vertical) {
        glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
    } else {
        glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Mouse(int btn, int state, int x, int y) {
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        x_pos_old = x;
        y_pos_old = y;
        status = 1;
    } else {
        status = 0;
    }
}

void Motion(GLsizei x, GLsizei y) {
    delta_x = x - x_pos_old;
    x_pos_old = x;

    delta_y = y - y_pos_old;
    y_pos_old = y;

    glutPostRedisplay();
}

void timer_start(std::function<void(void)> func, unsigned int interval) {
    std::thread([func, interval]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            func();
        }
        }).detach();
}


void do_something() {
    if (index < 3) {
        index++;
    } else {
        index = 0;
    }
    cout << "\n" << index;
    glBindTexture(GL_TEXTURE_2D, textures[index]);
}


int main() {
    srand((unsigned)time(NULL));

    cout << "Enter number of points: ";
    cin >> N;
    cout << "\n" << "Try: '+','-','l','p', 't', 'r' ";

    getCoords();

    int argc = 1;
    char* argv[1] = { const_cast<char*>("Something") };
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(300, 300);

    glutCreateWindow("ROSE EGG");

    glutKeyboardFunc(keys);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutDisplayFunc(RenderScene);
    glutIdleFunc(spinLight);

    glutReshapeFunc(ChangeSize);

    MyInit();
    timer_start(do_something, 5000);

    glEnable(GL_DEPTH_TEST);

    glutMainLoop();
    return 0;
}
