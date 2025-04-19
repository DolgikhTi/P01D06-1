#include <stdio.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25
#define PADDLE_SIZE 3

typedef struct { int x, y; } Ball;
typedef struct { int y; } Paddle;
typedef struct { int player1, player2; } Score;

// Настройка терминала для неблокирующего ввода (Linux/macOS)
void set_nonblocking_mode() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag &= ~(ICANON | ECHO);
    ttystate.c_cc[VMIN] = 1; // Ждём 1 символ
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

// Очистка экрана
void clear_screen() {
    printf("\033[H\033[J");
}

// Отрисовка поля
void draw_game(Ball ball, Paddle left, Paddle right, Score score) {
    clear_screen();
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y == 0 || y == HEIGHT - 1) printf("-");
            else if (x == 0 || x == WIDTH - 1) printf("|");
            else if (x == ball.x && y == ball.y) printf("O");
            else if (x == 1 && (y >= left.y && y < left.y + PADDLE_SIZE)) printf("[");
            else if (x == WIDTH - 2 && (y >= right.y && y < right.y + PADDLE_SIZE)) printf("]");
            else printf(" ");
        }
        printf("\n");
    }
    printf("Score: %d - %d\n", score.player1, score.player2);
}

int main() {
    Ball ball = {WIDTH / 2, HEIGHT / 2};
    Paddle left = {HEIGHT / 2}, right = {HEIGHT / 2};
    Score score = {0, 0};
    int dx = 1, dy = 1; // Направление мяча
    bool game_over = false;
    char input;

    set_nonblocking_mode();

    while (!game_over) {
        draw_game(ball, left, right, score);

        // Обработка ввода
        input = getchar();

        // Движение ракеток
        if (input == 'a' && left.y > 1) left.y--;
        if (input == 'z' && left.y < HEIGHT - PADDLE_SIZE - 1) left.y++;
        if (input == 'k' && right.y > 1) right.y--;
        if (input == 'm' && right.y < HEIGHT - PADDLE_SIZE - 1) right.y++;

        // Шаг игры (только при нажатии пробела)
        if (input == ' ') {
            ball.x += dx;
            ball.y += dy;

            // Отражение от стенок
            if (ball.y <= 0 || ball.y >= HEIGHT - 1) dy *= -1;

            // Отражение от ракеток
            if (ball.x == 1 && ball.y >= left.y && ball.y < left.y + PADDLE_SIZE) dx *= -1;
            if (ball.x == WIDTH - 2 && ball.y >= right.y && ball.y < right.y + PADDLE_SIZE) dx *= -1;

            // Гол
            if (ball.x <= 0) {
                score.player2++;
                ball = (Ball){WIDTH / 2, HEIGHT / 2};
            }
            if (ball.x >= WIDTH - 1) {
                score.player1++;
                ball = (Ball){WIDTH / 2, HEIGHT / 2};
            }

            // Проверка победы
            if (score.player1 >= 21 || score.player2 >= 21) game_over = true;
        }
    }

    clear_screen();
    printf("Player %d wins!\n", (score.player1 > score.player2) ? 1 : 2);
    return 0;
}
