/*
	Created by: Omar Pena, 
	Year: 2024,
	Version: 1.1,
	Language: C++,
	Credits: {}
*/
#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <list>
#include <iostream>

using namespace std;

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80
#define SHOOT_KEY 'a'

void gotoxy(int x, int y) {
	HANDLE hCon;
	hCon = GetStdHandle(STD_OUTPUT_HANDLE);

	COORD dwPos;
	dwPos.X = x;
	dwPos.Y = y;

	SetConsoleCursorPosition(hCon, dwPos);
}

void HideCursor() {
	HANDLE hCon;
	hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 1;
	cci.bVisible = FALSE;

	SetConsoleCursorInfo(hCon, &cci);
}

void draw_limit() {
	for (int i = 2; i < 78; i++) {
		gotoxy(i, 3);  printf("%c", 186);
		gotoxy(i, 33); printf("%c", 186);
	}
	for (int i = 4; i < 33; i++) {
		gotoxy(2, i); printf("%c", 186);
		gotoxy(77, i); printf("%c", 186);
	}

	gotoxy(2, 3); printf("%c", 201);
	gotoxy(2, 33); printf("%c", 200);
	gotoxy(77, 3); printf("%c", 187);
	gotoxy(77, 33); printf("%c", 188);

}

class SHIP {
private: 
	int x, y;
	int hearts;
	int lives;
	//std::vector<std::pair<int, int>> bullets; 

public: 
	SHIP(int _x, int _y, int _hearts, int _lives) : x(_x), y(_y), hearts(_hearts), lives(_lives) {}
	int X() { return x; }
	int Y() { return y; }
	int LIV() { return lives; }
	int get_lives() { return lives; }
	void set_lives(int lives) { this->lives = lives; }
	void HER() { hearts--; }
	void draw();
	void remove();
	void move();
	void draw_heart();
	void lose();
	void shoot();
};

void SHIP::draw() {
	gotoxy(x, y);     printf("  /\\  ");
	gotoxy(x, y + 1); printf(" /  \\");
	gotoxy(x, y + 2); printf("/_/\\_\\");
}

void SHIP::remove() {
	gotoxy(x, y);	  printf("        ");
	gotoxy(x, y + 1); printf("        ");
	gotoxy(x, y + 2); printf("        ");
}

void SHIP::move() {
	if (_kbhit()) {
		char key = _getch();
		remove();
		if (key == LEFT && x > 3) x--;
		if (key == RIGHT && x + 6 < 77) x++;
		if (key == UP && y > 4) y--;
		if (key == DOWN && y + 3 < 33) y++;
		draw();
	}

}

void SHIP::draw_heart() {
	gotoxy(50, 2); printf("Lives %d", lives);
	gotoxy(64, 2); printf("Health ");
	gotoxy(70, 2); printf("      ");

	for (int i = 0; i < hearts; i++) {
		
		gotoxy(71 + i, 2); printf("%c", 3);
	}
}

void SHIP::lose() {
	if (hearts == 0) {
		remove();

		gotoxy(x, y);    printf("   **   ");
		gotoxy(x, y+1);  printf("  ****  ");
		gotoxy(x, y+2);  printf("   **   ");
		Sleep(200);

		remove();
		gotoxy(x, y);	   printf(" * ** *");
		gotoxy(x, y + 1);  printf("  **** ");
		gotoxy(x, y + 2);  printf(" * ** *");
		Sleep(200);
		remove();
		lives--;
		hearts = 3;
		draw_heart();
		draw();
	}
}

class AST {
	int x, y;
public:
	AST(int _x, int _y) : x(_x), y(_y) {}
	void draw();
	void move();
	void crash(class SHIP& N); //Buenas practicas al utilizar la memoria heap quitarle el tipo a la clase o estructura
	int X() { return x; } //Getter 
	int Y() { return y; }

	
};

void AST::draw() {
	gotoxy(x, y); printf("0");
}

void AST::move() {
	gotoxy(x, y); printf(" ");
	y++;
	if (y > 32) {
		x = rand() % 71 + 4;
		y = 4;
	}

	draw();
}

void AST::crash(SHIP& N) {
	if (x >= N.X() && x < N.X() + 6 && y >= N.Y() && y <= N.Y() + 2) {
		N.HER();
		N.remove();
		N.draw();
		N.draw_heart();
		x = rand() % 71 + 4;
		y = 4;
	}
}

void PlayStartMusic() {
	PlaySound(L"C:\\Users\\AlexScott\\source\\repos\\AstrerTwo\\AstrerTwo\\splashsong.wav", NULL, SND_FILENAME | SND_LOOP);
}

void PlayGameMusic() {
	//Stop the splash
	PlaySound(NULL, 0, 0);

	PlaySound(L"C:\\Users\\AlexScott\\source\\repos\\AstrerTwo\\AstrerTwo\\gamesong.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

class BULLET {
	int x, y;
public:
	BULLET(int _x, int _y) : x(_x), y(_y) {}
	int X() { return x; }
	int Y() { return y; }
	void move();
	bool out();
};

void BULLET::move() {
	gotoxy(x,y); printf(" ");
	y--;
	gotoxy(x, y); printf("^");

}

bool BULLET::out() {
	if (y == 4) return true;
	return false;
}

void clear_board() {
	// Clears the contents of the screen within the boundaries set by draw_limit().
	for (int y = 4; y < 32; y++) {
		for (int x = 3; x < 77; x++) {
			gotoxy(x, y);  printf(" ");
		}
	}
}

bool end_game() {
	// displays game over message and asks for prompt to continue or quit.
	// Returns false if continue; true if quit.
	clear_board();
	gotoxy(35, 17); printf("GAME OVER!");
	gotoxy(35, 18); printf("Continue?");
	gotoxy(22, 19); printf("Press ENTER to continue. ESC to quit.");
	int choice = _getch();
	if (choice == 13) { return false; }
	if (choice == 27) { return true; }
}


class Game {
private:
	list<AST*> asteroids;
	list<BULLET*> bullets; 
	int points; 

public:
	Game() : points(0) {}
	~Game();
	void start();
	void drawGameInfo();
};

Game::~Game() {
	for (auto& asteroid : asteroids) {
		delete asteroid;
	}

	asteroids.clear();

	//Liberar memoria de las balas

	for (auto& bullet : bullets) {
		delete bullet;
	}

	bullets.clear();

}

void clearScreen() {
	system("cls");
}

void Game::start() {
		clearScreen();
		draw_limit();
		PlayGameMusic();
		SHIP N(37, 30, 3, 3);
		N.draw();
		N.draw_heart();

		list<AST*> A;
		list<AST*>::iterator itA;
		for (int i = 0; i < 5; i++) {
			A.push_back(new AST(rand() % 75 + 3, rand() % 5 + 4));
		}

		list<BULLET*> B;
		list<BULLET*>::iterator it;

		bool game_over = false;
		int points = 0;

		while (!game_over) {
			N.draw();
			gotoxy(4, 2); printf("Points %d", points);
			printf("\t\tPress 'a' to shoot!");
			if (_kbhit()) {

				char key = _getch();
				if (key == SHOOT_KEY)
					B.push_back(new BULLET(N.X() + 2, N.Y() - 1));
			}
			for (it = B.begin(); it != B.end();) {
				(*it)->move();
				if ((*it)->out()) {
					gotoxy((*it)->X(), (*it)->Y()); printf(" ");
					delete(*it); //el it se invalida son casillas se una con otra pierde el hi
					it = B.erase(it);
				}
				else {
					++it;
				}
			}

			for (itA = A.begin(); itA != A.end();itA++)
			{
				(*itA)->move();
				(*itA)->crash(N);
			}


			for (itA = A.begin(); itA != A.end();) {

				bool asteroidRemoved = false;

				for (it = B.begin(); it != B.end();) {
					if ((*itA)->X() == (*it)->X() && ((*itA)->Y() + 1 == (*it)->Y() || (*itA)->Y() == (*it)->Y())) {
						gotoxy((*it)->X(), (*it)->Y()); printf(" ");
						delete(*it);
						it = B.erase(it);

						A.push_back(new AST(rand() % 74 + 3, 4));
						gotoxy((*itA)->X(), (*itA)->Y()); printf(" ");
						delete(*itA);
						itA = A.erase(itA);
						asteroidRemoved = true;
						points += 5;


						break;
					}
					else {
						++it;
					}
				}

				if (!asteroidRemoved) {
					++itA;
				}
			}

			N.lose();
			N.move();
			Sleep(30);

			if (N.get_lives() <= 0) {
				game_over = end_game();
				clear_board();
				N.set_lives(3);
			}
		}
	
}

void draw_title() {
	system("cls");  // Limpiar la pantalla

	// Caracteres para formar el título AstherTwo - THE VENGACE
	/*std::cout << R"(
  ____ _ _     _     _______ _           _     
 / __ (_) |   | |   |__   __| |         | |    
| |  | |_| |_  | |_ _   _| | |__   __ _| |__  
| |  | | | __| | __| | | | | '_ \ / _` | '_ \ 
| |__| | | |_  | |_| |_| | | | | | (_| | | | |
 \____/|_|\__|  \__|\__, |_|_| |_|\__,_|_| |_|
                    __/ |                      
                   |___/                       
)" << std::endl << std::endl;*/
	draw_limit();
	printf("\n\n\t\t\t AstherTwo - The vengeance\n\n");
}

int menu() {
	int choice = 1;  // 1 para "Start", 2 para "Exit"
	int key;

	while (true) {
		system("cls");  // Limpiar la pantalla
		draw_title();

		// Mostrar opciones de menú y resaltar la opción actual
		std::cout << "\t\t\t";
		if (choice == 1) std::cout << "> ";
		std::cout << "1. Start" << std::endl;

		std::cout << "\t\t\t";
		if (choice == 2) std::cout << "> ";
		std::cout << "2. Exit" << std::endl;

		key = _getch();  // Leer la tecla sin esperar la pulsación de Enter

		switch (key) {
		case 224:  // Tecla de flecha (extensión)
			key = _getch();  // Leer la tecla de flecha real
			if (key == UP && choice > 1) choice--;
			else if (key == DOWN && choice < 2) choice++;
			break;
		case 13:  // Tecla Enter
			return choice;
		default:
			break;
		}
	}
}


int main() {
	//Configurations 
	HideCursor();
	draw_title();

	int choice = menu();

	switch (choice) {
	case 1: {
		clearScreen();
		Game game;
		game.start();
		break;
	}
	case 2:
		break;
	default:
		printf("Invalid choice\n");
		break;
	}

	return 0;
}
