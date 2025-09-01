#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <optional>
#include "json.hpp"
#include "external/httplib/httplib.h"

using json = nlohmann::json;

std::string getNPCResponse(const std::string& input) {
    httplib::Client cli("localhost", 5005);
    json payload = { {"message", input} };

    auto res = cli.Post("/chat", payload.dump(), "application/json");
    if (!res || res->status != 200) {
        std::cerr << "Failed to get response from server\n";
        return "We'll chat later";
    }

    auto replyJson = json::parse(res->body);
    return replyJson["reply"];
}

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "NPC Chatbot");
    window.setFramerateLimit(60);
    sf::Font font("assets/fonts/arial.ttf");

    // Create player
    sf::CircleShape player(50);
    player.setFillColor(sf::Color::Green);
    player.setPosition({200, 250});
    float speed = 3.f;

    // Set up player response
    std::string playerInput = "";
    sf::Text inputText(font, "", 20);
    // Place text above Player
    inputText.setPosition({
        player.getPosition().x + player.getRadius(),
        player.getPosition().y + player.getRadius() - 80
    });
    // Centre text
    sf::FloatRect inputBounds = inputText.getLocalBounds();
    inputText.setOrigin(inputBounds.size / 2.0f);

    // Create NPC
    sf::CircleShape npc(50);
    npc.setFillColor(sf::Color::White);
    npc.setPosition({500, 250});

    // Set up NPC text
    sf::Text npcText(font, "", 20);
    npcText.setString(std::string(getNPCResponse(playerInput)));
    // Place text above NPC
    npcText.setPosition({
        npc.getPosition().x + npc.getRadius(),
        npc.getPosition().y + npc.getRadius() - 80
    });
    // Centre text
    sf::FloatRect bounds = npcText.getLocalBounds();
    npcText.setOrigin(bounds.size / 2.0f);

    while (window.isOpen()) {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window properly
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            // Check for player input or backspace
            if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                // If backspace then remove from input
                if (static_cast<char>(textEntered->unicode) == '\b' && !playerInput.empty()) {
                    playerInput.pop_back();
                }
                // If a valid letter then add to input
                else if (textEntered->unicode < 128) {
                    playerInput += static_cast<char>(textEntered->unicode); }
            }
            // Check for Enter key
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                // If enter then send player input to npc
                if (keyPressed->scancode == sf::Keyboard::Scan::Enter) {
                    std::string reply = getNPCResponse(playerInput);
                    npcText.setString(reply);
                    playerInput.clear();
                    inputText.setString(""); // Clear the input display
                }
            }
        }
        inputText.setString(playerInput);

        // Move player with arrow keys
        // Diagonal
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up)) {
            player.move({-speed / 1.5f, -speed / 1.5f});
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down)) {
            player.move({-speed / 1.5f, speed / 1.5f});
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right) && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up)) {
            player.move({speed / 1.5f, -speed / 1.5f});
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right) && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down)) {
            player.move({speed / 1.5f, speed / 1.5f});
        }
        // Non diagonal
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left)) {
            player.move({-speed, 0.0f});
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right)) {
            player.move({speed, 0.0f});
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up)) {
            player.move({0.0f, -speed});
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down)) {
            player.move({0.0f, speed});
        }

        // Place text above Player
        inputText.setPosition({
            player.getPosition().x + player.getRadius(),
            player.getPosition().y + player.getRadius() - 80
        });
        // Centre text
        sf::FloatRect inputBounds = inputText.getLocalBounds();
        inputText.setOrigin(inputBounds.size / 2.0f);
            
        window.clear(sf::Color::Black);
        window.draw(player);
        window.draw(npc);
        window.draw(npcText);
        window.draw(inputText);
        window.display();
    }

    return 0;
}
