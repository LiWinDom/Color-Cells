#define _title "Color Cells - beta 2.0"
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <ctime>
#include <SFML/Graphics.hpp>

//visible options
const uint8_t cellSize = 50; //size of each sell in pixels
const uint8_t fontSize = 20; //size of font in pixels
const uint32_t backgroundColor = 0xFFFFFFFF; //color of background in HEX format (0x11223344: 11 - red color, 22 - green color, 33 - blue color, 44 - alpha channel)
const uint32_t textColor = 0x000000FF; //color of text in HEX format (0x11223344: 11 - red color, 22 - green color, 33 - blue color, 44 - alpha channel)
const uint32_t borderColor = 0xA0A0A0FF; //color of borders in HEX format (0x11223344: 11 - red color, 22 - green color, 33 - blue color, 44 - alpha channel)
const uint8_t borderThinkness = 2; //border thinkness in pixels, set to 0, if you don't want to see borders

uint8_t screen = 1; //0 - game, 1 - main menu
std::vector<std::vector<uint8_t>> field, next;
std::pair<int8_t, int8_t> selected(-1, -1);
uint16_t score = 0, highscore = 0;
bool gameOver = false;
std::fstream file;
sf::Font font;
const uint8_t colors[7] = {195, 51, 15, 243, 207, 63, 227};
//                          |   |   |    |    |   |    |
//                         red  |  blue  | magenta|  orange
//                            green    yellow    cyan

void save() {
    file.open("data.ccd");
    if (!file.is_open()) {
        std::ofstream kek;
        kek.open("data.ccd");
        kek.close();
    }
    file << highscore << '\n';
    for (uint8_t i = 0; i < 9; ++i) {
        for (uint8_t j = 0; j < 9; ++j) {
            file << field[i][j] << ' ';
        }
        file << '\n';
    }
    file << score << ' ' << gameOver << '\n';
    for (uint8_t i = 0; i < 9; ++i) {
        for (uint8_t j = 0; j < 9; ++j) {
            file << next[i][j] << ' ';
        }
        file << '\n';
    }
    file.close();

    return;
}

bool load() {
    file.open("data.ccd");
    if (!file.is_open()) {
        std::ofstream kek;
        kek.open("data.ccd");
        kek.close();
        return false;
    }
    file >> highscore;
    for (uint8_t i = 0; i < 9; ++i) {
        for (uint8_t j = 0; j < 9; ++j) {
            file >> field[i][j];
        }
    }
    file >> score >> gameOver;
    for (uint8_t i = 0; i < 9; ++i) {
        for (uint8_t j = 0; j < 9; ++j) {
            file >> next[i][j];
        }
    }
    file.close();

    return true;
}

uint8_t countFreeCells() {
    uint8_t free = 0;

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (field[i][j] == 0) ++free;
        }
    }
    return free;
}

bool deleteLines(const uint8_t& movedX, const uint8_t& movedY) {
    uint8_t curX = movedX, curY = movedY;
    uint8_t totalCells = 0, curCells1 = 0, curCells2 = 0, curColor = field[movedX][movedY];

    // | up line
    if (curColor == 1) {
        while (curColor == 1) {
            if (field[curX][curY] == 0) break;
            curColor = field[curX][curY];
            if (curX == 0) break;
            --curX;
        }
    }
    curX = movedX;
    while (true) {
        if (field[curX][curY] != curColor && field[curX][curY] != 1) break;
        ++curCells1;
        if (curX == 0) break;
        --curX;
    }
    curX = movedX; curColor = field[movedX][movedY];
    // | down line
    if (curColor == 1) {
        while (curColor == 1) {
            if (field[curX][curY] == 0) break;
            curColor = field[curX][curY];
            if (curX == 8) break;
            ++curX;
        }
    }
    curX = movedX;
    while (true) {
        if (field[curX][curY] != curColor && field[curX][curY] != 1) break;
        ++curCells2;
        if (curX == 8) break;
        ++curX;
    }
    //counting | cells
    if (curCells1 + curCells2 - 1 >= 5) {
        totalCells += curCells1 + curCells2 - 1;
        for (uint8_t i = 1; i < curCells1; ++i) {
            field[movedX - i][movedY] = 0;
        }
        for (uint8_t i = 1; i < curCells2; ++i) {
            field[movedX + i][movedY] = 0;
        }
    }
    curCells1 = 0; curCells2 = 0; curX = movedX; curColor = field[movedX][movedY];

    // - left line
    if (curColor == 1) {
        while (curColor == 1) {
            if (field[curX][curY] == 0) break;
            curColor = field[curX][curY];
            if (curY == 0) break;
            --curY;
        }
    }
    curY = movedY;
    while (true) {
        if (field[curX][curY] != curColor && field[curX][curY] != 1) break;
        ++curCells1;
        if (curY == 0) break;
        --curY;
    }
    curY = movedY; curColor = field[movedX][movedY];
    // - right line
    if (curColor == 1) {
        while (curColor == 1) {
            if (field[curX][curY] == 0) break;
            curColor = field[curX][curY];
            if (curY == 8) break;
            ++curY;
        }
    }
    curY = movedY;
    while (true) {
        if (field[curX][curY] != curColor && field[curX][curY] != 1) break;
        ++curCells2;
        if (curY == 8) break;
        ++curY;
    }
    //couting - cells
    if (curCells1 + curCells2 - 1 >= 5) {
        totalCells += curCells1 + curCells2 - 1;
        for (uint8_t i = 1; i < curCells1; ++i) {
            field[movedX][movedY - i] = 0;
        }
        for (uint8_t i = 1; i < curCells2; ++i) {
            field[movedX][movedY + i] = 0;
        }
    }
    curCells1 = 0; curCells2 = 0; curY = movedY; curColor = field[movedX][movedY];

    // \ up left line
    if (curColor == 1) {
        while (curColor == 1) {
            if (field[curX][curY] == 0) break;
            curColor = field[curX][curY];
            if (curX == 0 || curY == 0) break;
            --curX; --curY;
        }
    }
    curX = movedX; curY = movedY;
    while (true) {
        if (field[curX][curY] != curColor && field[curX][curY] != 1) break;
        ++curCells1;
        if (curX == 0 || curY == 0) break;
        --curX; --curY;
    }
    curX = movedX; curY = movedY; curColor = field[movedX][movedY];
    // \ down right line
    if (curColor == 1) {
        while (curColor == 1) {
            if (field[curX][curY] == 0) break;
            curColor = field[curX][curY];
            if (curX == 8 || curY == 8) break;
            ++curX; ++curY;
        }
    }
    curX = movedX; curY = movedY;
    while (true) {
        if (field[curX][curY] != curColor && field[curX][curY] != 1) break;
        ++curCells2;
        if (curX == 8 || curY == 8) break;
        ++curX; ++curY;
    }
    //counting \ cells
    if (curCells1 + curCells2 - 1 >= 5) {
        totalCells += curCells1 + curCells2 - 1;
        for (uint8_t i = 1; i < curCells1; ++i) {
            field[movedX - i][movedY - i] = 0;
        }
        for (uint8_t i = 1; i < curCells2; ++i) {
            field[movedX + i][movedY + i] = 0;
        }
    }
    curCells1 = 0; curCells2 = 0; curX = movedX; curY = movedY; curColor = field[movedX][movedY];

    // / up right line
    if (curColor == 1) {
        while (curColor == 1) {
            if (field[curX][curY] == 0) break;
            curColor = field[curX][curY];
            if (curX == 0 || curY == 8) break;
            --curX; ++curY;
        }
    }
    curX = movedX; curY = movedY;
    while (true) {
        if (field[curX][curY] != curColor && field[curX][curY] != 1) break;
        ++curCells1;
        if (curX == 0 || curY == 8) break;
        --curX; ++curY;
    }
    curX = movedX; curY = movedY; curColor = field[movedX][movedY];
    // / down left line
    if (curColor == 1) {
        while (curColor == 1) {
            if (field[curX][curY] == 0) break;
            curColor = field[curX][curY];
            if (curX == 8 || curY == 0) break;
            ++curX; --curY;
        }
    }
    curX = movedX; curY = movedY;
    while (true) {
        if (field[curX][curY] != curColor && field[curX][curY] != 1) break;
        ++curCells2;
        if (curX == 8 || curY == 0) break;
        ++curX; --curY;
    }
    //counting / cells
    if (curCells1 + curCells2 - 1 >= 5) {
        totalCells += curCells1 + curCells2 - 1;
        for (uint8_t i = 1; i < curCells1; ++i) {
            field[movedX - i][movedY + i] = 0;
        }
        for (uint8_t i = 1; i < curCells2; ++i) {
            field[movedX + i][movedY - i] = 0;
        }
    }

    if (totalCells > 4) {
        score += 10;
        totalCells -= 5;
        for (uint8_t i = 1; i <= totalCells; ++i) {
            score += i * 3;
        }
        field[movedX][movedY] = 0;
        highscore = std::max(highscore, score);
        save();
        return true;
    }
    return false;
}

void generateCells(const uint8_t& toGen = 3) {
    uint8_t left = countFreeCells();
    for (uint8_t k = 0; k < std::min(toGen, left); ++k) {
        do {
            uint8_t genX = std::rand() % 9, genY = std::rand() % 9;
            if (field[genX][genY] == 0 && next[genX][genY] == 0) {
                if (!(std::rand() % 30)) next[genX][genY] = 1;
                else next[genX][genY] = colors[std::rand() % 7];
                break;
            }
        }
        while (true);
    }
    int kek = 1;
    return;
}

bool addCells(const uint8_t& toAdd = 3) {
    uint8_t left = countFreeCells();
    for (uint8_t i = 0; i < 9; ++i) {
        for (uint8_t j = 0; j < 9; ++j) {
            if (next[i][j]) {
                if (field[i][j]) {
                    do {
                        uint8_t addX = std::rand() % 9, addY = std::rand() % 9;
                        if (field[addX][addY] == 0) {
                            field[addX][addY] = next[i][j];
                            deleteLines(addX, addY);
                            break;
                        }
                    }
                    while (true);
                }
                else {
                    field[i][j] = next[i][j];
                    deleteLines(i, j);
                }
                next[i][j] = 0;
            }
        }
    }
    if (left <= toAdd) {
        return false;
    }
    generateCells(std::min(left, toAdd));
    return true;
}

void reload() {
    field.assign(9, std::vector<uint8_t>(9, 0));
    next.assign(9, std::vector<uint8_t>(9, 0));
    gameOver = false;
    score = 0;
    generateCells();
    addCells();

    return;
}

void startup(sf::RenderWindow& window) {
    font.loadFromFile("resourses/Chava.ttf");
    sf::Image icon;
    icon.loadFromFile("resourses/grafics/icon.png");
    window.setIcon(48, 48, icon.getPixelsPtr());
    window.setVerticalSyncEnabled(true);
    std::srand(std::time(nullptr));
    reload();

    return;
}

void liAlg(std::vector<std::vector<int8_t>>& liField, const uint8_t& cellX, const uint8_t& cellY, const uint8_t& x, const uint8_t& y, const uint8_t counter = 0) {
    liField[x][y] = counter;
    if (x > 0) {
        if ((field[x - 1][y] == 0 || (x - 1 == cellX && y == cellY)) && (liField[x - 1][y] > counter + 1 || liField[x - 1][y] == -1)) {
            liAlg(liField, cellX, cellY, x - 1, y, counter + 1);
        }
    }
    if (x < 8) {
        if ((field[x + 1][y] == 0 || (x + 1 == cellX && y == cellY)) && (liField[x + 1][y] > counter + 1 || liField[x + 1][y] == -1)) {
            liAlg(liField, cellX, cellY, x + 1, y, counter + 1);
        }
    }
    if (y > 0) {
        if ((field[x][y - 1] == 0 || (x == cellX && y - 1 == cellY)) && (liField[x][y - 1] > counter + 1 || liField[x][y - 1] == -1)) {
            liAlg(liField, cellX, cellY, x, y - 1, counter + 1);
        }
    }
    if (y < 8) {
        if ((field[x][y + 1] == 0 || (x == cellX && y + 1 == cellY)) && (liField[x][y + 1] > counter + 1 || liField[x][y + 1] == -1)) {
            liAlg(liField, cellX, cellY, x, y + 1, counter + 1);
        }
    }
    return;
}

void drawBorders(sf::RenderWindow& window) {
    for (int i = 0; i <= 9; ++i) {
        sf::RectangleShape rect1;
        rect1.setOutlineThickness(0);
        rect1.setSize(sf::Vector2f((9 * cellSize) + 10 * borderThinkness, borderThinkness));
        rect1.setPosition(0, cellSize * i + i * borderThinkness);
        rect1.setFillColor(sf::Color(borderColor));
        window.draw(rect1);

        sf::RectangleShape rect2;
        rect2.setOutlineThickness(0);
        rect2.setSize(sf::Vector2f(borderThinkness, (9 * cellSize) + 10 * borderThinkness));
        rect2.setPosition(cellSize * i + i * borderThinkness, 0);
        rect2.setFillColor(sf::Color(borderColor));
       window.draw(rect2);
    }
    return;
}

uint32_t convertColor(const uint8_t& color) {
    if (color == 1) return 0x808080FF;
    uint8_t rColor, gColor, bColor, tColor;

    rColor = ((color >> 6) & 3) * 64;
    gColor = ((color >> 4) & 3) * 64;
    bColor = ((color >> 2) & 3) * 64;
    tColor = (color & 3) * 85;

    return (rColor << 24) + (gColor << 16) + (bColor << 8) + tColor;
}

void showField(sf::RenderWindow& window) {
    for (uint8_t i = 0; i < 9; ++i) {
        for (uint8_t j = 0; j < 9; ++j) {
            uint32_t color = convertColor(field[i][j]);

            sf::RectangleShape rect;
            if (i == selected.first && j == selected.second) {
                rect.setSize(sf::Vector2f(cellSize - borderThinkness * 6, cellSize - borderThinkness * 6));
                rect.setPosition(j * cellSize + (j + 4) * borderThinkness, i * cellSize + (i + 4) * borderThinkness);
            }
            else if (next[i][j]) {
                color = convertColor(next[i][j]);
                rect.setSize(sf::Vector2f(cellSize - borderThinkness * 18, cellSize - borderThinkness * 18));
                rect.setPosition(j * cellSize + (j + 10) * borderThinkness, i * cellSize + (i + 10) * borderThinkness);
            }
            else {
                rect.setSize(sf::Vector2f(cellSize - borderThinkness * 2, cellSize - borderThinkness * 2));
                rect.setPosition(j * cellSize + (j + 2) * borderThinkness, i * cellSize + (i + 2) * borderThinkness);
            }
            rect.setOutlineThickness(0);
            rect.setFillColor(sf::Color(color));
            window.draw(rect);
        }
    }
    return;
}

void showMainMenu(sf::RenderWindow& window) {
    sf::Text newText, loadText;
    newText.setFont(font);
    loadText.setFont(font);
    newText.setCharacterSize(fontSize * 2);
    loadText.setCharacterSize(fontSize * 2);
    newText.setFillColor(sf::Color(textColor));
    loadText.setFillColor(sf::Color(textColor));
    newText.setString("New game");
    loadText.setString("Load last game");

    newText.setPosition(((9 * cellSize) + 10 * borderThinkness - newText.getLocalBounds().width) / 2, ((9 * cellSize) + 10 * borderThinkness + (fontSize * 2.75) - newText.getLocalBounds().height * 5) / 2);
    loadText.setPosition(((9 * cellSize) + 10 * borderThinkness - loadText.getLocalBounds().width) / 2 - 0.5, ((9 * cellSize) + 10 * borderThinkness + (fontSize * 2.75) + loadText.getLocalBounds().height * 3) / 2);
    window.draw(newText);
    window.draw(loadText);

    return;
}

void showStatus(sf::RenderWindow& window, const std::string& first, const std::string& second) {
    sf::Text scoreText, highscoreText;
    scoreText.setFont(font);
    highscoreText.setFont(font);
    scoreText.setCharacterSize(fontSize);
    highscoreText.setCharacterSize(fontSize);
    scoreText.setFillColor(sf::Color(textColor));
    highscoreText.setFillColor(sf::Color(textColor));
    scoreText.setString(first);
    highscoreText.setString(second);

    scoreText.setPosition(fontSize * 0.25, (9 * cellSize) + 9 * borderThinkness + fontSize * 0.25);
    highscoreText.setPosition(fontSize * 0.25, (9 * cellSize) + 9 * borderThinkness + fontSize * 1.5);
    window.draw(scoreText);
    window.draw(highscoreText);

    return;
}

void display(sf::RenderWindow& window) {
    window.clear(sf::Color(backgroundColor));
    switch (screen) {
    case 0:
        drawBorders(window);
        showField(window);

        if (gameOver) showStatus(window, "You lose!", "Your score: " + std::to_string(score));
        else showStatus(window, "Your score: " + std::to_string(score), "Highscore: " + std::to_string(highscore));
        break;
    case 1:
        showMainMenu(window);
        break;
    }
    window.display();

    return;
}

bool moveCell(sf::RenderWindow& window, const uint8_t& x1, const uint8_t& y1, const uint8_t& x2, const uint8_t& y2) {
    std::vector<std::vector<int8_t>> liField(9, std::vector<int8_t>(9, -1));
    liAlg(liField, x1, y1, x2, y2);
    if (liField[x1][y1] == -1) return false;

    uint8_t curX = x1, curY = y1, left = liField[x1][y1];
    while (curX != x2 || curY != y2) {
        uint8_t cell = field[curX][curY];
        field[curX][curY] = 0;
        --left;

        if (curX > 0) {
            if (liField[curX - 1][curY] == left) {
                --curX;
            }
        }
        if (curX < 8) {
            if (liField[curX + 1][curY] == left) {
                ++curX;
            }
        }
        if (curY > 0) {
            if (liField[curX][curY - 1] == left) {
                --curY;
            }
        }
        if (curY < 8) {
            if (liField[curX][curY + 1] == left) {
                ++curY;
            }
        }

        field[curX][curY] = cell;
        selected.first = curX;
        selected.second = curY;
        display(window);
        sf::sleep(sf::milliseconds(60));
    }

    return true;
}

void clickEvent(sf::RenderWindow& window) {
    sf::Vector2i pos = sf::Mouse::getPosition(window);

    switch (screen) {
    case 0:
        if (pos.x > 0 && pos.y > 0) {
            if (pos.x < (9 * cellSize) + (9 * borderThinkness) && pos.y < (9 * cellSize) + (9 * borderThinkness)) {
                uint16_t fx = pos.y / (cellSize + borderThinkness), fy = pos.x / (cellSize + borderThinkness);
                if (fx == selected.first && fy == selected.second) {
                    selected.first = -1;
                    selected.second = -1;
                }
                else if (field[fx][fy] != 0) {
                    selected.first = fx;
                    selected.second = fy;
                }
                else if (selected.first != -1 && selected.second != -1) {
                    if (moveCell(window, selected.first, selected.second, fx, fy)) {
                        if (!deleteLines(fx, fy)) {
                            if (!addCells()) gameOver = true;
                        }
                    }
                    selected.first = -1;
                    selected.second = -1;
                    save();
                }
            }
        }
        break;
    case 1:
        if (pos.y >= ((9 * cellSize) + 10 * borderThinkness + (fontSize * 2.75)) / 2) {
            load();
        }
        screen = 0;
        break;
    }
}

void gameEventProcessing(sf::RenderWindow& window, sf::Event& event) {
    static bool isMousePressed = false;

    if (event.type == sf::Event::Closed) window.close();
    else if (event.type == sf::Event::KeyPressed) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            reload();
        }
        return;
    }
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (isMousePressed) return;
        clickEvent(window);
        isMousePressed = true;
    }
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        if (isMousePressed) return;
        selected.first = -1;
        selected.second = -1;
        isMousePressed = true;
    }
    else if (event.type == sf::Event::MouseButtonReleased) isMousePressed = false;

    return;
}

int main() {
    sf::RenderWindow gameWindow(sf::VideoMode((9 * cellSize) + 10 * borderThinkness, (9 * cellSize) + 10 * borderThinkness + (fontSize * 2.75)), _title, sf::Style::Close);
    startup(gameWindow);

    while (gameWindow.isOpen()) {
        sf::Event gameEvent;
        while (gameWindow.pollEvent(gameEvent)) gameEventProcessing(gameWindow, gameEvent);

        display(gameWindow);
    }

    return 0;
}