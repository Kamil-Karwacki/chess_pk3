#pragma once
#include <SFML/Graphics.hpp>
#include <array>

inline sf::Vector2u canvasSize;
inline sf::Vector2f pieceSize;
inline uint8_t highlitedTile;
void draw(sf::RenderWindow& window, std::array<sf::Sprite, 12> sprites);
void drawCheckboard(sf::RenderWindow& window);
std::array<sf::Sprite, 12> initTextures();