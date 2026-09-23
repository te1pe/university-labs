#include "Tower.hpp"

void Towers::reset() {
    cols.clear();
    spawnClock.restart();
}

void Towers::update() {
    if (spawnClock.getElapsedTime().asSeconds() >= spawnInterval) {
        spawnOne();
        spawnClock.restart();
    }

    const float dx = -speed;
    for (auto& c : cols) c.move(dx);

    cols.erase(std::remove_if(cols.begin(), cols.end(),
                              [](const Column& c){ return c.isOffscreen(); }),
               cols.end());
}

void Towers::draw(sf::RenderWindow& win) const {
    for (const auto& c : cols) {
        win.draw(c.top);
        win.draw(c.bottom);
    }
}

bool Towers::anyCollision(const sf::FloatRect& bird) const {
    for (const auto& c : cols) if (c.collides(bird)) return true;
    return false;
}

void Towers::spawnOne() {
    const float xRight = static_cast<float>(wndSize.x) + width;
    const float wndH   = static_cast<float>(wndSize.y);
    const float gapY   = randomGapCenter();

    const float topH    = std::max(0.f, gapY - gapHeight * 0.5f);
    const float bottomY = gapY + gapHeight * 0.5f;
    const float bottomH = std::max(0.f, wndH - bottomY);

    Column c;
    c.top.setSize(sf::Vector2f{width, topH});
    c.top.setPosition(sf::Vector2f{xRight, 0.f});
    c.bottom.setSize(sf::Vector2f{width, bottomH});
    c.bottom.setPosition(sf::Vector2f{xRight, bottomY});

    const sf::Color pipeColor(70, 200, 70);
    c.top.setFillColor(pipeColor);
    c.bottom.setFillColor(pipeColor);

    cols.push_back(std::move(c));
}

float Towers::randomGapCenter() const {
    const float margin = 50.f;
    const float minY = margin + gapHeight * 0.5f;
    const float maxY = static_cast<float>(wndSize.y) - margin - gapHeight * 0.5f;
    if (maxY < minY) return static_cast<float>(wndSize.y) * 0.5f;
    const float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return minY + r * (maxY - minY);
}
