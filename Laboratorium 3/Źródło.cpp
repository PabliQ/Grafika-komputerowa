/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj� osi 
//  uk�adu wsp�rzednych

/*************************************************************************************/

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <ctime>

#define M_PI           3.14159265358979323846

using namespace std;

// Definicja punktu OpenGL
typedef float point3[3];

// Zmienna przechowuj�ca aktualn� pozycj� obserwatora
// Wykorzystywana przy zbli�aniu i oddalaniu modelu
static GLfloat observerPosition[] = { 0.0, 0.0, 15.0 };

// Zmienne przchowuj�ce aktualny k�t obrotu obiektu
// Pocz�tkowa pozycja ma k�t 0 stopni
static GLfloat thetaU = 1.0;
static GLfloat thetaV = 1.0;

// Zmienna przechowuj�ca stan naci�ni�cia klawiszy
// Zmienia warto�� na 1 je�eli klawisz LMB jest naci�ni�ty
static int buttonState = 0;

// Zmienne przechowuj�ce pozycj� myszy oraz r�nic�
// W poztycji myszy w czasie
static int xMousePosition = 0;
static int xMousePositionDifference = 0;
static int yMousePosition = 0;
static int yMousePositionDifference = 0;

// Zmienne przechowuj�ce warto�� zmiany k�ta
// Wyra�on� w ilo�ci pikseli
static GLfloat xAngleInPixels;
static GLfloat yAngleInPixels;

// Promie� obiektu
static GLfloat radiusStart = 15.0;
static GLfloat raduisCurrent = 0.0;
static GLfloat radiusDifference = 0.0;

// Stosunek d�ugo�ci do wysoko�ci okna
float aspectRatio;

// G�sto�� wykona� algorytmu
const int N = 100;

// Struktura przechowuj�ca wsp�rz�dne i kolory danego punktu
struct point
{
    double x;
    double y;
    double z;
    float r;
    float g;
    float b;
};

// Macierz punkt�w
point matrix[N][N];

// Funkcja generuj�ca losow� warto�� typu float
float randomFloat()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

// Generowanie punkt�w do macierzy
void generateTable()
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            // Zmienne wykorzystywane w r�wnaniach
            double u = (double)i / ((double)N - 1);
            double v = (double)j / ((double)N - 1);
            // Implementacja r�wna� podanych w instrukcji
            matrix[i][j].x = (-(90 * pow(u, 5)) + (225 * pow(u, 4)) - (270 * pow(u, 3)) + (180 * pow(u, 2)) - 45 * u) * cos(M_PI * v);
            matrix[i][j].y = (160 * pow(u, 4)) - (320 * pow(u, 3)) + (160 * pow(u, 2));
            matrix[i][j].z = (-(90 * pow(u, 5)) + (225 * pow(u, 4)) - (270 * pow(u, 3)) + (180 * pow(u, 2)) - 45 * u) * sin(M_PI * v);
            // Losowe kolory dla punktu
            matrix[i][j].r = randomFloat();
            matrix[i][j].b = randomFloat();
            matrix[i][j].g = randomFloat();
        }
    }
};

/*poni�sza funkcja rysuje jajko poprzez po��czenie punkt�w tr�jk�tami */
void drawEgg()
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            // Pierwszy tr�jk�t
            glBegin(GL_TRIANGLES);
            glColor3f(matrix[i][j].r, matrix[i][j].g, matrix[i][j].b);
            glVertex3f((float)matrix[i][j].x, (float)matrix[i][j].y - 5.0f, (float)matrix[i][j].z);
            glColor3f(matrix[(i + 1) % N][j].r, matrix[(i + 1) % N][j].g, matrix[(i + 1) % N][j].b);
            glVertex3f((float)matrix[(i + 1) % N][j].x, (float)matrix[(i + 1) % N][j].y - 5.0f, (float)matrix[(i + 1) % N][j].z);
            glColor3f(matrix[(i + 1) % N][(j + 1) % N].r, matrix[(i + 1) % N][(j + 1) % N].g, matrix[(i + 1) % N][(j + 1) % N].b);
            glVertex3f((float)matrix[(i + 1) % N][(j + 1) % N].x, (float)matrix[(i + 1) % N][(j + 1) % N].y - 5.0f, (float)matrix[(i + 1) % N][(j + 1) % N].z);
            glEnd();
            // Drugi tr�jk�t
            glBegin(GL_TRIANGLES);
            glColor3f(matrix[i][j].r, matrix[i][j].g, matrix[i][j].b);
            glVertex3f((float)matrix[i][j].x, (float)matrix[i][j].y - 5.0f, (float)matrix[i][j].z);
            glColor3f(matrix[i][(j + 1) % N].r, matrix[i][(j + 1) % N].g, matrix[i][(j + 1) % N].b);
            glVertex3f((float)matrix[i][(j + 1) % N].x, (float)matrix[i][(j + 1) % N].y - 5.0f, (float)matrix[i][(j + 1) % N].z);
            glColor3f(matrix[(i + 1) % N][(j + 1) % N].x, matrix[(i + 1) % N][(j + 1) % N].g, matrix[(i + 1) % N][(j + 1) % N].b);
            glVertex3f((float)matrix[(i + 1) % N][(j + 1) % N].x, (float)matrix[(i + 1) % N][(j + 1) % N].y - 5.0f, (float)matrix[(i + 1) % N][(j + 1) % N].z);
            glEnd();
        }
    }
}

void RenderScene(void)
{
    // Czyszczenie ekranu
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    // Zmiana stanu zmiennej okreslaj�cej naci�ni�cie guzika
    if (buttonState == 1)
    {
        // Aktualizacja parametr�w k�towych
        thetaU += xMousePositionDifference * xAngleInPixels * 0.1;
        thetaV += yMousePositionDifference * yAngleInPixels * 0.1;
    }

    // Zmiana pozycji obserwatora zgodnie z r�wnaniami
    observerPosition[0] = radiusStart * cos(thetaU) * cos(thetaV);
    observerPosition[1] = radiusStart * sin(thetaV);
    observerPosition[2] = radiusStart * sin(thetaU) * cos(thetaV);
    gluLookAt(observerPosition[0], observerPosition[1], observerPosition[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    drawEgg();
    glFlush();
    glutSwapBuffers();
}

// Funkcja obs�uguj�ca naci�ni�cie guzika myszy
// Wywo�ywana w momencie naci�ni�cia przycisk�w myszy
void MouseButtonState(int btn, int state, int x, int y)
{

    // Zmiana stanu zmiennej okreslaj�cej naci�ni�cie guzika
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Aktualizacja pozycji kursora
        xMousePosition = x;
        yMousePosition = y;
        buttonState = 1;
    }
    else
        buttonState = 0;
}

// Funkcja oblicza potrzebne parametry w zalezno�ci od zmiany po�o�enia kursora
// Wywo�ywana w momencie poruszenia mysz�
void MousePosition(GLsizei x, GLsizei y)
{
    // Obliczenie r�nicy w pozycji myszy
    xMousePositionDifference = x - xMousePosition;
    yMousePositionDifference = y - yMousePosition;
    radiusDifference = y - raduisCurrent;
    // Zapisanie aktualnej pozycji myszy
    xMousePosition = x;
    yMousePosition = y;
    raduisCurrent = y;
    // Od�wie�enie widoku
    glutPostRedisplay();
}

// Obs�uga zmiany rozmiaru okna
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    // Przeliczenie zmiany ilo�ci pikseli w ruchu myszy na stopnie
    xAngleInPixels = 360.0 / (float)horizontal;
    yAngleInPixels = 360.0 / (float)vertical;
    // Przej�cie w tryb projekcji
    glMatrixMode(GL_PROJECTION);
    // Czyszczenie macierzy bie��cej
    glLoadIdentity();
    aspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    // Ustawienie parametr�w dla rzutu perspektywicznego
    gluPerspective(70, aspectRatio, 1.0, 30.0);
    // Ustawienie okna widoku w zalezno�ci od stostunku x i y okna
    if (horizontal <= vertical)
    {
        glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
    }
    else
    {
        glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
    }

    // Powr�t do trybu modelu
    glMatrixMode(GL_MODELVIEW);
    // Czyszczenie macierzy bie��cej
    glLoadIdentity();
}

int main(int argc, char** argv){
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    generateTable();
    glutInitWindowSize(800, 600);
    glutCreateWindow("Laboratorium 4");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutMouseFunc(MouseButtonState);
    glutMotionFunc(MousePosition);
    glutMainLoop();
}