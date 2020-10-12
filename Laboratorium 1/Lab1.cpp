#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

//wspolrzedne wierzcholka kwadratu
typedef float point2[2];

int rysowanie(point2 ld, point2 pd, int warstwy) { // ld-lewy dolny rog pd-prawy dolny rog, 
	float bok = pd[0] - ld[0];
	float bok_maly = bok / 3;

	if (warstwy == 1) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				point2 new_ld = { ld[0] + (bok_maly * i), ld[1] + (bok_maly * j) };
				point2 new_lg = { ld[0] + (bok_maly * i), ld[1] + bok_maly + (bok_maly * j) };
				point2 new_pg = { ld[0] + bok_maly + (bok_maly * i), ld[1] + bok_maly + (bok_maly * j) };
				point2 new_pd = { ld[0] + bok_maly + (bok_maly * i), ld[1] + (bok_maly * j) };
				if ((i != 1) || (j != 1)) { // i-wiersz, j-kolumna  jak i=1 j=1 to srodkowy kwadrat
					//wiec odrzucam
					glBegin(GL_POLYGON);
					glColor3f(float(rand() % 1000) / 1000, float(rand() % 1000) / 1000, float(rand() % 1000) / 1000);
					glVertex2fv(new_ld);
					glColor3f(float(rand() % 1000) / 1000, float(rand() % 1000) / 1000, float(rand() % 1000) / 1000);
					glVertex2fv(new_lg);
					glColor3f(float(rand() % 1000) / 1000, float(rand() % 1000) / 1000, float(rand() % 1000) / 1000);
					glVertex2fv(new_pg);
					glColor3f(float(rand() % 1000) / 1000, float(rand() % 1000) / 1000, float(rand() % 1000) / 1000);
					glVertex2fv(new_pd);
					glEnd();
				}
				else {
					//kwadrat pusty, bo i==1 j==1
					glBegin(GL_POLYGON);
					glColor3f(0.5f, 0.5f, 0.5f); // wierzcho�ek pusty
					glVertex2fv(new_ld);
					glVertex2fv(new_lg);
					glVertex2fv(new_pg);
					glVertex2fv(new_pd);
					glEnd();
				}
			}
		}
		return 0;
	}
	// warstwa > 1
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			point2 new_ld = { ld[0] + (bok_maly * i), ld[1] + (bok_maly * j) };
			point2 new_lg = { ld[0] + (bok_maly * i), ld[1] + bok_maly + (bok_maly * j) };
			point2 new_pg = { ld[0] + bok_maly + (bok_maly * i), ld[1] + bok_maly + (bok_maly * j) };
			point2 new_pd = { ld[0] + bok_maly + (bok_maly * i), ld[1] + (bok_maly * j) };
			if ((i != 1) || (j != 1)) {
				//rysuj w nowych wspolrzednych, licznik--
				rysowanie(new_ld, new_pd, warstwy - 1);
			}
			else {
				//rysuj szary kwadrat w srodku
				glBegin(GL_POLYGON);
				glColor3f(0.5f, 0.5, 0.5f); // wierzcho�ek pusty
				glVertex2fv(new_ld);
				glVertex2fv(new_lg);
				glVertex2fv(new_pg);
				glVertex2fv(new_pd);
				glEnd();
			}
		}
	}

	return 0;

}


// Funkcaja okre�laj�ca, co ma by� rysowane 
// (zawsze wywo�ywana, gdy trzeba przerysowa� scen�)


void RenderScene(void) {



	glClear(GL_COLOR_BUFFER_BIT);

	// Czyszczenie okna aktualnym kolorem czyszcz�cym
	point2 ld = { -50, -50 };
	point2 pd = { 50, -50 };
	int warstwy = 4;
	rysowanie(ld, pd, warstwy);
	glFlush();
	// Przekazanie polece� rysuj�cych do wykonania
}
/*************************************************************************************/
// Funkcja ustalaj�ca stan renderowania

void MyInit(void)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    // Kolor okna wn�trza okna - ustawiono na szary
}
/*************************************************************************************/
// Funkcja s�u��ca do kontroli zachowania proporcji rysowanych obiekt�w
// niezale�nie od rozmiar�w okna graficznego


void ChangeSize(GLsizei horizontal, GLsizei vertical)
// Parametry horizontal i vertical (szeroko�� i wysoko�� okna) s�
// przekazywane do funkcji za ka�dym razem, gdy zmieni si� rozmiar okna
{
    GLfloat AspectRatio;
    // Deklaracja zmiennej AspectRatio okre�laj�cej proporcj� wymiar�w okna

    if (vertical == 0)
        // Zabezpieczenie pzred dzieleniem przez 0
        vertical = 1;

    glViewport(0, 0, horizontal, vertical);
    // Ustawienie wielko�ciokna okna urz�dzenia (Viewport)
    // W tym przypadku od (0,0) do (horizontal, vertical)

    glMatrixMode(GL_PROJECTION);
    // Okre�lenie uk�adu wsp�rz�dnych obserwatora
    glLoadIdentity();
    // Okre�lenie przestrzeni ograniczaj�cej
    AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    // Wyznaczenie wsp�czynnika proporcji okna
    // Gdy okno na ekranie nie jest kwadratem wymagane jest 
    // okre�lenie okna obserwatora. 
    // Pozwala to zachowa� w�a�ciwe proporcje rysowanego obiektu
    // Do okre�lenia okna obserwatora s�u�y funkcja glOrtho(...)

    if (horizontal <= vertical)
        glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);
    else
        glOrtho(-100.0 * AspectRatio, 100.0 * AspectRatio, -100.0, 100.0, 1.0, -1.0);
    glMatrixMode(GL_MODELVIEW);
    // Okre�lenie uk�adu wsp�rz�dnych     
    glLoadIdentity();
}
/*************************************************************************************/
// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli

void main(void)
{
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    // Ustawienie trybu wy�wietlania
    // GLUT_SINGLE - pojedynczy bufor wy�wietlania
    // GLUT_RGBA - model kolor�w RGB

    glutCreateWindow("Drugi program w OpenGL");
    // Utworzenie okna i okre�lenie tre�ci napisu w nag��wku okna

    glutDisplayFunc(RenderScene);
    // Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn� (callback)
    // Biblioteka GLUT b�dzie wywo�ywa�a t� funkcj� za ka�dym razem, gdy
    // trzeba b�dzie przerysowa� okno


    glutReshapeFunc(ChangeSize);
    // Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn� za
    // zmiany rozmiaru okna

    MyInit();
    // Funkcja MyInit (zdefiniowana powy�ej) wykonuje wszelkie  
    // inicjalizacje konieczneprzed przyst�pieniem do renderowania

    glutMainLoop();
    // Funkcja uruchamia szkielet biblioteki GLUT
}