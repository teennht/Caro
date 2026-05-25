#include "ui/Button.hpp"

namespace caro {

Button::Button(const sf::String& label, const sf::Vector2f& size, const sf::Font& font, std::function<void()> onClick)
    : text_(font), onClick_(onClick), state_(ButtonState::Normal), scale_(1.0f), targetScale_(1.0f), baseCharacterSize_(32) {
    
    shadow_.setSize(size);
    shadow_.setOrigin(size / 2.0f);
    shadow_.setFillColor(sf::Color(0, 0, 0, 95));

    shape_.setSize(size);
    shape_.setOrigin(size / 2.0f);
    
    text_.setString(label);
    text_.setCharacterSize(32);
    
    // Set default colors
    bgNormal_ = sf::Color(0x4F, 0x93, 0xC2); // #4F93C2 Blue
    bgHover_ = sf::Color(0x63, 0xA5, 0xD4); // #63A5D4 Lighter Blue
    textNormal_ = sf::Color(0xF4, 0xF1, 0xE8); // #F4F1E8 Off-white
    borderSelected_ = sf::Color(0xF6, 0xD9, 0x5F); // #F6D95F Yellow

    shape_.setFillColor(bgNormal_);
    shape_.setOutlineColor(sf::Color::Transparent);
    shape_.setOutlineThickness(3.f);
    text_.setFillColor(textNormal_);

    recenterText();
}

void Button::setPosition(const sf::Vector2f& pos) {
    shape_.setPosition(pos);
    shadow_.setPosition({pos.x + 5.f, pos.y + 5.f});
    text_.setPosition(pos);
}

sf::Vector2f Button::getPosition() const {
    return shape_.getPosition();
}

sf::FloatRect Button::getGlobalBounds() const {
    return shape_.getGlobalBounds();
}

void Button::setClickCallback(std::function<void()> callback) {
    onClick_ = callback;
}

void Button::setState(ButtonState state) {
    state_ = state;
}

ButtonState Button::getState() const {
    return state_;
}

void Button::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (state_ == ButtonState::Disabled) return;

    sf::Vector2f mousePosF = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    bool hovered = shape_.getGlobalBounds().contains(mousePosF);

    if (hovered) {
        if (state_ != ButtonState::Selected) {
            state_ = ButtonState::Hovered;
        }

        if (event.getIf<sf::Event::MouseButtonPressed>()) {
            if (auto* mousePress = event.getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePress->button == sf::Mouse::Button::Left) {
                    state_ = ButtonState::Pressed;
                }
            }
        } else if (event.getIf<sf::Event::MouseButtonReleased>()) {
            if (auto* mouseRelease = event.getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseRelease->button == sf::Mouse::Button::Left) {
                    state_ = ButtonState::Hovered;
                    if (onClick_) {
                        onClick_();
                    }
                }
            }
        }
    } else {
        if (state_ != ButtonState::Selected) {
            state_ = ButtonState::Normal;
        }
    }
}

void Button::update(float dt) {
    // Scale animations based on state
    if (state_ == ButtonState::Pressed) {
        targetScale_ = 0.92f;
    } else if (state_ == ButtonState::Hovered) {
        targetScale_ = 1.05f;
    } else {
        targetScale_ = 1.00f;
    }

    // Smoothly interpolate scale
    scale_ += (targetScale_ - scale_) * 15.0f * dt;
    shape_.setScale({scale_, scale_});
    shadow_.setScale({scale_, scale_});
    text_.setScale({scale_, scale_});

    // Update color colors based on state
    if (state_ == ButtonState::Disabled) {
        shape_.setFillColor(sf::Color(bgNormal_.r, bgNormal_.g, bgNormal_.b, 100));
        shadow_.setFillColor(sf::Color(0, 0, 0, 35));
        text_.setFillColor(sf::Color(textNormal_.r, textNormal_.g, textNormal_.b, 100));
        shape_.setOutlineColor(sf::Color(0, 0, 0, 65));
        shape_.setOutlineThickness(3.f);
    } else if (state_ == ButtonState::Selected) {
        shape_.setFillColor(bgHover_);
        shadow_.setFillColor(sf::Color(0, 0, 0, 110));
        text_.setFillColor(textNormal_);
        shape_.setOutlineColor(borderSelected_);
        shape_.setOutlineThickness(5.f);
    } else if (state_ == ButtonState::Hovered || state_ == ButtonState::Pressed) {
        shape_.setFillColor(bgHover_);
        shadow_.setFillColor(sf::Color(0, 0, 0, 115));
        text_.setFillColor(textNormal_);
        shape_.setOutlineColor(sf::Color(0xF4, 0xF1, 0xE8, 150));
        shape_.setOutlineThickness(3.f);
    } else {
        shape_.setFillColor(bgNormal_);
        shadow_.setFillColor(sf::Color(0, 0, 0, 95));
        text_.setFillColor(textNormal_);
        shape_.setOutlineColor(sf::Color(0, 0, 0, 105));
        shape_.setOutlineThickness(3.f);
    }
}

void Button::render(sf::RenderTarget& target) {
    target.draw(shadow_);
    target.draw(shape_);
    target.draw(text_);
}

void Button::setColors(sf::Color bgNormal, sf::Color bgHover, sf::Color textNormal, sf::Color borderSelected) {
    bgNormal_ = bgNormal;
    bgHover_ = bgHover;
    textNormal_ = textNormal;
    borderSelected_ = borderSelected;
}

void Button::setLabel(const sf::String& text) {
    text_.setString(text);
    recenterText();
}

void Button::setCharacterSize(unsigned int size) {
    baseCharacterSize_ = size;
    text_.setCharacterSize(size);
    recenterText();
}

void Button::recenterText() {
    text_.setCharacterSize(baseCharacterSize_);
    const float maxWidth = shape_.getSize().x * 0.84f;
    while (text_.getCharacterSize() > 18 && text_.getLocalBounds().size.x > maxWidth) {
        text_.setCharacterSize(text_.getCharacterSize() - 1);
    }

    sf::FloatRect textRect = text_.getLocalBounds();
    text_.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
}

} // namespace caro
