#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include "json.hpp"
#include "external/httplib/httplib.h"  // Replace with your actual path

using json = nlohmann::json;

std::string getNPCResponse(const std::string& input) {
    httplib::Client cli("localhost", 5005);
    json payload = { {"message", input} };

    auto res = cli.Post("/chat", payload.dump(), "application/json");
    if (!res || res->status != 200) {
        std::cerr << "Failed to get response from server\n";
        return "Error: No reply";
    }

    auto replyJson = json::parse(res->body);
    return replyJson["reply"];
}

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "NPC Chatbot");

    // Create player
    sf::CircleShape player(50);
    player.setFillColor(sf::Color::Green);
    player.setPosition({200, 250});

    // Set up player response
    std::string playerInput = "Good day";

    // Create NPC
    sf::CircleShape npc(50);
    npc.setFillColor(sf::Color::White);
    npc.setPosition({500, 250});

    // Set up NPC text
    sf::Font font("assets/fonts/arial.ttf");
    sf::Text npcText(font, "Hello there", 20);
    npcText.setString(std::string(getNPCResponse(playerInput)));

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
