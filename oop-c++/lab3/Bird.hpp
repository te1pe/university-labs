#ifndef BIRD_HPP
#define BIRD_HPP

#include <SFML/Graphics.hpp>
#include <utility>

inline constexpr float STARTX = 200.f;
inline constexpr float STARTY = 300.f;

class Bird {
private:
    sf::Sprite spriteBird;
    bool  jumping = false;
    float Oy      = 0.f;     
    float gravity = 0.35f;   
    float jumpUp  = 7.5f;    
    float maxFall = 12.f;    

    static void downloadTexture(const sf::Texture& tex, sf::Sprite& s);
    static void placeAt(const std::pair<float, float>& p, sf::Sprite& s);

public:
    explicit Bird(const sf::Texture& tex, std::pair<float,float> start = {STARTX, STARTY});

    void update();
    void jump();
    void draw(sf::RenderWindow& window);
    bool hit(const sf::Sprite& other) const;
    void setSize(float w, float h);


    sf::FloatRect getBounds() const { return spriteBird.getGlobalBounds(); }
    void reset(std::pair<float,float> pos = {STARTX, STARTY}) {
        Oy = 0.f; jumping = false; placeAt(pos, spriteBird);
    }
};

#endif // BIRD_HPP
