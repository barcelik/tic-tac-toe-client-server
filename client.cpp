#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFSZ 80
#define PORT 12345

using namespace std;

void jouer(int sock) {
	// fonction principale du jeu côté client
	int mode;
	int x, y;
	char msg[BUFSZ];

	read(sock, msg, BUFSZ);
	cout << msg;
	cin >> mode;
	write(sock, &mode, sizeof(int));
	// lecture du choix du mode (facile ou difficile) et envoi au serveur

	read(sock, msg, BUFSZ);
	cout << msg << endl;

	while (true) {
		bzero(msg, BUFSZ);
		read(sock, msg, BUFSZ);

		if (strstr(msg, "win") || strstr(msg, "draw") || strstr(msg, "lost")) {
			// détection de fin de partie
			cout << msg << endl;
			break;
		}

		cout << msg;
		while (true) {
			// saisie de coup par le joueur et traitement en boucle jusqu'à un coup valide
			cin >> x >> y;
			write(sock, &x, sizeof(int));
			write(sock, &y, sizeof(int));
			
			bzero(msg, BUFSZ);
			read(sock, msg, BUFSZ);

			if (strcmp(msg, "Invalid move. Try again.\n") != 0)
				break;

			cout << msg;
			read(sock, msg, BUFSZ);
			cout << msg;
		}
		cout << msg;
	}

	bzero(msg, BUFSZ);
	read(sock, msg, BUFSZ);
	cout << msg << endl;
}

int main(int argc, char *argv[]) {
	// vérifie que l'adresse du serveur est fournie en argument
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " <server_address>" << endl;
		return 1;
	}

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	// création du socket TCP
	if (sock < 0) {
		perror("socket");
		return 1;
	}

	sockaddr_in serv_addr {};
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	hostent *host = gethostbyname(argv[1]);
	// résolution de l'adresse IP du serveur à partir du nom
	if (!host) {
		cerr << "DNS lookup failed\n";
		close(sock);
		return 1;
	}

	memcpy(&serv_addr.sin_addr, host->h_addr, host->h_length);

	if (connect(sock, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
	// connexion au serveur
		perror("connect");
		close(sock);
		return 1;
	}

	jouer(sock);
	close(sock);
	return 0;
}
