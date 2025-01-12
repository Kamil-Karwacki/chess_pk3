#include <iostream>
#include "render.h"
#include "input.h"


int main()
{
    auto window = sf::RenderWindow({640u, 480u}, "Szachy");

    LinkedList<sf::Sprite> linkedSprites = initTextures();

    window.setFramerateLimit(60);

    while (window.isOpen())
    {

        for (auto event = sf::Event(); window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::Resized)
            {
                // update the view to the new size of the window
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    std::cout << "the left button was pressed\n";
                    std::cout << "mouse x: " << event.mouseButton.x << std::endl;
                    std::cout << "mouse y: " << event.mouseButton.y << std::endl;
                    int xTile = std::clamp(event.mouseButton.x / static_cast<int>(pieceSize.x), 0, 7);
                    int yTile = std::clamp(event.mouseButton.y / static_cast<int>(pieceSize.y), 0, 7);
                    std::cout << xTile << "\n";
                    std::cout << yTile << "\n";
                    ProcessInput(xTile, yTile);
                }
            }
        }

        window.clear();

        draw(window, linkedSprites);
        
        window.display();
    }
}


