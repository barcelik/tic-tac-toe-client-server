#include <iostream>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>

#define BUFSZ 80

const char P1 = 'X';
const char P2 = 'O';
const char E = ' ';

using namespace std;


// affiche la grille dans le terminal (utile pour debug côté serveur)
inline void showGrid(const vector<vector<char>> &g) {
	cout << "-------------\n";
	for (int i = 0; i < 3; ++i) {
		cout << "| ";
		for (int j = 0; j < 3; ++j) {
			cout << g[i][j] << " | ";
		}
		cout << "\n-------------\n";
	}
}


// vérifie si un coup est valide et retourne true si la position est jouable, sinon false
inline bool valid(const vector<vector<char>> &g, int r, int c) {
	return r >= 0 && r < 3 && c >= 0 && c < 3 && g[r][c] == E;
}


// vérifie si la grille est pleine (plus aucun coup possible)
inline bool full(const vector<vector<char>> &g) {
	for (auto &row : g)
		for (char c : row)
			if (c == E) return false;
	return true;
}


// fonction qui détermine le vainqueur de la partie (ou s’il y a égalité, ou si la partie continue)
inline char winner(const vector<vector<char>> &g) {
	for (int i = 0; i < 3; ++i)
		if (g[i][0] != E && g[i][0] == g[i][1] && g[i][1] == g[i][2])
			return g[i][0];
	for (int i = 0; i < 3; ++i)
		if (g[0][i] != E && g[0][i] == g[1][i] && g[1][i] == g[2][i])
			return g[0][i];
	if (g[0][0] != E && g[0][0] == g[1][1] && g[1][1] == g[2][2]) return g[0][0];
	if (g[0][2] != E && g[0][2] == g[1][1] && g[1][1] == g[2][0]) return g[0][2];
	return full(g) ? 'D' : E;
}


// fonction récursive pour évaluer la grille avec l’algorithme Minimax
inline int score(vector<vector<char>> &g, int d, bool max, char me) {
	char foe = (me == P1) ? P2 : P1;
	char w = winner(g);
	if (w != E)
		return (w == me) ? 10 - d : (w == foe) ? d - 10 : 0;

	int best = max ? INT_MIN : INT_MAX;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			if (g[i][j] == E) {
				g[i][j] = max ? me : foe;
				int val = score(g, d + 1, !max, me);
				g[i][j] = E;
				best = max ? std::max(best, val) : std::min(best, val);
			}
	return best;
}


// trouve le meilleur coup à jouer pour me en utilisant score()
inline pair<int, int> bestMove(vector<vector<char>> &g, char me) {
	int best = INT_MIN;
	pair<int, int> move = {-1, -1};

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			if (g[i][j] == E) {
				g[i][j] = me;
				int val = score(g, 0, false, me);
				g[i][j] = E;
				if (val > best) {
					best = val;
					move = {i, j};
				}
			}
	return move;
}


// choisit un coup aléatoire parmi les cases vides
inline pair<int, int> randomMove(const vector<vector<char>> &g) {
	vector<pair<int, int>> moves;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (g[i][j] == E) {
				moves.push_back({i, j});
			}
		}
	}
	if (moves.empty()) return {-1, -1};
	int idx = rand() % moves.size();
	return moves[idx];
}


// formate la grille en une chaîne char* (utilisée pour les messages envoyés par le serveur)
inline char *formatGrid(const vector<vector<char>> &g, int rows, int cols) {
	char *txt = (char *)malloc(BUFSZ);
	int n = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			txt[n++] = g[i][j];
			if (j != cols - 1) {
				txt[n++] = ' ';
				txt[n++] = '|';
				txt[n++] = ' ';
			}
		}
		txt[n++] = '\n';
		if (i != rows - 1) {
			memset(&txt[n], '-', cols * 4 - 1);
			n += cols * 4 - 1;
			txt[n++] = '\n';
		}
	}
	txt[n] = '\0';
	return txt;
}
