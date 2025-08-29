#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "NPC Chatbot");

    // Create player
    sf::CircleShape player(50);
    player.setFillColor(sf::Color::Green);
    player.setPosition({200, 250});

    // Create NPC
    sf::CircleShape npc(50);
    npc.setFillColor(sf::Color::White);
    npc.setPosition({500, 250});

    // Set up NPC text
    sf::Font font("assets/fonts/arial.ttf");
    sf::Text npcText(font, "Hello, there!", 20);
    // Place text above NPC
    npcText.setPosition({
        npc.getPosition().x + npc.getRadius() - 80,
        npc.getPosition().y + npc.getRadius() - 80
    });

    while (window.isOpen()) {
        while (auto eventOpt = window.pollEvent()) {
            const sf::Event& event = *eventOpt;
            if (event.is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);
        window.draw(player);
        window.draw(npc);
        window.draw(npcText);
        window.display();
    }

    return 0;
}
