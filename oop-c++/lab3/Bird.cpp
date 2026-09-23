#include "Bird.hpp"
#include <SFML/Graphics.hpp>
#include <utility>

void Bird::downloadTexture(const sf::Texture& tex, sf::Sprite& s){
    s.setTexture(tex);
}

void Bird::placeAt(const std::pair<float,float>& p, sf::Sprite& s){
    s.setPosition(sf::Vector2f{p.first, p.second}); 
}

Bird::Bird(const sf::Texture& tex, std::pair<float,float> start)
: spriteBird(tex) {
    placeAt(start, spriteBird);
}

void Bird::update() {
    Oy += gravity;
    if (Oy > maxFall) Oy = maxFall;                
    spriteBird.move(sf::Vector2f{0.f, Oy});
    if (Oy > 0.f) jumping = false;
}

void Bird::jump() {
    Oy = -jumpUp;
    jumping = true;
}

void Bird::draw(sf::RenderWindow& win) {
    win.draw(spriteBird);
}

bool Bird::hit(const sf::Sprite& other) const {
    const auto a = spriteBird.getGlobalBounds();
    const auto b = other.getGlobalBounds();
    return a.findIntersection(b).has_value();       
}
