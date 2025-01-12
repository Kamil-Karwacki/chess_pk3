#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include "lista.h"

inline sf::Vector2u canvasSize;
inline sf::Vector2f pieceSize;
inline uint8_t highlitedTile;

inline bool displayPromotion;
inline sf::Vector2f promotionPos;
inline bool whitePromoting;

inline bool checkmate = false;
inline bool resultDraw = false;
inline std::string winner;
inline sf::Font font;
inline std::string text;

void draw(sf::RenderWindow& window, LinkedList<sf::Sprite> sprites);
void drawCheckboard(sf::RenderWindow& window);
LinkedList<sf::Sprite> initTextures();