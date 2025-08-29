#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "NPC Chatbot");
    sf::Font font;
    font.loadFromFile("ARIAL.ttf");

    // Create player
    sf::CircleShape player(50);
    player.setFillColor(sf::Color::Green);
    player.setPosition(200, 250);

    // Create NPC
    sf::CircleShape npc(50);
    npc.setFillColor(sf::Color::White);
    npc.setPosition(500, 250);
    // Center text on circle
    sf::FloatRect textBounds = npcText.getLocalBounds();
    npcText.setOrigin(textBounds.width / 2, textBounds.height / 2);
    npcText.setPosition(npc.getPosition().x + npc.getRadius(),
                        npc.getPosition().y + npc.getRadius() - 10); // Slightly above center
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        window.draw(player);
        window.draw(npc);
        window.display();
    }

    return 0;
}
