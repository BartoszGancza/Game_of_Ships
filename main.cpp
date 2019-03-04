#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

default_random_engine generator; // generator is being seeded with time since epoch at every roll of the dice
uniform_int_distribution<int> direction(1,4);
uniform_int_distribution<int> coordinate(0,9);

vector<vector<int>> enemyBoard(10, vector<int> (10, 0));
vector<vector<int>> playerBoard(10, vector<int> (10, 0));

static int enemyHits, playerHits;

enum Direction {up = 1, down = 2, left = 3, right = 4};
enum FieldState {water, ship, hit, miss, attacked};
enum Turn {player, enemy};

class Ship {
public:
    Ship();
    int shipDirection; // 1 - up, 2 - down, 3 - left, 4 - right
};

Ship::Ship() {
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

// set up ships on boards randomly
void SetUpBoards(vector<vector<int>> &board, vector<Ship> &shipsArray) {
    int directionRoll = 0; // decides the direction the ship is going to face
    int x = 0, y = 0; // coordinates on the board
    int numberOfShips = int(shipsArray.size()); // easily change the number of ships on the board by only changing initializer in main()
    
    for (int i = 1; i<=numberOfShips; i++) {

        bool shipSet, overlap;

        do {
            RollTheDice(directionRoll, x, y);
            shipsArray[i-1].shipDirection = directionRoll;
            shipSet = false; // reset the values at every run of the loop
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
                        if (!overlap) { // if nothing overlaps, proceed to set up the ship
                            for (int j = 0; j < i; j++) {
                                board[x][y - j] = ship;
                            }
                            shipSet = true; // ship is set, ready to sail, proceed to the next loop run (next ship)
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
                            for (int j = 0; j < i; j++) {
                                board[x][y + j] = ship;
                            }
                            shipSet = true;
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
                            for (int j = 0; j < i; j++) {
                                board[x - j][y] = ship;
                            }
                            shipSet = true;
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
                            for (int j = 0; j < i; j++) {
                                board[x + j][y] = ship;
                            }
                            shipSet = true;
                        }
                    }
                    break;
            }
        } while(!shipSet || overlap);
    }
}

// prints the boards out in a visual format
void PrintBoards() {
    stringstream boards;
    string toPrint;

    boards << "      Player Board                  Enemy Board" << endl;
    boards << "   1 2 3 4 5 6 7 8 9 10         1 2 3 4 5 6 7 8 9 10" << endl;
    for (int i = 0; i < 10; i++) {
        if (i == 9) {
            boards << i + 1 << " ";
        } else {
            boards << i + 1 << "  ";
        }
        for (int j = 0; j < 10; j++) {
            if (playerBoard[i][j] == ship) {
                boards << "S ";
            } else if (playerBoard[i][j] == hit){
                boards << "X ";
            } else if (playerBoard[i][j] == miss) {
                boards << "* ";
            } else {
                boards << "~ ";
            }
        }
        boards << "      ";
        if (i == 9) {
            boards << i + 1 << " ";
        } else {
            boards << i + 1 << "  ";
        }
        for (int n = 0; n < 10; n++) {
            if (enemyBoard[i][n] == ship) {
                boards << "S ";
            } else if (enemyBoard[i][n] == hit){
                boards << "X ";
            } else if (enemyBoard[i][n] == miss) {
                boards << "* ";
            } else {
                boards << "~ ";
            }
        }
        boards << endl;
    }
    boards << endl;

    toPrint = boards.str();
    cout << toPrint;
}

// evaluates the effect of an attack
int Attack(vector<vector<int>> &board, int &x, int &y) {
    if (board[x][y] == ship) {
        return hit;
    } else if (board[x][y] == water) {
        return miss;
    } else if (board[x][y] == hit || board[x][y] == miss) {
        return attacked;
    } else {
        return 5; // this should absolutely never happen - it's here to disable the end of non-void function warning
    }
}

// "Main logic" of the game
void Turn(int whoseTurn) {
    int x = 0;
    int y = 0;
    int attackEffect = 0;

    int *fieldAddress = nullptr;

    // depending on who's attacking, make a random roll or let the player choose the field
    if (whoseTurn == enemy) {
        RollAttackCords(x, y);
        attackEffect = Attack(playerBoard, x, y); // checks the field status and returns an attack effect
        fieldAddress = &playerBoard[x][y]; // sets the pointer to appropriate field on player board
    } else if (whoseTurn == player) {
        cout << "Input attack coordinates (Y then X separated by space):";
        cin >> x >> y;
        cout << endl;
        x -= 1;
        y -= 1;
        attackEffect = Attack(enemyBoard, x, y); // checks the field status and returns an attack effect
        fieldAddress = &enemyBoard[x][y]; // sets the pointer to appropriate field on enemy board
    }

    if (attackEffect == attacked) { // if that field already attacked before
        Turn(whoseTurn); // recursively call the function until a field not attacked before is chosen
    } else if (attackEffect == hit) { // if it's a hit
        *fieldAddress = hit; // set the field to a hit status
        (whoseTurn == player ? playerHits += 1 : enemyHits += 1); // add 1 to the appropriate counter
        cout << (whoseTurn == player ? "You" : "Enemy") << " hit in the " << x + 1 << "," << y + 1 << " field." << endl;
        cout << endl;
    } else if (attackEffect == miss) { // if it's a miss
        *fieldAddress = miss; // set the field to a miss status
        cout << (whoseTurn == player ? "You" : "Enemy") << " missed in the " << x + 1 << "," << y + 1 << " field." << endl;
        cout << endl;
    }
}

int main(int argc, char **argv)
{
    enemyHits = 0, playerHits = 0;
    vector<Ship> enemyShips(4);
    vector<Ship> playerShips(4);
    SetUpBoards(enemyBoard, enemyShips);
    SetUpBoards(playerBoard, playerShips);
    //cout << "\x1B[2J\x1B[H";
    do {
        PrintBoards();
        //cout << "\x1B[2J\x1B[H";
        Turn(player);
        Turn(enemy);
    } while (playerHits != 10 && enemyHits != 10);

    return 0;
}
