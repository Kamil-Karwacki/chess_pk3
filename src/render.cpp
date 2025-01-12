#include "render.h"
#include <iostream>
#include "board.h"
#include "input.h"
#include "debug.h"

LinkedList<sf::Sprite> initTextures()
{
    sf::Texture *piecesTexture = new sf::Texture();
    if(!piecesTexture->loadFromFile("../../img/ChessPiecesArray.png"))
    {
        throw std::runtime_error("Error: Cant open chesspieces file\n");
    }

    LinkedList<sf::Sprite> sprites;

    sprites.pushBack(sf::Sprite(*piecesTexture, sf::IntRect(0, 0, 60, 60)));
    sprites.pushBack(sf::Sprite(*piecesTexture, sf::IntRect(60, 0, 60, 60)));
    sprites.pushBack(sf::Sprite(*piecesTexture, sf::IntRect(120, 0, 60, 60)));
    sprites.pushBack(sf::Sprite(*piecesTexture, sf::IntRect(180, 0, 60, 60)));
    sprites.pushBack(sf::Sprite(*piecesTexture, sf::IntRect(240, 0, 60, 60)));
    sprites.pushBack(sf::Sprite(*piecesTexture, sf::IntRect(300, 0, 60, 60)));

    sprites.pushBack(sf::Sprite(*piecesTexture, sf::IntRect(0, 60, 60, 60)));
    sprites.pushBack(sf::Sprite(*piecesTexture, sf::IntRect(60, 60, 60, 60)));
    sprites.pushBack(sf::Sprite(*piecesTexture, sf::IntRect(120, 60, 60, 60)));
    sprites.pushBack(sf::Sprite(*piecesTexture, sf::IntRect(180, 60, 60, 60)));
    sprites.pushBack(sf::Sprite(*piecesTexture, sf::IntRect(240, 60, 60, 60)));
    sprites.pushBack(sf::Sprite(*piecesTexture, sf::IntRect(300, 60, 60, 60)));

    if (!font.loadFromFile("../../img/Arial.ttf"))
    {
        throw std::runtime_error("Error: Cant open font\n");
    }

    return sprites;
}

void draw(sf::RenderWindow& window, LinkedList<sf::Sprite> sprites)
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

    if(displayPromotion)
    {
        int min = whitePromoting ? 6 : 0;
        int max = whitePromoting ? 11 : 5;
        int offset = whitePromoting ? 1 : -1;

        sf::RectangleShape square(sf::Vector2f(canvasSize.x / 8, canvasSize.y / 2));
        if(whitePromoting)
            square.setPosition(sf::Vector2f(canvasSize.x * promotionPos.x / 8, canvasSize.y * (promotionPos.y + 1 * offset) / 8));
        else
            square.setPosition(sf::Vector2f(canvasSize.x * promotionPos.x / 8, canvasSize.y - canvasSize.y * (promotionPos.y + 1 * offset) / 8));

        square.setFillColor(sf::Color(135, 201, 249));
        window.draw(square);

        sprites[max - 1].setPosition(sf::Vector2f((promotionPos.x) * pieceSize.x, (promotionPos.y + 1 * offset) * pieceSize.y));
        window.draw(sprites[max - 1]);
        sprites[max - 2].setPosition(sf::Vector2f((promotionPos.x) * pieceSize.x, (promotionPos.y + 2 * offset) * pieceSize.y));
        window.draw(sprites[max - 2]);
        sprites[max - 3].setPosition(sf::Vector2f((promotionPos.x) * pieceSize.x, (promotionPos.y + 3 * offset) * pieceSize.y));
        window.draw(sprites[max - 3]);
        sprites[max - 5].setPosition(sf::Vector2f((promotionPos.x) * pieceSize.x, (promotionPos.y + 4 * offset) * pieceSize.y));
        window.draw(sprites[max - 5]);
    }

    if(checkmate)
    {
        sf::Text text;

        text.setFont(font);

        std::string displayText = winner + " has won!";

        text.setString(displayText);

        text.setCharacterSize(pieceSize.x);

        text.setOutlineColor(sf::Color(255,255,255,255));
        text.setOutlineThickness(4);
        text.setFillColor(sf::Color(0,0,0,255));

        text.setPosition(canvasSize.x / 2, canvasSize.y / 2);

        window.draw(text);
    }

    if(resultDraw)
    {
        sf::Text text;

        text.setFont(font);

        std::string displayText = "DRAW!";

        text.setString(displayText);

        text.setCharacterSize(pieceSize.x);

        text.setOutlineColor(sf::Color(255,255,255,255));
        text.setOutlineThickness(4);
        text.setFillColor(sf::Color(0,0,0,255));

        text.setPosition(canvasSize.x / 2, canvasSize.y / 2);

        window.draw(text);
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