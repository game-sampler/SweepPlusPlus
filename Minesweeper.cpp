#include <iostream>
#include <vector>
#include <random>

struct Mine {
	bool pressed, flagged, contains_mine;
	int value;
	Mine(bool p, bool m) : pressed(p), contains_mine(m), value((m ? 9 : 0)), flagged(false) {};
};
typedef std::vector<std::vector<Mine>> mgrid;

int randnumber() {
	std::random_device r;
	std::mt19937 gen(r());
	std::uniform_int_distribution<> distrib(1, 100);
	return (distrib(gen));
}

void gpop(mgrid* m, int s, int d) {
	for (int i = 0; i < s; i++) {
		std::vector<Mine> row;
		for (int j = 0; j < s; j++) row.push_back(Mine(false, (randnumber() <= d ? true : false)));
		m->push_back(row);
	}
}

void print_board(mgrid* m, bool print_loss_mode) {
	for (auto c : m[0]) {
		for (auto r : c) {
			if (print_loss_mode) {
				std::cout << r.value << " ";
				continue;
			}
			if (r.flagged) {std::cout << "F";}
			else if (!(r.pressed)) {std::cout << "?";}
			else std::cout << r.value;
			std::cout << " ";
		}
		std::cout << std::endl;
	}
}

void recurse(mgrid* m, int x, int y, int s) {
	m[0][x][y].pressed = true;
	if (m[0][x][y].value == 0) {
		if (x > 0) {
			if (y > 0 && !(m[0][x - 1][y - 1].pressed) && m[0][x - 1][y - 1].value != 9) recurse(m, x - 1, y - 1, s);
			if (y < s-1 && !(m[0][x - 1][y + 1].pressed) && m[0][x - 1][y + 1].value != 9) recurse(m, x - 1, y + 1, s);
			if (!(m[0][x - 1][y].pressed) && m[0][x - 1][y].value != 9) recurse(m, x - 1, y, s);
		}
		if (x < s - 1) {
			if (y > 0 && !(m[0][x + 1][y - 1].pressed) && m[0][x + 1][y - 1].value != 9) recurse(m, x + 1, y - 1, s);
			if (!(m[0][x + 1][y].pressed) && m[0][x + 1][y].value != 9) recurse(m, x + 1, y, s);
			if (y < s - 1 && !(m[0][x + 1][y + 1].pressed) && m[0][x + 1][y + 1].value != 9) recurse(m, x + 1, y + 1, s);
		}
		if (y > 0 && !(m[0][x][y - 1].pressed) && m[0][x][y - 1].value != 9) recurse(m, x, y - 1, s);
		if (y < s-1 && !(m[0][x][y + 1].pressed) && m[0][x][y + 1].value != 9) recurse(m, x, y + 1, s);
	}
}

void set_values(mgrid* m) {
	for (int i = 0; i < m->size(); i++) {
		for (int j = 0; j < m->size(); j++) {
			if (m[0][i][j].value != 9) {
				if (i > 0) {
					if (j > 0 && m[0][i - 1][j - 1].contains_mine) m[0][i][j].value++;
					if (j < m->size()-1 && m[0][i-1][j+1].contains_mine) m[0][i][j].value++;
					if (m[0][i - 1][j].contains_mine) m[0][i][j].value++;
				}
				if (i < m->size() - 1) {
					if (j > 0 && m[0][i + 1][j - 1].contains_mine) m[0][i][j].value++;
					if (m[0][i + 1][j].contains_mine) m[0][i][j].value++;
					if (j < m->size() - 1 && m[0][i + 1][j + 1].contains_mine) m[0][i][j].value++;
				}
				if (j > 0 && m[0][i][j - 1].contains_mine) m[0][i][j].value++;
				if (j < m->size() - 1 && m[0][i][j + 1].contains_mine) m[0][i][j].value++;
			}
		}
	}
}

bool checkwin(mgrid* m) {
	for (int i = 0; i < m->size(); i++) {
		for (int j = 0; j < m->size(); j++) {
			if (!(m[0][i][j].pressed) && !(m[0][i][j].contains_mine)) return false;
		}
	}
	return true;
}

const int MIN_SZ = 7, MAX_SZ = 200;

int main()
{
	Mine m = Mine(false, true);
	int size;
	int density;
	std::cout << "Enter a board size from " << MIN_SZ << "-" << MAX_SZ << ", and a mine density from 1-100, representing the approx. % of the board that will be mines.\n Separate the numbers with a space.";
	if (scanf_s("%d %d", &size, &density) != 2 || density <= 0 || density > 100 || size < MIN_SZ || size > MAX_SZ) {
		std::cerr << "Error: Invalid numbers.";
		return 1;
	}
	mgrid matrix;
	gpop(&matrix, size, density);
	set_values(&matrix);
	std::cout << "0's are empty tiles, and the numbers 1-8 on a tile represent the number of mines on a tile.\nF represents a flag, and ? is an unclicked tile.\n";
	bool lost = false;
	while (!(lost)) {
		print_board(&matrix, false);
		std::cout << "Enter F to flag or C to click." << std::endl;
		char mode = std::cin.get();
		if (mode != 'F' && mode != 'C') {
			std::cerr << "Error: Invalid mode. Use capital F for flagging and capital C for clicking\n";
		}
		else {
			std::cout << "Enter two numbers from 1 to " << size << " separated by a space." << std::endl;
			int first = 0;
			int second = 0;
			if (scanf_s("%d %d", &first, &second) != 2 || first < 1 || second < 1 || second > size || first > size) {
				std::cerr << "Error: Invalid numbers.";
				continue;
			}
			if (matrix[first-1][second-1].contains_mine && mode == 'C') {
				lost = true;
				std::cout << "final board :\n\n";
				print_board(&matrix, true);
			}
			if (mode == 'C') {
				recurse(&matrix, first - 1, second - 1, size);
			}
			else matrix[first - 1][second - 1].flagged = true;
		}
		if (checkwin(&matrix)) {
			std::cout << "You won!\n\n";
			print_board(&matrix, true);
			exit(0);
		}
	}	
}
