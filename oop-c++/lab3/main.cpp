#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Bird.hpp"
#include "Tower.hpp"

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    const unsigned WIN_W = 800;
    const unsigned WIN_H = 600;
    sf::RenderWindow window(sf::VideoMode({WIN_W, WIN_H}), "Flappy bird");
    window.setFramerateLimit(60);

    sf::Texture texBird;
    if (!texBird.loadFromFile("assets/bird.png")) {
        std::cerr << "Не удалось загрузить assets/bird.png\n";
        return 1;
    }
    Bird bird(texBird, {STARTX, STARTY});

    Towers towers({WIN_W, WIN_H});

    bool gameOver = false;
    auto restart = [&]{
        bird.reset({STARTX, STARTY});
        towers.reset();
        gameOver = false;
    };

    while (window.isOpen()) {
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) { window.close(); continue; }

            if (const auto* key = ev->getIf<sf::Event::KeyPressed>()) {
                if (!gameOver && (key->code == sf::Keyboard::Key::Space ||
                                  key->scancode == sf::Keyboard::Scancode::Space)) {
                    bird.jump();
                }
                if (key->code == sf::Keyboard::Key::R ||
                    key->scancode == sf::Keyboard::Scancode::R) {
                    restart();
                }
            }
            if (!gameOver) {
                if (const auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mb->button == sf::Mouse::Button::Left) bird.jump();
                }
            }
        }

        if (!gameOver) {
            bird.update();
            towers.update();

            const auto b = bird.getBounds();
            const float topY    = b.position.y;
            const float bottomY = b.position.y + b.size.y;

            if (topY < -50.f || bottomY > static_cast<float>(WIN_H) + 10.f) gameOver = true;
            if (towers.anyCollision(bird.getBounds()))                     gameOver = true;
        }

        window.clear(sf::Color::Black); 
        towers.draw(window);
        bird.draw(window);

        if (gameOver) {
            sf::RectangleShape shade(sf::Vector2f{static_cast<float>(WIN_W), static_cast<float>(WIN_H)});
            shade.setFillColor(sf::Color(0,0,0,120));
            window.draw(shade);
        }

        window.display();
    }
    return 0;
}
