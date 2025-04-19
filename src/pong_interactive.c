#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 25
#define PADDLE_SIZE 3
#define BALL_DELAY 50000 // Задержка в микросекундах (50 мс)

typedef struct { int x, y; } Ball;
typedef struct { int y; } Paddle;
typedef struct { int player1, player2; } Score;

// Инициализация игры
void init_game(Ball *ball, Paddle *left, Paddle *right, Score *score) {
    ball->x = WIDTH / 2;
    ball->y = HEIGHT / 2;
    left->y = right->y = HEIGHT / 2;
    score->player1 = score->player2 = 0;
    srand(time(NULL));
}

// Отрисовка игрового поля
void draw_game(Ball ball, Paddle left, Paddle right, Score score) {
    clear();
    // Границы поля
    for (int y = 0; y < HEIGHT; y++) {
        mvprintw(y, 0, "|");
        mvprintw(y, WIDTH - 1, "|");
    }
    for (int x = 0; x < WIDTH; x++) {
        mvprintw(0, x, "-");
        mvprintw(HEIGHT - 1, x, "-");
    }
    // Ракетки и мяч
    for (int i = 0; i < PADDLE_SIZE; i++) {
        mvprintw(left.y + i, 1, "[");
        mvprintw(right.y + i, WIDTH - 2, "]");
    }
    mvprintw(ball.y, ball.x, "O");
    // Счёт
    mvprintw(HEIGHT, 0, "Score: %d - %d", score.player1, score.player2);
    refresh();
}

int main() {
    // Инициализация ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); // Неблокирующий ввод

    Ball ball;
    Paddle left, right;
    Score score;
    int dx = 1, dy = 1; // Направление мяча
    bool game_over = false;

    init_game(&ball, &left, &right, &score);

    while (!game_over) {
        // Обработка ввода
        int input = getch();
        if (input == 'a' && left.y > 1) left.y--;
        if (input == 'z' && left.y < HEIGHT - PADDLE_SIZE - 1) left.y++;
        if (input == 'k' && right.y > 1) right.y--;
        if (input == 'm' && right.y < HEIGHT - PADDLE_SIZE - 1) right.y++;

        // Движение мяча
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
            ball.x = WIDTH / 2;
            ball.y = HEIGHT / 2;
            dx = (rand() % 2) ? 1 : -1; // Случайное направление
        }
        if (ball.x >= WIDTH - 1) {
            score.player1++;
            ball.x = WIDTH / 2;
            ball.y = HEIGHT / 2;
            dx = (rand() % 2) ? 1 : -1;
        }

        // Проверка победы
        if (score.player1 >= 21 || score.player2 >= 21) game_over = true;

        draw_game(ball, left, right, score);
        usleep(BALL_DELAY); // Задержка для плавности
    }

    // Завершение игры
    endwin();
    printf("Player %d wins!\n", (score.player1 > score.player2) ? 1 : 2);
    return 0;
}
