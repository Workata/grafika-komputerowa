// Rysowanie kwadratu + proporcjonalne skalowanie w zaleznosci od wielkosci okna
#include <windows.h>
#include <utility> 
#include <iostream> 
#include <stdlib.h> // rand
#include <time.h> // ustawienie ziarna rand
#include <gl/gl.h>
#include <gl/glut.h>

using namespace std;

int N; // liczba poziomow

float genFloatRGB() // generaterandom number <0.0, 1.0>
{
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);  //https://stackoverflow.com/questions/686353/random-float-number-generation
    return r;
}

float genFloatOffset() // generaterandom number <<?>
{
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);  
    return (float) 2*r;
}

void drawPolygon(float x1, float y1, float x2, float y2)
{
    glBegin(GL_POLYGON);

    glColor3f(genFloatRGB(), genFloatRGB(), genFloatRGB());
    glVertex2f(x1+ genFloatOffset(), y1+ genFloatOffset());

    glColor3f(genFloatRGB(), genFloatRGB(), genFloatRGB());
    glVertex2f(x2 + genFloatOffset(), y1 + genFloatOffset());

    glColor3f(genFloatRGB(), genFloatRGB(), genFloatRGB());
    glVertex2f(x2 + genFloatOffset(), y2 + genFloatOffset());

    glColor3f(genFloatRGB(), genFloatRGB(), genFloatRGB());
    glVertex2f(x1 + genFloatOffset(), y2 + genFloatOffset());

    glEnd();
}

void divRect(float x1, float y1, float x2, float y2, int numberOfLevels, bool firstTime)
{
    if (numberOfLevels <= 0) return;

    if (firstTime)  // transform -> no zle to sobie narysowalem na kartce i to jest jednak ta druga przekatna dlatego jest ta transformacja wierzcholkow; Transformacja jest wykonywana tylko raz!
    {
        y1 = 0 - y1;
        y2 = 0 - y2;
        firstTime = FALSE;
    }

    //drawPolygon(x1, y1, x2, y2);
    //return;

    // height = width 
    float width = x2 - x1;
    float height = y2 - y1;

    //glColor3f(0.0f, 1.0f, 0.0f);
    drawPolygon(x1, y1, x2, y2);   //glRectf(x1, y1, x2, y2);        // kwadrat bazowy

    pair <float, float> vertex1(x1, y1);
    pair <float, float> vertex2(x1, height/3 + y1);
    pair <float, float> vertex3(x1, (2*height)/3 + y1);
    pair <float, float> vertex4(width/3 + x1, height + y1);
    pair <float, float> vertex5(height/3 + x1, (2*height)/3 + y1);
    pair <float, float> vertex6(height/3 + x1, height/3 + y1);
    pair <float, float> vertex7(height/3 + x1, y1);
    pair <float, float> vertex8((2*height)/3 + x1, y1);
    pair <float, float> vertex9((2*height)/3 + x1, height/3 + y1);
    pair <float, float> vertex10((2*height)/3 + x1, (2*height)/3 + y1);
    pair <float, float> vertex11((2*height)/3 + x1, height + y1);
    pair <float, float> vertex12(height + x1, height + y1);
    pair <float, float> vertex13(height + x1, (2*height)/3 + y1);
    pair <float, float> vertex14(height + x1, height/3 + y1);

    //cout << "x1, y1: " << x1 << y1 << "x2, y2: " << x2 << y2;
    //cout << "v6x: " << vertex6.first << "v6y" << vertex6.second;
    //cout << vertex6.first << " " << vertex6.second << " " << vertex10.first << " " << vertex10.second;

    glColor3f(0.305f, 0.305f, 0.294f);    // tutaj insert background color
    glRectf(vertex6.first, vertex6.second, vertex10.first, vertex10.second);    // kwadrat srodkowy

    // wywolania rekurencyjne dla pozostalych kwadratow
    divRect(vertex1.first, vertex1.second, vertex6.first, vertex6.second, numberOfLevels - 1, FALSE);       // 1
    divRect(vertex2.first, vertex2.second, vertex5.first, vertex5.second, numberOfLevels - 1, FALSE);       // 2
    divRect(vertex3.first, vertex3.second, vertex4.first, vertex4.second, numberOfLevels - 1, FALSE);       // 3
    divRect(vertex5.first, vertex5.second, vertex11.first, vertex11.second, numberOfLevels - 1, FALSE);     // 4
    divRect(vertex7.first, vertex7.second, vertex9.first, vertex9.second, numberOfLevels - 1, FALSE);        // 6
    divRect(vertex10.first, vertex10.second, vertex12.first, vertex12.second, numberOfLevels - 1, FALSE);   // 9
    divRect(vertex9.first, vertex9.second, vertex13.first, vertex13.second, numberOfLevels - 1, FALSE);     // 8
    divRect(vertex8.first, vertex8.second, vertex14.first, vertex14.second, numberOfLevels - 1, FALSE);     // 7
}



// Funkcaja okre�laj�ca, co ma by� rysowane
// (zawsze wywo�ywana, gdy trzeba przerysowa� scen�)
void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT); // Czyszczenie okna (aktualnym kolorem czyszcz�cym)

    // polecenia rysuj�ce do wykonania

    divRect(-50.0f, 50.0f, 50.0f, -50.0f, N, TRUE);

    glFlush();                             // Przekazanie polece� rysuj�cych do wykonania
}

// Funkcja ustalaj�ca stan renderowania
void MyInit(void)
{
    glClearColor(0.305f, 0.305f, 0.294f, 1.0f);     // Kolor okna wn�trza okna - ustawiono na szary (R, G, B, alpha)
}

// Parametry horizontal i vertical (szeroko�� i wysoko�� okna) s� przekazywane do funkcji za ka�dym razem, gdy zmieni si� rozmiar okna
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    // Deklaracja zmiennej AspectRatio okre�laj�cej proporcj� wymiar�w okna
    GLfloat AspectRatio;

    // Zabezpiecznie przed dzieleniem przez zero
    if (vertical == 0) vertical = 1;

    // Ustawienie wielko�ci okna urz�dzenia (Viewport)
    // W tym przypadku od (0, 0) do (horizontal, vertical)
    glViewport(0, 0, horizontal, vertical);

    // Okre�lenie uk�adu wsp�rz�dnych obserwatora
    glMatrixMode(GL_PROJECTION);

    // Okre�lenie przestrzeni ograniczaj�cej
    glLoadIdentity();

    // Wyznaczenie wsp�czynnika proporcji okna
    AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    // Gdy okno na ekranie nie jest kwadratem wymagane jest okr�lenie okna obserwatora
    // Pozwala to zachowa� w�a�ciwe proporcje rysowanego obiektu
    // Do okre�lenie okna obserwatora s�u�y funkcja glOrtho(...)

    if (horizontal <= vertical) glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);
    else glOrtho(-100.0 * AspectRatio, 100.0 * AspectRatio, -100.0, 100.0, 1.0, -1.0);

    // Okre�lenie uk�adu wsp�rz�dnych
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli
int main()
{
    srand((unsigned)time(NULL)); // rand sie nie powtarza z kazdym uruchomieniem; inne ziarno

    cout << "How many levels?\n";
    cin >> N;

    // freeglut ERROR: Function <glutCreateWindow> called without first calling 'glutInit'; glutInit() nie ma w pierwszym programie przykladowym na stronie kursu; bez tego byl blad kompilacji
    // https://stackoverflow.com/questions/14410404/glutinit-arguments 
    int argc = 1;
    char* argv[1] = { (char*)"Something" };
    glutInit(&argc, argv);

    // Ustawienie trybu wy�wietlania
    // GLUT_SINGLE - pojedynczy bufor wy�wietlania
    // GLUT_RGBA - model kolor�w RGB
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

    glutCreateWindow("Trzeci program w OpenGL"); // Utworzenie okna i okre�lenie tre�ci napisu w nag��wku okna

    // Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn� (callback)
    // Biblioteka GLUT b�dzie wywo�ywa�a t� funkcj� za ka�dym razem, gdy trzeba b�dzie przerysowa� okno
    glutDisplayFunc(RenderScene);

    // Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn� za zmiany rozmiaru okna
    glutReshapeFunc(ChangeSize);

    // Funkcja MyInit (zdefiniowana powy�ej) wykonuje wszelkie inicjalizacje konieczne przed przyst�pieniem do renderowania
    MyInit();

    // Funkcja uruchamia szkielet biblioteki GLUT
    glutMainLoop();

    return 0;

}