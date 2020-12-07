
//Copyright 2020 <Pawe³ Œlêzak> //NOLINT
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <ctime>
#define M_PI           3.14159265358979323846
#define _USE_MATH_DEFINES
// Definicja punktu OpenGL
typedef float point3[3];
// Tablice przechowuj¹ce punkty oraz wektory normalne //NOLINT
point3** pointsMatrix;
point3** pointsMatrixNormalized;
// Zmienne przechowuj¹ce k¹ty fi i theta //NOLINT
static GLfloat fi[2] = { 5.76f, 1.05f };
static GLfloat theta[2] = { 4.68f, 4.68f };
// Zmienne przechowuj¹ce konwersjê ogleg³oœci w pikselach //NOLINT
// Na k¹t obrotu wg. okreœlonej osi //NOLINT
static GLfloat pixelsToAngleX = 0.0;
static GLfloat pixelsToAngleY = 0.0;
// Zmienna reprezentuj¹ca status wciœniêtych przycisków //NOLINT
static GLint buttonState = 0;
// Zmienne przechowuj¹ce poprzednie pozycje kursora //NOLINT
static int oldCursorX = 0;
static int oldCursorY = 0;
// Zmienne przechowuj¹ce ró¿nicê w pozycji kursora //NOLINT
static int cursorDeltaX = 0;
static int cursorDeltaY = 0;
// Gêstoœæ wykonañ algorytmu generuj¹cego punkty jajka //NOLINT
int N = 50;
// D³ugoœæ parametrów u i v //NOLINT
float variableLength = 1.0;
// Promieñ okrêgu tworzonego przez œwiat³o //NOLINT
float lightRadius = 10.0;
// Parametry równañ przekszta³ceñ //NOLINT
float xu;
float xv;
float yu;
float yv;
float zu;
float zv;
float x, y, z, a;
// Funkcje obliczaj¹ce wartoœci ze wzorów //NOLINT
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
// Funkcja generuje siatkê punktów wykorzystywan¹ przy tworzeniu jajka //NOLINT
void generatePoints() {
    // Wspó³czynnik okreœlaj¹cy wartoœci u i v //NOLINT
    float stepXY = variableLength / N;
    // Pêtla generuj¹ca parametry u i v dla punktów tabeli //NOLINT
    for (int i = 0; i < N + 1; i++) {
        for (int j = 0; j < N + 1; j++) {
            pointsMatrix[i][j][0] = j * stepXY;
            pointsMatrix[i][j][1] = i * stepXY;
        }
    }
    // Petla generuj¹ca punkty jajka //NOLINT
    // Oraz ich wektory normalne
    float u, v;
    for (int i = 0; i < N + 1; i++) {
        for (int j = 0; j < N + 1; j++) {
            // Pobranie wczeœniej wyznaczonych wartoœci U i V //NOLINT
            v = pointsMatrix[i][j][0];
            u = pointsMatrix[i][j][1];
            // Wygenerowanie wspó³rzêdnych punktów na bazie u i v //NOLINT
            pointsMatrix[i][j][0] = getValueX(u, v);
            pointsMatrix[i][j][1] = getValueY(u, v);
            pointsMatrix[i][j][2] = getValueZ(u, v);
            // Wygenerowanie wektorów prostopad³ych //NOLINT
            float x = getNormalizedValueX(u, v);
            float y = getNormalizedValueY(u, v);
            float z = getNormalizedValueZ(u, v);
            // Normalizacja wartoœci wektorów //NOLINT
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
// Metoda rysuj¹ca jajko //NOLINT
void drawEgg(void) {
    // Wygenerowanie siatki punktów i wektorów normalnych //NOLINT
    generatePoints();
    // Ustawienie koloru rysowania
    glColor3f(0.8, 0.8, 0.8);
    // Pêtla rysuj¹ca //NOLINT
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            // Rysowanie pierwszego trójk¹ta //NOLINT
            glBegin(GL_TRIANGLES);
            glNormal3fv(pointsMatrixNormalized[i][j + 1]);
            glVertex3fv(pointsMatrix[i][j + 1]);
            glNormal3fv(pointsMatrixNormalized[i + 1][j]);
            glVertex3fv(pointsMatrix[i + 1][j]);
            glNormal3fv(pointsMatrixNormalized[i + 1][j + 1]);
            glVertex3fv(pointsMatrix[i + 1][j + 1]);
            glEnd();
            // Rysowanie drugiego trójk¹ta //NOLINT
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
// Funkcja renderuj¹ca widok //NOLINT
void RenderScene(void) {
    // Wyczyszczenie renderu
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Za³adowanie macierzy jednostkowej //NOLINT
    glLoadIdentity();
    // Operacje na k¹tach pierwszego œwiat³a //NOLINT
    // Je¿eli wciœniêty jest lewy przycisk //NOLINT
    if (buttonState == 1) {
        // Zamiana wartoœci theta na przedzia³ 0-2 //NOLINT
        theta[0] -= cursorDeltaX * pixelsToAngleX;
        if (theta[0] <= 0) theta[0] += 2 * M_PI;
        if (theta[0] >= 2 * M_PI) theta[0] -= 2 * M_PI;
        // Zamiana wartoœci fi na przedzia³ 0-2 //NOLINT
        fi[0] -= cursorDeltaY * pixelsToAngleY;
        if (fi[0] <= 0) fi[0] += 2 * M_PI;
        if (fi[0] >= 2 * M_PI) fi[0] -= 2 * M_PI;
    }
    // Operacje na k¹tach drugiego œwiat³a //NOLINT
    // Je¿eli wciœniêty jest prawy przycisk //NOLINT
    else if (buttonState == 2) { //NOLINT
       // Zamiana wartoœci theta na przedzia³ 0-2 //NOLINT
        theta[1] -= cursorDeltaX * pixelsToAngleX;
        if (theta[1] <= 0) theta[1] += 2 * M_PI;
        if (theta[1] >= 2 * M_PI) theta[1] -= 2 * M_PI;
        // Zamiana wartoœci fi na przedzia³ 0-2 //NOLINT
        fi[1] -= cursorDeltaY * pixelsToAngleY;
        if (fi[1] <= 0) fi[1] += 2 * M_PI;
        if (fi[1] >= 2 * M_PI) fi[1] -= 2 * M_PI;
    }
    // Wyzerowanie k¹ta patrzenia kamery //NOLINT
    gluLookAt(0.0, 0.0, -10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    // Zmienna przechowuj¹ca pozycjê ¿ród³a œwiat³a //NOLINT
    GLfloat lightPosition[4] = { 0 };
    // Zmiana pozycji Ÿród³a pierwszego œwiat³a //NOLINT
    lightPosition[0] = lightRadius * cos(theta[0]) * cos(fi[0]);
    lightPosition[1] = lightRadius * sin(fi[0]);
    lightPosition[2] = lightRadius * sin(theta[0]) * cos(fi[0]);
    lightPosition[3] = 1.0;
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    // Zmiana pozycji Ÿród³a drugiego œwiat³a //NOLINT
    lightPosition[0] = lightRadius * cos(theta[1]) * cos(fi[1]);
    lightPosition[1] = lightRadius * sin(fi[1]);
    lightPosition[2] = lightRadius * sin(theta[1]) * cos(fi[1]);
    lightPosition[3] = 1.0;
    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
    // Narysowanie jajka
    drawEgg();
    // Zakoñczenie renderu //NOLINT
    glFlush();
    glutSwapBuffers();
}
void MyInit(void) {
    // Definicja koloru czyszcz¹czego //NOLINT
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Definicja parametrów materia³u //NOLINT
    GLfloat materialAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat materialDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat materialSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat materialShine = { 20.0 };
    // Definicja parametrów œwiate³ //NOLINT
    GLfloat lightOnePosition[] = { -10.0, -10.0, -10.0, 1.0 };
    GLfloat lightTwoPosition[] = { -10.0, -10.0, -10.0, 1.0 };
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightOneDiffuse[] = { 1.0, 0.0, 1.0, 0.0 };
    GLfloat lightTwoDiffuse[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat lightOneSpecular[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    GLfloat lightTwoSpecular[] = { 0.7f, 0.7f, 1.0f, 1.0f };
    // Os³abienie œwiate³ //NOLINT
    GLfloat attenuationConstant = { 1.0f };
    GLfloat attenuationLinear = { 0.001f };
    GLfloat attenuationQuadratic = { 0.001f };
    // Zastosowanie parametrów materia³u //NOLINT
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

    glMaterialf(GL_FRONT, GL_SHININESS, materialShine);
    // Zastosowanie parametrów œwiat³a pierwszego //NOLINT
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightOneDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightOneSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightOnePosition);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, attenuationConstant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, attenuationLinear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, attenuationQuadratic);
    // Zastosowanie parametrów œwiat³a drugiego //NOLINT
    glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightTwoDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightTwoSpecular);
    glLightfv(GL_LIGHT1, GL_POSITION, lightTwoPosition);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, attenuationConstant);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, attenuationLinear);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, attenuationQuadratic);
    // Wyg³adzanie cieni //NOLINT
    glShadeModel(GL_SMOOTH);
    // Aktywacja œwiate³ //NOLINT
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);
}
void ChangeSize(GLsizei horizontal, GLsizei vertical) {
    // Obliczenie nowych wartoœci k¹ta //NOLINT
    pixelsToAngleX = 360.0 * 0.0125 / (float)horizontal; //NOLINT
    pixelsToAngleY = 360.0 * 0.0125 / (float)vertical; //NOLINT
    // Aktywowanie trybu macierzy
    glMatrixMode(GL_PROJECTION);
    // Za³adowanie macierzy jednostkowej //NOLINT
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
    // Za³adowanie macierzy jednostkowej //NOLINT
    glLoadIdentity();
}

// Funkcja obs³uguj¹ca klikniêcia myszy //NOLINT
void Mouse(int btn, int state, int x, int y) {
    // Wciœniêty lewy przycisk //NOLINT
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Zapisanie pozycji kursora
        oldCursorX = x;
        oldCursorY = y;
        buttonState = 1;
    }
    // Wciœniêty prawy przycisk //NOLINT
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
// Funkcja obliczaj¹ca ró¿nice pomiêdzy pozycj¹ kursora //NOLINT
void Motion(GLsizei x, GLsizei y) {
    cursorDeltaX = x - oldCursorX;
    oldCursorX = x;
    cursorDeltaY = y - oldCursorY;
    oldCursorY = y;
    glutPostRedisplay();
}
// Funkcja generuj¹ca puste tablice punktów //NOLINT
// i wektorów normalnych //NOLINT
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
// Punkt wejœciowy programu //NOLINT
int main(int argc, char* argv[]) {
    // Losowoœæ //NOLINT
    srand((unsigned)time(NULL));
    // Wygenerowanie pustych tablic
    makeArrays();
    // Inicjalizacja biblioteki glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    // Stworzenie okna
    glutCreateWindow("Laboratorium 4");
    // Funkcja wyœwietlania //NOLINT
    glutDisplayFunc(RenderScene);
    // Funkcja zmiany rozmiaru //NOLINT
    glutReshapeFunc(ChangeSize);
    // Inicjalizacja materia³ów, œwiate³, itp //NOLINT
    MyInit();
    // Funkcja myszy //NOLINT
    glutMouseFunc(Mouse);
    // Funkcja ruchu //NOLINT
    glutMotionFunc(Motion);
    // Aktywacja g³ebokoœci // NOLINT
    glEnable(GL_DEPTH_TEST);
    // Uruchomienie g³ównej pêtli GLUT // NOLINT
    glutMainLoop();
}
