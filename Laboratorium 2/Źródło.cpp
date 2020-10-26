/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj¹ osi 
//  uk³adu wspó³rzednych

/*************************************************************************************/

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <ctime>
# define M_PI           3.14159265358979323846
using namespace std;
typedef float point3[3];
//zageszcczenie punktow
const int N = 30;
//os obrotu
static GLfloat theta[] = { 0.0,0.0,0.0 };

//struktura przechowujaca wspolrzedne poszczegolnych punktow
struct point {
    double x;
    double y;
    double z;
    float r;
    float g;
    float b;
};

//Macierz punktów
point matrix[N][N];


//do losowania kolorow losowe floaty
float randomFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}



//generowanie punktów do macierzy
void generateTable() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            //zmienne do równañ
            double u = (double)i / ((double)N - 1);
            double v = (double)j / ((double)N - 1);
            //rownania z instrukcji
            matrix[i][j].x = (-(90 * pow(u, 5)) + (225 * pow(u,
                4)) - (270 * pow(u, 3)) + (180 * pow(u, 2)) - 45 * u) * cos(M_PI * v);
            matrix[i][j].y = (160 * pow(u, 4)) - (320 * pow(u,
                3)) + (160 * pow(u, 2));
            matrix[i][j].z = (-(90 * pow(u, 5)) + (225 * pow(u,
                4)) - (270 * pow(u, 3)) + (180 * pow(u, 2)) - 45 * u) * sin(M_PI * v);
            //losowe kolory punktow
            matrix[i][j].r = randomFloat();
            matrix[i][j].g = randomFloat();
            matrix[i][j].b = randomFloat();
        }
    }
}

//funkcja rysujaca jajko
void drawEGG() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            //pierwszy trojkat
            glBegin(GL_TRIANGLES);
            glColor3f(matrix[i][j].r, matrix[i][j].g, matrix[i][j].b);
            glVertex3f((float)matrix[i][j].x, (float)matrix[i][j].y - 5.0f, (float)matrix[i][j].z);
            glColor3f(matrix[(i + 1) % N][j].r, matrix[(i + 1) % N][j].g, matrix[(i + 1) % N][j].b);
            glVertex3f((float)matrix[(i+1)%N][j].x,(float)matrix[(i + 1) % N][j].y - 5.0f, (float)matrix[(i + 1) % N][j].z);
            glColor3f(matrix[(i + 1) % N][(j + 1) % N].r,matrix[(i + 1) % N][(j + 1) % N].g, matrix[(i + 1) % N][(j + 1) % N].b);
            glVertex3f((float)matrix[(i + 1) % N][(j + 1) %N].x, (float)matrix[(i + 1) % N][(j + 1) % N].y - 5.0f, (float)matrix[(i +1) % N][(j + 1) % N].z);
            glEnd();
            //drugi trojkat
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



/*************************************************************************************/

// Funkcja okreœlaj¹ca co ma byæ rysowane (zawsze wywo³ywana gdy trzeba
// przerysowaæ scenê)



void RenderScene(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Czyszczenie okna aktualnym kolorem czyszcz¹cym


    glLoadIdentity();
    // Czyszczenie macierzy bie¿¹cej
    drawEGG();
    //rysowanie jajka
    glFlush();
    // Przekazanie poleceñ rysuj¹cych do wykonania
    glutSwapBuffers();
    //
}

/*************************************************************************************/

// Funkcja ustalaj¹ca stan renderowania



void MyInit(void)
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Kolor czyszc¹cy (wype³nienia okna) ustawiono na czarny

}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹
// przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna.



// Obs³uga zmiany rozmiaru okna
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    GLfloat AspectRatio;
    if (vertical == 0)
        vertical = 1;
    glViewport(0, 0, horizontal, vertical);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    if (horizontal <= vertical)
    {
        glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio,
            10.0, -10.0);
    }
    else
    {
        glOrtho(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5,
            10.0, -10.0);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*************************************************************************************/

// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli



int  main(int argc, char **argv)
{
    srand(time(NULL));
    glutInit(&argc, argv);
    generateTable();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(800, 600);
    glutCreateWindow("Laboratorium 3");
    glutDisplayFunc(RenderScene);
    // Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹
    // (callback function).  Bedzie ona wywo³ywana za ka¿dym razem
    // gdy zajdzie potrzba przeryswania okna 
    glutReshapeFunc(ChangeSize);
    // Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹
    // zazmiany rozmiaru okna      
    glClearColor(0.0f, 0.0f,0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    // W³¹czenie mechanizmu usuwania powierzchni niewidocznych
    glutMainLoop();
    // Funkcja uruchamia szkielet biblioteki GLUT
    return 0;
}

/*************************************************************************************/