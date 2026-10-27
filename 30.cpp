#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <limits>
using namespace std;

class ConnectFour {

public:
    //  her hucrenin kendi bilgisi var
    class Cell {
    private:
        char column;   // sutun harfi (A, B, C...)
        int row;       // satir numarasi
        char state;    // hucrenin durumu

    public:
        // varsayilan hucre
        Cell() : column('A'), row(1), state('.') {}

        // konuma gore hucre
        Cell(char c, int r) : column(c), row(r), state('.') {}

        // konum + durum alan hucre yapici
        Cell(char c, int r, char s) : column(c), row(r), state(s) {}

        // basit getter/setterlar
        char getState() const { return state; }
        void setState(char s) { state = s; }
        char getColumn() const { return column; }
        int getRow() const { return row; }

        // karsılastirma operatorleri
        bool operator==(const Cell& other) const {
            return (column == other.column && row == other.row && state == other.state);
        }
        bool operator!=(const Cell& other) const {
            return !(*this == other);
        }

        // ++ ve -- operatorleri
        Cell& operator++() {
            if (state == '.') state = 'X';
            else if (state == 'X') state = 'O';
            else if (state == 'O') state = 'C';
            else state = '.';
            return *this;
        }
        Cell operator++(int) { Cell t=*this; ++(*this); return t; }
        Cell& operator--() { ++(*this); ++(*this); ++(*this); return *this; }
        Cell operator--(int) { Cell t=*this; --(*this); return t; }

        // hucreyi tek karakter olarak yazdirma
        friend ostream& operator<<(ostream& os, const Cell& c) {
            os << c.state;
            return os;
        }

        // hucreyi inputtan okumak
        friend istream& operator>>(istream& is, Cell& c) {
            is >> c.state;
            return is;
        }
    };

private:
    Cell** board;   // 2D board pointer
    int rows, cols; 
    char gameMode = 'C'; // user vs pc varsayilan

    // verilen sutuna tasi dusecek uygun satir
    int getDropRow(int col_index) const {
        for (int r = rows - 1; r >= 0; r--)
            if (board[r][col_index].getState() == '.')
                return r;
        return -1; // sutun doluysa
    }

    // sutuna tas birakir
    bool dropPiece(char col_char, char piece) {
        int c = toupper(col_char) - 'A';
        if (c < 0 || c >= cols) return false;

        int r = getDropRow(c);
        if (r == -1) return false;

        board[r][c].setState(piece);
        return true;
    }

    // 4 yon icin kazanan kontrolu
    bool checkDir(int r, int c, int dr, int dc) const {
        char s = board[r][c].getState();
        if (s != 'X' && s != 'O') return false;

        for (int k = 1; k < 4; k++) {
            int rr = r + dr * k;
            int cc = c + dc * k;

            // sinir disiysa
            if (rr < 0 || rr >= rows || cc < 0 || cc >= cols)
                return false;

            if (board[rr][cc].getState() != s)
                return false;
        }
        return true;
    }

public:

    int getRows() const { return rows; }
    int getCols() const { return cols; }

    // varsayilan board: 5x5
    ConnectFour() {
        rows = 5;
        cols = 5;
        allocateBoard();
        initializeEmpty();
    }

    // Boyut alarak board olusturma
    ConnectFour(int r, int c) {
        rows = max(3, r); // min 3x3 sarti
        cols = max(3, c);
        allocateBoard();
        initializeEmpty();
    }

    // dosyadan board alan kurucu
    ConnectFour(const string& filename) {
        loadBoardShape(filename);
    }

    // destructor
    ~ConnectFour(){
        for(int i = 0; i < rows; i++) delete[] board[i];
        delete[] board;
    }

    // board icin bellek ayirma
    void allocateBoard() {
        board = new Cell*[rows];
        for (int i = 0; i < rows; i++)
            board[i] = new Cell[cols];
    }

    // tum hucreleri bos yapma
    void initializeEmpty() {
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++) {
                board[i][j] = Cell('A' + j, i + 1);
                board[i][j].setState('.');
            }
    }

  
    //filedan board okuma
    void loadBoardShape(const string& filename) {

        ifstream file(filename);
        if (!file) {
            // dosya yoksa default 5x5 ac
            rows = 5; cols = 5;
            allocateBoard();
            initializeEmpty();
            return;
        }

        const int MAX = 500;
        string* lines = new string[MAX];
        int count = 0;

        // dosyadaki satirlari tek tek aliyoruz
        string line;
        while (getline(file, line) && count < MAX)
            lines[count++] = line;
        file.close();

        rows = count;

        // en uzun satiri bulup kolon sayisi olarak aliyoruz
        int maxC = 0;
        for (int i = 0; i < rows; i++)
            maxC = max(maxC, (int)lines[i].size());

        cols = maxC;

        // board cok kucukse oynanamaz , default'a don
        if (rows < 3 || cols < 3) {
            delete[] lines;
            rows = 5; cols = 5;
            allocateBoard();
            initializeEmpty();
            return;
        }

        allocateBoard();

        // dosyadaki karakterleri boarda yerlestiriyoruz
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                board[r][c] = Cell('A' + c, r + 1);

                if (c >= (int)lines[r].size()) {
                    // satir kisa ise bosluk
                    board[r][c].setState(' ');
                    continue;
                }

                char ch = lines[r][c];

                // uygun karakterse statee koyuyoruz
                if (ch == '*' || ch == '.')
                    board[r][c].setState('.');
                else if (ch == 'X' || ch == 'O')
                    board[r][c].setState(ch);
                else
                    board[r][c].setState(' ');
            }
        }

        delete[] lines;
    }

    void setGameMode(char m) { gameMode = toupper(m); }

    // boardi ekrana yazdirma
    void printBoard() const {
        cout << "  ";
        for (int j = 0; j < cols; j++)
            cout << (char)('A' + j);
        cout << "\n";

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++)
                cout << board[i][j];
            cout << "\n";
        }
    }

    // user move
    bool play(char col) {
        return dropPiece(col, 'O');
    }

    // computer move (rastgele sutun)
    bool play() {
        for (int t = 0; t < 20; t++) {
            int c = rand() % cols;
            if (dropPiece('A' + c, 'X'))
                return true;
        }
        return false;
    }

    // oyun bitti mi kontrolu
    bool gameEnded() const {
        // kazanan var mi?
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                if (checkDir(r,c,0,1) || checkDir(r,c,1,0) || checkDir(r,c,1,1) || checkDir(r,c,1,-1))
                    return true;

        // bos yer varsa devam
        for (int c = 0; c < cols; c++)
            if (getDropRow(c) != -1)
                return false;

        // bos yoksa oyun bitti
        return true;
    }

    // S tusu ile dosyaya kaydet
    void quickSave(const string& filename) const {
        ofstream out(filename);

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++)
                out << board[r][c].getState();
            out << "\n";
        }
    }

    // ana oyun dongusu
    void playGame() {
        char currentPlayer = 'O'; // kullanici basliyor

        while (!gameEnded()) {
            printBoard();
            bool moveSuccess = false;

         
            // USER VS USER MODE
           
            if (gameMode == 'U') {
                cout << "Player " << currentPlayer << " move (Column / S: Save / Q: Quit): ";
                char c;
                cin >> c;
                c = toupper(c);

                if (c == 'Q') break;

                if (c == 'S') {
                    quickSave("saved_game.txt");
                    cout << "Game saved!\n";
                    continue;
                }

                if (!dropPiece(c, currentPlayer)) {
                    cout << "Invalid move!\n";
                    continue;
                }

                moveSuccess = true;
            }

           
            // USER VS PC MODE
        
            else {  
                if (currentPlayer == 'O') {
                    cout << "Your move (Column / S: Save / Q: Quit): ";
                    char c;
                    cin >> c;
                    c = toupper(c);

                    if (c == 'Q') break;

                    if (c == 'S') {
                        quickSave("saved_game.txt");
                        cout << "Game saved!\n";
                        continue;
                    }

                    if (!dropPiece(c, 'O')) {
                        cout << "Invalid move!\n";
                        continue;
                    }

                    moveSuccess = true;
                }
                else {
                    // bilgisayar rastgele oynuyor
                    cout << "Computer (X) playing...\n";
                    play();
                    moveSuccess = true;
                }
            }

            // oyuncu degisimi
            if (moveSuccess) {
                if (gameEnded()) {
                    printBoard();
                    cout << "--- GAME FINISHED ---\n";
                    break;
                }
                currentPlayer = (currentPlayer == 'O' ? 'X' : 'O');
            }
        }
    }

    // board karsilastirmalari
    bool operator==(const ConnectFour& other) const {
        if (rows != other.rows || cols != other.cols) return false;
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                if (board[r][c] != other.board[r][c]) return false;
        return true;
    }

    bool operator!=(const ConnectFour& other) const {
        return !(*this == other);
    }
};


int main() {
    srand((unsigned int)time(nullptr)); // random seed

    string f1, f2, f3;
    int r, c;

    cout << "File1: "; cin >> f1;
    cout << "File2: "; cin >> f2;
    cout << "File3: "; cin >> f3;

    cout << "Rows for game2: "; cin >> r; 
    cout << "Cols for game2: "; cin >> c;

    // farkli sekillerde 5 oyun olusturuyoruz
    ConnectFour g1;
    ConnectFour g2(r, c);
    ConnectFour g3(f1);
    ConnectFour g4(f2);
    ConnectFour g5(f3);

    ConnectFour* games[5] = {&g1, &g2, &g3, &g4, &g5};

    for (int i = 0; i < 5; i++) {
        char mode;
        do {
            cout << "Game " << i+1 << " Mode (U/C): ";
            cin >> mode;
            mode = toupper(mode);
        } while (mode != 'U' && mode != 'C');

        games[i]->setGameMode(mode);
        games[i]->playGame();
    }

    return 0;
}
