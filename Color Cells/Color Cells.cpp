#define _title "Color Cells - ver. 0.42"
#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <SFML/Graphics.hpp>

//visible options
const uint8_t cellSize = 50; //size of each sell in pixels
const uint8_t fontSize = 20; //size of font in pixels
const uint32_t backgroundColor = 0xFFFFFFFF; //color of background in HEX format (0x11223344: 11 - red color, 22 - green color, 33 - blue color, 44 - alpha channel)
const uint32_t textColor = 0x000000FF; //color of text in HEX format (0x11223344: 11 - red color, 22 - green color, 33 - blue color, 44 - alpha channel)
const uint32_t borderColor = 0xA0A0A0FF; //color of borders in HEX format (0x11223344: 11 - red color, 22 - green color, 33 - blue color, 44 - alpha channel)
const uint8_t borderThinkness = 2; //border thinkness in pixels, set to 0, if you don't want to see borders

std::vector<std::vector<uint8_t>> field;
std::pair<int8_t, int8_t> selected(-1, -1);
bool gameOver = false;
const uint8_t colors[7] = {195, 51, 15, 243, 207, 63, 227};
//                          |   |   |    |    |   |    |
//                         red  |  blue  | magenta|  orange
//                            green    yellow    cyan

uint8_t countFreeCells() {
    uint8_t free = 0;

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (field[i][j] == 0) ++free;
        }
    }
    return free;
}

bool addCells(uint8_t toAdd = 3) {
    if (countFreeCells() < toAdd) return false;
    for (int k = 0; k < toAdd; ++k) {
        do {
            uint8_t addX = std::rand() % 9, addY = std::rand() % 9;
            if (field[addX][addY] == 0) {
                field[addX][addY] = colors[std::rand() % 7];
                break;
            }
        }
        while (true);
    }
    return true;
}

void reload() {
    field.assign(9, std::vector<uint8_t>(9, 0));
    gameOver = false;
    addCells();

    return;
}

void startup(sf::RenderWindow& window) {
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

bool searchLines() {
    return false;
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
    uint8_t rColor, gColor, bColor, tColor;

    rColor = (color >> 6) * 64;

    gColor = color << 2;
    gColor = (gColor >> 6) * 64;

    bColor = color << 4;
    bColor = (bColor >> 6) * 64;

    tColor = color << 6;
    tColor = (tColor >> 6) * 85;

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

void showText(sf::RenderWindow& window, const std::string& first, const std::string& second) {
    sf::Font font;

    font.loadFromFile("resourses/Chava.ttf");

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
    drawBorders(window);
    showField(window);
    if (gameOver) showText(window, "You lose!", "Your score:");
    else showText(window, "Your score:", "Highscore:");
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
        if (isMousePressed || gameOver) return;
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        if (pos.x > 0 && pos.y > 0) {
            if (pos.x < (9 * cellSize) + (9 * borderThinkness) && pos.y < (9 * cellSize) + (9 * borderThinkness)) {
                uint16_t fx = pos.y / (cellSize + borderThinkness), fy = pos.x / (cellSize + borderThinkness);
                if (selected.first == -1 || selected.second == -1) {
                    if (field[fx][fy] != 0) {
                        selected.first = fx;
                        selected.second = fy;
                    }
                }
                else {
                    if (field[fx][fy] == 0) {
                        if (moveCell(window, selected.first, selected.second, fx, fy)) {
                            if (!searchLines()) {
                                if (!addCells()) gameOver = true;
                            }
                        }
                    }
                    selected.first = -1;
                    selected.second = -1;
                }
            }
        }
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