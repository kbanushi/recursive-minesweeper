#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> gameBoard;//2D vector to represent width and height of board

//State of cells
const int UNFLAGGED_MINE = -1; //The cell is mine that has not been flagged yet
const int FLAGGED_MINE = -2; //The cell has been correctly flagged as a mine
const int INCORRECT_FLAGGED_MINE = -3; //The cell has been incorrectly flagged as a mine
const int UNKNOWN = -4; //The initial state of the cell.

int numMines = 10; //Default is 10, but the user can increase based on difficulty

int numCorrectlyFlaggedMines = 0; //If the user correctly flags all mines then they win
int numIncorrectlyFlaggedMines = 0;
bool explode = false;

void initField(); //Function used to set all cells to unknown, then randomly set mines
void setMines(); //Function used to randomly set mines
string getTag(int, int); //When called this will return the correct character for the cell based on its state
void displayBoard(); //Displays the board using getTag
bool isMine(int, int); //Will return true or falsse if cell is mine
void reveal(int, int); //Will reveal the adjacent # of mines, plus recursively reveals more mines if # is 0
void executeCmd(string cmd); //Breaks up user input to commands, then calls appropriate function
void flagCell(int, int); //Will flag the cell and update global variables
void undoCommand(int, int); //Will undo flag, if the flag was correct then game ends

//Initializes field and sets mines. Takes no parameters and returns nothing.
void initField(){
    for (int i = 0; i < gameBoard.size(); i++){
        for (int j = 0; j < gameBoard[i].size(); j++){
            gameBoard[i][j] = UNKNOWN;
        }
    }
    setMines();
}

//Is used to randomly set mines, makes sure no cell is repeated.  Takes no parameters and returns nothing
void setMines(){
    int count = 0;
    int j,k;

    while (count < numMines){
        j = rand() % gameBoard.size();
        k = rand() % gameBoard.size();

        if (gameBoard[j][k] != UNFLAGGED_MINE){
            gameBoard[j][k] = UNFLAGGED_MINE;
            count++;
        }
    }
}

//Returns char based on what the cell state is, takes in coordinates as parameters and returns string.
string getTag(int x, int y){  //Parameters are the coordinates of the cell
    string tag;
    switch (gameBoard[x][y]){
        case UNKNOWN:
            tag = "-";
            break;
        case UNFLAGGED_MINE:
            if (explode)
                tag = "*";
            else
                tag = "-";
            break;
        case FLAGGED_MINE:
            tag = "!";
            break;
        case INCORRECT_FLAGGED_MINE:
            if (explode)
                tag = "&";
            else
                tag = "!";
            break;
        default:
            tag = to_string(gameBoard[x][y]);

    }
    return tag;
}

//Will display the entire board using getTag.  Takes no parameters and returns nothing
void displayBoard(){
    cout << "   ";
    for (int i = 0; i < gameBoard.at(0).size(); i++){ //Loop to display column numbers
        cout << i + 1 << " ";
    }
    cout << endl;

    for (int i = 0; i < gameBoard.size(); i++){ // Begin displaying the status of each cell in order.
        cout << i + 1 << "  ";
        for (int j = 0; j < gameBoard.at(i).size(); j++){
            cout << getTag(i,j) << " ";
        }
        cout << endl;
    }
}

//Will check if the given cell was a mine or not.  Takes coordinates as input, returns true or false.
bool isMine(int x, int y){
    if (gameBoard[x][y] == FLAGGED_MINE || gameBoard[x][y] == UNFLAGGED_MINE)
        return true;
    return false;
}

//Finds the adjacent number of mines by checking each adjacent cell's status. Takes coordinates as parameters, returns number
// of adjacent mines.
int findNumAdjacentMines(int x, int y){
    int count = 0;
    for (int i = -1; i <= 1; i++){ //Loop used to go through each row
        if (x - i == -1 || x - i == gameBoard.size()) //Verify we do not check off of the board
            continue;
        for (int j = -1; j <= 1; j++){ //Loop used to go through each column
            if (y - j == -1 || y - j == gameBoard.at(x - i).size()) //Verify we do not check off of the board
                continue;
            if (isMine(x - i, y - j)){
                count++;
            }
        }
    }
    return count;
}

//This will reveal the adjacent number of mines at a cell.  Takes coordinates as parameter, returns nothing.
void reveal(int x, int y) {
    if (!isMine(x, y)) {
        int numAdjacentMines = findNumAdjacentMines(x, y);
        if (numAdjacentMines == 0) { //If the cell has no adjacent mines, then the program will keep revealing adjacent cells.
            gameBoard[x][y] = numAdjacentMines; //Must set cell or else the recursion will not end.

            for (int i = -1; i <= 1; i++) {
                if (x - i == -1 || x - i == gameBoard.size())
                    continue;
                for (int j = -1; j <= 1; j++) {
                    if (y - j == -1 || y - j == gameBoard.at(x - i).size())
                        continue;
                    if (i != 0 || j != 0) { //After verifying we are not off of the board, we must also verify we do not check
                                            // the current cell or else we could get stuck in recursion.
                        if (gameBoard[x - i][y - j] < 0) {
                            reveal(x - i, y - j);
                        }
                    }
                }
            }
        }
        gameBoard[x][y] = numAdjacentMines; //This will reveal the cells that have an adjacent value > 0
    }
    else{
        explode = true;
    }
}

//Will flag the cell if it has not already been flagged.  Takes coordinates as parameters and returns nothing.
void flagCell(int x, int y){
    if (gameBoard[x][y] < 0){
        if (isMine(x,y) && (gameBoard[x][y] != FLAGGED_MINE)){ //Verify the cell was not already flagged
            gameBoard[x][y] = FLAGGED_MINE;
            numCorrectlyFlaggedMines++;
        }
        else if (!isMine(x,y) && (gameBoard[x][y] != INCORRECT_FLAGGED_MINE)){ //Verify the cell was not already flagged
            gameBoard[x][y] = INCORRECT_FLAGGED_MINE;
            numIncorrectlyFlaggedMines++;
        }
    }
}

//Will undo a flag on a cell.  If the cell was correctly flagged then the player loses.  Takes coordinate input and returns
// nothing.
void undoCommand(int x, int y){
    if (gameBoard[x][y] == FLAGGED_MINE){
        gameBoard[x][y] = UNFLAGGED_MINE;
        numCorrectlyFlaggedMines--;
        explode = true;
    }
    else if (gameBoard[x][y] == INCORRECT_FLAGGED_MINE){
        gameBoard[x][y] = UNKNOWN;
        numIncorrectlyFlaggedMines--;
    }
}

//Used to interpret and execute players commands. Takes in string command and returns nothing.
void executeCmd(string cmd){
    char action = cmd.at(0);
    int x = cmd.at(1) - '0' - 1;
    int y = cmd.at(2) - '0' - 1;

    switch (action) {
        case 'r':
            reveal(x,y);
            break;
        case 'f':
            flagCell(x,y);
            break;
        case 'u':
            undoCommand(x,y);
            break;
    }
}

//main function
int main() {
    int width = 0,height = 0;
    string cmd;

    cout << "Enter the height of the board: ";
    cin >> height;
    cout << "Enter the width of the board: ";
    cin >> width;
    cout << "Enter number of mines you would like to play with (Default is 10): ";
    cin >> numMines;

    //Resize the gameboard based on the user input.  gameboard.resize is the height, then resizing each element lets us resize
    // the width.
    gameBoard.resize(height);
    for (int i = 0; i < gameBoard.size(); i++){
        gameBoard[i].resize(width);
    }
    initField();

    //Game continues until player loses or wins.
    while (!explode && (numCorrectlyFlaggedMines < numMines || numIncorrectlyFlaggedMines > 0)){
        displayBoard();
        srand(0); //Here we are setting the seed of the rand command.  This is just to make testing easier.  An actual random
                    // solution would use the time to set the random seed.
        cout << endl << "Enter command: ";
        cin >> cmd;
        executeCmd(cmd);

        cout << "Number of mines left: " << numMines - (numIncorrectlyFlaggedMines + numCorrectlyFlaggedMines) << endl;
    }
    displayBoard();
    if (explode)
        cout << "Sorry, you lose";
    if (!explode)
        cout << "Nice, you win!";

    return 0;
}
