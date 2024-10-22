#include <SDL2/SDL.h>
#include <stdio.h>
#define screenWidth 300
#define screenHeight 360
#define Square 15
#define mapW 10
#define mapH 20
#define mapOffsetX 75
#define mapOffsetY 50
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
uint32_t pixels[screenHeight * screenWidth] = {0}; // a, b, g, r em ordem de maior bit
SDL_bool map[mapH + 1][mapW];
int points = 0;
int totalLinhasCompletas = 0;
const SDL_bool lettersDisplay[26][7] = {{1, 1, 1, 1, 1, 1, 0}, {1, 1, 1, 1, 1, 1, 1}, {1, 1, 0, 0, 1, 0, 1}, {1, 1, 1, 0, 1, 1, 1}, {1, 1, 0, 1, 1, 0, 1}, {1, 1, 0, 1, 1, 0, 0}, {1, 1, 0, 0, 1, 1, 1}, {0, 1, 1, 1, 1, 1, 0}, {0, 0, 1, 0, 0, 1, 0}, {1, 0, 1, 0, 1, 1, 1}, {1, 0, 0, 1, 1, 0, 1}, {0, 1, 0, 0, 1, 0, 1}, {1, 0, 0, 1, 1, 1, 0}, {0, 0, 0, 1, 1, 1, 0}, {0, 0, 0, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 0, 0}, {1, 1, 1, 1, 0, 1, 0}, {1, 1, 0, 0, 1, 0, 0}, {1, 1, 0, 1, 0, 1, 1}, {1, 0, 1, 0, 0, 1, 0}, {0, 1, 1, 0, 1, 1, 1}, {1, 0, 0, 0, 1, 1, 1}, {0, 1, 1, 1, 1, 1, 1}, {0, 1, 1, 0, 1, 1, 0}, {0, 1, 1, 1, 0, 1, 1}, {1, 0, 0, 1, 0, 0, 1}};
const SDL_bool numbersDisplay[10][7] = {{1, 1, 1, 0, 1, 1, 1}, {0, 0, 1, 0, 0, 1, 0}, {1, 0, 1, 1, 1, 0, 1}, {1, 0, 1, 1, 0, 1, 1}, {0, 1, 1, 1, 0, 1, 0}, {1, 1, 0, 1, 0, 1, 1}, {1, 1, 0, 1, 1, 1, 1}, {1, 0, 1, 0, 0, 1, 0}, {1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 0, 1, 1}};
const uint32_t colorBlock[7] = {0x00FFFF00, 0x0000FFFF, 0x00F800F8, 0x0000FF00, 0x000000FF, 0x00FF0000, 0x00F070FF};
int squaresPositions[4][2] = {0};
int mostBounds[4][2] = {{1, 1}, {0, 1}, {1, 1}, {1, 0}};
int exceptionsBounds[2][4][2] = {{{2, 1}, {0, 0}, {2, 1}, {1, -1}}, {{1, 0}, {1, 0}, {1, 0}, {1, 0}}};

void clearScreen()
{
    for (int i = 0; i < screenHeight; i++)
    {
        for (int j = 0; j < screenWidth; j++)
        {
            pixels[j + i * screenWidth] = 0x00000000;
        }
    }
}
void renderLeter(char *string, int x, int y, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (string[i] != ' ')
        {
            drawLetter(x + i * 8, y, string[i] - 'a');
        }
    }
}
void drawLetter(int x, int y, uint8_t letter)
{
    drawLineX(x + 1, y, lettersDisplay[letter][0] * 0x00FFFFFF, 5);
    drawLineY(x, y + 1, lettersDisplay[letter][1] * 0x00FFFFFF, 5);
    drawLineY(x + 6, y + 1, lettersDisplay[letter][2] * 0x00FFFFFF, 5);
    drawLineX(x + 1, y + 1 + 6, lettersDisplay[letter][3] * 0x00FFFFFF, 5);
    drawLineY(x, y + 1 + 7, lettersDisplay[letter][4] * 0x00FFFFFF, 5);
    drawLineY(x + 6, y + 1 + 7, lettersDisplay[letter][5] * 0x00FFFFFF, 5);
    drawLineX(x + 1, y + 13, lettersDisplay[letter][6] * 0x00FFFFFF, 5);
}
void renderNumber(int n, int x, int y)
{
    if (n > 0)
    {
        renderNumber(n / 10, x - 9, y);
        drawNumber(x, y, n % 10);
    }
}
void drawNumber(int x, int y, uint8_t number)
{
    drawLineX(x + 1, y, numbersDisplay[number][0] * 0x00FFFFFF, 5);
    drawLineY(x, y + 1, numbersDisplay[number][1] * 0x00FFFFFF, 5);
    drawLineY(x + 6, y + 1, numbersDisplay[number][2] * 0x00FFFFFF, 5);
    drawLineX(x + 1, y + 1 + 6, numbersDisplay[number][3] * 0x00FFFFFF, 5);
    drawLineY(x, y + 1 + 7, numbersDisplay[number][4] * 0x00FFFFFF, 5);
    drawLineY(x + 6, y + 1 + 7, numbersDisplay[number][5] * 0x00FFFFFF, 5);
    drawLineX(x + 1, y + 13, numbersDisplay[number][6] * 0x00FFFFFF, 5);
}
void drawLineX(int x, int y, uint32_t color, int size)
{
    for (int i = x; i < x + size; i++)
    {
        pixels[i + y * screenWidth] = color;
    }
}
void drawLineY(int x, int y, uint32_t color, int size)
{
    for (int i = y; i < y + size; i++)
    {
        pixels[x + i * screenWidth] = color;
    }
}
void render()
{
    int texture_pitch = 0;
    void *texture_pixels = NULL;

    if (SDL_LockTexture(texture, NULL, &texture_pixels, &texture_pitch) != 0)
    {
        SDL_Log("Unable to lock texture: %s", SDL_GetError());
    }
    else
    {
        memcpy(texture_pixels, pixels, texture_pitch * screenHeight);
    }
    SDL_UnlockTexture(texture);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}
void renderSquaresMap(uint32_t color)
{
    if (color)
    {
        for (short i = 0; i < 4; i++)
        {
            drawSquare(squaresPositions[i][0], squaresPositions[i][1], color);
            map[squaresPositions[i][1]][squaresPositions[i][0]] = 1;
        }
    }
    else
    {
        for (short i = 0; i < 4; i++)
        {
            drawSquare(squaresPositions[i][0], squaresPositions[i][1], color);
            map[squaresPositions[i][1]][squaresPositions[i][0]] = 0;
        }
    }
}
void renderSquares(uint32_t color)
{
    for (short i = 0; i < 4; i++)
    {
        drawSquare(squaresPositions[i][0], squaresPositions[i][1], color);
    }
}
void drawSquare(int x, int y, uint32_t color) // top left
{
    x *= Square;
    x += mapOffsetX;
    y *= Square;
    y += mapOffsetY;
    for (int i = 0; i < Square * Square; i++)
    {
        pixels[x + i % Square + (y + i / Square) * screenWidth] = color;
    }
}
void lineClear(int y)
{
    for (int i = y - 1; i >= 0; i -= 1)
    {
        copyMapLine(i + 1, i);
        for (int j = 0; j < Square; j++)
        {
            copyLinePixel((i + 1) * Square + j, i * Square + j);
        }
    }
    for (int i = 0; i < mapW; i++)
    {
        map[0][i] = 0;
    }
    for (int i = 0; i < Square; i++)
    {
        for (int j = mapOffsetX; j < mapW * Square; j++)
        {
            pixels[j + (i + mapOffsetY) * screenWidth] = 0x00000000;
        }
    }
}
void copyMapLine(int yD, int yO)
{
    for (int i = 0; i < mapW; i++)
    {
        map[yD][i] = map[yO][i];
    }
}
void copyLinePixel(int yD, int yO)
{ // coordenadas map
    for (int i = 0; i < mapW * Square; i++)
    {
        pixels[(yD + mapOffsetY) * screenWidth + i + mapOffsetX] = pixels[(yO + mapOffsetY) * screenWidth + i + mapOffsetX];
    }
}
void checkLineClear()
{
    SDL_bool lineFull;
    int linesCleared = 0;
    for (int i = mapH - 1; i >= 0; i--)
    {
        lineFull = 1;
        for (int j = 0; j < mapW; j++)
        {
            if (map[i][j] == 0)
            {
                lineFull = 0;
                break;
            }
        }
        if (lineFull)
        {
            lineClear(i);
            //  points++; 1 ponto por linha
            linesCleared++;
            i++;
        }
    }
    points += 5 * (linesCleared + 1) * linesCleared / 2.0; // soma de pa base 5
    totalLinhasCompletas += linesCleared;
}
SDL_bool verificarColissao()
{
    for (int i = 0; i < 4; i++)
    {
        if (map[squaresPositions[i][1]][squaresPositions[i][0]])
        {
            return 1;
        }
    }
    return 0;
}
SDL_bool fixarBloco()
{
    if (verificarColissao())
    {
        while (verificarColissao())
        {
            for (int i = 0; i < 4; i++)
            {
                squaresPositions[i][1]--;
            }
        }
        return 1;
    }
    return 0;
}
SDL_bool checkLoss()
{
    for (int i = 0; i < 4; i++)
    {
        if (squaresPositions[i][1] < 0)
        {
            return 1;
        }
    }
    return 0;
}
void bI(int x, int y, uint8_t r) // canto do 2 3 bloco
{
    if (r % 2)
    {
        if (r == 3)
        {
            x -= 1;
        }
        squaresPositions[0][0] = x;
        squaresPositions[0][1] = y;
        squaresPositions[1][0] = x;
        squaresPositions[1][1] = y - 1;
        squaresPositions[2][0] = x;
        squaresPositions[2][1] = y - 2;
        squaresPositions[3][0] = x;
        squaresPositions[3][1] = y + 1;
        /*    drawSquare(x, y, color);
            drawSquare(x, y - Square, color);
            drawSquare(x, y - 2 * Square, color);
            drawSquare(x, y + Square, color); */
    }
    else
    {
        if (r == 0)
        {
            y -= 1;
        }
        squaresPositions[0][0] = x;
        squaresPositions[0][1] = y;
        squaresPositions[1][0] = x - 1;
        squaresPositions[1][1] = y;
        squaresPositions[2][0] = x - 2;
        squaresPositions[2][1] = y;
        squaresPositions[3][0] = x + 1;
        squaresPositions[3][1] = y;
        /*  drawSquare(x, y, color);
          drawSquare(x - Square, y, color);
          drawSquare(x - 2 * Square, y, color);
          drawSquare(x + Square, y, color); */
    }
}

void bJ(int x, int y, uint8_t r) // canto do centro
{
    int8_t sign = 1;
    if (r >= 2)
    {
        sign = -1;
    }
    if (r % 2)
    {
        squaresPositions[0][0] = x;
        squaresPositions[0][1] = y - 1;
        squaresPositions[1][0] = x;
        squaresPositions[1][1] = y + 1;
        squaresPositions[2][0] = x;
        squaresPositions[2][1] = y;
        squaresPositions[3][0] = x + 1 * sign;
        squaresPositions[3][1] = y - 1 * sign;
        /*  drawSquare(x, y, color);
          drawSquare(x, y - Square, color);
          drawSquare(x, y + Square, color);
          drawSquare(x + Square * sign, y - Square * sign, color);*/
    }
    else
    {
        squaresPositions[0][0] = x;
        squaresPositions[0][1] = y;
        squaresPositions[1][0] = x - 1;
        squaresPositions[1][1] = y;
        squaresPositions[2][0] = x + 1;
        squaresPositions[2][1] = y;
        squaresPositions[3][0] = x - 1 * sign;
        squaresPositions[3][1] = y - 1 * sign;
        /* drawSquare(x, y, color);
         drawSquare(x - Square, y, color);
         drawSquare(x + Square, y, color);
         drawSquare(x - Square * sign, y - Square * sign, color);*/
    }
}

void bL(int x, int y, uint8_t r) // canto do centro
{
    int8_t sign = 1;
    if (r >= 2)
    {
        sign = -1;
    }
    if (r % 2)
    {
        squaresPositions[0][0] = x;
        squaresPositions[0][1] = y;
        squaresPositions[1][0] = x;
        squaresPositions[1][1] = y - 1;
        squaresPositions[2][0] = x;
        squaresPositions[2][1] = y + 1;
        squaresPositions[3][0] = x + 1 * sign;
        squaresPositions[3][1] = y + 1 * sign;
        /*drawSquare(x, y, color);
        drawSquare(x, y - Square, color);
        drawSquare(x, y + Square, color);
        drawSquare(x + Square * sign, y + Square * sign, color);*/
    }
    else
    {
        squaresPositions[0][0] = x;
        squaresPositions[0][1] = y;
        squaresPositions[1][0] = x - 1;
        squaresPositions[1][1] = y;
        squaresPositions[2][0] = x + 1;
        squaresPositions[2][1] = y;
        squaresPositions[3][0] = x + 1 * sign;
        squaresPositions[3][1] = y - 1 * sign;
        /*  drawSquare(x, y, color);
          drawSquare(x - Square, y, color);
          drawSquare(x + Square, y, color);
          drawSquare(x + Square * sign, y - Square * sign, color);*/
    }
}

void bO(int x, int y, uint8_t r) // canto do rd
{
    squaresPositions[0][0] = x;
    squaresPositions[0][1] = y;
    squaresPositions[1][0] = x - 1;
    squaresPositions[1][1] = y;
    squaresPositions[2][0] = x - 1;
    squaresPositions[2][1] = y - 1;
    squaresPositions[3][0] = x;
    squaresPositions[3][1] = y - 1;
    /*drawSquare(x, y, color);
    drawSquare(x - Square, y, color);
    drawSquare(x - Square, y - Square, color);
    drawSquare(x, y - Square, color);*/
}

void bS(int x, int y, uint8_t r) // canto do centro
{
    int8_t sign = 1;
    if (r >= 2)
    {
        sign = -1;
    }
    if (r % 2)
    {
        squaresPositions[0][0] = x;
        squaresPositions[0][1] = y;
        squaresPositions[1][0] = x;
        squaresPositions[1][1] = y - 1 * sign;
        squaresPositions[2][0] = x + 1 * sign;
        squaresPositions[2][1] = y;
        squaresPositions[3][0] = x + 1 * sign;
        squaresPositions[3][1] = y + 1 * sign;
        /*drawSquare(x, y, color);
        drawSquare(x, y - Square * sign, color);
        drawSquare(x + Square * sign, y, color);
        drawSquare(x + Square * sign, y + Square * sign, color);*/
    }
    else
    {
        squaresPositions[0][0] = x;
        squaresPositions[0][1] = y;
        squaresPositions[1][0] = x - 1 * sign;
        squaresPositions[1][1] = y;
        squaresPositions[2][0] = x;
        squaresPositions[2][1] = y - 1 * sign;
        squaresPositions[3][0] = x + 1 * sign;
        squaresPositions[3][1] = y - 1 * sign;
        /*drawSquare(x, y, color);
        drawSquare(x - Square * sign, y, color);
        drawSquare(x, y - Square * sign, color);
        drawSquare(x + Square * sign, y - Square * sign, color);*/
    }
}

void bT(int x, int y, uint8_t r) // canto do centro
{
    int8_t sign = 1;
    if (r >= 2)
    {
        sign = -1;
    }
    if (r % 2)
    {
        squaresPositions[0][0] = x;
        squaresPositions[0][1] = y;
        squaresPositions[1][0] = x;
        squaresPositions[1][1] = y - 1 * sign;
        squaresPositions[2][0] = x + 1 * sign;
        squaresPositions[2][1] = y;
        squaresPositions[3][0] = x;
        squaresPositions[3][1] = y + 1 * sign;
        /*drawSquare(x, y, color);
        drawSquare(x, y - Square * sign, color);
        drawSquare(x + Square * sign, y, color);
        drawSquare(x, y + Square * sign, color);*/
    }
    else
    {
        squaresPositions[0][0] = x;
        squaresPositions[0][1] = y;
        squaresPositions[1][0] = x - 1 * sign;
        squaresPositions[1][1] = y;
        squaresPositions[2][0] = x;
        squaresPositions[2][1] = y - 1 * sign;
        squaresPositions[3][0] = x + 1 * sign;
        squaresPositions[3][1] = y;
        /*drawSquare(x, y, color);
        drawSquare(x - Square * sign, y, color);
        drawSquare(x, y - Square * sign, color);
        drawSquare(x + Square * sign, y, color);*/
    }
}

void bZ(int x, int y, uint8_t r) // canto do centro
{
    int8_t sign = 1;
    if (r >= 2)
    {
        sign = -1;
    }
    if (r % 2)
    {
        squaresPositions[0][0] = x;
        squaresPositions[0][1] = y;
        squaresPositions[1][0] = x;
        squaresPositions[1][1] = y + 1 * sign;
        squaresPositions[2][0] = x + 1 * sign;
        squaresPositions[2][1] = y;
        squaresPositions[3][0] = x + 1 * sign;
        squaresPositions[3][1] = y - 1 * sign;
        /*drawSquare(x, y, color);
        drawSquare(x, y + Square * sign, color);
        drawSquare(x + Square * sign, y, color);
        drawSquare(x + Square * sign, y - Square * sign, color);*/
    }
    else
    {
        squaresPositions[0][0] = x;
        squaresPositions[0][1] = y;
        squaresPositions[1][0] = x + 1 * sign;
        squaresPositions[1][1] = y;
        squaresPositions[2][0] = x;
        squaresPositions[2][1] = y - 1 * sign;
        squaresPositions[3][0] = x - 1 * sign;
        squaresPositions[3][1] = y - 1 * sign;
        /*drawSquare(x, y, color);
        drawSquare(x + Square * sign, y, color);
        drawSquare(x, y - Square * sign, color);
        drawSquare(x - Square * sign, y - Square * sign, color);*/
    }
}

void (*block[7])(int, int, uint8_t) = {bI, bJ, bL, bO, bS, bT, bZ}; // vetor das funciois

int main()
{
    /* for (int i = 0; i < screenHeight; i++)
     {
         pixels[i * screenWidth + 74] = 0x00FFFFFF;
         pixels[i * screenWidth + 225] = 0x00FFFFFF;
     }*/
    // drawLineY(74, mapOffsetY, 0x00FFFFFF, 300);
    // drawLineY(225, mapOffsetY, 0x00FFFFFF, 300);
    /* for (int i = 0; i < screenWidth; i++)
      {
          pixels[(mapOffsetY + mapH * Square) * screenWidth + i] = 0x00FFFFFF;
          pixels[(mapOffsetY - 1) * screenWidth + i] = 0x00FFFFFF;
      }*/
    //  drawLineX(mapOffsetX, mapOffsetY + mapH * Square, 0x00FFFFFF, 150);
    //  drawLineX(mapOffsetX, mapOffsetY - 1, 0x00FFFFFF, 150);
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, screenWidth, screenHeight);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);

    for (int i = 0; i < mapW; i++)
    {
        map[mapH][i] = 1;
    }
    renderLeter("josebeco", 118, 160, 8);
    renderLeter("tetris", 126, 180, 6);
    render();
    clearScreen();
    SDL_Delay(3000);
    drawLineY(74, mapOffsetY, 0x00FFFFFF, 300);
    drawLineY(225, mapOffsetY, 0x00FFFFFF, 300);
    drawLineX(mapOffsetX, mapOffsetY + mapH * Square, 0x00FFFFFF, 150);
    drawLineX(mapOffsetX, mapOffsetY - 1, 0x00FFFFFF, 150);
    SDL_Event event;
    uint64_t currentTime = 0;
    uint64_t difTime = 0;
    srand(SDL_GetTicks64());
    short activeBlock = rand() % 7;
    short nextBlock = rand() % 7;
    (*block[nextBlock])(12, 5, 1);
    renderSquares(colorBlock[nextBlock]);
    int x = 5;
    int y = 1;
    uint8_t r = 0;
    int cooldownTime = 0;
    uint64_t cooldownBlock = SDL_GetTicks64();
    while (1)
    {
        currentTime = SDL_GetTicks64();
        if (currentTime - cooldownBlock >= 500 - cooldownTime) // add cooldown time if wanted
        {
            cooldownBlock = SDL_GetTicks64();
            y++;
        }
        (*block[activeBlock])(x, y, r);
        if (fixarBloco())
        {
            if (checkLoss())
            {
                clearScreen();
                renderLeter("game over", 116, 160, 9);
                renderNumber(points, 212, 25);
                renderNumber(totalLinhasCompletas, 100, 25);
                render();
                SDL_Delay(5000);
                break;
            }
            renderSquaresMap(colorBlock[activeBlock]);
            render();
            checkLineClear();
            renderNumber(points, 212, 25);
            renderNumber(totalLinhasCompletas, 100, 25);
            cooldownTime = totalLinhasCompletas * 10;
            if (cooldownTime > 300)
            {
                cooldownTime = 300;
            }
            x = 5;
            y = 1;
            r = 0;
            (*block[nextBlock])(12, 5, 1);
            renderSquares(0x00000000);
            activeBlock = nextBlock;
            nextBlock = rand() % 7;
            (*block[nextBlock])(12, 5, 1);
            renderSquares(colorBlock[nextBlock]);
        }
        else
        {
            renderSquaresMap(colorBlock[activeBlock]);
            render();
            renderSquaresMap(0x00000000);
        }
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_LEFT:
                x -= 1;
                if (activeBlock == 0 || activeBlock == 3)
                {
                    if (x < exceptionsBounds[activeBlock % 2][r][0])
                    {
                        x = exceptionsBounds[activeBlock % 2][r][0];
                    }
                }
                else if (x < mostBounds[r][0])
                {
                    x = mostBounds[r][0];
                }
                break;
            case SDLK_RIGHT:
                x += 1;
                if (activeBlock == 0 || activeBlock == 3)
                {
                    if (x >= mapW - exceptionsBounds[activeBlock % 2][r][1])
                    {
                        x = mapW - 1 - exceptionsBounds[activeBlock % 2][r][1];
                    }
                }
                else if (x >= mapW - mostBounds[r][1])
                {
                    x = mapW - 1 - mostBounds[r][1];
                }
                break;
            case SDLK_r:
                r += 1;
                r %= 4;
                if (activeBlock == 0 || activeBlock == 3)
                {
                    if (x < exceptionsBounds[activeBlock % 2][r][0])
                    {
                        x = exceptionsBounds[activeBlock % 2][r][0];
                    }
                    else if (x >= mapW - exceptionsBounds[activeBlock % 2][r][1])
                    {
                        x = mapW - 1 - exceptionsBounds[activeBlock % 2][r][1];
                    }
                }
                else if (x >= mapW - mostBounds[r][1])
                {
                    x = mapW - 1 - mostBounds[r][1];
                }
                else if (x < mostBounds[r][0])
                {
                    x = mostBounds[r][0];
                }
                break;
            case SDLK_DOWN:
                y++;
                break;
            case SDLK_p:
                difTime = currentTime - cooldownBlock;
                do
                {
                    SDL_PollEvent(&event);
                } while (event.type != SDL_KEYDOWN && event.key.keysym.sym != SDLK_g);
                currentTime = SDL_GetTicks64();
                cooldownBlock = currentTime - difTime;
                break;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);

    return 0;
}