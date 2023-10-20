#include <assert.h>
#include <stdint.h>
#include <stdio.h>


enum {
    EMPTY = 0,
    R,
    B,
};

typedef char board_t[4][5];
typedef char player_t;

int has_won(board_t board, player_t player)
{

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 2; col++) {
            if (board[row][col] == player && board[row][col + 1] == player &&
                board[row][col + 2] == player && board[row][col + 3] == player) {
                return 1;
            }
        }
    }


    for (int row = 0; row < 1; row++) {
        for (int col = 0; col < 5; col++) {
            if (board[row][col] == player && board[row + 1][col] == player &&
                board[row + 2][col] == player && board[row + 3][col] == player) {
                return 1;
            }
        }
    }


    for (int row = 0; row < 1; row++) {
        for (int col = 0; col < 2; col++) {
            if (board[row][col] == player && board[row + 1][col + 1] == player &&
                board[row + 2][col + 2] == player && board[row + 3][col + 3] == player) {
                return 1;
            }
        }
    }


    for (int row = 3; row >= 2; row--) {
        for (int col = 0; col < 2; col++) {
            if (board[row][col] == player && board[row - 1][col + 1] == player &&
                board[row - 2][col + 2] == player && board[row - 3][col + 3] == player) {
                return 1;
            }
        }
    }

    return 0;
}


int is_full(board_t board)
{
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 5; col++) {
            if (board[row][col] == '.') {
                return 0;
            }
        }
    }
    return 1;
}

typedef struct {
    int col;
    int score;
} move_t;

player_t other_player(player_t player)
{
    return (player == 'R') ? 'B' : 'R';
}


// Max possible position
#define MAX_ORD (28629151)

// Array for optimization
uint8_t computed_moves[MAX_ORD+1];

// Storing move in one byte
uint8_t encode_move(move_t m)
{   // first 3 bit for column
    assert(0 <= m.col && m.col <= 4);
    // next 3 bit for win draw and loss
    uint8_t b = m.col;
    b |= (m.score << 3);

    return b;
}


// Decoded the move
move_t decode_move(uint8_t b)
{
    move_t m;
    m.col = (b & 0x7); // extract the column
    // extracting the score
    if (b & 010) m.score = 1;
    if (b & 020) m.score = 0;
    if (b & 040) m.score = -1;
    return m;
}

// Calculating the order of board
int ord(board_t board)
{
    int o = 0;
    int p2 = 1;

    for (int col = 0; col < 5; ++col) {
        int i = 0;

        for (int row = 3; row >= 0; row--) {
            int d = 0;

            if (board[row][col] == 'R') {
                d = 1;
            } else if (board[row][col] == 'B') {
                d = 2;
            }

            i += d << (3 - row); // Equivalent to i += d * 2^(3-row)
        }

        o += i * p2;
        p2 *= 30;
    }

    return o;
}


move_t best_move(board_t board, player_t player)
{
    // If move is already computed return that
    int o = ord(board);
    if (computed_moves[o]) {return decode_move(computed_moves[o]);}

    // Asserting board is empty and not won
    assert(!is_full(board));
    assert(!has_won(board, player));
    assert(!has_won(board, other_player(player)));

    // Initializing variables
    move_t response;
    move_t candidate;
    int no_candidate = 1;

// Bottom-most empty spot for each column
    int row[5];
    for (int col = 0; col < 5; col++)
    {
        int r = 3;  // Start from the bottom row
        while (r >= 0 && board[r][col] == '.')
        {
            r--;
        }
        row[col] = r; 
    }

    // Base case for recursion
    for (int col = 0; col < 5; col++)
    {
        if (row[col] == -1) {continue;} // If column is full move to next column

        board[row[col]][col] = player;

        if (has_won(board, player))
        {
            board[row[col]][col] = '.';
            computed_moves[o] = encode_move(candidate = (move_t) {.col = col, .score = 1});
            return candidate;
        }

        if (is_full(board))
        {
            candidate = (move_t) {.col = col, .score = 0};
            computed_moves[o] = encode_move(candidate);
            board[row[col]][col] = '.';
            return candidate;
        }

        response = best_move(board, other_player(player));
        board[row[col]][col] = '.';

        if (response.score == -1)
        {
            computed_moves[o] = encode_move(candidate = (move_t) {.col = col, .score = 1});
            return candidate;
        }

        else if (response.score == 0)
        {
            candidate = (move_t) {.col = col, .score = 0};
            no_candidate = 0;
        }

        else  // response.score = 1
        {
            if (no_candidate)
            {
                candidate = (move_t) {.col = col, .score = -1};
                no_candidate = 0;
            }
        }
    }

    computed_moves[o] = encode_move(candidate);
    return candidate;
}


void print_board(board_t board)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

int main()
{
    // Initialize the board
    board_t board;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            board[i][j] = '.';
        }
    }

    // Ask the user to choose 'R' (red) or 'B' (blue) player
    player_t player;
    printf("Enter 'R' for red (P1) or 'B' for blue (P2) : ");
    scanf(" %c", &player);

    // Initialize variables
    int col;
    move_t response;
    player_t current = 'R';

    while (1)
    {
        // Player's turn
        if (current == player)
        {
            // Get a valid column from the player
            printf("Play disc in column(0-4): ");
            scanf("%i", &col);

            while (board[0][col] != '.')
            {
                printf("Invalid Move!\n");
                printf("Play disc in column(0-4): ");
                scanf("%i", &col);
            }

            // Find the value of row
            int row = 0;
            while ((board[row][col] == '.') && (row < 4))
            {
                row++;
            }
            row--;

            board[row][col] = current;
            print_board(board);
        }
        // Computer's turn
        else
        {
            response = best_move(board, current);

            // Find row
            int r = 0;
            while ((board[r][response.col] == '.') && (r < 4))
            {
                r++;
            }
            r--;

            board[r][response.col] = current;
            print_board(board);
            printf("Computer played %i\nComputer Score %i\n", response.col, response.score);
        }

        // Check if the current player has won
        if (has_won(board, current))
        {
            if (current == player)
            {
                printf("You won!");
            }
            else
            {
                printf("Computer won!");
            }
            break;
        }
        // Check if the board is full without anyone winning
        else if (is_full(board))
        {
            printf("Draw.\n");
            break;
        }

        // Give the chance to the other player
        current = other_player(current);
        printf("_____________\n");
    }

    return 0;
}

