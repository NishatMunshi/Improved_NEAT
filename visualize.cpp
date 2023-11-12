#include "include/Defines.hpp"

int main()
{
#if ENABLE_GRAPHICS
    // -----------------------------------------------------------
    if (_window.hasFocus()) // otherwise perform calculations internally
    {
        std::stringstream ss;
        ss << "Generation: " << _generation << ' ' << "Individual: " << individualIndex;
        _window.setTitle(ss.str());

        sf::Event _event;
        while (_window.pollEvent(_event))
        {
            if (_event.type == sf::Event::Closed)
            {
                _window.close();
                abort();
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp))
            _window.setFramerateLimit(UINT32_MAX);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown))
            _window.setFramerateLimit(5);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Home))
            while (true)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::End))
                    break;
            }

        _window.clear();
        individualBrain.g_draw(_window);
        m_board.g_draw(_window);
        _window.display();
    }
    // -------------------------------------
#endif
    return 0;
}
