#define _title "Color cells - ver. 0.11"
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
uint8_t colors[7] = {195, 51, 15, 243, 207, 63, 227};
//                    |   |   |    |    |   |    |
//                   red  |  blue  | magenta|  orange
//                      green    yellow    cyan

void startup(sf::RenderWindow& window) {
    std::srand(std::time(nullptr));
    field.assign(9, std::vector<uint8_t>(9, 0));
    sf::Image icon;
    icon.loadFromFile("resourses/grafics/icon.png");
    window.setIcon(48, 48, icon.getPixelsPtr());
    window.setVerticalSyncEnabled(true);

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
            rect.setSize(sf::Vector2f(cellSize - borderThinkness * 2, cellSize - borderThinkness * 2));
            rect.setOutlineThickness(0);
            rect.setPosition(j * cellSize + (j + 1) * borderThinkness + borderThinkness, i * cellSize + (i + 1) * borderThinkness + borderThinkness);
            rect.setFillColor(sf::Color(color));
            window.draw(rect);
        }
    }
    return;
}

void showText(sf::RenderWindow& window, const std::string& score, const std::string& highscore) {
    sf::Font font;

    font.loadFromFile("resourses/Chava.ttf");

    sf::Text scoreText, highscoreText;
    scoreText.setFont(font);
    highscoreText.setFont(font);
    scoreText.setCharacterSize(fontSize);
    highscoreText.setCharacterSize(fontSize);
    scoreText.setFillColor(sf::Color(textColor));
    highscoreText.setFillColor(sf::Color(textColor));
    scoreText.setString("Your score: " + score);
    highscoreText.setString("Highscore: " + highscore);

    scoreText.setPosition(fontSize * 0.25, (9 * cellSize) + 9 * borderThinkness + fontSize * 0.25);
    highscoreText.setPosition(fontSize * 0.25, (9 * cellSize) + 9 * borderThinkness + fontSize * 1.5);
    window.draw(scoreText);
    window.draw(highscoreText);

    return;
}

void gameEventProcessing(sf::RenderWindow& window, sf::Event& event) {
    static bool isMousePressed = false;

    if (event.type == sf::Event::Closed) window.close();
    else if (event.type == sf::Event::KeyPressed) {
        return;
    }
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (isMousePressed) return;
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        if (pos.x > 0 && pos.y > 0) {
            if (pos.x < (9 * cellSize) + (9 * borderThinkness) && pos.y < (9 * cellSize) + (9 * borderThinkness)) {
                //createCell(field, pos.x / (cellSize + borderThinkness), pos.y / (cellSize + borderThinkness), curMode);
                uint16_t fx = pos.y / (cellSize + borderThinkness), fy = pos.x / (cellSize + borderThinkness);
                field[fx][fy] = colors[std::rand() % 7];
            }
        }
        isMousePressed = true;
    }
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        return;
    }
    else if (event.type == sf::Event::MouseButtonReleased) isMousePressed = false;

    return;
}

int main() {
    sf::RenderWindow gameWindow(sf::VideoMode((9 * cellSize) + 10 * borderThinkness, (9 * cellSize) + 10 * borderThinkness + (fontSize * 2.75)), _title, sf::Style::Close);
    startup(gameWindow);

    while (gameWindow.isOpen()) {
        gameWindow.clear(sf::Color(backgroundColor));
        sf::Event gameEvent;
        while (gameWindow.pollEvent(gameEvent)) gameEventProcessing(gameWindow, gameEvent);

        drawBorders(gameWindow);
        showField(gameWindow);
        showText(gameWindow, "100", "250");
        gameWindow.display();
    }

    return 0;
}