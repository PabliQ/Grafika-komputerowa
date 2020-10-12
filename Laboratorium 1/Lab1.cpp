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
					glColor3f(0.5f, 0.5f, 0.5f); // wierzcho³ek pusty
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
				glColor3f(0.5f, 0.5, 0.5f); // wierzcho³ek pusty
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


// Funkcaja okreœlaj¹ca, co ma byæ rysowane 
// (zawsze wywo³ywana, gdy trzeba przerysowaæ scenê)


void RenderScene(void) {



	glClear(GL_COLOR_BUFFER_BIT);

	// Czyszczenie okna aktualnym kolorem czyszcz¹cym
	point2 ld = { -50, -50 };
	point2 pd = { 50, -50 };
	int warstwy = 4;
	rysowanie(ld, pd, warstwy);
	glFlush();
	// Przekazanie poleceñ rysuj¹cych do wykonania
}
/*************************************************************************************/
// Funkcja ustalaj¹ca stan renderowania

void MyInit(void)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    // Kolor okna wnêtrza okna - ustawiono na szary
}
/*************************************************************************************/
// Funkcja s³u¿¹ca do kontroli zachowania proporcji rysowanych obiektów
// niezale¿nie od rozmiarów okna graficznego


void ChangeSize(GLsizei horizontal, GLsizei vertical)
// Parametry horizontal i vertical (szerokoœæ i wysokoœæ okna) s¹
// przekazywane do funkcji za ka¿dym razem, gdy zmieni siê rozmiar okna
{
    GLfloat AspectRatio;
    // Deklaracja zmiennej AspectRatio okreœlaj¹cej proporcjê wymiarów okna

    if (vertical == 0)
        // Zabezpieczenie pzred dzieleniem przez 0
        vertical = 1;

    glViewport(0, 0, horizontal, vertical);
    // Ustawienie wielkoœciokna okna urz¹dzenia (Viewport)
    // W tym przypadku od (0,0) do (horizontal, vertical)

    glMatrixMode(GL_PROJECTION);
    // Okreœlenie uk³adu wspó³rzêdnych obserwatora
    glLoadIdentity();
    // Okreœlenie przestrzeni ograniczaj¹cej
    AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    // Wyznaczenie wspó³czynnika proporcji okna
    // Gdy okno na ekranie nie jest kwadratem wymagane jest 
    // okreœlenie okna obserwatora. 
    // Pozwala to zachowaæ w³aœciwe proporcje rysowanego obiektu
    // Do okreœlenia okna obserwatora s³u¿y funkcja glOrtho(...)

    if (horizontal <= vertical)
        glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);
    else
        glOrtho(-100.0 * AspectRatio, 100.0 * AspectRatio, -100.0, 100.0, 1.0, -1.0);
    glMatrixMode(GL_MODELVIEW);
    // Okreœlenie uk³adu wspó³rzêdnych     
    glLoadIdentity();
}
/*************************************************************************************/
// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli

void main(void)
{
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    // Ustawienie trybu wyœwietlania
    // GLUT_SINGLE - pojedynczy bufor wyœwietlania
    // GLUT_RGBA - model kolorów RGB

    glutCreateWindow("Drugi program w OpenGL");
    // Utworzenie okna i okreœlenie treœci napisu w nag³ówku okna

    glutDisplayFunc(RenderScene);
    // Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹ (callback)
    // Biblioteka GLUT bêdzie wywo³ywa³a t¹ funkcjê za ka¿dym razem, gdy
    // trzeba bêdzie przerysowaæ okno


    glutReshapeFunc(ChangeSize);
    // Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹ za
    // zmiany rozmiaru okna

    MyInit();
    // Funkcja MyInit (zdefiniowana powy¿ej) wykonuje wszelkie  
    // inicjalizacje konieczneprzed przyst¹pieniem do renderowania

    glutMainLoop();
    // Funkcja uruchamia szkielet biblioteki GLUT
}