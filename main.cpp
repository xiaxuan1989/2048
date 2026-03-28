#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <iomanip>
#include <fstream>
#include <random>
using namespace std;

string text_colors[] = { "117;110;102", "248;246;242", "255;255;255" };
string tile_colors[] = { "202;193;181", "236;228;219", "235;224;203", "232;180;129",
                         "232;154;108", "230;131;103", "228;104;71", "232;208;127",
                         "218;190;107", "241;202;108", "236;200;93", "240;197;80",
                         "235;133;123", "235;78;52", "185;173;161" };
string num_show[] = { "    ", " ２ ", " ４ ", " ８ ",
                      " 16 ", " 32 ", " 64 ", "128 ",
                      "256 ", "512 ", "1024", "2048",
                      "4096", "8192", "XXXX" };

const int plotX = 2, plotY = 1; // plotX 至少 2，plotY 至少 1

const float Time = 15; // 动画延迟
const float Time_d = 60; // 倍增延迟

struct Board {
    int number;
    int number_copy;
    int move;
    bool isDouble;
};

//------------------------备用函数------------------------
// 指定坐标输出
void gotoxy(int x, int y) {
    COORD pos;
    pos.X = plotX + x;
    pos.Y = plotY + y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
// 隐藏光标
void HideCursor() {
    CONSOLE_CURSOR_INFO Cursor;
    Cursor.bVisible = FALSE;
    Cursor.dwSize = sizeof(Cursor); //若无赋值，光标隐藏无效
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Cursor);
}
// ANSI 转义序列
void enableAnsiSupport() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
// 高精度延迟
void high_precision_sleep(float milliseconds) {
    LARGE_INTEGER frequency;
    LARGE_INTEGER start, end;
    QueryPerformanceFrequency(&frequency);    // 获取高精度计时器的频率
    float wait_Time = milliseconds * (frequency.QuadPart / 1000.0);    // 计算等待时间
    QueryPerformanceCounter(&start);    // 获取当前时间
    while (1) {
        QueryPerformanceCounter(&end);        // 获取当前时间
        float elapsed_Time = (float)(end.QuadPart - start.QuadPart);        // 计算已过去的时间
        if (elapsed_Time >= wait_Time) break;
    }
}
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
//------------------------图形界面------------------------
// 背景
void print_interface() {
    gotoxy(-plotX, -plotY);
    cout << "\x1b[48;2;250;248;240m"; // 背景色
    for (int i = 0; i < 20 + 2 * plotY; ++i) {
        for (int j = 0; j < 30 + 2 * plotX; ++j) {
            cout << ' ';
        }
        cout << "\n";
    }
    //分数
    gotoxy(1, 0); cout << "\x1b[38;2;185;173;161;48;2;250;248;240m▄▄▄▄▄▄▄▄▄▄▄▄▄";
    gotoxy(16, 0); cout << "▄▄▄▄▄▄▄▄▄▄▄▄▄";
    gotoxy(1, 1); cout << "\x1b[38;2;236;228;219;48;2;185;173;161m SCORE       ";
    gotoxy(16, 1); cout << " BEST        ";
    gotoxy(1, 2); cout << "\x1b[38;2;250;248;240;48;2;185;173;161m▄▄▄▄▄▄▄▄▄▄▄▄▄";
    gotoxy(16, 2); cout << "▄▄▄▄▄▄▄▄▄▄▄▄▄";
    //外框
    gotoxy(2, 3); cout << "\x1b[38;2;185;173;161;48;2;250;248;240m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄";
    for (int n = 0; n < 12; ++n) {
        gotoxy(2, 4 + n); cout << "\x1b[48;2;185;173;161m ";
        gotoxy(27, 4 + n); cout << " ";
    }
    gotoxy(2, 16); cout << "\x1b[38;2;250;248;240;48;2;185;173;161m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄";
    //网格
    for (int n = 0; n < 4; ++n) {
        for (int m = 0; m < 4; ++m) {
            gotoxy(3 + 6 * m, 4 + 3 * n); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m ▄▄▄▄ ";
            gotoxy(3 + 6 * m, 5 + 3 * n); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m█    █";
            gotoxy(3 + 6 * m, 6 + 3 * n); cout << "█▄▄▄▄█";
        }
    }
}
// 显示方块
void display_tile(int x, int y, int num, int back) {
    int text_order;
    if (num < 8200) text_order = num < 5 ? 0 : 1;
    else text_order = 2;
    int tile_order = num == 0 ? 0 : log2(num); // 注意定义域
    gotoxy(x, y - 1); cout << "\x1b[38;2;" << tile_colors[tile_order] << ";48;2;" << tile_colors[back] << "m▄▄▄▄";
    gotoxy(x, y); cout << "\x1b[38;2;" << text_colors[text_order] << ";48;2;" << tile_colors[tile_order] << "m" << num_show[tile_order];
    gotoxy(x, y + 1); cout << "\x1b[38;2;" << tile_colors[back] << ";48;2;" << tile_colors[tile_order] << "m▄▄▄▄";
}
// 倍增动画
void double_animation(Board board[4][4]) {
    int text_order, tile_order;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (board[i][j].isDouble) {
                if (board[i][j].number < 8200) text_order = board[i][j].number < 5 ? 0 : 1;
                else text_order = 2;
                tile_order = board[i][j].number == 0 ? 0 : log2(board[i][j].number);
                gotoxy(4 + j * 6 - 1, 5 + i * 3 - 1); cout << "\x1b[48;2;" << tile_colors[tile_order] << "m      ";
                gotoxy(4 + j * 6 - 1, 5 + i * 3); cout << "\x1b[38;2;" << text_colors[text_order] << ";48;2;" << tile_colors[tile_order] << "m " << num_show[tile_order] << " ";
                gotoxy(4 + j * 6 - 1, 5 + i * 3 + 1); cout << "\x1b[48;2;" << tile_colors[tile_order] << "m      ";
            }
        }
    }
    high_precision_sleep(Time_d);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (board[i][j].isDouble) {
                tile_order = board[i][j].number == 0 ? 0 : log2(board[i][j].number);
                cout << "\x1b[48;2;185;173;161m";
                for (int m = -1; m <= 4; m += 5) {
                    for (int n = -1; n <= 1; ++n) {
                        gotoxy(4 + j * 6 + m, 5 + i * 3 + n); cout << " ";
                    }
                }
                gotoxy(4 + j * 6, 5 + i * 3 - 1); cout << "\x1b[38;2;" << tile_colors[tile_order] << ";48;2;" << tile_colors[14] << "m▄▄▄▄";
                gotoxy(4 + j * 6, 5 + i * 3 + 1); cout << "\x1b[38;2;" << tile_colors[14] << ";48;2;" << tile_colors[tile_order] << "m▄▄▄▄";
            }
        }
    }
}
// 更新数据
void update_board(Board board[4][4], int score, int high_score) {
    gotoxy(7, 1);
    cout << "\x1b[38;2;255;255;255;48;2;185;173;161m" << setw(6) << score;
    gotoxy(22, 1);
    cout << setw(6) << high_score;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            display_tile(4 + j * 6, 5 + i * 3, board[i][j].number, 14);
        }
    }
}
// 上移及动画
bool move_up(Board board[4][4], int& score) {
    int max_move = 0;
    for (int j = 0; j < 4; ++j) {
        for (int i = 1; i < 4; ++i) {
            if (board[i][j].number != 0) {
                int k = i;
                while (k > 0 && board[k - 1][j].number == 0) {
                    board[k - 1][j].number = board[k][j].number;
                    board[k][j].number = 0;
                    k--;
                }
                if (k > 0 && !board[k - 1][j].isDouble && board[k - 1][j].number == board[k][j].number) {
                    board[k - 1][j].isDouble = true; //倍增过
                    board[k - 1][j].number *= 2;
                    score += board[k][j].number;
                    board[k][j].number = 0;
                    k--;
                }
                if (i - k > 0) { //移动程度
                    board[i][j].move = i - k;
                    if (board[i][j].move > max_move) max_move = board[i][j].move;
                }
            }
        }
    }
    //滑动
    if (max_move > 0) {
        for (int e = 1; e <= max_move * 3; ++e) {
            for (int j = 0; j < 4; ++j) {
                for (int i = 1; i < 4; ++i) {
                    if (board[i][j].move > 0) {
                        int distant = e * board[i][j].move / max_move;
                        if (distant == 0) {}
                        else if (distant % 3 == 1) {
                            display_tile(4 + j * 6, 5 + i * 3 - distant, board[i][j].number_copy, 0);
                            gotoxy(4 + j * 6, 5 + i * 3 - distant + 2); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m▄▄▄▄";
                        }
                        else if (distant % 3 == 2) {
                            display_tile(4 + j * 6, 5 + i * 3 - distant, board[i][j].number_copy, 0);
                            gotoxy(4 + j * 6, 5 + i * 3 - distant + 2); cout << "\x1b[48;2;202;193;181m    ";
                        }
                        else if (distant % 3 == 0) {
                            display_tile(4 + j * 6, 5 + i * 3 - distant, board[i][j].number_copy, 14);
                            gotoxy(4 + j * 6, 5 + i * 3 - distant + 2); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m▄▄▄▄";
                        }
                    }
                }
            }
            high_precision_sleep(Time / (max_move * 3));
        }
        return true;
    }
    return false;
}
// 下移及动画
bool move_down(Board board[4][4], int& score) {
    int max_move = 0;
    for (int j = 0; j < 4; ++j) {
        for (int i = 2; i >= 0; --i) {
            if (board[i][j].number != 0) {
                int k = i;
                while (k < 3 && board[k + 1][j].number == 0) {
                    board[k + 1][j].number = board[k][j].number;
                    board[k][j].number = 0;
                    k++;
                }
                if (k < 3 && !board[k + 1][j].isDouble && board[k + 1][j].number == board[k][j].number) {
                    board[k + 1][j].isDouble = true; //倍增过
                    board[k + 1][j].number *= 2;
                    score += board[k][j].number;
                    board[k][j].number = 0;
                    k++;
                }
                if (k - i > 0) { //移动程度
                    board[i][j].move = k - i;
                    if (board[i][j].move > max_move) max_move = board[i][j].move;
                }
            }
        }
    }
    //滑动
    if (max_move > 0) {
        for (int e = 1; e <= max_move * 3; ++e) {
            for (int j = 0; j < 4; ++j) {
                for (int i = 2; i >= 0; --i) {
                    if (board[i][j].move > 0) {
                        int distant = e * board[i][j].move / max_move;
                        if (distant == 0) {}
                        else if (distant % 3 == 1) {
                            display_tile(4 + j * 6, 5 + i * 3 + distant, board[i][j].number_copy, 0);
                            gotoxy(4 + j * 6, 5 + i * 3 + distant - 2); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m▄▄▄▄";
                        }
                        else if (distant % 3 == 2) {
                            display_tile(4 + j * 6, 5 + i * 3 + distant, board[i][j].number_copy, 0);
                            gotoxy(4 + j * 6, 5 + i * 3 + distant - 2); cout << "\x1b[48;2;202;193;181m    ";
                        }
                        else if (distant % 3 == 0) {
                            display_tile(4 + j * 6, 5 + i * 3 + distant, board[i][j].number_copy, 14);
                            gotoxy(4 + j * 6, 5 + i * 3 + distant - 2); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m▄▄▄▄";
                        }
                    }
                }
            }
            high_precision_sleep(Time / (max_move * 3));
        }
        return true;
    }
    return false;
}
// 左移及动画
bool move_left(Board board[4][4], int& score) {
    int max_move = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 1; j < 4; ++j) {
            if (board[i][j].number != 0) {
                int k = j;
                while (k > 0 && board[i][k - 1].number == 0) {
                    board[i][k - 1].number = board[i][k].number;
                    board[i][k].number = 0;
                    k--;
                }
                if (k > 0 && !board[i][k - 1].isDouble && board[i][k - 1].number == board[i][k].number) {
                    board[i][k - 1].isDouble = true; //倍增过
                    board[i][k - 1].number *= 2;
                    score += board[i][k].number;
                    board[i][k].number = 0;
                    k--;
                }
                if (j - k > 0) { //移动程度
                    board[i][j].move = j - k;
                    if (board[i][j].move > max_move) max_move = board[i][j].move;
                }
            }
        }
    }
    //滑动
    if (max_move > 0) {
        for (int e = 1; e <= max_move * 6; ++e) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 1; j < 4; ++j) {
                    if (board[i][j].move > 0) {
                        int distant = e * board[i][j].move / max_move;
                        if (distant % 6 == 1 or distant % 6 == 2 or distant % 6 == 3 or distant % 6 == 4) {
                            display_tile(4 + j * 6 - distant, 5 + i * 3, board[i][j].number_copy, 14);
                            gotoxy(4 + j * 6 - distant + 4, 5 + i * 3 - 1); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m▄";
                            gotoxy(4 + j * 6 - distant + 4, 5 + i * 3); cout << "\x1b[48;2;202;193;181m ";
                            gotoxy(4 + j * 6 - distant + 4, 5 + i * 3 + 1); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m▄";
                        }
                        else if (distant % 6 == 5 or distant % 6 == 0) {
                            display_tile(4 + j * 6 - distant, 5 + i * 3, board[i][j].number_copy, 14);
                            gotoxy(4 + j * 6 - distant + 4, 5 + i * 3 - 1); cout << "\x1b[48;2;185;173;161m ";
                            gotoxy(4 + j * 6 - distant + 4, 5 + i * 3); cout << "\x1b[48;2;185;173;161m ";
                            gotoxy(4 + j * 6 - distant + 4, 5 + i * 3 + 1); cout << "\x1b[48;2;185;173;161m ";
                        }
                    }
                }
            }
            high_precision_sleep(Time / (max_move * 6));
        }
        return true;
    }
    return false;
}
// 右移及动画
bool move_right(Board board[4][4], int& score) {
    int max_move = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 2; j >= 0; --j) {
            if (board[i][j].number != 0) {
                int k = j;
                while (k < 3 && board[i][k + 1].number == 0) {
                    board[i][k + 1].number = board[i][k].number;
                    board[i][k].number = 0;
                    k++;
                }
                if (k < 3 && !board[i][k + 1].isDouble && board[i][k + 1].number == board[i][k].number) {
                    board[i][k + 1].isDouble = true; //倍增过
                    board[i][k + 1].number *= 2;
                    score += board[i][k].number;
                    board[i][k].number = 0;
                    k++;
                }
                if (k - j > 0) { //移动程度
                    board[i][j].move = k - j;
                    if (board[i][j].move > max_move) max_move = board[i][j].move;
                }
            }
        }
    }
    //滑动
    if (max_move > 0) {
        for (int e = 1; e <= max_move * 6; ++e) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 2; j >= 0; --j) {
                    if (board[i][j].move > 0) {
                        int distant = e * board[i][j].move / max_move;
                        if (distant % 6 == 1 or distant % 6 == 2 or distant % 6 == 3 or distant % 6 == 4) {
                            display_tile(4 + j * 6 + distant, 5 + i * 3, board[i][j].number_copy, 14);
                            gotoxy(4 + j * 6 + distant - 1, 5 + i * 3 - 1); cout << "\x1b[38;2;202;193;181;48;2;185;173;161m▄";
                            gotoxy(4 + j * 6 + distant - 1, 5 + i * 3); cout << "\x1b[48;2;202;193;181m ";
                            gotoxy(4 + j * 6 + distant - 1, 5 + i * 3 + 1); cout << "\x1b[38;2;185;173;161;48;2;202;193;181m▄";
                        }
                        else if (distant % 6 == 5 or distant % 6 == 0) {
                            display_tile(4 + j * 6 + distant, 5 + i * 3, board[i][j].number_copy, 14);
                            gotoxy(4 + j * 6 + distant - 1, 5 + i * 3 - 1); cout << "\x1b[48;2;185;173;161m ";
                            gotoxy(4 + j * 6 + distant - 1, 5 + i * 3); cout << "\x1b[48;2;185;173;161m ";
                            gotoxy(4 + j * 6 + distant - 1, 5 + i * 3 + 1); cout << "\x1b[48;2;185;173;161m ";
                        }
                    }
                }
            }
            high_precision_sleep(Time / (max_move * 6));
        }
        return true;
    }
    return false;
}
//------------------------逻辑主体-----------------------
// 随机添加 2/4 并显示
void add_random_tile(Board board[4][4]) {
    random_device rd; // 使用随机设备作为种子
    mt19937 gen(rd());    // 随机数生成器
    int empty_tiles[16][2];
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (board[i][j].number == 0) {
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
        board[x][y].number = (gen() % 10 + 10) % 10 > 0 ? 2 : 4; // 按概率比 9:1 随机生成 2 或 4
        display_tile(4 + y * 6, 5 + x * 3, board[x][y].number, 14);
    }
}
// 游戏主循环
bool play_game() {
    Board board[4][4] = {};
    int score = 0;        // 本轮分数
    char choice = '\0';   // 按键选择
    bool game_won = false; // 是否已达2048
    bool keepOn = false; // 是否继续
    int high_score = read_high_score();    // 读取最高分

    update_board(board, score, high_score);
    Sleep(100);
    add_random_tile(board);     // 添加随机方块并显示
    add_random_tile(board);

    while (1) {
        // 复制
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                board[i][j].number_copy = board[i][j].number;
            }
        }
        choice = _getch();        // 获取键盘

        if (choice == 27) {  // ESC
            write_high_score(high_score); //写入最高分
            return true;
        }
        else if (choice == '\r') break; // 新游戏

        bool moved = false;
        // 将 WASD 键映射到方向键，未与后方合并代码是考虑到保留动画
        switch (choice) {
        case 'w':
        case 'W':
            choice = 72; break; // W -> 上
        case 's':
        case 'S':
            choice = 80; break; // S -> 下
        case 'a':
        case 'A':
            choice = 75; break; // A -> 左
        case 'd':
        case 'D':
            choice = 77; break; // D -> 右
        }
        switch (choice) {
        case 72: // 上
            moved = move_up(board, score);
            break;
        case 80: // 下
            moved = move_down(board, score);
            break;
        case 75: // 左
            moved = move_left(board, score);
            break;
        case 77: // 右
            moved = move_right(board, score);
            break;
        }

        if (moved) {
            double_animation(board);     //倍增动画
            //清空
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    board[i][j].isDouble = false;
                    board[i][j].move = 0;
                }
            }
            if (score > high_score) high_score = score; // 更新最高分
            update_board(board, score, high_score); // 更新数据
            Sleep(100);
            add_random_tile(board);
        }

        // 是否赢过
        if (!game_won) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (board[i][j].number == 2048) game_won = true;
                }
            }
        }

        if (game_won and !keepOn) {
            gotoxy(11, 18); cout << "\x1b[38;2;117;110;102;48;2;250;248;240mYOU  WIN";
            gotoxy(-1, 19); cout << "New game or keep on(enter/space)";
            while (1) {
                choice = _getch();
                if (choice == ' ' or choice == '\r') break;
            }
            gotoxy(11, 18); cout << "        ";
            gotoxy(-1, 19); cout << "                                ";
            if (choice == ' ')  keepOn = true;
            else if (choice == '\r') break;
        }

        // 是否结束
        bool can_move = false;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j].number == 0) can_move = true;
                if (i < 3 && board[i][j].number == board[i + 1][j].number) can_move = true;
                if (j < 3 && board[i][j].number == board[i][j + 1].number) can_move = true;
            }
        }
        if (!can_move) {
            gotoxy(10, 18); cout << "\x1b[38;2;117;110;102;48;2;250;248;240mGAME  OVER\n" << endl << endl;
            gotoxy(1, 19); cout << "New game or exit?(enter/ESC)";
            while (1) {
                choice = _getch();
                if (choice == '\r' or choice == 27) {
                    break;
                }
            }
            gotoxy(10, 18); cout << "          ";
            gotoxy(1, 19); cout << "                            ";
            if (choice == '\r') {
                break;
            }
            else if (choice == 27) {
                write_high_score(high_score); //写入最高分
                return true;
            }
        }
    }
    return false;
}
// 主函数
int main() {
    bool isExit = false;
    HideCursor();
    enableAnsiSupport();
    SetConsoleTitle(TEXT("2048"));    // 设置标题
    print_interface();    // 基本界面
    while (1) {
        if (play_game()) break; // 按 ESC 才退出程序
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15); // 黑白色
    system("cls");
    return 0;
}