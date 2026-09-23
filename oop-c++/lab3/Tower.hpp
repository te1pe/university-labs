#ifndef TOWERS_HPP
#define TOWERS_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <algorithm>

class Towers {
private:
    struct Column {
        sf::RectangleShape top;
        sf::RectangleShape bottom;
        bool isOffscreen() const {
            const auto gb = top.getGlobalBounds(); 
            return (gb.position.x + gb.size.x) < 0.f;
        }
        bool collides(const sf::FloatRect& bird) const {
            const auto a = top.getGlobalBounds();
            const auto b = bottom.getGlobalBounds();
            return a.findIntersection(bird).has_value() ||
                   b.findIntersection(bird).has_value();
        }
        void move(float dx) {
            top.move(sf::Vector2f{dx, 0.f});
            bottom.move(sf::Vector2f{dx, 0.f});
        }
    };

    std::vector<Column> cols;
    sf::Clock spawnClock;

    sf::Vector2u wndSize{800,600};
    float spawnInterval = 1.5f;  
    float gapHeight     = 160.f; 
    float width         = 64.f; 
    float speed         = 2.f;   

public:
    Towers() = default;
    explicit Towers(sf::Vector2u size): wndSize(size) {}

    void reset();
    void update();
    void draw(sf::RenderWindow& win) const;
    bool anyCollision(const sf::FloatRect& bird) const;

    void setSpawnInterval(float s) { spawnInterval = std::max(0.2f, s); }
    void setGapHeight(float h)     { gapHeight     = std::max(40.f,  h); }
    void setWidth(float w)         { width         = std::max(10.f,  w); }
    void setSpeed(float v)         { speed         = std::max(0.f,   v); }

private:
    void spawnOne();
    float randomGapCenter() const;
};

#endif // TOWERS_HPP
