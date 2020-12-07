
//Copyright 2020 <Pawe� �l�zak> //NOLINT
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <ctime>
#define M_PI           3.14159265358979323846
#define _USE_MATH_DEFINES
// Definicja punktu OpenGL
typedef float point3[3];
// Tablice przechowuj�ce punkty oraz wektory normalne //NOLINT
point3** pointsMatrix;
point3** pointsMatrixNormalized;
// Zmienne przechowuj�ce k�ty fi i theta //NOLINT
static GLfloat fi[2] = { 5.76f, 1.05f };
static GLfloat theta[2] = { 4.68f, 4.68f };
// Zmienne przechowuj�ce konwersj� ogleg�o�ci w pikselach //NOLINT
// Na k�t obrotu wg. okre�lonej osi //NOLINT
static GLfloat pixelsToAngleX = 0.0;
static GLfloat pixelsToAngleY = 0.0;
// Zmienna reprezentuj�ca status wci�ni�tych przycisk�w //NOLINT
static GLint buttonState = 0;
// Zmienne przechowuj�ce poprzednie pozycje kursora //NOLINT
static int oldCursorX = 0;
static int oldCursorY = 0;
// Zmienne przechowuj�ce r�nic� w pozycji kursora //NOLINT
static int cursorDeltaX = 0;
static int cursorDeltaY = 0;
// G�sto�� wykona� algorytmu generuj�cego punkty jajka //NOLINT
int N = 50;
// D�ugo�� parametr�w u i v //NOLINT
float variableLength = 1.0;
// Promie� okr�gu tworzonego przez �wiat�o //NOLINT
float lightRadius = 10.0;
// Parametry r�wna� przekszta�ce� //NOLINT
float xu;
float xv;
float yu;
float yv;
float zu;
float zv;
float x, y, z, a;
// Funkcje obliczaj�ce warto�ci ze wzor�w //NOLINT
float getValueX(float u, float v) {
    a = v * M_PI;
    x = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(a); //NOLINT
    return x;
}
float getValueY(float u, float v) {
    y = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2);
    return y - 5;
}
float getValueZ(float u, float v) {
    a = v * M_PI;
    z = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(a); //NOLINT
    return z;
}
float getNormalizedValueX(float u, float v) {
    a = v * M_PI;
    yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
    yv = 0;
    zu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(a); //NOLINT
    zv = -M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * //NOLINT
        cos(a);
    return (GLfloat)(yu * zv - zu * yv);
}
float getNormalizedValueY(float u, float v) {
    a = v * M_PI;
    xu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(a); //NOLINT
    xv = M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * //NOLINT
        sin(a);
    zu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(a); //NOLINT
    zv = -M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * //NOLINT
        cos(a);
    return (GLfloat)(zu * xv - xu * zv);
}

float getNormalizedValueZ(float u, float v) {
    a = v * M_PI;
    xu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(a); //NOLINT
    xv = M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * //NOLINT
        sin(a);
    yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
    yv = 0;
    return (GLfloat)(xu * yv - yu * xv);
}
// Funkcja generuje siatk� punkt�w wykorzystywan� przy tworzeniu jajka //NOLINT
void generatePoints() {
    // Wsp�czynnik okre�laj�cy warto�ci u i v //NOLINT
    float stepXY = variableLength / N;
    // P�tla generuj�ca parametry u i v dla punkt�w tabeli //NOLINT
    for (int i = 0; i < N + 1; i++) {
        for (int j = 0; j < N + 1; j++) {
            pointsMatrix[i][j][0] = j * stepXY;
            pointsMatrix[i][j][1] = i * stepXY;
        }
    }
    // Petla generuj�ca punkty jajka //NOLINT
    // Oraz ich wektory normalne
    float u, v;
    for (int i = 0; i < N + 1; i++) {
        for (int j = 0; j < N + 1; j++) {
            // Pobranie wcze�niej wyznaczonych warto�ci U i V //NOLINT
            v = pointsMatrix[i][j][0];
            u = pointsMatrix[i][j][1];
            // Wygenerowanie wsp�rz�dnych punkt�w na bazie u i v //NOLINT
            pointsMatrix[i][j][0] = getValueX(u, v);
            pointsMatrix[i][j][1] = getValueY(u, v);
            pointsMatrix[i][j][2] = getValueZ(u, v);
            // Wygenerowanie wektor�w prostopad�ych //NOLINT
            float x = getNormalizedValueX(u, v);
            float y = getNormalizedValueY(u, v);
            float z = getNormalizedValueZ(u, v);
            // Normalizacja warto�ci wektor�w //NOLINT
            if (i < N / 2) {
                pointsMatrixNormalized[i][j][0] = x / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); //NOLINT
                pointsMatrixNormalized[i][j][1] = y / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); //NOLINT
                pointsMatrixNormalized[i][j][2] = z / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); //NOLINT
            }
            if (i > N / 2) {
                pointsMatrixNormalized[i][j][0] = -1.0 * x / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); //NOLINT
                pointsMatrixNormalized[i][j][1] = -1.0 * y / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); //NOLINT
                pointsMatrixNormalized[i][j][2] = -1.0 * z / (float)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); //NOLINT
            }
            if (i == N / 2) {
                pointsMatrixNormalized[i][j][0] = 0;
                pointsMatrixNormalized[i][j][1] = 1;
                pointsMatrixNormalized[i][j][2] = 0;
            }
            if (i == 0 || i == N) {
                pointsMatrixNormalized[i][j][0] = 0;
                pointsMatrixNormalized[i][j][1] = -1;
                pointsMatrixNormalized[i][j][2] = 0;
            }
        }
    }
}
// Metoda rysuj�ca jajko //NOLINT
void drawEgg(void) {
    // Wygenerowanie siatki punkt�w i wektor�w normalnych //NOLINT
    generatePoints();
    // Ustawienie koloru rysowania
    glColor3f(0.8, 0.8, 0.8);
    // P�tla rysuj�ca //NOLINT
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            // Rysowanie pierwszego tr�jk�ta //NOLINT
            glBegin(GL_TRIANGLES);
            glNormal3fv(pointsMatrixNormalized[i][j + 1]);
            glVertex3fv(pointsMatrix[i][j + 1]);
            glNormal3fv(pointsMatrixNormalized[i + 1][j]);
            glVertex3fv(pointsMatrix[i + 1][j]);
            glNormal3fv(pointsMatrixNormalized[i + 1][j + 1]);
            glVertex3fv(pointsMatrix[i + 1][j + 1]);
            glEnd();
            // Rysowanie drugiego tr�jk�ta //NOLINT
            glBegin(GL_TRIANGLES);
            glNormal3fv(pointsMatrixNormalized[i][j]);
            glVertex3fv(pointsMatrix[i][j]);
            glNormal3fv(pointsMatrixNormalized[i + 1][j]);
            glVertex3fv(pointsMatrix[i + 1][j]);
            glNormal3fv(pointsMatrixNormalized[i][j + 1]);
            glVertex3fv(pointsMatrix[i][j + 1]);
            glEnd();
        }
    }
}
// Funkcja renderuj�ca widok //NOLINT
void RenderScene(void) {
    // Wyczyszczenie renderu
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Za�adowanie macierzy jednostkowej //NOLINT
    glLoadIdentity();
    // Operacje na k�tach pierwszego �wiat�a //NOLINT
    // Je�eli wci�ni�ty jest lewy przycisk //NOLINT
    if (buttonState == 1) {
        // Zamiana warto�ci theta na przedzia� 0-2 //NOLINT
        theta[0] -= cursorDeltaX * pixelsToAngleX;
        if (theta[0] <= 0) theta[0] += 2 * M_PI;
        if (theta[0] >= 2 * M_PI) theta[0] -= 2 * M_PI;
        // Zamiana warto�ci fi na przedzia� 0-2 //NOLINT
        fi[0] -= cursorDeltaY * pixelsToAngleY;
        if (fi[0] <= 0) fi[0] += 2 * M_PI;
        if (fi[0] >= 2 * M_PI) fi[0] -= 2 * M_PI;
    }
    // Operacje na k�tach drugiego �wiat�a //NOLINT
    // Je�eli wci�ni�ty jest prawy przycisk //NOLINT
    else if (buttonState == 2) { //NOLINT
       // Zamiana warto�ci theta na przedzia� 0-2 //NOLINT
        theta[1] -= cursorDeltaX * pixelsToAngleX;
        if (theta[1] <= 0) theta[1] += 2 * M_PI;
        if (theta[1] >= 2 * M_PI) theta[1] -= 2 * M_PI;
        // Zamiana warto�ci fi na przedzia� 0-2 //NOLINT
        fi[1] -= cursorDeltaY * pixelsToAngleY;
        if (fi[1] <= 0) fi[1] += 2 * M_PI;
        if (fi[1] >= 2 * M_PI) fi[1] -= 2 * M_PI;
    }
    // Wyzerowanie k�ta patrzenia kamery //NOLINT
    gluLookAt(0.0, 0.0, -10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    // Zmienna przechowuj�ca pozycj� �r�d�a �wiat�a //NOLINT
    GLfloat lightPosition[4] = { 0 };
    // Zmiana pozycji �r�d�a pierwszego �wiat�a //NOLINT
    lightPosition[0] = lightRadius * cos(theta[0]) * cos(fi[0]);
    lightPosition[1] = lightRadius * sin(fi[0]);
    lightPosition[2] = lightRadius * sin(theta[0]) * cos(fi[0]);
    lightPosition[3] = 1.0;
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    // Zmiana pozycji �r�d�a drugiego �wiat�a //NOLINT
    lightPosition[0] = lightRadius * cos(theta[1]) * cos(fi[1]);
    lightPosition[1] = lightRadius * sin(fi[1]);
    lightPosition[2] = lightRadius * sin(theta[1]) * cos(fi[1]);
    lightPosition[3] = 1.0;
    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
    // Narysowanie jajka
    drawEgg();
    // Zako�czenie renderu //NOLINT
    glFlush();
    glutSwapBuffers();
}
void MyInit(void) {
    // Definicja koloru czyszcz�czego //NOLINT
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Definicja parametr�w materia�u //NOLINT
    GLfloat materialAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat materialDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat materialSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat materialShine = { 20.0 };
    // Definicja parametr�w �wiate� //NOLINT
    GLfloat lightOnePosition[] = { -10.0, -10.0, -10.0, 1.0 };
    GLfloat lightTwoPosition[] = { -10.0, -10.0, -10.0, 1.0 };
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightOneDiffuse[] = { 1.0, 0.0, 1.0, 0.0 };
    GLfloat lightTwoDiffuse[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat lightOneSpecular[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    GLfloat lightTwoSpecular[] = { 0.7f, 0.7f, 1.0f, 1.0f };
    // Os�abienie �wiate� //NOLINT
    GLfloat attenuationConstant = { 1.0f };
    GLfloat attenuationLinear = { 0.001f };
    GLfloat attenuationQuadratic = { 0.001f };
    // Zastosowanie parametr�w materia�u //NOLINT
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

    glMaterialf(GL_FRONT, GL_SHININESS, materialShine);
    // Zastosowanie parametr�w �wiat�a pierwszego //NOLINT
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightOneDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightOneSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightOnePosition);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, attenuationConstant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, attenuationLinear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, attenuationQuadratic);
    // Zastosowanie parametr�w �wiat�a drugiego //NOLINT
    glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightTwoDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightTwoSpecular);
    glLightfv(GL_LIGHT1, GL_POSITION, lightTwoPosition);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, attenuationConstant);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, attenuationLinear);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, attenuationQuadratic);
    // Wyg�adzanie cieni //NOLINT
    glShadeModel(GL_SMOOTH);
    // Aktywacja �wiate� //NOLINT
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);
}
void ChangeSize(GLsizei horizontal, GLsizei vertical) {
    // Obliczenie nowych warto�ci k�ta //NOLINT
    pixelsToAngleX = 360.0 * 0.0125 / (float)horizontal; //NOLINT
    pixelsToAngleY = 360.0 * 0.0125 / (float)vertical; //NOLINT
    // Aktywowanie trybu macierzy
    glMatrixMode(GL_PROJECTION);
    // Za�adowanie macierzy jednostkowej //NOLINT
    glLoadIdentity();
    // Okreslenie perspektywy
    gluPerspective(70.0, 1.0, 1.0, 30.0);
    // Zmiana rozmiaru
    if (horizontal <= vertical) {
        glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
    } else {
        glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
    }
    // Ponowne aktywowanie trybu modelu
    glMatrixMode(GL_MODELVIEW);
    // Za�adowanie macierzy jednostkowej //NOLINT
    glLoadIdentity();
}

// Funkcja obs�uguj�ca klikni�cia myszy //NOLINT
void Mouse(int btn, int state, int x, int y) {
    // Wci�ni�ty lewy przycisk //NOLINT
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Zapisanie pozycji kursora
        oldCursorX = x;
        oldCursorY = y;
        buttonState = 1;
    }
    // Wci�ni�ty prawy przycisk //NOLINT
    else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) { //NOLINT
       // Zapisanie pozycji kursora
        oldCursorX = x;
        oldCursorY = y;
        buttonState = 2;
    } else {
        // Wyzerowanie buttonState
        buttonState = 0;
    }
}
// Funkcja obliczaj�ca r�nice pomi�dzy pozycj� kursora //NOLINT
void Motion(GLsizei x, GLsizei y) {
    cursorDeltaX = x - oldCursorX;
    oldCursorX = x;
    cursorDeltaY = y - oldCursorY;
    oldCursorY = y;
    glutPostRedisplay();
}
// Funkcja generuj�ca puste tablice punkt�w //NOLINT
// i wektor�w normalnych //NOLINT
void makeArrays() {
    pointsMatrix = new point3 * [N + 1];
    for (int i = 0; i < N + 1; i++) {
        pointsMatrix[i] = new point3[N + 1];
    }
    pointsMatrixNormalized = new point3 * [N + 1];
    for (int i = 0; i < N + 1; i++) {
        pointsMatrixNormalized[i] = new point3[N + 1];
    }
}
// Punkt wej�ciowy programu //NOLINT
int main(int argc, char* argv[]) {
    // Losowo�� //NOLINT
    srand((unsigned)time(NULL));
    // Wygenerowanie pustych tablic
    makeArrays();
    // Inicjalizacja biblioteki glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    // Stworzenie okna
    glutCreateWindow("Laboratorium 4");
    // Funkcja wy�wietlania //NOLINT
    glutDisplayFunc(RenderScene);
    // Funkcja zmiany rozmiaru //NOLINT
    glutReshapeFunc(ChangeSize);
    // Inicjalizacja materia��w, �wiate�, itp //NOLINT
    MyInit();
    // Funkcja myszy //NOLINT
    glutMouseFunc(Mouse);
    // Funkcja ruchu //NOLINT
    glutMotionFunc(Motion);
    // Aktywacja g�eboko�ci // NOLINT
    glEnable(GL_DEPTH_TEST);
    // Uruchomienie g��wnej p�tli GLUT // NOLINT
    glutMainLoop();
}
