// Ethan Rickert - COSC140
// Sudoku.cpp - auto solves sudoku games from txt files
// 10-27-2020

#include <sys/time.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

class omega {
public:
    omega();
    float get_sec();

private:
    long Tinit;
    long Tmark;
};

omega::omega() {
    struct timeval T;
    gettimeofday(&T, NULL);

    Tinit = 1000000 * T.tv_sec + T.tv_usec;
}

float omega::get_sec() {
    struct timeval T;
    gettimeofday(&T, NULL);

    Tmark = 1000000 * T.tv_sec + T.tv_usec;

    return (float)(Tmark - Tinit) / 1000000.0;
}

// char array that will hold time in msec
const char* units[255];

// recursive time-elapsed generator
string elapsed(float duration, int i=0) {
    if (duration < 0.1)
        return elapsed(duration * 1000, i + 1);
    else {
        ostringstream os;
        os << duration << units[i];
        return os.str();
    }
}

class sudoku {
public:
    sudoku();

    void solve();

    void read(const char*);
    void write(const char*);
    void write(const char*, const char*);

private:
    bool solve(vector<int>, int);
    bool error_check_value(bool);
    bool error_check_uniqueness();

    // helper functions checks validity of row col & square
    bool check_row(int, int);
    bool check_col(int, int);
    bool check_square(int, int, int);
    vector<int> valid_values(int, int);

    void display();

    // sudoku grid in 2D array [row][column]
    int game[9][9];
};

sudoku::sudoku() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++)
            game[i][j] = 0;
    }
}

void sudoku::solve() {
    bool error = false;
    vector<int> cells;
    cout << "SOLVE\n";

    // filling cells vector with empty cells
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (game[i][j] == 0) {
                cells.push_back((i * 9) + j);
            }
        }
    }
    // call recursive solve function

    if (solve(cells, 0))
        display();

    // error checking
    if (error_check_value(false)) error = true;
    if (error_check_uniqueness()) error = true;
    if (error) exit(0);
}

bool sudoku::error_check_value(bool zero_valid) {
  // checking if values are in valid bounds
    bool error = false;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
          // for grid with empty slots
            if (zero_valid) {
                if (!(game[i][j] >= 0 && game[i][j] <= 9)) {
                    error = true;
                }
            }
          // for grid without empty slots
            if (!zero_valid) {
                if (!(game[i][j] >= 1 && game[i][j] <= 9)) {
                    error = true;
                }
            }
        }
    }
    return error;
}

// checks forr repeated values within a column row or square
bool sudoku::error_check_uniqueness() {
    bool error = false;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (game[i][j] > 0) {
                if (!check_row(i, game[i][j])) { error = true; cerr << "cell " << i << " " << j << ": non-unique value " << game[i][j] << '\n'; }   
                else if (!check_col(j, game[i][j])) { error = true; cerr << "cell " << i << " " << j << ": non-unique value " << game[i][j] << '\n'; }
                else if (!check_square(i, j, game[i][j])) { error = true; cerr << "cell " << i << " " << j << ": non-unique value " << game[i][j] << '\n'; }
            }
        }
    }
    return error;
}

bool sudoku::check_row(int r, int v) {
    int count = 0;
    for (int i = 0; i < 9; i++) {
        if (game[r][i] == v) { count++; }
        if (count >= 2) return false;
    }
    return true;
}

bool sudoku::check_col(int c, int v) {
    int count = 0;
    for (int i = 0; i < 9; i++) {
        if (game[i][c] == v) { count++; }
        if (count >= 2) return false;
    }
    return true;
}

bool sudoku::check_square(int i, int j, int v) {
    int count = 0;
    // magic formula ~ (a / 3) * 3 ~ derived from Clara Nguyen writeup - retrives top left corner of a given cell at the center
    for (int a = (i / 3) * 3; a < ((i / 3) * 3) + 3; a++) {
        for (int b = (j / 3) * 3; b < ((j / 3) * 3) + 3; b++) {
            if (game[a][b] == v) { count++; }
        }
    }
    if (count >= 2) return false;
    return true;
}

vector<int> sudoku::valid_values(int r, int c) {
    vector<int> vv;
    int tmp = game[r][c];
    for (int i = 1; i <= 9; i++) {
        game[r][c] = i;
        if (check_row(r, game[r][c]) && check_col(c, game[r][c]) && check_square(r, c, game[r][c]))
            vv.push_back(i);
    }
    game[r][c] = tmp;
    return vv;
}

void sudoku::read(const char* fname) {
    bool error = false;
    int line = 1;
    cout << "READ\n";

    ifstream fin(fname);

    unsigned int i, j, v;

    while (fin >> i >> j >> v) {
        // error check grid indices
        if ((i > 8) || (j > 8) || ((v > 9) || (v < 1))) {
            error = true;
            cerr << "line " << line << ": " << i << " " << j << " " << v << " out-of-bounds grid index\n";
        }
        line++;
        game[i][j] = v;
    }
    if (error) exit(0);

    fin.close();
    display();

    if (error_check_value(true)) error = true;
    if (error_check_uniqueness()) error = true;
    if (error) exit(0);
}

void sudoku::write(const char* fname) {
    ofstream fout(fname);

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (0 < game[i][j]) {
                fout << i << " "
                    << j << " "
                    << game[i][j] << "\n";
            }
        }
    }

    fout.close();
}

void sudoku::write(const char* fname, const char* addon) {
    int N1 = strlen(fname);
    int N2 = strlen(addon);

    char* n_fname = new char[N1 + N2 + 2];

    // strip .txt suffix, then concatenate _addon.txt
    strncpy(n_fname, fname, N1 - 4);
    strcpy(n_fname + N1 - 4, "_");
    strcpy(n_fname + N1 - 3, addon);
    strcpy(n_fname + N1 - 3 + N2, ".txt");

    write(n_fname);

    delete[] n_fname;
}

void sudoku::display() {
    cout << "| --------------------------- |\n";
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (j == 0)
                cout << "| " << game[i][j];
            else if (j % 3 == 0)
                cout << " | " << game[i][j];
            else
                cout << "  " << game[i][j];
        }
        cout << " |\n";
        if (i % 3 == 2)
            cout << "| --------------------------- |\n";
    }
}

bool sudoku::solve(vector<int> cells, int c) {

    if (c == cells.size())
        return true;

    // optimization by using the cells with the lowest amount of tries first
    vector<int> lowest_vv;
    int lowest_vv_i = c; // holds grid index with lowest amount of potential numbers
    int min = 9;
    for (int i = 0; i < cells.size(); i++) {
        lowest_vv = valid_values(cells[i] / 9, cells[i] % 9);
        if (lowest_vv.size() < min && game[cells[i] / 9][cells[i] % 9] == 0) {
            min = lowest_vv.size();
            lowest_vv_i = i;
        }
    }

    vector<int> values;
    int row = cells[lowest_vv_i] / 9; // retrieving row
    int col = cells[lowest_vv_i] % 9; // retrieving column
    values = valid_values(row, col);

    if (values.size() == 0) return false;
    vector<int>::iterator it = values.begin();
    while (it != values.end()) {
        game[row][col] = *it;
        if (solve(cells, c + 1))
            return true;
        it++;
    }

    game[row][col] = 0;
    return false;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    if ((argc != 3) ||
        (strcmp(argv[1], "-s") != 0) ||
        strstr(argv[argc - 1], ".txt") == NULL) {
        cerr << "usage: Sudoku -s game.txt\n";
        exit(0);
    }

    omega timer;
    sudoku sudoku_game;

    float T0 = 0.0;
    float T1 = 0.0;

    if (strcmp(argv[1], "-s") == 0) {
        sudoku_game.read(argv[2]);

        T0 = timer.get_sec();
        sudoku_game.solve();
        T1 = timer.get_sec();

        sudoku_game.write(argv[2], "solved");
    }

    string elapsed_time = elapsed(T1 - T0);
    cout << elapsed_time << " msec (" << setprecision(6) << fixed << T1 - T0 << " sec )\n"; 
    // print string elapsed_time using two decimal places
    // print T1-T0 using six decimal places for comparison
}
