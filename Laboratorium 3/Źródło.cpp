/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj¹ osi 
//  uk³adu wspó³rzednych

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

// Zmienna przechowuj¹ca aktualn¹ pozycjê obserwatora
// Wykorzystywana przy zbli¿aniu i oddalaniu modelu
static GLfloat observerPosition[] = { 0.0, 0.0, 15.0 };

// Zmienne przchowuj¹ce aktualny k¹t obrotu obiektu
// Pocz¹tkowa pozycja ma k¹t 0 stopni
static GLfloat thetaU = 1.0;
static GLfloat thetaV = 1.0;

// Zmienna przechowuj¹ca stan naciœniêcia klawiszy
// Zmienia wartoœæ na 1 je¿eli klawisz LMB jest naciœniêty
static int buttonState = 0;

// Zmienne przechowuj¹ce pozycjê myszy oraz ró¿nicê
// W poztycji myszy w czasie
static int xMousePosition = 0;
static int xMousePositionDifference = 0;
static int yMousePosition = 0;
static int yMousePositionDifference = 0;

// Zmienne przechowuj¹ce wartoœæ zmiany k¹ta
// Wyra¿on¹ w iloœci pikseli
static GLfloat xAngleInPixels;
static GLfloat yAngleInPixels;

// Promieñ obiektu
static GLfloat radiusStart = 15.0;
static GLfloat raduisCurrent = 0.0;
static GLfloat radiusDifference = 0.0;

// Stosunek d³ugoœci do wysokoœci okna
float aspectRatio;

// Gêstoœæ wykonañ algorytmu
const int N = 100;

// Struktura przechowuj¹ca wspó³rzêdne i kolory danego punktu
struct point
{
    double x;
    double y;
    double z;
    float r;
    float g;
    float b;
};

// Macierz punktów
point matrix[N][N];

// Funkcja generuj¹ca losow¹ wartoœæ typu float
float randomFloat()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

// Generowanie punktów do macierzy
void generateTable()
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            // Zmienne wykorzystywane w równaniach
            double u = (double)i / ((double)N - 1);
            double v = (double)j / ((double)N - 1);
            // Implementacja równañ podanych w instrukcji
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

/*poni¿sza funkcja rysuje jajko poprzez po³¹czenie punktów trójk¹tami */
void drawEgg()
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            // Pierwszy trójk¹t
            glBegin(GL_TRIANGLES);
            glColor3f(matrix[i][j].r, matrix[i][j].g, matrix[i][j].b);
            glVertex3f((float)matrix[i][j].x, (float)matrix[i][j].y - 5.0f, (float)matrix[i][j].z);
            glColor3f(matrix[(i + 1) % N][j].r, matrix[(i + 1) % N][j].g, matrix[(i + 1) % N][j].b);
            glVertex3f((float)matrix[(i + 1) % N][j].x, (float)matrix[(i + 1) % N][j].y - 5.0f, (float)matrix[(i + 1) % N][j].z);
            glColor3f(matrix[(i + 1) % N][(j + 1) % N].r, matrix[(i + 1) % N][(j + 1) % N].g, matrix[(i + 1) % N][(j + 1) % N].b);
            glVertex3f((float)matrix[(i + 1) % N][(j + 1) % N].x, (float)matrix[(i + 1) % N][(j + 1) % N].y - 5.0f, (float)matrix[(i + 1) % N][(j + 1) % N].z);
            glEnd();
            // Drugi trójk¹t
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
    // Zmiana stanu zmiennej okreslaj¹cej naciœniêcie guzika
    if (buttonState == 1)
    {
        // Aktualizacja parametrów k¹towych
        thetaU += xMousePositionDifference * xAngleInPixels * 0.1;
        thetaV += yMousePositionDifference * yAngleInPixels * 0.1;
    }

    // Zmiana pozycji obserwatora zgodnie z równaniami
    observerPosition[0] = radiusStart * cos(thetaU) * cos(thetaV);
    observerPosition[1] = radiusStart * sin(thetaV);
    observerPosition[2] = radiusStart * sin(thetaU) * cos(thetaV);
    gluLookAt(observerPosition[0], observerPosition[1], observerPosition[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    drawEgg();
    glFlush();
    glutSwapBuffers();
}

// Funkcja obs³uguj¹ca naciœniêcie guzika myszy
// Wywo³ywana w momencie naciœniêcia przycisków myszy
void MouseButtonState(int btn, int state, int x, int y)
{

    // Zmiana stanu zmiennej okreslaj¹cej naciœniêcie guzika
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

// Funkcja oblicza potrzebne parametry w zaleznoœci od zmiany po³o¿enia kursora
// Wywo³ywana w momencie poruszenia mysz¹
void MousePosition(GLsizei x, GLsizei y)
{
    // Obliczenie ró¿nicy w pozycji myszy
    xMousePositionDifference = x - xMousePosition;
    yMousePositionDifference = y - yMousePosition;
    radiusDifference = y - raduisCurrent;
    // Zapisanie aktualnej pozycji myszy
    xMousePosition = x;
    yMousePosition = y;
    raduisCurrent = y;
    // Odœwie¿enie widoku
    glutPostRedisplay();
}

// Obs³uga zmiany rozmiaru okna
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    // Przeliczenie zmiany iloœci pikseli w ruchu myszy na stopnie
    xAngleInPixels = 360.0 / (float)horizontal;
    yAngleInPixels = 360.0 / (float)vertical;
    // Przejœcie w tryb projekcji
    glMatrixMode(GL_PROJECTION);
    // Czyszczenie macierzy bie¿¹cej
    glLoadIdentity();
    aspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    // Ustawienie parametrów dla rzutu perspektywicznego
    gluPerspective(70, aspectRatio, 1.0, 30.0);
    // Ustawienie okna widoku w zaleznoœci od stostunku x i y okna
    if (horizontal <= vertical)
    {
        glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
    }
    else
    {
        glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
    }

    // Powrót do trybu modelu
    glMatrixMode(GL_MODELVIEW);
    // Czyszczenie macierzy bie¿¹cej
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