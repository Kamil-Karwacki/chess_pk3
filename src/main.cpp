#include "render.h"

int main()
{
    auto window = sf::RenderWindow({640u, 480u}, "Szachy");

    std::array<sf::Sprite, 12> sprites = initTextures();

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
        }

        window.clear();

        draw(window, sprites);
        
        window.display();
    }
}


