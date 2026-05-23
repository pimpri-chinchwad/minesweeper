#include <iostream>
#include <cstdlib>
#include <ctime>


class InvalidMoveException {
private:
    std::string message;
public:
    InvalidMoveException(std::string msg) : message(msg) {}
    std::string what() const { return message; }
};

class Cell {
private:
    bool isMine;
    bool isRevealed;
    int adjacentMines;

public:
    Cell() : isMine(false), isRevealed(false), adjacentMines(0) {}

    // Getters and Setters (Encapsulation)
    void setMine() { isMine = true; }
    bool getMine() const { return isMine; }
   
    void reveal() { isRevealed = true; }
    bool getRevealed() const { return isRevealed; }
   
    void setAdjacentMines(int count) { adjacentMines = count; }
    int getAdjacentMines() const { return adjacentMines; }

    friend void cheatMode(Cell grid[9][9]);
};

// Friendly function that can bypass private variables to show mine locations
void cheatMode(Cell grid[9][9]) {
    std::cout << "\n[CHEAT MODE ACTIVE - MINE LOCATIONS]:\n  ";
    for (int j = 0; j < 9; j++) std::cout << j << " ";
    std::cout << "\n";
    for (int i = 0; i < 9; i++) {
        std::cout << i << " ";
        for (int j = 0; j < 9; j++) {
            if (grid[i][j].getMine()) std::cout << "X ";
            else std::cout << ". ";
        }
        std::cout << "\n";
    }
    std::cout << "------------------------------------\n";
}


class GameBoard {
protected:
    Cell grid[9][9];
    int totalMines;

public:
    GameBoard() : totalMines(10) {}
   
    // Pure Virtual Function making this an Abstract Class
    virtual void initializeBoard() = 0;
   
    virtual ~GameBoard() {}

    // Helper to check if coordinates are inside the 9x9 matrix
    bool isValid(int r, int c) {
        return (r >= 0 && r < 9 && c >= 0 && c < 9);
    }

    void revealCell(int r, int c) {
        if (!isValid(r, c) || grid[r][c].getRevealed()) return;

        grid[r][c].reveal();

        // If the cell has 0 neighboring mines, recursively reveal its 8 neighbors
        if (grid[r][c].getAdjacentMines() == 0 && !grid[r][c].getMine()) {
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    revealCell(r + i, c + j); // Recursive Call
                }
            }
        }
    }

    bool hitMine(int r, int c) {
        return grid[r][c].getMine();
    }

    bool checkWin() {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                // If there is a safe cell still hidden, player hasn't won yet
                if (!grid[i][j].getMine() && !grid[i][j].getRevealed()) {
                    return false;
                }
            }
        }
        return true;
    }

    void displayBoard() {
        std::cout << "\n  ";
        for (int j = 0; j < 9; j++) std::cout << j << " ";
        std::cout << "\n";

        for (int i = 0; i < 9; i++) {
            std::cout << i << " ";
            for (int j = 0; j < 9; j++) {
                if (!grid[i][j].getRevealed()) {
                    std::cout << "- ";
                } else if (grid[i][j].getMine()) {
                    std::cout << "* ";
                } else {
                    std::cout << grid[i][j].getAdjacentMines() << " ";
                }
            }
            std::cout << "\n";
        }
    }

    void activateCheat() {
        cheatMode(grid);
    }
};

class ClassicBoard : public GameBoard {
public:
    // Method Overriding
    void initializeBoard() override {
        srand(time(0));
        int placedMines = 0;

        // Randomly place 10 mines
        while (placedMines < totalMines) {
            int r = rand() % 9;
            int c = rand() % 9;
            if (!grid[r][c].getMine()) {
                grid[r][c].setMine();
                placedMines++;
            }
        }

        // Calculate adjacent mine counts for every cell
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                if (grid[r][c].getMine()) continue;

                int count = 0;
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        if (isValid(r + i, c + j) && grid[r + i][c + j].getMine()) {
                            count++;
                        }
                    }
                }
                grid[r][c].setAdjacentMines(count);
            }
        }
    }
};


int main() {
    GameBoard* game = new ClassicBoard();
    game->initializeBoard();

    std::cout << "====================================\n";
    std::cout << "      WELCOME TO  MINESWEEPER     \n";
    std::cout << "====================================\n";
    std::cout << "Enter '-1 -1' at any time to activate Cheat Mode.\n";

    bool gameOver = false;

    while (!gameOver) {
        game->displayBoard();
        int row, col;
        std::cout << "\nEnter Row and Column (0-8) separated by a space: ";
        std::cin >> row >> col;

        // Handle Cheat Mode Trigger
        if (row == -1 && col == -1) {
            game->activateCheat();
            continue;
        }

        
        try {
            if (row < 0 || row > 8 || col < 0 || col > 8) {
                throw InvalidMoveException("Coordinates out of bounds! Must be between 0 and 8.");
            }

            if (game->hitMine(row, col)) {
                std::cout << "\n💥 BOOM! You hit a mine. Game Over! 💥\n";
                // Reveal everything to show the final state
                for(int i=0; i<9; i++) {
                    for(int j=0; j<9; j++) game->revealCell(i,j);
                }
                game->displayBoard();
                gameOver = true;
            } else {
                game->revealCell(row, col);
                if (game->checkWin()) {
                    std::cout << "\n🎉 Congratulations! You cleared the board and won! 🎉\n";
                    game->displayBoard();
                    gameOver = true;
                }
            }
        }
        catch (const InvalidMoveException& e) {
            std::cout << "\n[ERROR]: " << e.what() << " Please try again.\n";
        }
    }

    delete game; // Cleanup allocated memory
    return 0;
}
