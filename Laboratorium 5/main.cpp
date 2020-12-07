//Copyright 2020 <Paweł Ślęzak> //NOLINT
#include <windows.h>
#include <iostream>
#include <functional>
#include <gl/gl.h> //NOLINT
#include <gl/glut.h> //NOLINT
#include <vector>
#include <cmath>
#include <ctime>
#define M_PI           3.14159265358979323846
#define _USE_MATH_DEFINES
// Gęstość wykonania algorytmu
const int N = 20;
// Definicja punktów
typedef float point3[3];
// Definicja obserwatora
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };
// Zmienne przechowujące przeliczenia pikseli na wartości kątowe
static GLfloat pix2angle;
static GLfloat pix2angleX;
static GLfloat pix2angleY;
// Kąt obrotu theta
static GLfloat theta = 0.0;
// Kąt elewacji fi
static float fi = 0.0;
// Promień jajka
float radius = 15.0;
// Naciśnięty klawisz myszy
static GLint status = 0;
// Zmienne przechowujące informacje o poprzedniej pozycji myszy
// Oraz stopniu przybliżenia
static int x_pos_old = 0;
static int y_pos_old = 0;
static int zoom_pos_old = 0;
// Różnica pomiędzy pozycją myszy
// Oraz stopniem przybliżenia
static int delta_x = 0;
static int delta_y = 0;
static int delta_zoom = 0;
// Tablica przechowująca kolory danego piksela w mapie bitowej
float colors[N + 1][N + 1][2];

// Współrzędne punktów jajka
struct EggPoints {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};
// Określenie parametra gęstości w postaci
// Wartości z przedziału 0...1
float parametr = N / static_cast<float>(N * N);
// Wektory odpowiadające struktorze punktów jajka
std::vector<std::vector<EggPoints> > points(N + 1, std::vector<EggPoints>(N + 1)); //NOLINT
std::vector<std::vector<EggPoints> > pointsU(N + 1, std::vector<EggPoints>(N + 1)); //NOLINT
std::vector<std::vector<EggPoints> > pointsV(N + 1, std::vector<EggPoints>(N + 1)); //NOLINT
std::vector<std::vector<EggPoints> > normalVector(N + 1, std::vector<EggPoints>(N + 1)); //NOLINT
float length;
// Metoda wczytująca plik TGA
// Według dokumentacji biblioteki OpenGL
GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint //NOLINT
    * ImComponents, GLenum* ImFormat) {
    // Struktura przechowująca nagłówek pliku TGA
#pragma pack(1)
    typedef struct {
        GLbyte idlength;
        GLbyte colormaptype;
        GLbyte datatypecode;
        unsigned short colormapstart; //NOLINT
        unsigned short colormaplength; //NOLINT
        unsigned char colormapdepth; //NOLINT
        unsigned short x_orgin; //NOLINT
        unsigned short y_orgin; //NOLINT
        unsigned short width; //NOLINT
        unsigned short height; //NOLINT
        GLbyte bitsperpixel; //NOLINT
        GLbyte descriptor; //NOLINT
    } TGAHEADER;
    // Zmienne przechowujące informacje o wczytanym pliku //NOLINT
    FILE* pFile;
    TGAHEADER tgaHeader;
    unsigned long lImageSize; //NOLINT
    short sDepth; //NOLINT
    GLbyte* pbitsperpixel = NULL; //NOLINT
    // Domyślne wartości, wykorzystane w przypadku problemów z wczytaniem pliku //NOLINT
    *ImWidth = 0;
    *ImHeight = 0;
    *ImFormat = GL_BGR_EXT;
    *ImComponents = GL_RGB8;
    // Wczytanie pliku o nazwie określonej w parametrach funkcji
    pFile = fopen(FileName, "rb");
    if (pFile == NULL) return NULL;
    // Wczytanie nagłówka pliku do zdefiniowanej powyżej struktury
    fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);
    // Wczytanie parametrów obrazu
    *ImWidth = tgaHeader.width;
    *ImHeight = tgaHeader.height;
    sDepth = tgaHeader.bitsperpixel / 8;
    // Sprawdzenie, czy parametr głębi kolorów speałnia wymagania OpenGL (ma 8, 24 lub 32 bity) //NOLINT
    if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32) //NOLINT
        return NULL;
    // Określenie rozmiaru tekstury i alokacja odpowiedniej ilości pamięci
    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
    pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte)); //NOLINT
    // Weryfikacja poprawności wczytanej tekstury
    if (pbitsperpixel == NULL) return NULL;
    if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1) {
        free(pbitsperpixel);
        return NULL;
    }
    // Ustawienie formatu na format OpenGL
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
    }
    // Zamknięcie pliku i zwrócenie wskaźnika na miejsce wczytanych danych w pamięci  //NOLINT
    fclose(pFile);
    return pbitsperpixel;
}

void DrawEgg() {
    // Generowanie punktów początkowych
    for (int i = 0; i <= N; i++) {
        for (int j = 0; j <= N; j++) {
            points[i][j].x = (parametr * i);
            points[i][j].y = (parametr * j);
        }
    }

    // Pętle wypełniające tablicę punktów
    for (int i = 0; i <= N; i++) {
        for (int j = 0; j <= N; j++) {
            // Zmiana dziedziny funkcji na przestrzeń trójwymiarową
            float u = points[i][j].x;
            float v = points[i][j].y;
            // przypisanie bitów koloru z tekstury
            colors[i][j][0] = u;
            colors[i][j][1] = v;
            // Punkty i wektory x
            points[i][j].x = (-90 * pow(u, 5) + 225 * pow(u, 4) -
                270 * pow(u, 3) + 180 * (u * u) - 45 * u) * cos(M_PI * v);
            pointsU[i][j].x = (-450 * pow(u, 4) + 900 * pow(u, 3)
                - 810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v);
            pointsV[i][j].x = M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 *
                pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(M_PI * v);
            // Punkty i wektory y
            points[i][j].y = (160 * pow(u, 4) - 320 * pow(u, 3) +
                160 * (u * u));
            pointsU[i][j].y = (640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u);
            pointsV[i][j].y = 0;
            // Punkty i wektory z
            points[i][j].z = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + //NOLINT
                180 * (u * u) - 45 *u) * sin(M_PI * v);
            pointsU[i][j].z = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 *
                pow(u, 2) + 360 * u - 45) *sin(M_PI * v);
            pointsV[i][j].z = (-1 * M_PI) * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 *  //NOLINT
                pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(M_PI * v);
            // Obliczenie wektorów normalnych dla punktu
            normalVector[i][j].x = ((pointsU[i][j].y * pointsV[i][j].z) - (pointsU[i][j].z * pointsV[i][j].y)); //NOLINT
            normalVector[i][j].y = ((pointsU[i][j].z * pointsV[i][j].x) - (pointsU[i][j].x * pointsV[i][j].z)); //NOLINT
            normalVector[i][j].z = ((pointsU[i][j].x * pointsV[i][j].y) - (pointsU[i][j].y * pointsV[i][j].x)); //NOLINT
            // Obliczenie długości
            length = sqrt(pow(normalVector[i][j].x, 2) + pow(normalVector[i][j].y, 2) + pow(normalVector[i][j].z, 2)); //NOLINT
            // Zmiana przedziału wektorów na 0/1
            if (i == N + 1 / 2) {
                normalVector[i][j].x = 0;
                normalVector[i][j].y = 1;
                normalVector[i][j].z = 0;
            }
            if (i == 0 || i == N) {
                normalVector[i][j].x = 0;
                normalVector[i][j].y = -1;
                normalVector[i][j].z = 0;
            }
            if (i > (N + 1) / 2) {
                normalVector[i][j].x = -1 * (normalVector[i][j].x / length);
                normalVector[i][j].y = -1 * (normalVector[i][j].y / length);
                normalVector[i][j].z = -1 * (normalVector[i][j].z / length);
            }
            else {  //NOLINT
                normalVector[i][j].x = normalVector[i][j].x / length;
                normalVector[i][j].y = normalVector[i][j].y / length;
                normalVector[i][j].z = normalVector[i][j].z / length;
            }
        }
    }
    // Translacja kamery
    glTranslated(0, -5, 0);
    // Ustawienie koloru rysowania
    glColor3f(1.0, 1.0, 1.0);
    // Narysowanie jajka z punktów
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            glBegin(GL_TRIANGLES);
            glTexCoord2fv(colors[i][j]);
            glNormal3f(normalVector[i][j].x, normalVector[i][j].y, normalVector[i][j].z); //NOLINT
            glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z); //NOLINT
            glTexCoord2fv(colors[i][j + 1]);
            glNormal3f(normalVector[i][j + 1].x, normalVector[i][j + 1].y, normalVector[i][j + 1].z); //NOLINT
            glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z); //NOLINT
            glNormal3f(normalVector[i + 1][j].x, normalVector[i + 1][j].y, normalVector[i + 1][j].z); //NOLINT
            glTexCoord2fv(colors[i + 1][j]);
            glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z); //NOLINT
            glEnd();
            glBegin(GL_TRIANGLES);
            glTexCoord2fv(colors[i + 1][j]);
            glNormal3f(normalVector[i + 1][j].x, normalVector[i + 1][j].y, normalVector[i + 1][j].z); //NOLINT
            glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z); //NOLINT
            glTexCoord2fv(colors[i][j + 1]);
            glNormal3f(normalVector[i][j + 1].x, normalVector[i][j + 1].y, normalVector[i][j + 1].z); //NOLINT
            glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z); //NOLINT
            glTexCoord2fv(colors[i + 1][j + 1]);
            glNormal3f(normalVector[i + 1][j + 1].x, normalVector[i + 1][j + 1].y, normalVector[i + 1][j + 1].z); //NOLINT
            glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z); //NOLINT
            glEnd();
        }
    }
}
// Metoda obsługująca kliknięcia myszy //NOLINT
void Mouse(int btn, int state, int x, int y) {
    // Lewy klawisz myszy
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        x_pos_old = x;
        y_pos_old = y;
        status = 1;
    } else if (btn = GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {  // prawy klawisz myszy  //NOLINT
        zoom_pos_old = y;
        status = 2;
    }
    else status = 0;  // brak klawisza //NOLINT
}
// Obsługa ruchów kursora myszy //NOLINT
void Motion(GLsizei x, GLsizei y) {
    delta_x = x - x_pos_old;
    delta_y = y - y_pos_old;
    delta_zoom = y - zoom_pos_old;
    x_pos_old = x;
    y_pos_old = y;
    zoom_pos_old = y;
    glutPostRedisplay();
}
// Funkcja renderująca obraz //NOLINT
void RenderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    // Zmiana kąta obrotu przy wciśniętym lewym klawiszu myszy
    if (status == 1) {
        fi += delta_y * pix2angleY;
        theta += delta_x * pix2angleX;
        if (theta >= 360.0) { theta = 0.0; }
        if (fi >= 360.0) { fi = 0.0; }
    } else if (status == 2) {
        radius += 0.1 * delta_zoom;
        if (radius >= 25.0) { radius = 25.0; }
        if (radius <= 10.0) { radius = 10.0; }
    }
    // Obrót jajka o nowy kąt
    glRotatef(theta, 0.0, 1.0, 0.0);
    // Zmiana położenia obserwatora
    viewer[0] = radius * cos(theta) * cos(fi);
    viewer[1] = radius * sin(fi);
    viewer[2] = radius * sin(theta) * cos(fi);
    gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, cos(fi), 0.0); //NOLINT
    // Narysowanie jajka
    DrawEgg();
    // Wykonanie poleceń rysujących
    glFlush();
    glutSwapBuffers();
}

// Funkcja ustalaj ca stan renderowania   //NOLINT
void MyInit(void) {
    // Kolorowanie okna na czarno
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Zmienne przechowujące dane o teksturze
    GLbyte* pBytes;
    GLint ImWidth, ImHeight, ImComponents;
    GLenum ImFormat;
    // Definicja materiału z jakiego zrobiony jest przedmiot
    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess = { 100.0 };
    // Definicja światła
    GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
    GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat att_constant = { 1.0f };
    GLfloat att_linear = { 0.05f };
    GLfloat att_quadratic = { 0.001f };
    // Ustawienie patrametrów materiału
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    // Ustawienie parametrów światła
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
    // Ustawienie opcji systemu oświetlania sceny
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    // Przeczytanie obrazu tekstury z pliku
    pBytes = LoadTGAImage("marmur.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat); //NOLINT
    // Zdefiniowanie tekstury 2-D
    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes); //NOLINT
    // Zwolnienie pamięci po załadowanej teksturze
    free(pBytes);
    // Włączenie mechanizmu teksturowania
    glEnable(GL_TEXTURE_2D);
    // Ustalenie trybu teksturowania
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    // Określenie sposobu nałożenia tekstury
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych w przypadku zmiany rozmiarów okna //NOLINT
void ChangeSize(GLsizei horizontal, GLsizei vertical) {
    pix2angleX = 360.0 * 0.01 / (float)horizontal; //NOLINT
    pix2angleY = 360.0 * 0.01 / (float)vertical; //NOLINT
    GLfloat AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    if (vertical == 0) { vertical = 1; }
    glViewport(0, 0, horizontal, vertical);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, AspectRatio, 1.0, 40.0);
    if (horizontal <= vertical)
        glViewport(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio);
    else
        glViewport(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Laboratorium 5");
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    MyInit();
    glEnable(GL_DEPTH_TEST);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutMainLoop();
}
