#include <mega128.h>
#include <delay.h>
#include <stdlib.h>

// -------------------------------------------------------------------------
// 1. CAU HINH PHAN CUNG PORTG (MAN HINH NOKIA 5110)
// -------------------------------------------------------------------------
#define LCD_DDR  DDRG
#define LCD_PORT PORTG

#define DC_BIT   0  // PG0
#define DIN_BIT  1  // PG1
#define CLK_BIT  2  // PG2
#define RST_BIT  3  // PG3
#define CE_BIT   4  // PG4

#define SET_DC   LCD_PORT |= (1 << DC_BIT)
#define CLR_DC   LCD_PORT &= ~(1 << DC_BIT)
#define SET_DIN  LCD_PORT |= (1 << DIN_BIT)
#define CLR_DIN  LCD_PORT &= ~(1 << DIN_BIT)
#define SET_CLK  LCD_PORT |= (1 << CLK_BIT)
#define CLR_CLK  LCD_PORT &= ~(1 << CLK_BIT)
#define SET_RST  LCD_PORT |= (1 << RST_BIT)
#define CLR_RST  LCD_PORT &= ~(1 << RST_BIT)
#define SET_CE   LCD_PORT |= (1 << CE_BIT)
#define CLR_CE   LCD_PORT &= ~(1 << CE_BIT)

// -------------------------------------------------------------------------
// 2. CAU HINH PHAN CUNG PORTB (NUT BAM)
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// 2. KEYPAD MATRIX 3x3
// -------------------------------------------------------------------------

void keypad_init()
{
    // =========================
    // COL OUTPUT
    // =========================

    DDRC |= (1<<4); // COL1 PC4
    DDRC |= (1<<5); // COL2 PC5
    DDRE |= (1<<0); // COL3 PE0

    // =========================
    // ROW INPUT
    // =========================

    DDRC &= ~(1<<6); // ROW1 PC6
    DDRE &= ~(1<<1); // ROW2 PE1
    DDRD &= ~(1<<0); // ROW3 PD0

    // pullup
    PORTC |= (1<<6);
    PORTE |= (1<<1);
    PORTD |= (1<<0);

    // all col HIGH
    PORTC |= (1<<4);
    PORTC |= (1<<5);
    PORTE |= (1<<0);
}

// =====================================================

unsigned char BT_LEFT_READ()
{
    // COL1 LOW
    PORTC |= (1<<5);
    PORTE |= (1<<0);

    PORTC &= ~(1<<4);

    delay_us(5);

    // ROW2
    if(!(PINE & (1<<1)))
    {
        delay_ms(20);

        if(!(PINE & (1<<1)))
            return 1;
    }

    return 0;
}

// =====================================================

unsigned char BT_RIGHT_READ()
{
    // COL3 LOW
    PORTC |= (1<<4);
    PORTC |= (1<<5);

    PORTE &= ~(1<<0);

    delay_us(5);

    // ROW2
    if(!(PINE & (1<<1)))
    {
        delay_ms(20);

        if(!(PINE & (1<<1)))
            return 1;
    }

    return 0;
}

// =====================================================

unsigned char BT_ROTATE_READ()
{
    // COL2 LOW
    PORTC |= (1<<4);
    PORTE |= (1<<0);

    PORTC &= ~(1<<5);

    delay_us(5);

    // ROW1
    if(!(PINC & (1<<6)))
    {
        delay_ms(20);

        if(!(PINC & (1<<6)))
            return 1;
    }

    return 0;
}

// =====================================================

unsigned char BT_DROP_READ()
{
    // COL2 LOW
    PORTC |= (1<<4);
    PORTE |= (1<<0);

    PORTC &= ~(1<<5);

    delay_us(5);

    // ROW3
    if(!(PIND & (1<<0)))
    {
        delay_ms(20);

        if(!(PIND & (1<<0)))
            return 1;
    }

    return 0;
}

// =====================================================

#define BT_LEFT    BT_LEFT_READ()
#define BT_RIGHT   BT_RIGHT_READ()
#define BT_ROTATE  BT_ROTATE_READ()
#define BT_DROP    BT_DROP_READ()

// -------------------------------------------------------------------------
// 3. DINH NGHIA KHOI GACH TETRIS (7 khoi, 4 huong xoay duoi dang nhi phan 4x4)
// -------------------------------------------------------------------------
flash unsigned int tetrominoes[7][4] = {
    {0x0F00, 0x2222, 0x0F00, 0x2222}, // I
    {0x0660, 0x0660, 0x0660, 0x0660}, // O
    {0x0E40, 0x4C40, 0x4E00, 0x4640}, // T
    {0x06C0, 0x8C40, 0x06C0, 0x8C40}, // S
    {0x0C60, 0x4C80, 0x0C60, 0x4C80}, // Z
    {0x44C0, 0x8E00, 0x6220, 0x0E20}, // J
    {0x2260, 0x0E80, 0xC440, 0x2E00}  // L
};

// -------------------------------------------------------------------------
// 4. PHONG CHU 5X7 TIEU CHUAN (Chi so tu 0x20 Space den 0x5A Z)
// -------------------------------------------------------------------------
flash unsigned char font[59][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // space (0x20)
    {0x00, 0x00, 0x5f, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, // #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1c, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1c, 0x00}, // )
    {0x14, 0x08, 0x3e, 0x08, 0x14}, // *
    {0x08, 0x08, 0x3e, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 0 (0x30)
    {0x00, 0x42, 0x7f, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4b, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7f, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3e}, // @
    {0x7e, 0x11, 0x11, 0x11, 0x7e}, // A (0x41)
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, // D
    {0x7f, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7f, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3e, 0x41, 0x49, 0x49, 0x7a}, // G
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // H
    {0x00, 0x41, 0x7f, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3f, 0x01}, // J
    {0x7f, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7f, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // M
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, // N
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, // O
    {0x7f, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3e, 0x41, 0x51, 0x21, 0x5e}, // Q
    {0x7f, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7f, 0x01, 0x01}, // T
    {0x3f, 0x40, 0x40, 0x40, 0x3f}, // U
    {0x1f, 0x20, 0x40, 0x20, 0x1f}, // V
    {0x3f, 0x40, 0x38, 0x40, 0x3f}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}  // Z
};

// -------------------------------------------------------------------------
// 5. BIEN TOAN CUC CUA TRO CHOI
// -------------------------------------------------------------------------
unsigned char display_buffer[504];        // RAM dem man hinh 84x48
unsigned char game_board[15][10];         // Ban do game (15 hang x 10 cot)

unsigned char current_shape, current_rotation; // Loai gach va goc xoay hien tai
int piece_x, piece_y;                          // Toa do khoi dang roi
unsigned char next_shape;                      // Khoi tiep theo
unsigned int score;                            // Ðiem so
unsigned char game_over;                       // Trang thái ket thúc game

// -------------------------------------------------------------------------
// 6. THU VIEN GIAO TIEP MÀN HÌNH NOKIA 5110
// -------------------------------------------------------------------------
void SPI_Write(unsigned char data) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        if (data & 0x80) SET_DIN;
        else CLR_DIN;
        SET_CLK;
        data <<= 1;
        CLR_CLK;
    }
}

void LCD_WriteCmd(unsigned char cmd) {
    CLR_DC;
    CLR_CE;
    SPI_Write(cmd);
    SET_CE;
}

void LCD_Init() {
    LCD_DDR |= 0x1F; // // Output tu PG0 den PG4
    CLR_RST;
    delay_ms(10);
    SET_RST;

    LCD_WriteCmd(0x21); // Che do mo rong
    LCD_WriteCmd(0xBF); // Do tuong phan (Co the chinh len 0xCF neu mo) 
    // 0XBF
    LCD_WriteCmd(0x04); // Temp coefficient
    LCD_WriteCmd(0x12); // Bias 1:48
    LCD_WriteCmd(0x20); // Che do co ban
    LCD_WriteCmd(0x0C); // Normal display
}

void displayClear() {
    unsigned int i;
    for (i = 0; i < 504; i++) display_buffer[i] = 0;
}
                                                                                                            

void displayUpdate() {
    unsigned int i;
    LCD_WriteCmd(0x80); // Reset X = 0
    LCD_WriteCmd(0x40); // Reset Y = 0
    SET_DC;
    CLR_CE;
    for (i = 0; i < 504; i++) {
        SPI_Write(display_buffer[i]);
    }
    SET_CE;
}

void drawPixel(int x, int y, int color) {
    if (x < 0 || x >= 84 || y < 0 || y >= 48) return;
    if (color)
        display_buffer[x + (y / 8) * 84] |= (1 << (y % 8));
    else
        display_buffer[x + (y / 8) * 84] &= ~(1 << (y % 8));
}

// -------------------------------------------------------------------------
// 7. CAC HAM DO HOA CHU VA KHUNG GIAO DIEN
// -------------------------------------------------------------------------
void drawChar(int x, int y, char c) {
    int i, j;
    if (c < 0x20 || c > 0x5A) return;
    c -= 0x20;
    for (i = 0; i < 5; i++) {
        unsigned char line = font[(int)c][i];
        for (j = 0; j < 7; j++) {
            if (line & (1 << j)) drawPixel(x + i, y + j, 1);
            else drawPixel(x + i, y + j, 0);
        }
    }
}

void drawString(int x, int y, char *str) {
    while (*str) {
        drawChar(x, y, *str);
        x += 6;
        str++;
    }
}

void drawScore(int x, int y, unsigned int s) {
    char buf[6];
    int idx = 4;
    buf[5] = '\0';
    if (s == 0) {
        drawString(x, y, "00000");
        return;
    }
    while (idx >= 0) {
        buf[idx] = (s % 10) + '0';
        s /= 10;
        idx--;
    }
    drawString(x, y, buf);
}

// -------------------------------------------------------------------------
// 8. LOGIC GAME TETRIS
// -------------------------------------------------------------------------
unsigned char get_piece_bit(unsigned char shape, unsigned char rotation, unsigned char r, unsigned char c) {
    unsigned int pattern = tetrominoes[shape][rotation];
    return (pattern & (1 << (15 - (r * 4 + c)))) ? 1 : 0;
}

// Ve mot khoi 3x3 (thanh phan cau tao nen khoi gach lon)
void draw_board_block(int bx, int by, int color) {
    int sx = 2 + bx * 3;
    int sy = 1 + by * 3;
    drawPixel(sx, sy, color);
    drawPixel(sx + 1, sy, color);
    drawPixel(sx, sy + 1, color);
    drawPixel(sx + 1, sy + 1, color);
}

void drawNextPiece() {
    int r, c;
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            if (get_piece_bit(next_shape, 0, r, c)) {
                int sx = 48 + c * 3;
                int sy = 32 + r * 3;
                drawPixel(sx, sy, 1);
                drawPixel(sx + 1, sy, 1);
                drawPixel(sx, sy + 1, 1);
                drawPixel(sx + 1, sy + 1, 1);
            }
        }
    }
}

unsigned char check_collision(int px, int py, unsigned char r) {
    int r_idx, c_idx;
    for (r_idx = 0; r_idx < 4; r_idx++) {
        for (c_idx = 0; c_idx < 4; c_idx++) {
            if (get_piece_bit(current_shape, r, r_idx, c_idx)) {
                int board_x = px + c_idx;
                int board_y = py + r_idx;
                
                if (board_x < 0 || board_x >= 10 || board_y >= 15) return 1;
                if (board_y >= 0) {
                    if (game_board[board_y][board_x]) return 1;
                }
            }
        }
    }
    return 0;
}

void check_lines() {
    int r, c, target_r;
    int cleared = 0;
    for (r = 14; r >= 0; r--) {
        unsigned char full = 1;
        for (c = 0; c < 10; c++) {
            if (game_board[r][c] == 0) {
                full = 0;
                break;
            }
        }
        if (full) {
            cleared++;
            for (target_r = r; target_r > 0; target_r--) {
                for (c = 0; c < 10; c++) {
                    game_board[target_r][c] = game_board[target_r - 1][c];
                }
            }
            for (c = 0; c < 10; c++) game_board[0][c] = 0;
            r++; // Kiem tra lai hang hien tai sau khi truot xuong
        }
    }
    if (cleared > 0) score += cleared * 10;
}

void lock_piece() {
    int r, c;
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            if (get_piece_bit(current_shape, current_rotation, r, c)) {
                int board_x = piece_x + c;
                int board_y = piece_y + r;
                if (board_y >= 0 && board_y < 15) {
                    game_board[board_y][board_x] = 1;
                }
            }
        }
    }
}

void spawn_new_piece() {
    current_shape = next_shape;
    next_shape = rand() % 7;
    current_rotation = 0;
    piece_x = 3;
    piece_y = -2;
    if (check_collision(piece_x, 0, current_rotation)) {
        game_over = 1;
    }
}

// -------------------------------------------------------------------------
// 9. HÀM CHÍNH (MAIN LOOP)
// -------------------------------------------------------------------------
void main(void) {
    unsigned int drop_counter = 0;
    unsigned int input_delay = 0;
    unsigned int seed_val = 0;
    unsigned int drop_speed = 10;
    int i, r, c;

    // 1. // Khoi tao huong cong IO
    DDRG |= 0x1F;                               // PORTG 0->4 là Output cho LCD
    keypad_init();
    LCD_Init();

    // 2.Man hinh cho bat dau tro choi (Tao hat giong ngau nhien)
    displayClear();
    drawString(12, 12, "PRESS KEY");
    drawString(16, 24, "TO START");
    displayUpdate();

    while (1) {
        seed_val++; // Tang lien tuc de sinh so ngau nhien thuc su
        if (BT_LEFT || BT_RIGHT || BT_ROTATE) {
            srand(seed_val);
            delay_ms(200); // Tranh dinh phim khoi dau
            break;
        }
    }

    // 3. Khoi tao trang thai ban dau cua bang game
    for (r = 0; r < 15; r++) {
        for (c = 0; c < 10; c++) {
            game_board[r][c] = 0;
        }
    }
    score = 0;
    game_over = 0;
    next_shape = rand() % 7;
    spawn_new_piece();

    // 4. Vòng lap chính cua trò choi
    while (1) {
        seed_val++;

        // Xu LY KHI THUA CUOC
        if (game_over) {
            displayClear();
            drawString(15, 12, "GAME OVER");
            drawString(12, 24, "SCORE:");
            drawScore(48, 24, score);
            drawString(8, 38, "PRESS ROTATE");
            displayUpdate();

            if (BT_ROTATE) {// Nhan BT-3 de reset choi lai
                delay_ms(200);
                srand(seed_val);
                for (r = 0; r < 15; r++) {
                    for (c = 0; c < 10; c++) {
                        game_board[r][c] = 0;
                    }
                }
                score = 0;
                game_over = 0;
                next_shape = rand() % 7;
                spawn_new_piece();
            }
            delay_ms(50);
            continue;
        }

        // DOC NUT DIEU KHIEN (Can debounce bang input_delay)
        if (input_delay > 0) {
            input_delay--;
        }

        if (input_delay == 0) {

        if (BT_LEFT) {
            if (!check_collision(piece_x - 1, piece_y, current_rotation)) {
                piece_x--;
            }
            input_delay = 3;
        }

        if (BT_RIGHT) {
            if (!check_collision(piece_x + 1, piece_y, current_rotation)) {
                piece_x++;
        }
        input_delay = 3;
    }

    if (BT_ROTATE) {
        unsigned char next_rot = (current_rotation + 1) % 4;

        if (!check_collision(piece_x, piece_y, next_rot)) {
            current_rotation = next_rot;
        }

        input_delay = 4;
    }
}

// HAM ROI TU DO (Moi 500ms roi 1 o)
// Neu giu nut DROP thi roi nhanh
    if (BT_DROP)
        drop_speed = 2;
    else
        drop_speed = 10;

// HAM ROI TU DO (Moi 500ms roi 1 o)
    drop_counter++;

    if (drop_counter >= drop_speed) {
        drop_counter = 0;

        if (!check_collision(piece_x, piece_y + 1, current_rotation)) {
            piece_y++;
        } else {
            lock_piece();
            check_lines();
            spawn_new_piece();
        }
    }

       // Ve KHUNG CANH GAME (RENDER GAME)
        displayClear();

        // 1. Ve vach chia man hinh
        for (i = 0; i < 48; i++) drawPixel(33, i, 1);

        // 2. Ve thong tin chu ben phai
        drawString(36, 2, "SCORE:");
        drawScore(36, 11, score);
        drawString(36, 22, "NEXT:");
        drawNextPiece();

        // 3. Ve khung vien khu vuc choi (Trai, Phai, Duoi)
        for (i = 0; i < 46; i++) drawPixel(1, i, 1);
        for (i = 0; i < 46; i++) drawPixel(32, i, 1);
        for (i = 1; i <= 32; i++) drawPixel(i, 46, 1);

        // 4. Ve cac khoi gach da co dinh o day
        for (r = 0; r < 15; r++) {
            for (c = 0; c < 10; c++) {
                if (game_board[r][c]) draw_board_block(c, r, 1);
            }
        }

        // 5. Ve khoi gach hien tai dang roi
        for (r = 0; r < 4; r++) {
            for (c = 0; c < 4; c++) {
                if (get_piece_bit(current_shape, current_rotation, r, c)) {
                    int bx = piece_x + c;
                    int by = piece_y + r;
                    if (by >= 0 && by < 15) {
                        draw_board_block(bx, by, 1);
                    }
                }
            }
        }

        displayUpdate(); // Day toan bo du lieu ra man hinh Nokia 5110
        delay_ms(50);    // Toc do cap nhat khung hinh (50ms moi frame)
    }
}