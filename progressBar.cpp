// demoTesting/progressBar.cpp
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <string>

namespace {
using Pattern = std::array<const char *, 5>;

constexpr std::array<Pattern, 10> kDigits{
    {Pattern{"XXX", "X X", "X X", "X X", "XXX"},
     Pattern{"  X", "  X", "  X", "  X", "  X"},
     Pattern{"XXX", "  X", "XXX", "X  ", "XXX"},
     Pattern{"XXX", "  X", "XXX", "  X", "XXX"},
     Pattern{"X X", "X X", "XXX", "  X", "  X"},
     Pattern{"XXX", "X  ", "XXX", "  X", "XXX"},
     Pattern{"XXX", "X  ", "XXX", "X X", "XXX"},
     Pattern{"XXX", "  X", "  X", "  X", "  X"},
     Pattern{"XXX", "X X", "XXX", "X X", "XXX"},
     Pattern{"XXX", "X X", "XXX", "  X", "XXX"}}};
constexpr Pattern kPercent{"X  ", "  X", " X ", "X  ", "  X"};

void drawPattern(sf::RenderTarget &target, sf::Vector2f origin,
                 const Pattern &pattern, float pixelSize, sf::Color color) {
  const float gap = pixelSize * 0.22f;
  sf::RectangleShape pixel({pixelSize, pixelSize});
  pixel.setFillColor(color);

  for (std::size_t r = 0; r < pattern.size(); ++r) {
    const char *line = pattern[r];
    for (std::size_t c = 0; line[c] != '\0'; ++c) {
      if (line[c] == ' ')
        continue;
      pixel.setPosition({origin.x + static_cast<float>(c) * (pixelSize + gap),
                         origin.y + static_cast<float>(r) * (pixelSize + gap)});
      target.draw(pixel);
    }
  }
}

void drawPercentText(sf::RenderTarget &target, sf::Vector2f anchor, int value,
                     float scale, sf::Color color) {
  value = std::clamp(value, 0, 100);
  const float pixelSize = 6.f * scale;
  const float glyphWidth = 3.f * (pixelSize + pixelSize * 0.22f);
  const float spacing = pixelSize * 1.2f;

  std::string text = std::to_string(value);
  const float totalWidth =
      static_cast<float>(text.size()) * (glyphWidth + spacing) + glyphWidth;

  float x = anchor.x - totalWidth / 2.f;
  for (char ch : text) {
    drawPattern(target, {x, anchor.y}, kDigits[ch - '0'], pixelSize, color);
    x += glyphWidth + spacing;
  }
  drawPattern(target, {x, anchor.y}, kPercent, pixelSize, color);
}
} // namespace

int main() {
  sf::RenderWindow window(sf::VideoMode({640u, 360u}), "Progress Demo",
                          sf::Style::Titlebar | sf::Style::Close);
  window.setFramerateLimit(60);

  sf::VertexArray background(sf::PrimitiveType::TriangleStrip, 4);
  background[0].position = {0.f, 0.f};
  background[1].position = {640.f, 0.f};
  background[2].position = {0.f, 360.f};
  background[3].position = {640.f, 360.f};
  background[0].color = background[1].color = sf::Color(24, 26, 38);
  background[2].color = background[3].color = sf::Color(30, 34, 48);

  sf::RectangleShape card({500.f, 200.f});
  card.setOrigin(card.getSize() / 2.f);
  card.setPosition({320.f, 180.f});
  card.setFillColor(sf::Color(36, 39, 52, 235));
  card.setOutlineThickness(1.2f);
  card.setOutlineColor(sf::Color(70, 90, 120, 140));

  sf::RectangleShape cardShadow(card.getSize());
  cardShadow.setOrigin(card.getSize() / 2.f);
  cardShadow.setPosition(card.getPosition() + sf::Vector2f{0.f, 10.f});
  cardShadow.setFillColor(sf::Color(10, 10, 20, 110));

  sf::RectangleShape track({420.f, 30.f});
  track.setOrigin(track.getSize() / 2.f);
  track.setPosition({card.getPosition().x, card.getPosition().y + 24.f});
  track.setFillColor(sf::Color(48, 53, 68));
  track.setOutlineThickness(1.5f);
  track.setOutlineColor(sf::Color(70, 80, 100));

  const sf::Color baseFillColor(82, 198, 151);
  const sf::Color glowFillColor(196, 255, 228);

  sf::RectangleShape fill({0.f, track.getSize().y});
  fill.setOrigin({0.f, track.getSize().y / 2.f});
  fill.setPosition(
      {track.getPosition().x - track.getSize().x / 2.f, track.getPosition().y});
  fill.setFillColor(baseFillColor);

  sf::RectangleShape highlight({110.f, track.getSize().y - 6.f});
  highlight.setOrigin({0.f, highlight.getSize().y / 2.f});
  highlight.setFillColor(sf::Color(255, 255, 255, 60));

  sf::RectangleShape highlightSoft = highlight;
  highlightSoft.setFillColor(sf::Color(255, 255, 255, 24));
  highlightSoft.setScale({1.12f, 1.35f});

  sf::RectangleShape glowBloom({track.getSize().x + 60.f, track.getSize().y + 30.f});
  glowBloom.setOrigin(glowBloom.getSize() / 2.f);
  glowBloom.setPosition(track.getPosition());
  glowBloom.setFillColor(sf::Color(255, 255, 255, 0));
  glowBloom.setScale({1.f, 1.2f});

  sf::CircleShape pulse(16.f);
  pulse.setOrigin({16.f, 16.f});
  pulse.setFillColor(sf::Color(255, 255, 255, 45));

  sf::Font font;
  bool fontLoaded = font.openFromFile("/System/Library/Fonts/SFNSRounded.ttf");
  std::optional<sf::Text> title;
  std::optional<sf::Text> caption;

  if (fontLoaded) {
    title.emplace(font, "Preparing Assets", 20);
    caption.emplace(font, "Hang tight while we finish setting things up.", 14);

    auto center = [](sf::Text &txt, sf::Vector2f pos) {
      auto bounds = txt.getLocalBounds();
      txt.setOrigin({bounds.getCenter().x, bounds.getCenter().y});
      txt.setPosition(pos);
    };
    center(*title, {card.getPosition().x, card.getPosition().y - 68.f});
    center(*caption, {card.getPosition().x, card.getPosition().y + 70.f});

    title->setFillColor(sf::Color(198, 206, 224));
    title->setStyle(sf::Text::Bold);
    caption->setFillColor(sf::Color(150, 160, 185, 200));
  }

  constexpr float duration = 5.f;
  constexpr float glowDuration = 1.2f;
  sf::Clock clock;
  sf::Clock glowClock;
  float progress = 0.f;
  bool finished = false;
  bool glowActive = false;
  bool showBlank = false;

  auto lerpColor = [](const sf::Color &a, const sf::Color &b, float t) {
    t = std::clamp(t, 0.f, 1.f);
    auto channel = [t](sf::Uint8 start, sf::Uint8 end) {
      return static_cast<sf::Uint8>(start + (end - start) * t);
    };
    return sf::Color(channel(a.r, b.r), channel(a.g, b.g), channel(a.b, b.b),
                     channel(a.a, b.a));
  };

  while (window.isOpen()) {
    while (auto event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>())
        window.close();
    }

    if (showBlank) {
      window.clear(sf::Color::Black);
      window.display();
      continue;
    }

    const float elapsed = clock.getElapsedTime().asSeconds();

    if (!finished) {
      progress = std::min(elapsed / duration, 1.f);
      if (progress >= 1.f) {
        finished = true;
        glowActive = true;
        glowClock.restart();
      }
    }

    float filled = track.getSize().x * progress;
    fill.setSize({filled, track.getSize().y});

    const float animationTime = finished ? (glowActive ? glowClock.getElapsedTime().asSeconds()
                                                      : elapsed)
                                         : elapsed;

    const float travel = track.getSize().x - highlight.getSize().x;
    const float offset = std::clamp(filled - highlight.getSize().x, 0.f, travel);
    const float wobble = std::sin(animationTime * 2.4f) * 12.f;
    const float highlightX = fill.getPosition().x + offset + wobble;

    highlight.setPosition({highlightX, track.getPosition().y});
    highlightSoft.setPosition({highlightX - 10.f, track.getPosition().y});

    const float pulseRadius = 14.f + 5.f * std::sin(animationTime * 6.f);
    pulse.setRadius(pulseRadius);
    pulse.setOrigin({pulseRadius, pulseRadius});
    pulse.setPosition({fill.getPosition().x + filled, track.getPosition().y});

    if (glowActive) {
      const float glowTime = glowClock.getElapsedTime().asSeconds();
      const float glowProgress = std::min(glowTime / glowDuration, 1.f);
      const float glowPulse = 0.5f + 0.5f * std::sin(glowTime * 4.5f);
      const float glowMix = std::max(glowProgress, glowPulse);

      fill.setFillColor(lerpColor(baseFillColor, glowFillColor, glowMix));
      glowBloom.setFillColor(
          sf::Color(255, 255, 255, static_cast<sf::Uint8>(180 * glowMix)));
      glowBloom.setScale({1.f + 0.2f * glowMix, 1.2f + 0.35f * glowMix});

      if (glowTime >= glowDuration) {
        glowActive = false;
        showBlank = true;
      }
    } else {
      fill.setFillColor(baseFillColor);
      glowBloom.setFillColor(sf::Color(255, 255, 255, 0));
      glowBloom.setScale({1.f, 1.2f});
    }

    window.clear();
    window.draw(background);
    window.draw(cardShadow);
    window.draw(card);
    window.draw(track);

    if (glowActive) {
      window.draw(glowBloom);
    }

    if (filled > 0.f) {
      window.draw(fill);
      if (filled > highlight.getSize().x) {
        window.draw(highlightSoft);
        window.draw(highlight);
      }
      window.draw(pulse);
    }

    if (title && caption) {
      window.draw(*title);
      window.draw(*caption);

      sf::Text percent(
          font, std::to_string(static_cast<int>(progress * 100.f + 0.5f)) + "%",
          42);
      auto bounds = percent.getLocalBounds();
      percent.setOrigin({bounds.getCenter().x, bounds.getCenter().y});
      percent.setPosition({card.getPosition().x, card.getPosition().y - 10.f});
      percent.setFillColor(sf::Color(236, 243, 255));
      percent.setStyle(sf::Text::Bold);
      window.draw(percent);
    } else {
      drawPercentText(window,
                      {card.getPosition().x, card.getPosition().y - 10.f},
                      static_cast<int>(progress * 100.f + 0.5f), 1.6f,
                      sf::Color(236, 243, 255));
    }

    window.display();
  }
  return 0;
}
