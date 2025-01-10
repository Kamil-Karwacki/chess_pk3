#include "render.h"
#include <iostream>
#include "board.h"
#include "input.h"
#include "debug.h"

std::array<sf::Sprite, 12> initTextures()
{
    sf::Texture *piecesTexture = new sf::Texture();
    if(!piecesTexture->loadFromFile("../../img/ChessPiecesArray.png"))
    {
        throw std::runtime_error("Error: Cant open chesspieces file\n");
    }

    std::array<sf::Sprite, 12> sprites;

    sprites[0] = sf::Sprite(*piecesTexture, sf::IntRect(0, 0, 60, 60));
    sprites[1] = sf::Sprite(*piecesTexture, sf::IntRect(60, 0, 60, 60));
    sprites[2] = sf::Sprite(*piecesTexture, sf::IntRect(120, 0, 60, 60));
    sprites[3] = sf::Sprite(*piecesTexture, sf::IntRect(180, 0, 60, 60));
    sprites[4] = sf::Sprite(*piecesTexture, sf::IntRect(240, 0, 60, 60));
    sprites[5] = sf::Sprite(*piecesTexture, sf::IntRect(300, 0, 60, 60));

    sprites[6] = sf::Sprite(*piecesTexture, sf::IntRect(0, 60, 60, 60));
    sprites[7] = sf::Sprite(*piecesTexture, sf::IntRect(60, 60, 60, 60));
    sprites[8] = sf::Sprite(*piecesTexture, sf::IntRect(120, 60, 60, 60));
    sprites[9] = sf::Sprite(*piecesTexture, sf::IntRect(180, 60, 60, 60));
    sprites[10] = sf::Sprite(*piecesTexture, sf::IntRect(240, 60, 60, 60));
    sprites[11] = sf::Sprite(*piecesTexture, sf::IntRect(300, 60, 60, 60));

    return sprites;

}

void draw(sf::RenderWindow& window, std::array<sf::Sprite, 12> sprites)
{
    drawCheckboard(window);
    for(int j = 0; j < 12; j++)
    {
        sprites[j].setScale(pieceSize.x / 60, pieceSize.y / 60);
        for(int i = 0; i < 64; i++)
        {
            if((board.bitboards[j] >> i) & 1)
            {
                unsigned int row = i / 8;
                unsigned int column = i - (row * 8);
                sprites[j].setPosition(sf::Vector2f((canvasSize.y - pieceSize.x) - (column * canvasSize.y / 8), canvasSize.y - (row * canvasSize.x / 8) - pieceSize.y));
                window.draw(sprites[j]);
            }
        }
    }
    
}

void drawCheckboard(sf::RenderWindow& window)
{
    sf::Vector2u windowSize = window.getSize();
    bool color = false;
    for(int i = std::min(windowSize.x, windowSize.y); i > 0; i--)
    {
        if(i % 8 == 0)
        {
            canvasSize = sf::Vector2u(i, i);
            break;
        }
    }

    pieceSize = sf::Vector2f(canvasSize.x / 8, canvasSize.y / 8);


    for(int y = 0; y < 8; y++)
    {
        color = !color;
        for(int x = 0; x < 8; x++)
        {
            sf::RectangleShape square(sf::Vector2f(canvasSize.x / 8, canvasSize.y / 8));
            square.setPosition(sf::Vector2f(canvasSize.x * x / 8, canvasSize.y * y / 8));

            if((board.generatedMoves >> (7 - x + (7 - y) * 8) & 1))
            {
                square.setFillColor(sf::Color(240, 128, 128));
                color = !color;
                window.draw(square);
                continue;
            }

            if(y * 8 + x == highlitedTile && board.selectedTile)
            {
                square.setFillColor(sf::Color(135, 201, 249));
                color = !color;
                window.draw(square);
                continue;
            }

            if(color)
            {
                square.setFillColor(sf::Color(195, 211, 149));
            }
            else
            {
                square.setFillColor(sf::Color(147, 64, 47));
            }
            window.draw(square);
            color = !color;
        }
    }
}