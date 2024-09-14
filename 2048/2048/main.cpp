#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <iomanip>
#include <fstream> // 包含文件操作相关函数
#include <random> // 随机数
using namespace std;

void gotoxy(int x, int y);
void HideCursor();
void color(int c);
void enableAnsiSupport();

void wait_for_enter();
void print_menu();
void print_help();
void background();
void print_interface();

void play_game();
void add_random_tile(int board[4][4]);
bool move_tiles(int board[4][4], char direction, int& score);
void update_board(int board[4][4], int score, int step, int high_score);
void move_with_animation(int x1, int y1, char direction, int num);
void double_with_animation(int x1, int y1, char direction, int num);
void high_precision_sleep(float milliseconds);
void display_tile(int x, int y, int num, int back);

int read_high_score();
void write_high_score(int high_score);

int main() {
    HideCursor();
    enableAnsiSupport();

    char choice = '\0';
    SetConsoleTitle(TEXT("2048"));    // 设置控制台标题为2048
    print_menu();    // 调用菜单显示函数

    int select = 0;
    // 主循环
    while (1) {
        color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        gotoxy(16, select + 2); cout << ">";

        // 菜单选项枚举
        enum MenuOptions { CLASSIC_MODE, GAME_RULES, EXIT_GAME, MENU_OPTIONS_COUNT };
        // 根据用户选择进行相应操作
        while (1) {
            choice = _getch();
            if (choice == '\r' or choice == ' ') {       // 回车或空格
                break;
            }
            else if (choice == 72) {    // 上
                gotoxy(16, select + 2); cout << " ";
                select = (select - 1 + MENU_OPTIONS_COUNT) % MENU_OPTIONS_COUNT;
                gotoxy(16, select + 2); cout << ">";
            }
            else if (choice == 80) {    // 下
                gotoxy(16, select + 2); cout << " ";
                select = (select + 1) % MENU_OPTIONS_COUNT;
                gotoxy(16, select + 2); cout << ">";
            }
        }

        switch (select) {
        case CLASSIC_MODE:
            play_game();
            color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            system("CLS");
            print_menu();
            break;
        case GAME_RULES:
            print_help();
            system("CLS");
            print_menu();
            break;
        case EXIT_GAME:
            color(0 | 15);
            system("CLS");
            return 0;
        }
    }
    return 0;
}

//-------标准输出设备句柄设置，除非编译器头文件库有这些函数-------
//在指定坐标输出字符
void gotoxy(int x, int y) {
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
//隐藏光标
void HideCursor() {
    CONSOLE_CURSOR_INFO Cursor;
    Cursor.bVisible = FALSE;
    Cursor.dwSize = sizeof(Cursor); //如果没赋值的话，光标隐藏无效
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Cursor);
}
//设置字符串或单个字符的颜色及背景
void color(int c) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
// ANSI 转义序列
void enableAnsiSupport() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
//------------------------游戏界面------------------------
// 等待回车键
void wait_for_enter() {
    cout << "按回车键退出";
    while (_getch() != '\r');
    cout << "\n\n";
}
// 菜单
void print_menu() {
    color(FOREGROUND_BLUE | FOREGROUND_GREEN);    // 前景文字颜色
    cout << "              ┏━━━━━━━━━━━━┓\n";
    for (int i = 0; i < 4; ++i) cout << "              ┃            ┃\n";
    cout << "              ┗━━━━━━━━━━━━┛\n";
    color(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    gotoxy(17, 1); cout << "  2048  ";
    gotoxy(16, 2); cout << " 经典模式";
    gotoxy(16, 3); cout << " 游戏规则";
    gotoxy(16, 4); cout << " 退出游戏";
}
// 帮助
void print_help() {
    system("CLS");
    color(FOREGROUND_BLUE | FOREGROUND_GREEN);
    cout << "╭─────────────────────────────────────────────╮\n";
    cout << "│                                             │\n";
    cout << "├┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┤\n";
    cout << "│                                             │\n";
    cout << "╞═════════════════════════════════════════════╡\n";
    cout << "│                                             │\n";
    cout << "├┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┤\n";
    for (int i = 0; i < 4; ++i) cout << "│                                             │\n";
    cout << "│ ╭─╮─────────────────────────────────────────┴─╮\n";
    cout << "│ │ │                                           │\n";
    cout << "╰─╯─│                                           │\n";
    cout << "    │                                           │\n";
    cout << "    └───────────────────────────────────────────┘\n";

    color(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    gotoxy(18, 1); cout << "操 作 说 明";
    gotoxy(1, 3); cout << "用 ↑←↓→ 或 WASD 操作             ESC键：退出";
    gotoxy(18, 5); cout << "游 戏 介 绍";
    gotoxy(1, 7); cout << "每次选择一个方向移动，移动时数字向该方向靠拢";    // 游戏规则
    gotoxy(1, 8); cout << "相同数字可合并，移动后空格处会生成随机数字2/4";
    gotoxy(1, 9); cout << "如果得到数字2048，则游戏胜利!";
    gotoxy(1, 10); cout << "如果棋盘被数字填满，无法进行移动，则游戏结束";

    color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);    // 恢复控制台文字颜色为默认颜色
    gotoxy(8, 14);
    wait_for_enter();
}
// 背景颜色
void background() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;    // 获取控制台窗口尺寸
    int cols, rows;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    cout << "\x1b[48;2;250;248;240m";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << ' ';
        }
        cout << "\n";
    }
}
// 游戏界面
void print_interface() {
    background();

    gotoxy(12, 0); cout << "\x1b[38;2;185;173;161;48;2;250;248;240m▄▄▄▄▄▄▄▄▄▄▄▄▄";
    gotoxy(26, 0); cout << "▄▄▄▄▄▄▄▄▄▄▄▄▄";
    gotoxy(40, 0); cout << "▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄";
    gotoxy(12, 1); cout << "\x1b[38;2;236;228;219;48;2;185;173;161m 步数：      ";
    gotoxy(26, 1); cout << " 分数：      ";
    gotoxy(40, 1); cout << " 最高分：      ";
    gotoxy(12, 2); cout << "\x1b[38;2;250;248;240;48;2;185;173;161m▄▄▄▄▄▄▄▄▄▄▄▄▄";
    gotoxy(26, 2); cout << "▄▄▄▄▄▄▄▄▄▄▄▄▄";
    gotoxy(40, 2); cout << "▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄";

    gotoxy(22, 3); cout << "\x1b[38;2;185;173;161;48;2;250;248;240m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄";
    for (int n = 0; n < 12; ++n) {
        gotoxy(22, 4 + n); cout << "\x1b[48;2;185;173;161m ";
        gotoxy(47, 4 + n); cout << "\x1b[48;2;185;173;161m ";
    }
    gotoxy(22, 16); cout << "\x1b[38;2;250;248;240;48;2;185;173;161m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄";

    for (int n = 0; n < 4; ++n) {
        for (int m = 0; m < 4; ++m) {
            gotoxy(23 + 6 * m, 4 + 3 * n); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m ▄▄▄▄ ";
            gotoxy(23 + 6 * m, 5 + 3 * n); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m█    █";
            gotoxy(23 + 6 * m, 6 + 3 * n); cout << "█▄▄▄▄█";
        }
    }
}
//------------------------逻辑主体-----------------------
// 游戏主循环
void play_game() {
    int board[4][4] = {}; // 4x4棋盘，并初始化
    int score = 0;        // 分数
    int step = 0;         // 步数
    char choice = '\0';   // 用户选择
    bool game_won = false; // 标志位，标记游戏是否已达2048

    int high_score = read_high_score();    // 最高分

    system("CLS");
    print_interface();    // 打印游戏界面
    update_board(board, score, step, high_score);
    Sleep(200);
    add_random_tile(board);     // 生成随机数并显示
    add_random_tile(board);

    while (1) {
        choice = _getch();        // 获取用户输入

        if (choice == 27) {  // 如果用户按下ESC键，跳出循环
            break;
        }

        // 根据用户输入进行相应操作
        if (move_tiles(board, choice, score)) {
            if (score > high_score) {      // 更新最高分
                high_score = score;
            }
            update_board(board, score, ++step, high_score);
            Sleep(200);
            add_random_tile(board);
        }

        // 检查是否达到了2048，如果还没有赢过游戏
        if (!game_won) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (board[i][j] == 2048) {
                        game_won = true; // 设置游戏胜利标志
                        gotoxy(29, 18); cout << "\x1b[38;2;117;110;102;48;2;250;248;240m你赢了！";
                        gotoxy(27, 19); cout << "是否继续(y/n)";
                        while (1) {
                            choice = _getch();
                            if (choice == 'y' or choice == 'Y') {
                                gotoxy(29, 18); cout << "        ";
                                gotoxy(27, 19); cout << "             ";
                                break;
                            }
                            else if (choice == 'n' or choice == 'N') {
                                write_high_score(high_score); //写入最高分
                                return; //直接退出
                            }
                        }
                        break;
                    }
                }
                if (game_won)
                    break;
            }
        }

        // 检查是否还能移动方块，以判断游戏是否结束，若结束则跳出循环
        bool can_move = false;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j] == 0)
                    can_move = true;
                if (i < 3 && board[i][j] == board[i + 1][j])
                    can_move = true;
                if (j < 3 && board[i][j] == board[i][j + 1])
                    can_move = true;
            }
        }
        if (!can_move) {
            gotoxy(29, 18); cout << "\x1b[38;2;117;110;102;48;2;250;248;240m游戏结束！\n" << endl << endl;
            gotoxy(27, 19); wait_for_enter();
            break;
        }
    }
    write_high_score(high_score); //写入最高分
}
// 在棋盘上随机地添加随机数字 2/4 并显示
void add_random_tile(int board[4][4]) {
    random_device rd; // 使用随机设备作为种子
    mt19937 gen(rd());    // 随机数生成器
    int empty_tiles[16][2];
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (board[i][j] == 0) {
                empty_tiles[count][0] = i;
                empty_tiles[count][1] = j;
                count++;
            }
        }
    }

    if (count > 0) {
        int random_index = gen() % count;
        int x = empty_tiles[random_index][0];
        int y = empty_tiles[random_index][1];
        board[x][y] = (gen() % 10 + 10) % 10 > 0 ? 2 : 4; // 按概率比 9:1 随机生成 2 或 4
        display_tile(24 + y * 6, 5 + x * 3, board[x][y], 14);
    }
}
// 根据用户操作，移动&倍增方块
bool move_tiles(int board[4][4], char direction, int& score) {
    bool moved = false;

    // 将 WASD 键映射到方向键，未与后方合并代码是考虑到保留动画
    switch (direction) {
    case 'w':
    case 'W':
        direction = 72; break; // W -> 上
    case 's':
    case 'S':
        direction = 80; break; // S -> 下
    case 'a':
    case 'A':
        direction = 75; break; // A -> 左
    case 'd':
    case 'D':
        direction = 77; break; // D -> 右
    }

    switch (direction) {
    case 72: // 上
        for (int j = 0; j < 4; ++j) {
            for (int i = 1; i < 4; ++i) {
                if (board[i][j] != 0) {
                    int k = i;
                    while (k > 0 && board[k - 1][j] == 0) {        // 移动
                        move_with_animation(j, k, direction, board[k][j]);
                        board[k - 1][j] = board[k][j];
                        board[k][j] = 0;
                        k--;
                        moved = true;
                    }
                    if (k > 0 && board[k - 1][j] == board[k][j]) {        // 合并
                        double_with_animation(j, k, direction, board[k][j]);
                        board[k - 1][j] *= 2;
                        score += board[k - 1][j];
                        board[k][j] = 0;
                        moved = true;
                    }
                }
            }
        }
        break;
    case 80: // 下
        for (int j = 0; j < 4; ++j) {
            for (int i = 2; i >= 0; --i) {
                if (board[i][j] != 0) {
                    int k = i;
                    while (k < 3 && board[k + 1][j] == 0) {
                        move_with_animation(j, k, direction, board[k][j]);
                        board[k + 1][j] = board[k][j];
                        board[k][j] = 0;
                        k++;
                        moved = true;
                    }
                    if (k < 3 && board[k + 1][j] == board[k][j]) {
                        double_with_animation(j, k, direction, board[k][j]);
                        board[k + 1][j] *= 2;
                        score += board[k + 1][j];
                        board[k][j] = 0;
                        moved = true;
                    }
                }
            }
        }
        break;
    case 75: // 左
        for (int i = 0; i < 4; ++i) {
            for (int j = 1; j < 4; ++j) {
                if (board[i][j] != 0) {
                    int k = j;
                    while (k > 0 && board[i][k - 1] == 0) {
                        move_with_animation(k, i, direction, board[i][k]);
                        board[i][k - 1] = board[i][k];
                        board[i][k] = 0;
                        k--;
                        moved = true;
                    }
                    if (k > 0 && board[i][k - 1] == board[i][k]) {
                        double_with_animation(k, i, direction, board[i][k]);
                        board[i][k - 1] *= 2;
                        score += board[i][k - 1];
                        board[i][k] = 0;
                        moved = true;
                    }
                }
            }
        }
        break;
    case 77: // 右
        for (int i = 0; i < 4; ++i) {
            for (int j = 2; j >= 0; --j) {
                if (board[i][j] != 0) {
                    int k = j;
                    while (k < 3 && board[i][k + 1] == 0) {
                        move_with_animation(k, i, direction, board[i][k]);
                        board[i][k + 1] = board[i][k];
                        board[i][k] = 0;
                        k++;
                        moved = true;
                    }
                    if (k < 3 && board[i][k + 1] == board[i][k]) {
                        double_with_animation(k, i, direction, board[i][k]);
                        board[i][k + 1] *= 2;
                        score += board[i][k + 1];
                        board[i][k] = 0;
                        moved = true;
                    }
                }
            }
        }
        break;
    }
    return moved;
}
// 更新数据
void update_board(int board[4][4], int score, int step, int high_score) {
    gotoxy(18, 1);
    cout << "\x1b[38;2;255;255;255;48;2;185;173;161m" << setw(6) << step;
    gotoxy(32, 1);
    cout << setw(6) << score;
    gotoxy(48, 1);
    cout << setw(6) << high_score;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            display_tile(24 + j * 6, 5 + i * 3, board[i][j], 14);
        }
    }
}
// 移动动画
void move_with_animation(int x, int y, char direction, int num) {
    if (num == 0) return;
    const float time = 2; // 延迟

    switch (direction) {
    case 72: // 上
        display_tile(24 + x * 6, 5 + y * 3 - 1, num, 0);
        gotoxy(24 + x * 6, 5 + y * 3 + 1); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m▄▄▄▄"; // 清除之前的位置
        high_precision_sleep(time);

        display_tile(24 + x * 6, 5 + y * 3 - 2, num, 0);
        gotoxy(24 + x * 6, 5 + y * 3); cout << "\x1b[48;2;202;193;181m    ";
        high_precision_sleep(time);

        display_tile(24 + x * 6, 5 + y * 3 - 3, num, 14);
        gotoxy(24 + x * 6, 5 + y * 3 - 1); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m▄▄▄▄";
        high_precision_sleep(time);
        break;
    case 80: // 下
        display_tile(24 + x * 6, 5 + y * 3 + 1, num, 0);
        gotoxy(24 + x * 6, 5 + y * 3 - 1); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m▄▄▄▄";
        high_precision_sleep(time);

        display_tile(24 + x * 6, 5 + y * 3 + 2, num, 0);
        gotoxy(24 + x * 6, 5 + y * 3); cout << "\x1b[48;2;202;193;181m    ";
        high_precision_sleep(time);

        display_tile(24 + x * 6, 5 + y * 3 + 3, num, 14);
        gotoxy(24 + x * 6, 5 + y * 3 + 1); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m▄▄▄▄";
        high_precision_sleep(time);
        break;
    case 75: // 左
        for (int n = 1; n <= 4; ++n) {
            display_tile(24 + x * 6 - n, 5 + y * 3, num, 14);
            gotoxy(24 + x * 6 - n + 4, 5 + y * 3 - 1); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m▄";
            gotoxy(24 + x * 6 - n + 4, 5 + y * 3); cout << "\x1b[48;2;202;193;181m ";
            gotoxy(24 + x * 6 - n + 4, 5 + y * 3 + 1); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m▄";
            high_precision_sleep(time);
        }
        for (int n = 5; n <= 6; ++n) {
            display_tile(24 + x * 6 - n, 5 + y * 3, num, 14);
            gotoxy(24 + x * 6 - n + 4, 5 + y * 3 - 1); cout << "\x1b[48;2;185;173;161m ";
            gotoxy(24 + x * 6 - n + 4, 5 + y * 3); cout << "\x1b[48;2;185;173;161m ";
            gotoxy(24 + x * 6 - n + 4, 5 + y * 3 + 1); cout << "\x1b[48;2;185;173;161m ";
            high_precision_sleep(time);
        }
        break;
    case 77: // 右
        for (int n = 1; n <= 4; ++n) {
            display_tile(24 + x * 6 + n, 5 + y * 3, num, 14);
            gotoxy(24 + x * 6 + n - 1, 5 + y * 3 - 1); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m▄";
            gotoxy(24 + x * 6 + n - 1, 5 + y * 3); cout << "\x1b[48;2;202;193;181m ";
            gotoxy(24 + x * 6 + n - 1, 5 + y * 3 + 1); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m▄";
            high_precision_sleep(time);
        }
        for (int n = 5; n <= 6; ++n) {
            display_tile(24 + x * 6 + n, 5 + y * 3, num, 14);
            gotoxy(24 + x * 6 + n - 1, 5 + y * 3 - 1); cout << "\x1b[48;2;185;173;161m ";
            gotoxy(24 + x * 6 + n - 1, 5 + y * 3); cout << "\x1b[48;2;185;173;161m ";
            gotoxy(24 + x * 6 + n - 1, 5 + y * 3 + 1); cout << "\x1b[48;2;185;173;161m ";
            high_precision_sleep(time);
        }
        break;
    }
}
// 合并&倍增动画
void double_with_animation(int x, int y, char direction, int num) {
    string tile_colors[] = { "202;193;181", "236;228;219", "235;224;203", "232;180;129",
                         "232;154;108", "230;131;103", "228;104;71", "232;208;127",
                         "218;190;107", "241;202;108", "236;200;93", "240;197;80",
                         "235;133;123", "235;78;52", "185;173;161" };
    if (num == 0) return;
    const float time = 2; // 延迟
    const float time_d = 50; // 倍增延迟
    int tile_order = log2(num);
    switch (direction) {
    case 72: // 上
        display_tile(24 + x * 6, 5 + y * 3 - 1, num, 0);
        gotoxy(24 + x * 6, 5 + y * 3 + 1); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m▄▄▄▄";
        gotoxy(24 + x * 6, 5 + y * 3 - 2); cout << "\x1b[48;2;" << tile_colors[tile_order] << "m    ";
        high_precision_sleep(time);

        display_tile(24 + x * 6, 5 + y * 3 - 2, num, 0);
        gotoxy(24 + x * 6, 5 + y * 3); cout << "\x1b[48;2;202;193;181m    ";
        gotoxy(24 + x * 6, 5 + y * 3 - 3); cout << "\x1b[48;2;" << tile_colors[tile_order] << "m    ";
        high_precision_sleep(time);

        display_tile(24 + x * 6, 5 + y * 3 - 3, num, 14);
        gotoxy(24 + x * 6, 5 + y * 3 - 1); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m▄▄▄▄";
        high_precision_sleep(time);

        // 倍增
        gotoxy(24 + x * 6 - 1, 5 + y * 3 - 3); cout << "\x1b[48;2;" << tile_colors[tile_order + 1] << "m      ";
        display_tile(24 + x * 6, 5 + y * 3 - 3, num * 2, 14);
        gotoxy(24 + x * 6 - 1, 5 + y * 3 - 3 - 1); cout << "\x1b[48;2;" << tile_colors[tile_order + 1] << "m      ";
        gotoxy(24 + x * 6 - 1, 5 + y * 3 - 3 + 1); cout << "\x1b[48;2;" << tile_colors[tile_order + 1] << "m      ";
        high_precision_sleep(time_d);

        gotoxy(24 + x * 6 - 1, 5 + y * 3 - 3); cout << "\x1b[48;2;185;173;161m      ";
        gotoxy(24 + x * 6 - 1, 5 + y * 3 - 3 - 1); cout << "\x1b[48;2;185;173;161m      ";
        gotoxy(24 + x * 6 - 1, 5 + y * 3 - 3 + 1); cout << "\x1b[48;2;185;173;161m      ";
        display_tile(24 + x * 6, 5 + y * 3 - 3, num * 2, 14);
        high_precision_sleep(time_d);
        break;
    case 80: // 下
        display_tile(24 + x * 6, 5 + y * 3 + 1, num, 0);
        gotoxy(24 + x * 6, 5 + y * 3 - 1); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m▄▄▄▄";
        gotoxy(24 + x * 6, 5 + y * 3 + 2); cout << "\x1b[48;2;" << tile_colors[tile_order] << "m    ";
        high_precision_sleep(time);

        display_tile(24 + x * 6, 5 + y * 3 + 2, num, 0);
        gotoxy(24 + x * 6, 5 + y * 3); cout << "\x1b[48;2;202;193;181m    ";
        gotoxy(24 + x * 6, 5 + y * 3 + 3); cout << "\x1b[48;2;" << tile_colors[tile_order] << "m    ";
        high_precision_sleep(time);

        display_tile(24 + x * 6, 5 + y * 3 + 3, num, 14);
        gotoxy(24 + x * 6, 5 + y * 3 + 1); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m▄▄▄▄";
        gotoxy(24 + x * 6, 5 + y * 3 + 4); cout << "\x1b[48;2;" << tile_colors[tile_order] << "m    ";
        high_precision_sleep(time);

        // 倍增
        gotoxy(24 + x * 6 - 1, 5 + y * 3 + 3); cout << "\x1b[48;2;" << tile_colors[tile_order + 1] << "m      ";
        display_tile(24 + x * 6, 5 + y * 3 + 3, num * 2, 14);
        gotoxy(24 + x * 6 - 1, 5 + y * 3 + 3 - 1); cout << "\x1b[48;2;" << tile_colors[tile_order + 1] << "m      ";
        gotoxy(24 + x * 6 - 1, 5 + y * 3 + 3 + 1); cout << "\x1b[48;2;" << tile_colors[tile_order + 1] << "m      ";
        high_precision_sleep(time_d);

        gotoxy(24 + x * 6 - 1, 5 + y * 3 + 3); cout << "\x1b[48;2;185;173;161m      ";
        gotoxy(24 + x * 6 - 1, 5 + y * 3 + 3 - 1); cout << "\x1b[48;2;185;173;161m      ";
        gotoxy(24 + x * 6 - 1, 5 + y * 3 + 3 + 1); cout << "\x1b[48;2;185;173;161m      ";
        display_tile(24 + x * 6, 5 + y * 3 + 3, num * 2, 14);
        high_precision_sleep(time_d);
        break;
    case 75: // 左
        for (int n = 1; n <= 4; ++n) {
            display_tile(24 + x * 6 - n, 5 + y * 3, num, 14);
            gotoxy(24 + x * 6 - n + 4, 5 + y * 3 - 1); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m▄";
            gotoxy(24 + x * 6 - n + 4, 5 + y * 3); cout << "\x1b[48;2;202;193;181m ";
            gotoxy(24 + x * 6 - n + 4, 5 + y * 3 + 1); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m▄";
            high_precision_sleep(time);
        }
        for (int n = 5; n <= 6; ++n) {
            display_tile(24 + x * 6 - n, 5 + y * 3, num, 14);
            gotoxy(24 + x * 6 - n + 4, 5 + y * 3 - 1); cout << "\x1b[48;2;185;173;161m ";
            gotoxy(24 + x * 6 - n + 4, 5 + y * 3); cout << "\x1b[48;2;185;173;161m ";
            gotoxy(24 + x * 6 - n + 4, 5 + y * 3 + 1); cout << "\x1b[48;2;185;173;161m ";
            high_precision_sleep(time);
        }
        // 倍增
        gotoxy(24 + x * 6 - 6 - 1, 5 + y * 3); cout << "\x1b[48;2;" << tile_colors[tile_order + 1] << "m      ";
        display_tile(24 + x * 6 - 6, 5 + y * 3, num * 2, 14);
        gotoxy(24 + x * 6 - 6 - 1, 5 + y * 3 - 1); cout << "\x1b[48;2;" << tile_colors[tile_order + 1] << "m      ";
        gotoxy(24 + x * 6 - 6 - 1, 5 + y * 3 + 1); cout << "\x1b[48;2;" << tile_colors[tile_order + 1] << "m      ";
        high_precision_sleep(time_d);

        gotoxy(24 + x * 6 - 6 - 1, 5 + y * 3); cout << "\x1b[48;2;185;173;161m      ";
        gotoxy(24 + x * 6 - 6 - 1, 5 + y * 3 - 1); cout << "\x1b[48;2;185;173;161m      ";
        gotoxy(24 + x * 6 - 6 - 1, 5 + y * 3 + 1); cout << "\x1b[48;2;185;173;161m      ";
        display_tile(24 + x * 6 - 6, 5 + y * 3, num * 2, 14);
        high_precision_sleep(time_d);
        break;
    case 77: // 右
        for (int n = 1; n <= 4; ++n) {
            display_tile(24 + x * 6 + n, 5 + y * 3, num, 14);
            gotoxy(24 + x * 6 + n - 1, 5 + y * 3 - 1); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m▄";
            gotoxy(24 + x * 6 + n - 1, 5 + y * 3); cout << "\x1b[48;2;202;193;181m ";
            gotoxy(24 + x * 6 + n - 1, 5 + y * 3 + 1); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m▄";
            high_precision_sleep(time);
        }
        for (int n = 5; n <= 6; ++n) {
            display_tile(24 + x * 6 + n, 5 + y * 3, num, 14);
            gotoxy(24 + x * 6 + n - 1, 5 + y * 3 - 1); cout << "\x1b[48;2;185;173;161m ";
            gotoxy(24 + x * 6 + n - 1, 5 + y * 3); cout << "\x1b[48;2;185;173;161m ";
            gotoxy(24 + x * 6 + n - 1, 5 + y * 3 + 1); cout << "\x1b[48;2;185;173;161m ";
            high_precision_sleep(time);
        }
        // 倍增
        gotoxy(24 + x * 6 + 6 - 1, 5 + y * 3); cout << "\x1b[48;2;" << tile_colors[tile_order + 1] << "m      ";
        display_tile(24 + x * 6 + 6, 5 + y * 3, num * 2, 14);
        gotoxy(24 + x * 6 + 6 - 1, 5 + y * 3 - 1); cout << "\x1b[48;2;" << tile_colors[tile_order + 1] << "m      ";
        gotoxy(24 + x * 6 + 6 - 1, 5 + y * 3 + 1); cout << "\x1b[48;2;" << tile_colors[tile_order + 1] << "m      ";
        high_precision_sleep(time_d);

        gotoxy(24 + x * 6 + 6 - 1, 5 + y * 3); cout << "\x1b[48;2;185;173;161m      ";
        gotoxy(24 + x * 6 + 6 - 1, 5 + y * 3 - 1); cout << "\x1b[48;2;185;173;161m      ";
        gotoxy(24 + x * 6 + 6 - 1, 5 + y * 3 + 1); cout << "\x1b[48;2;185;173;161m      ";
        display_tile(24 + x * 6 + 6, 5 + y * 3, num * 2, 14);
        high_precision_sleep(time_d);
        break;
    }
}
// 高精度延迟
void high_precision_sleep(float milliseconds) {
    LARGE_INTEGER frequency;
    LARGE_INTEGER start, end;

    QueryPerformanceFrequency(&frequency);    // 获取高精度计时器的频率
    float wait_time = milliseconds * (frequency.QuadPart / 1000.0);    // 计算等待时间
    QueryPerformanceCounter(&start);    // 获取当前时间

    while (1) {
        QueryPerformanceCounter(&end);        // 获取当前时间
        float elapsed_time = (float)(end.QuadPart - start.QuadPart);        // 计算已过去的时间
        // 如果已过去的时间大于等于等待时间，退出循环
        if (elapsed_time >= wait_time)
            break;
    }
}
// 显示方块
void display_tile(int x, int y, int num, int back) {
    if (num == 0) num++;
    string text_colors[] = { "117;110;102", "248;246;242", "255;255;255" };
    string tile_colors[] = { "202;193;181", "236;228;219", "235;224;203", "232;180;129",
                             "232;154;108", "230;131;103", "228;104;71", "232;208;127",
                             "218;190;107", "241;202;108", "236;200;93", "240;197;80",
                             "235;133;123", "235;78;52", "185;173;161" };
    string num_show[] = { "    ", " ２ ", " ４ ", " ８ ",
                          " 16 ", " 32 ", " 64 ", "128 ",
                          "256 ", "512 ", "1024", "2048",
                          "4096", "8192", "XXXX" };

    int text_order;
    if (num < 8200) {
        text_order = num < 5 ? 0 : 1;
    }
    else {
        text_order = 2;
    }
    int tile_order = log2(num);

    gotoxy(x, y - 1); cout << "\x1b[38;2;" << tile_colors[tile_order] << ";48;2;" << tile_colors[back] << "m▄▄▄▄";
    gotoxy(x, y); cout << "\x1b[38;2;" << text_colors[text_order] << ";48;2;" << tile_colors[tile_order] << "m" << num_show[tile_order];
    gotoxy(x, y + 1); cout << "\x1b[38;2;" << tile_colors[back] << ";48;2;" << tile_colors[tile_order] << "m▄▄▄▄";
}
//----------------------读写最高分---------------------
// 读取最高分
int read_high_score() {
    ifstream file("2048_highscore.txt");
    int high_score = 0;
    if (file.is_open()) {
        file >> high_score;
        file.close();
    }
    return high_score;
}
// 写入最高分
void write_high_score(int high_score) {
    ofstream file("2048_highscore.txt");
    if (file.is_open()) {
        file << high_score;
        file.close();
    }
}