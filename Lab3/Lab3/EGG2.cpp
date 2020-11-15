#define _USE_MATH_DEFINES   // https://www.quantstart.com/articles/Mathematical-Constants-in-C/

#include <cmath>
#include <math.h>           // pow, sin, cos

#include<windows.h>
#include<gl/gl.h>
#include<gl/glut.h>

#include<iostream>
#include<vector>
#include<stdlib.h>          // rand
#include<time.h>            // ustawienie ziarna rand

using namespace std;

typedef float point3[3];
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };       // coords of observer NEW!!!!!!!!!!!!

struct point {
    float x;
    float y;
    float z;
};

vector<vector<point>> coords;

int N;
int model = 1;                                      // 1- punkty, 2- linie, 3 - wype³nione trójk¹ty; default model -> points

// global variables for interactions
static GLfloat theta = 0.0;     // k¹t obrotu obiektu w poziomie
static GLfloat omega = 0.0;     // k¹t obrotu obiektu w pionie  NEW!
static GLfloat pix2angleHorizontal;       // przelicznik pikseli na stopnie (horizontal)
static GLfloat pix2angleVertical;         // przelicznik pikseli na stopnie (vertical) NEW!
static GLint status = 0;        // stan klawiszy myszy: 0 - nie naciœniêto ¿adnego klawisza, 1 - naciœniêty zostaæ lewy klawi
static int x_pos_old = 0;       // poprzednia pozycja kursora myszy pozioma
static int y_pos_old = 0;       // poprzednia pozycja kursora myszy pionowa NEW!
static int delta_x = 0;         // ró¿nica pomiêdzy pozycj¹ bie¿¹c¹ i poprzedni¹ kursora myszy
static int delta_y = 0;         // NEW!

// INSTRUKCJA: Zmiana osi obrotu: q/w, model line -> l, model triangle -> , model points -> p; 
// UWAGA: Prosze nie wlaczac trojkatow, jezeli odbiorca ma epilepsje; In case of epilepsy do not press "t"

float genFloatRGB() // generaterandom number <0.0, 1.0>
{
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);  //https://stackoverflow.com/questions/686353/random-float-number-generation
    return r;
}


// Funkcja rysuj¹ca osie uk³adu wspó³rzêdnych
void Axes(void)
{

    point3  x_min = { -5.0, 0.0, 0.0 };
    point3  x_max = { 5.0, 0.0, 0.0 };
    // pocz?tek i koniec obrazu osi x

    point3  y_min = { 0.0, -5.0, 0.0 };
    point3  y_max = { 0.0,  5.0, 0.0 };
    // pocz?tek i koniec obrazu osi y

    point3  z_min = { 0.0, 0.0, -5.0 };
    point3  z_max = { 0.0, 0.0,  5.0 };
    //  pocz?tek i koniec obrazu osi y
    glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
    glBegin(GL_LINES); // rysowanie osi x
    glVertex3fv(x_min);
    glVertex3fv(x_max);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
    glBegin(GL_LINES);  // rysowanie osi y

    glVertex3fv(y_min);
    glVertex3fv(y_max);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
    glBegin(GL_LINES); // rysowanie osi z

    glVertex3fv(z_min);
    glVertex3fv(z_max);
    glEnd();

}

void getCoords()
{
    float x, y, z, u, v;
    vector<point> vec;
    point point;
    for (int i = 0; i < N; i++)
    {

        for (int j = 0; j < N; j++)
        {
            u = (float)j / (float)N;
            v = (float)i / (float)N;

            point.x = ((-90) * pow(u, 5) + 225 * pow(u, 4) + (-270) * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v);
            point.y = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2);
            point.z = ((-90) * pow(u, 5) + 225 * pow(u, 4) + (-270) * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v);

            vec.push_back(point);
        }
        coords.push_back(vec);
        vec.resize(0);
    }
}


void canIOfferYouANiceEggInThisTryingTime()
{
    point3 point;
    float x, y, z, u, v;

    glColor3f(0.960f, 0.917f, 0.000f);

    // Set position of an EGG and also maybe rotation

    // glRotated(30.0, 1.0, 1.0, 1.0); // left
    glRotated(20.0, 1.0, 0.0, 0.0); // (angle) [ vector ] 
    glTranslatef(0.0f, -5.0f, 0.0f);

    if (model == 1)
    {
        glBegin(GL_POINTS);

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                glVertex3f(coords[i][j].x, coords[i][j].y, coords[i][j].z);
            }

        }
        glEnd();

    }
    else if (model == 2)
    {
        glBegin(GL_LINES);

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (i != 0 and j != 0)
                {
                    glVertex3f(coords[i - 1][j - 1].x, coords[i - 1][j - 1].y, coords[i - 1][j - 1].z);
                    glVertex3f(coords[i - 1][j].x, coords[i - 1][j].y, coords[i - 1][j].z);
                    glVertex3f(coords[i][j].x, coords[i][j].y, coords[i][j].z);
                    glVertex3f(coords[i][j - 1].x, coords[i][j - 1].y, coords[i][j - 1].z);
                    //glVertex3f(coords[i - 1][j - 1].x, coords[i - 1][j - 1].y, coords[i - 1][j - 1].z);
                }
                else glVertex3f(coords[i][j].x, coords[i][j].y, coords[i][j].z);
            }

        }
        glEnd();
    }
    else if (model == 3)
    {
        glBegin(GL_TRIANGLES);

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (i != 0 and j != 0)
                {
                    glColor3f(genFloatRGB(), genFloatRGB(), genFloatRGB());
                    glVertex3f(coords[i - 1][j - 1].x, coords[i - 1][j - 1].y, coords[i - 1][j - 1].z);
                    glVertex3f(coords[i - 1][j].x, coords[i - 1][j].y, coords[i - 1][j].z);
                    glVertex3f(coords[i][j].x, coords[i][j].y, coords[i][j].z);
                    //glVertex3f(coords[i - 1][j - 1].x, coords[i - 1][j - 1].y, coords[i - 1][j - 1].z);
                }
                else glVertex3f(coords[i][j].x, coords[i][j].y, coords[i][j].z);
            }

        }
        glEnd();
    }
}

void canIOfferYouANiceMiniEggInThisTryingTime()
{
    point3 point;
    float x, y, z, u, v;

    glColor3f(0.960f, 0.917f, 0.000f);

    // Set position of an EGG and also maybe rotation

    // glRotated(30.0, 1.0, 1.0, 1.0); // left
    //glRotated(20.0, 1.0, 0.0, 0.0); // (angle) [ vector ] 
    glTranslatef(4.0f, 5.5f, 0.0f);
    glScalef(0.5, 0.5, 0.5);

    if (model == 1)
    {

        glBegin(GL_POINTS);
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                glVertex3f(coords[i][j].x, coords[i][j].y, coords[i][j].z);
            }

        }
        glEnd();
    }
    else if (model == 2)
    {
        glBegin(GL_LINES);

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (i != 0 and j != 0)
                {
                    glVertex3f(coords[i - 1][j - 1].x, coords[i - 1][j - 1].y, coords[i - 1][j - 1].z);
                    glVertex3f(coords[i - 1][j].x, coords[i - 1][j].y, coords[i - 1][j].z);
                    glVertex3f(coords[i][j].x, coords[i][j].y, coords[i][j].z);
                    glVertex3f(coords[i][j - 1].x, coords[i][j - 1].y, coords[i][j - 1].z);
                    //glVertex3f(coords[i - 1][j - 1].x, coords[i - 1][j - 1].y, coords[i - 1][j - 1].z);
                }
                else glVertex3f(coords[i][j].x, coords[i][j].y, coords[i][j].z);
            }

        }
        glEnd();
    }
    else if (model == 3)
    {
        glBegin(GL_TRIANGLES);

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (i != 0 and j != 0)
                {
                    glColor3f(genFloatRGB(), genFloatRGB(), genFloatRGB());
                    glVertex3f(coords[i - 1][j - 1].x, coords[i - 1][j - 1].y, coords[i - 1][j - 1].z);
                    glVertex3f(coords[i - 1][j].x, coords[i - 1][j].y, coords[i - 1][j].z);
                    glVertex3f(coords[i][j].x, coords[i][j].y, coords[i][j].z);
                    //glVertex3f(coords[i - 1][j - 1].x, coords[i - 1][j - 1].y, coords[i - 1][j - 1].z);
                }
                else glVertex3f(coords[i][j].x, coords[i][j].y, coords[i][j].z);
            }

        }
        glEnd();
    }
}



// Funkcja okreœlaj¹ca co ma byæ rysowane (zawsze wywo³ywana gdy trzeba
// przerysowaæ scenê)
void RenderScene(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Czyszczenie okna aktualnym kolorem czyszcz¹cym


    glLoadIdentity();   // Czyszczenie macierzy bie¿¹cej

    gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);  // Zdefiniowanie po³o¿enia obserwatora NEW!!!!!!!!!!!!
   

    Axes();             // Narysowanie osi przy pomocy funkcji zdefiniowanej wy¿ej

    if (status == 1)                     // jeœli lewy klawisz myszy wciêniêty
    {
        theta += delta_x * pix2angleHorizontal;    // modyfikacja k¹ta obrotu o kat proporcjonalny  do ró¿nicy po³o¿eñ kursora myszy
        omega += delta_y * pix2angleVertical;      // NEW 
    }                                  

    glRotatef(theta, 0.0, 1.0, 0.0);  //obrót obiektu o nowy k¹t    glRotated(angle, x, y, z) [x, y, z] vector?, Probably yes.
    glRotatef(omega, 1.0, 0.0, 0.0);    // New

    // Tutaj insert polecenia do wykonania

    //if (rotationX) glRotatef(theta[0], 1.0, 0.0, 0.0);   // obrot wokol osi x     
    //if (rotationY) glRotatef(theta[1], 0.0, 1.0, 0.0);   // obrot wokol osi y

    canIOfferYouANiceEggInThisTryingTime();

    //if (rotationX) glRotatef(thetaMini[0], 1.0, 0.0, 0.0);   // obrot wokol osi x     
    //if (rotationY) glRotatef(thetaMini[1], 0.0, 1.0, 0.0);   // obrot wokol osi y

    canIOfferYouANiceMiniEggInThisTryingTime();

    glFlush();                          // Przekazanie poleceñ rysuj¹cych do wykonania
    glutSwapBuffers();
}



void keysZoom(unsigned char key, int x, int y)
{
    if (key == '+') viewer[2] -= 1;
    if (key == '-') viewer[2] += 1;

    RenderScene(); // przerysowanie obrazu sceny
}

void keysModel(unsigned char key, int x, int y)
{
    if (key == 'p') model = 1;
    if (key == 'l') model = 2;
    if (key == 't') model = 3;
    if (key == 'r')
    {
        theta = 0.0;
        omega = 0.0;
        viewer[0] = 0.0;    // not needed
        viewer[1] = 0.0;    // not needed
        viewer[2] = 10.0;

    }

    RenderScene(); // przerysowanie obrazu sceny
}

void keys(unsigned char key, int x, int y)
{
    keysModel(key, x, y);
    keysZoom(key, x, y);
}

// Funkcja ustalaj¹ca stan renderowania
void MyInit(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Kolor czyszc¹cy (wype³nienia okna) ustawiono na czarny
}

// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹
// przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)       // NEW !!!!!!!!!!!!
{
    pix2angleHorizontal = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie
    pix2angleVertical = 360.0 / (float)vertical;  // przeliczenie pikseli na stopnie
    glMatrixMode(GL_PROJECTION);            // Prze³¹czenie macierzy bie¿¹cej na macierz projekcji
    glLoadIdentity();                       // Czyszcznie macierzy bie¿¹cej
    gluPerspective(70, 1.0, 1.0, 30.0);      // Ustawienie parametrów dla rzutu perspektywicznego
  
    if (horizontal <= vertical) glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal); // Ustawienie wielkoœci okna okna widoku (viewport) w zale¿noœci relacji pomiêdzy wysokoœci¹ i szerokoœci¹ okna
    else glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
        
    glMatrixMode(GL_MODELVIEW);     // Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu 
    glLoadIdentity();               // Czyszczenie macierzy bie¿¹cej
}

// new callback functions for mouse interaction:
// Funkcja "bada" stan myszy i ustawia wartoœci odpowiednich zmiennych globalnych
void Mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        x_pos_old = x;           // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
        y_pos_old = y;           // NEW!
        status = 1;              // wciêniêty zosta³ lewy klawisz myszy
    }
    else status = 0;             // nie zosta³ wciêniêty ¿aden klawisz
}

// Funkcja "monitoruje" po³o¿enie kursora myszy i ustawia wartoœci odpowiednich
// zmiennych globalnych
void Motion(GLsizei x, GLsizei y)
{
    delta_x = x - x_pos_old;     // obliczenie ró¿nicy po³o¿enia kursora myszy
    x_pos_old = x;               // podstawienie bie¿¹cego po³o¿enia jako poprzednie

    delta_y = y - y_pos_old;     // NEW
    y_pos_old = y;               // 

    glutPostRedisplay();         // przerysowanie obrazu sceny
}

// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli
int main()
{
    srand((unsigned)time(NULL)); // rand sie nie powtarza z kazdym uruchomieniem; inne ziarno

    cout << "Enter number of points: ";
    cin >> N;
    cout << "\n" << "Try: '+','-','l','p', 't', 'r' ";

    getCoords();

    // freeglut ERROR: Function <glutCreateWindow> called without first calling 'glutInit'; glutInit() nie ma w pierwszym programie przykladowym na stronie kursu; bez tego byl blad kompilacji
    // https://stackoverflow.com/questions/14410404/glutinit-arguments 
    int argc = 1;
    char* argv[1] = { (char*)"Something" };
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(300, 300);

    glutCreateWindow("Nice egg2 bro");

   // glutIdleFunc(spinEgg);                  // rejestracja funkcji spinEgg (funkcja zwrotna)
    // glutKeyboardFunc(keysRotation);       // rejestracja funkcji keysRotation (funkcja zwrotna)
    // glutKeyboardFunc(keysModel);          // rejestracja funkcji keysModel (funkcja zwrotna)
    glutKeyboardFunc(keys);                  // moze byc tylko jedna funkcja przyciskow zarejestrowana jednoczesnie
    glutMouseFunc(Mouse);                    // Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie stanu myszy
    glutMotionFunc(Motion);                  // Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie ruchu m
    glutDisplayFunc(RenderScene);            // Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹ (callback function).  Bedzie ona wywo³ywana za ka¿dym razem gdy zajdzie potrzba przeryswania okna 

    glutReshapeFunc(ChangeSize);             // Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹ za zmiany rozmiaru okna      
    MyInit();                                // Funkcja MyInit() (zdefiniowana powy¿ej) wykonuje wszelkie inicjalizacje konieczne  przed przyst¹pieniem do renderowania
    
    glEnable(GL_DEPTH_TEST);                 // W³¹czenie mechanizmu usuwania powierzchni niewidocznych
    
    glutMainLoop();                          // Funkcja uruchamia szkielet biblioteki GLUT
    return 0;
}
