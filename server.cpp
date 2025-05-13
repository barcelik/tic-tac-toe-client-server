#include <iostream>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "rules.h"

#define PORT 12345
#define BUFSZ 80

using namespace std;

void *handle(void *arg) {
	// fonction exécutée dans un thread pour gérer un client
	int sock = *((int *)arg);
	delete (int *)arg;
	
	srand(time(NULL));

	const char *modePrompt = "Choose the difficulty level:\n1. Easy\n2. Hard\nYour choice: ";
	write(sock, modePrompt, BUFSZ);
	// envoi du menu de choix du mode au client
	int mode;
	read(sock, &mode, sizeof(int));
	bool easyMode = (mode == 1);
	
	vector<vector<char>> grid(3, vector<char>(3, E));
	// initialisation de la grille vide
	char current = P1;
	char *view = formatGrid(grid, 3, 3);
	write(sock, view, BUFSZ);

	while (true) {
		char msg[BUFSZ] = {};

		if (current == P1) {
			// tour du joueur "humain"
			int row, col;
			strcpy(msg, "Enter move (row col): ");
			write(sock, msg, BUFSZ);
			read(sock, &row, sizeof(int));
			read(sock, &col, sizeof(int));
			row--; col--;

			if (valid(grid, row, col)) {
				// vérifie si le coup est valide, sinon renvoie un message d’erreur
				grid[row][col] = P1;
			} else {
				write(sock, "Invalid move.\n", BUFSZ);
				view = formatGrid(grid, 3, 3);
				write(sock, view, BUFSZ);
				continue;
			}
		} else {
			pair<int, int> botMove = easyMode ? randomMove(grid) : bestMove(grid, P2);
			// choix du coup de l'IA (facile ou difficile selon le mode choisi)
			grid[botMove.first][botMove.second] = P2;
			view = formatGrid(grid, 3, 3);
			write(sock, view, BUFSZ);
		}

	char win = winner(grid);
	// vérifie si quelqu’un a gagné ou si la partie est terminée
	if (win != E) {
		const char *result = (win == P1)
		? "Well done! You’ve won!\n"
		: (win == P2) ? "Oh no, you lost. Give it another shot!\n"
		: "No winner this time!";
		write(sock, result, strlen(result) + 1);
		break;
	}

	current = (current == P1) ? P2 : P1;
	}

	close(sock);
	pthread_detach(pthread_self());
	return nullptr;
}

int main() {
	int listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0) {
		perror("socket");
		return 1;
	}

	int opt = 1;
	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in addr {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (::bind(listener, (sockaddr *)&addr, sizeof(addr)) < 0 ||
		listen(listener, 5) < 0) {
		perror("bind/listen");
		return 1;
	}

	while (true) {
		int client = accept(listener, nullptr, nullptr);
		if (client < 0) continue;

		pthread_t tid;
		int *pclient = new int(client);
		pthread_create(&tid, nullptr, handle, pclient);
	}

	return 0;
}