#ifndef CARO_VIET_UI_BUTTON_HPP
#define CARO_VIET_UI_BUTTON_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

namespace caro {

enum class ButtonState {
    Normal,
    Hovered,
    Pressed,
    Selected,
    Disabled
};

class Button {
public:
    Button(const sf::String& label, const sf::Vector2f& size, const sf::Font& font, std::function<void()> onClick = nullptr);

    void setPosition(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;

    void setClickCallback(std::function<void()> callback);
    void setState(ButtonState state);
    ButtonState getState() const;

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(float dt);
    void render(sf::RenderTarget& target);

    void setColors(sf::Color bgNormal, sf::Color bgHover, sf::Color textNormal, sf::Color borderSelected = sf::Color(0xF6, 0xD9, 0x5F));
    void setLabel(const sf::String& text);
    void setCharacterSize(unsigned int size);

private:
    void recenterText();

    sf::RectangleShape shadow_;
    sf::RectangleShape shape_;
    sf::Text text_;
    std::function<void()> onClick_;
    ButtonState state_;
    
    sf::Color bgNormal_;
    sf::Color bgHover_;
    sf::Color textNormal_;
    sf::Color borderSelected_;

    float scale_;
    float targetScale_;
    unsigned int baseCharacterSize_;
};

} // namespace caro

#endif // CARO_VIET_UI_BUTTON_HPP
