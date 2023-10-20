#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

// Enumeration for the status of a line
enum LineStatus {
    LINE_EMPTY = 0,
    LINE_RED = 1,
    LINE_BLUE = 2
};

// The board records the colors of the lines
typedef char LineBoard[15];
typedef char Player; // A player should be LINE_RED or LINE_BLUE

// Function to check if the board is full
int isBoardFull(LineBoard board) {
    for (int i = 0; i < 15; i++) {
        if (board[i] == LINE_EMPTY) {
            return 0;
        }
    }
    return 1;
}

// Initialize the board state
void initializeBoard(LineBoard board) {
    for (int i = 0; i < 15; ++i) {
        board[i] = LINE_EMPTY;
    }
}

// Function to check if the given player (Player A) has won (1) or should continue playing the game (0)
// by checking the other player's (Player B) game status.
int hasPlayerLost(LineBoard board, Player player) {
    int triangles[20][3] = {
        {0, 1, 5}, {0, 6, 2}, {0, 7, 3}, {0, 8, 4}, {1, 9, 2},
        {1, 10, 3}, {1, 11, 4}, {2, 12, 3}, {2, 13, 4}, {3, 14, 4},
        {1, 9, 6}, {1, 10, 7}, {1, 11, 8}, {2, 12, 7}, {2, 13, 8},
        {3, 14, 8}, {9, 12, 10}, {9, 13, 11}, {10, 14, 11}, {12, 14, 13}
    };

    // Check each triangle
    for (int i = 0; i < 20; i++) {
        int edge1 = triangles[i][0];
        int edge2 = triangles[i][1];
        int edge3 = triangles[i][2];

        // Check if all three edges in the triangle have the same color (player)
        if (board[edge1] == player && board[edge2] == player && board[edge3] == player) {
            return 1; // Player has won
        }
    }

    return 0; // Player has not won
}

// Structure to represent a move
typedef struct {
    int line;   // Line number (0 for 12, 1 for 13, ..., 14 for 56)
    int score;  // -1 for loss, 1 for win
    int depth;  // Depth
} Move;

// Function to determine the other player
Player getOtherPlayer(Player player) {
    switch (player) {
    case LINE_BLUE:
        return LINE_RED;
    case LINE_RED:
        return LINE_BLUE;
    default:
        assert(0); // Invalid player
    }
}

// Function to encode a move into a 16-bit value
uint16_t encodeMove(Move move) {
    uint16_t encodedMove = 0;
    encodedMove |= move.line & 0xF;             // First 4 bits for line
    encodedMove |= (move.score == 1) ? (1 << 4) : 0;  // 5th bit for win
    encodedMove |= (move.score == -1) ? (1 << 5) : 0; // 6th bit for loss
    encodedMove |= (move.depth & 0xF) << 6;       // 7th to 10th bits for depth

    return encodedMove;
}

// Function to decode a 16-bit value into a move
Move decodeMove(uint16_t encodedMove) {
    Move move;

    move.line = encodedMove & 0xF;
    move.score = ((encodedMove >> 4) & 1) ? 1 : -1;
    move.depth = (encodedMove >> 6) & 0xF;

    return move;
}

// Maximum number of board states (3^15)
#define MAX_BOARD_STATES (43046721)

// Array to store computed values (for memoization)
uint16_t computed[MAX_BOARD_STATES + 1];

// Function to compute a unique index for the board state
int computeBoardStateIndex(LineBoard board) {
    int index = 0;
    int powerOf3 = 1;

    for (int i = 0; i < 15; i++) {
        int value = board[i];
        index += value * powerOf3;
        powerOf3 *= 3;
    }

    return index;
}

// Define the bestMove function based on your game's logic
Move bestMove(LineBoard board, Player player, int depth) {
    int boardStateIndex = computeBoardStateIndex(board);
    
    if (computed[boardStateIndex]) {
        return decodeMove(computed[boardStateIndex]);
    }

    Move response;
    Move candidate;
    int noCandidate = 1;

    assert(!isBoardFull(board));
    assert(!hasPlayerLost(board, player));
    assert(!hasPlayerLost(board, getOtherPlayer(player)));
    
    int maxDepthLoss = -1;

    for (int i = 0; i < 15; i++) {
        if (board[i] == LINE_EMPTY) {
            board[i] = player;

            if (isBoardFull(board)) {
                Move move = {.line = i, .score = -1, .depth = depth};
                computed[0] = encodeMove(move);
                board[i] = LINE_EMPTY;
                return move;
            }

            if (!hasPlayerLost(board, player)) {
                response = bestMove(board, getOtherPlayer(player), depth + 1);

                if (response.score == -1) {
                    Move move = {.line = i, .score = 1, .depth = depth};
                    computed[boardStateIndex] = encodeMove(move);
                    board[i] = LINE_EMPTY;
                    return move;
                } else if (response.depth > maxDepthLoss) {
                    board[i] = LINE_EMPTY;
                    maxDepthLoss = response.depth;
                    candidate = (Move) {.line = i, .score = -1, .depth = response.depth};
                    noCandidate = 0;
                }
            }
            board[i] = LINE_EMPTY;
        }
    }
    
    if (noCandidate) {
        for (int i = 0; i < 15; i++) {
            if (board[i] == LINE_EMPTY) {
                Move move = {.line = i, .score = -1, .depth = depth};
                computed[boardStateIndex] = encodeMove(move);
                return move;
            }
        }
    }

    computed[boardStateIndex] = encodeMove(candidate);
    return candidate;
}

// Function to print the board
void printBoard(LineBoard board) {
    char symbolMap[] = {'.', 'R', 'B'};

    for (int i = 0; i < 15; ++i) {
        char symbol = symbolMap[board[i]];
        printf("%c  ", symbol);
    }
    printf("\n");
}

// Function to print the key and accept user input for a move
int printKey(void) {
    int playerMove;
    printf("(1,2) -> 0, (1,3) -> 1, (1,4) -> 2,\n(1,5) -> 3, (1,6) -> 4, (2,3) -> 5,\n(2,4) -> 6, (2,5) -> 7, (2,6) -> 8,\n(3,4) -> 9, (3,5) -> 10, (3,6) -> 11,\n(4,5) -> 12, (4,6) -> 13, (5,6) -> 14\n");
    printf("Enter your move: ");
    scanf("%d", &playerMove);
    return playerMove;
}

int main() {
    // Define the variables and/or constants
    LineBoard board;
    Player currentPlayer;

    // Initialize the board
    initializeBoard(board);

    // Allow the player to choose their color
    char playerColor;
    printf("Enter 'B' for Blue (P1), 'R' for Red (P2)): ");
    scanf(" %c", &playerColor);
    assert(playerColor == 'R' || playerColor == 'B');

    if (playerColor == 'R') {
        // The computer plays first if the player chooses Red
        currentPlayer = LINE_BLUE;
    
        while (1) {
            // Check for a win
            if (hasPlayerLost(board, currentPlayer)) {
                printf("%c Won!", (currentPlayer == LINE_RED) ? 'B' : 'R');
                break;
            }

            // Display the current board
            printf("Current Board:\n");
            printBoard(board);

            if (currentPlayer == LINE_RED) {
                // Player's turn
                int playerMove = printKey();
                if (board[playerMove] == LINE_EMPTY) {
                    board[playerMove] = currentPlayer;
                } else {
                    printf("Invalid move. Line already colored.\n");
                    continue;
                }
            } else {
                // Computer's turn
                Move move = bestMove(board, currentPlayer, 0);
                if (board[move.line] == LINE_EMPTY) {
                    board[move.line] = currentPlayer;
                    printf("Computer chose %d\n", move.line);
                }
            }

            // Check for a win
            if (hasPlayerLost(board, currentPlayer)) {
                printf("%c Won!", (currentPlayer == LINE_RED) ? 'B' : 'R');
                break;
            }

            // Switch to the other player
            currentPlayer = getOtherPlayer(currentPlayer);
        }

    } else {
        // The player plays first if the player chooses Blue
        currentPlayer = LINE_BLUE;
    
        while (1) {
            // Check for a win
            if (hasPlayerLost(board, currentPlayer)) {
                printf("%c Won!", (currentPlayer == LINE_RED) ? 'B' : 'R');
                break;
            }

            // Display the current board
            printf("Current Board:\n");
            printBoard(board);

            if (currentPlayer == LINE_BLUE) {
                // Player's turn
                int playerMove = printKey();
                if (board[playerMove] == LINE_EMPTY) {
                    board[playerMove] = currentPlayer;
                } else {
                    printf("Invalid move. Line already colored.\n");
                    continue;
                }
            } else {
                // Computer's turn
                Move move = bestMove(board, currentPlayer, 0);
                if (board[move.line] == LINE_EMPTY) {
                    board[move.line] = currentPlayer;
                    printf("Computer chose %d\n", move.line);
                }
            }

            // Check for a win
            if (hasPlayerLost(board, currentPlayer)) {
                printf("%c Won!\n", (currentPlayer == LINE_RED) ? 'B' : 'R');
                break;
            }

            // Switch to the other player
            currentPlayer = getOtherPlayer(currentPlayer);
        }
    }
    return 0;
}
