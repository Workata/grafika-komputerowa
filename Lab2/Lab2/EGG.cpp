#define _USE_MATH_DEFINES   // https://www.quantstart.com/articles/Mathematical-Constants-in-C/

#include <cmath>
#include <math.h>       // pow, sin, cos
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include<iostream>
#include<vector>
#include <stdlib.h> // rand
#include <time.h> // ustawienie ziarna rand

using namespace std;

typedef float point3[3];

struct point {
    float x;
    float y;
    float z;
};

vector<vector<point>> coords;

int N;
static GLfloat theta[] = { 0.0, 0.0, 0.0 };  // trzy k¹ty obrotu; theta[0] - x, theta[1] - y, theta[2] - z, 
static GLfloat thetaMini[] = { 0.0, 0.0, 0.0 };  // trzy k¹ty obrotu; theta[0] - x, theta[1] - y, theta[2] - z, 
bool rotationX = FALSE, rotationY = TRUE;    // default rotation is Y
int model = 1;                              // 1- punkty, 2- linie, 3 - wype³nione trójk¹ty; default model -> points

// INSTRUKCJA: Zmiana osi obrotu: q/w, model line -> l, model triangle -> , model points -> p; 


// UWAGA: Prosze nie wlaczac trojkatow, jezeli odbiorca ma epilepsje; In case of epilepsy do not press "t"

float genFloatRGB() // generaterandom number <0.0, 1.0>
{
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);  //https://stackoverflow.com/questions/686353/random-float-number-generation
    return r;
}

// Funkcja, ktora zmienia kat polozenia jajka; jest automatycznie wywolywana co jakis czas (bo wczesniej byla zerejestrowana)
void spinEgg()
{
    if (rotationX)
    {
        theta[0] -= 0.5;
        if (theta[0] > 360.0) theta[0] -= 360.0;
    }

    if (rotationY)
    {
        theta[1] -= 0.5;
        if (theta[1] > 360.0) theta[1] -= 360.0;
    }

    // theta[2] -= 0.5;
    //  if (theta[2] > 360.0) theta[2] -= 360.0;

    if (rotationX)
    {
        thetaMini[0] -= 3;
        if (thetaMini[0] > 360.0) thetaMini[0] -= 360.0;
    }

    if (rotationY)
    {
        thetaMini[1] -= 3;
        if (thetaMini[1] > 360.0) thetaMini[1] -= 360.0;
    }

    glutPostRedisplay(); //odœwie¿enie zawartoœci aktualnego okna
}

// Funkcja rysuj¹ca osie uk³adu wspó³rzêdnych
void Axes(void)
{

    point3  x_min = { -5.0, 0.0, 0.0 };
    point3  x_max = { 5.0, 0.0, 0.0 };
    // pocz¹tek i koniec obrazu osi x

    point3  y_min = { 0.0, -5.0, 0.0 };
    point3  y_max = { 0.0,  5.0, 0.0 };
    // pocz¹tek i koniec obrazu osi y

    point3  z_min = { 0.0, 0.0, -5.0 };
    point3  z_max = { 0.0, 0.0,  5.0 };
    //  pocz¹tek i koniec obrazu osi y
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
                glVertex3f(coords[i][j].x,coords[i][j].y, coords[i][j].z);
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
                    glVertex3f(coords[i - 1][j-1].x, coords[i - 1][j-1].y, coords[i-1][j-1].z);
                    glVertex3f(coords[i - 1][j].x, coords[i - 1][j].y, coords[i-1][j].z);
                    glVertex3f(coords[i][j].x, coords[i][j].y, coords[i][j].z);
                    glVertex3f(coords[i][j-1].x, coords[i][j-1].y, coords[i][j-1].z);
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
   
    Axes();             // Narysowanie osi przy pomocy funkcji zdefiniowanej wy¿ej
    
    // Tutaj insert polecenia do wykonania

    if(rotationX) glRotatef(theta[0], 1.0, 0.0, 0.0);   // obrot wokol osi x     
    if(rotationY) glRotatef(theta[1], 0.0, 1.0, 0.0);   // obrot wokol osi y

    canIOfferYouANiceEggInThisTryingTime();

    if (rotationX) glRotatef(thetaMini[0], 1.0, 0.0, 0.0);   // obrot wokol osi x     
    if (rotationY) glRotatef(thetaMini[1], 0.0, 1.0, 0.0);   // obrot wokol osi y

    canIOfferYouANiceMiniEggInThisTryingTime();

    glFlush();                          // Przekazanie poleceñ rysuj¹cych do wykonania
    glutSwapBuffers();
}

void keysRotation(unsigned char key, int x, int y)
{
    if (key == 'q')
    {
        rotationX = TRUE;
        rotationY = FALSE;
    }
    if (key == 'w')
    {
        rotationX = FALSE;
        rotationY = TRUE;
    }

    RenderScene(); // przerysowanie obrazu sceny
}

void keysModel(unsigned char key, int x, int y)
{
    if (key == 'p') model = 1;
    if (key == 'l') model = 2;
    if (key == 't') model = 3;

    RenderScene(); // przerysowanie obrazu sceny
}

void keys(unsigned char key, int x, int y)
{
    keysRotation(key, x, y);
    keysModel(key,  x, y);
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
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

    GLfloat AspectRatio;
    // Deklaracja zmiennej AspectRatio  okreœlaj¹cej proporcjê
    // wymiarów okna 
    if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
        vertical = 1;
    glViewport(0, 0, horizontal, vertical);
    // Ustawienie wielkoœciokna okna widoku (viewport)
    // W tym przypadku od (0,0) do (horizontal, vertical)  
    glMatrixMode(GL_PROJECTION);
    // Prze³¹czenie macierzy bie¿¹cej na macierz projekcji 
    glLoadIdentity();
    // Czyszcznie macierzy bie¿¹cej            
    AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    // Wyznaczenie wspó³czynnika  proporcji okna
    // Gdy okno nie jest kwadratem wymagane jest okreœlenie tak zwanej
    // przestrzeni ograniczaj¹cej pozwalaj¹cej zachowaæ w³aœciwe
    // proporcje rysowanego obiektu.
    // Do okreslenia przestrzeni ograniczj¹cej s³u¿y funkcja
    // glOrtho(...)            
    if (horizontal <= vertical)

        glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
    else

        glOrtho(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5, 10.0, -10.0);
    glMatrixMode(GL_MODELVIEW);
    // Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu                                   

    glLoadIdentity();
    // Czyszcenie macierzy bie¿¹cej
}

/*************************************************************************************/

// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli



int main()
{
    /*canIOfferYouANiceEggInThisTryingTime();
    return 0;*/

    srand((unsigned)time(NULL)); // rand sie nie powtarza z kazdym uruchomieniem; inne ziarno

    cout << "Enter number of points: ";
    cin >> N;
    cout << "\n" << "Try: 'q','w','l','p' and on your own responsibility 't' ";

    getCoords();

    // freeglut ERROR: Function <glutCreateWindow> called without first calling 'glutInit'; glutInit() nie ma w pierwszym programie przykladowym na stronie kursu; bez tego byl blad kompilacji
    // https://stackoverflow.com/questions/14410404/glutinit-arguments 
    int argc = 1;
    char* argv[1] = { (char*)"Something" };
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(300, 300);

    glutCreateWindow("Nice egg bro");

    glutIdleFunc(spinEgg);              // rejestracja funkcji spinEgg (funkcja zwrotna)
    // glutKeyboardFunc(keysRotation);     // rejestracja funkcji keysRotation (funkcja zwrotna)
    // glutKeyboardFunc(keysModel);        // rejestracja funkcji keysModel (funkcja zwrotna)
    glutKeyboardFunc(keys);                // moze byc tylko jedna funkcja przyciskow zarejestrowana jednoczesnie
    glutDisplayFunc(RenderScene);      // Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹ (callback function).  Bedzie ona wywo³ywana za ka¿dym razem gdy zajdzie potrzba przeryswania okna 
    

    glutReshapeFunc(ChangeSize);
    // Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹
    // zazmiany rozmiaru okna      
    MyInit();
    // Funkcja MyInit() (zdefiniowana powy¿ej) wykonuje wszelkie
    // inicjalizacje konieczne  przed przyst¹pieniem do renderowania
    glEnable(GL_DEPTH_TEST);
    // W³¹czenie mechanizmu usuwania powierzchni niewidocznych

    glutMainLoop();
    // Funkcja uruchamia szkielet biblioteki GLUT
    return 0;
}
