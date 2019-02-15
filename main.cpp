#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <string>

using namespace std;

default_random_engine generator;
uniform_int_distribution<int> direction(1,4);
uniform_int_distribution<int> coordinate(0,9);

vector<vector<int>> enemyBoard(10, vector<int> (10, 0));
vector<vector<int>> playerBoard(10, vector<int> (10, 0));

int enemyHits = 0;
int playerHits = 0;

enum Direction {up = 1, down = 2, left = 3, right = 4};
enum FieldState {water, ship, hit, miss, attacked};

class Ship {
public:
    Ship();
    int shipSize; // 1 - frigate, 2 - cruiser, 3 - destroyer, 4 - battleship
    int shipDirection; // 1 - up, 2 - down, 3 - left, 4 - right
};

Ship::Ship() {
    shipSize = 0;
    shipDirection = 0;
}

void RollTheDice(int &directionRoll, int &x, int &y) {
    unsigned seed = unsigned(chrono::system_clock::now().time_since_epoch().count());
    generator.seed(seed);
    directionRoll = direction(generator);
    x = coordinate(generator);
    y = coordinate(generator);
}

void RollAttackCords(int &x, int &y) {
    unsigned seed = unsigned(chrono::system_clock::now().time_since_epoch().count());
    generator.seed(seed);
    x = coordinate(generator);
    y = coordinate(generator);
}

void SetUpBoards(vector<vector<int>> &board, vector<Ship> &shipsArray) {
    
    int directionRoll = 0;
    int x = 0;
    int y = 0;
    int numberOfShips = int(shipsArray.size());
    
    for (int i = 1; i<=numberOfShips; i++) {

        shipsArray[i-1].shipSize = i;
        bool shipSet;
        bool overlap;

        do {
            RollTheDice(directionRoll, x, y);
            shipsArray[i-1].shipDirection = directionRoll;
            shipSet = false;
            overlap = false;
            switch (shipsArray[i-1].shipDirection) {
                case Direction::up: // up
                // these first check for bounds, then if there is any ship in the way
                    if ((y-(i-1)) >= 0 && board[x][y] == water) {
                        for (int j = 1; j < i; j++) {
                            if (board[x][y - j] == ship) {
                                overlap = true;
                            }
                        }
                        if (!overlap) {
                            board[x][y] = ship;
                            for (int j = 1; j < i; j++) {
                                board[x][y - j] = ship;
                            }
                            shipSet = true;
                        } else {
                            RollTheDice(directionRoll, x, y);
                            shipsArray[i-1].shipDirection = directionRoll;
                        }
                    }
                    break;
                case Direction::down: // down
                    if ((y+(i-1)) <= 9 && board[x][y] == water) {
                        for (int j = 1; j < i; j++) {
                            if (board[x][y + j] == ship) {
                                overlap = true;
                            }
                        }
                        if (!overlap) {
                            board[x][y] = ship;
                            for (int j = 1; j < i; j++) {
                                board[x][y + j] = ship;
                            }
                            shipSet = true;
                        } else {
                            RollTheDice(directionRoll, x, y);
                            shipsArray[i-1].shipDirection = directionRoll;
                        }
                    }
                    break;
                case Direction::left: // left
                    if ((x-(i-1)) >= 0 && board[x][y] == water) {
                        for (int j = 1; j < i; j++) {
                            if (board[x - j][y] == ship) {
                                overlap = true;
                            }
                        }
                        if (!overlap) {
                            board[x][y] = ship;
                            for (int j = 1; j < i; j++) {
                                board[x - j][y] = ship;
                            }
                            shipSet = true;
                        } else {
                            RollTheDice(directionRoll, x, y);
                            shipsArray[i-1].shipDirection = directionRoll;
                        }
                    }
                    break;
                case Direction::right: // right
                    if ((x+(i-1)) <= 9 && board[x][y] == water) {
                        for (int j = 1; j < i; j++) {
                            if (board[x + j][y] == ship) {
                                overlap = true;
                            }
                        }
                        if (!overlap) {
                            board[x][y] = ship;
                            for (int j = 1; j < i; j++) {
                                board[x + j][y] = ship;
                            }
                            shipSet = true;
                        } else {
                            RollTheDice(directionRoll, x, y);
                            shipsArray[i-1].shipDirection = directionRoll;
                        }
                    }
                    break;
            }
        } while(!shipSet || overlap);
    }
}

void PrintBoards() {
    cout << "   Player Board               Enemy Board" << endl;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (playerBoard[i][j] == ship) {
                cout << "□ ";
            } else if (playerBoard[i][j] == hit){
                cout << "⊠ ";
            } else if (playerBoard[i][j] == miss) {
                cout << "* ";
            } else {
                cout << "~ ";
            }
        }
        cout << "      ";
        for (int n = 0; n < 10; n++) {
            if (enemyBoard[i][n] == ship) {
                cout << "□ ";
            } else if (enemyBoard[i][n] == hit){
                cout << "⊠ ";
            } else if (enemyBoard[i][n] == miss) {
                cout << "* ";
            } else {
                cout << "~ ";
            }
        }
        cout << endl;
    }
}

int Attack(vector<vector<int>> &board, int &x, int &y) {
    if (board[x][y] == ship) {
        return hit;
    } else if (board[x][y] == water) {
        return miss;
    } else if (board[x][y] == hit || board[x][y] == miss) {
        return attacked;
    }
}

void EnemyTurn() {
    int x = 0;
    int y = 0;
    int attackEffect = 0;
    
    RollAttackCords(x, y);
    attackEffect = Attack(playerBoard, x, y);
    
    if (attackEffect == attacked) { // if that field already attacked before
        EnemyTurn();
    } else if (attackEffect == hit) { // if it's a hit
        playerBoard[x][y] = hit;
        enemyHits += 1;
        cout << "Hit in the " << x + 1 << "," << y + 1 << " field." << endl;
    } else if (attackEffect == miss) { // if it's a miss
        playerBoard[x][y] = miss;
        cout << "Miss in the " << x + 1 << "," << y + 1 << " field." << endl;
    }
}

int main(int argc, char **argv)
{
    
    vector<Ship> enemyShips(4);
    vector<Ship> playerShips(4);
    SetUpBoards(enemyBoard, enemyShips);
    SetUpBoards(playerBoard, playerShips);
    string endProgram;
    //cout << "\x1B[2J\x1B[H";
    do {
        PrintBoards();
        cin >> endProgram;
        //cout << "\x1B[2J\x1B[H";
        EnemyTurn();
    } while (endProgram != "t" && enemyHits != 10);
    
}
