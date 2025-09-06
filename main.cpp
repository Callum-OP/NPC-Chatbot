#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <optional>
#include "json.hpp"
#include "external/httplib/httplib.h"

using json = nlohmann::json;

// Function to get reply from backend chatbot
std::string getNPCResponse(const std::string& input, const std::string& npc) {
    httplib::Client cli("localhost", 5005);
    json payload = { {"message", input}, {"npc_id", npc} };

    auto res = cli.Post("/chat", payload.dump(), "application/json");
    if (!res || res->status != 200) {
        std::cerr << "Failed to get response from server\n";
        return "We'll chat later";
    }

    auto replyJson = json::parse(res->body);
    return replyJson["reply"];
}

// Function to wrap text when it is too long
std::string wrapText(const std::string& text, const sf::Font& font, int characterSize, float maxWidth) {
    std::string wrappedText;
    std::string word;
    sf::Text tempText(font, "", characterSize);

    for (char c : text) {
        if (c == ' ' || c == '\n') {
            tempText.setString(wrappedText + word);
            if (tempText.getLocalBounds().size.x > maxWidth) {
                wrappedText += '\n';
            }
            wrappedText += word + c;
            word.clear();
        } else {
            word += c;
        }
    }
    // Add any remaining word
    tempText.setString(wrappedText + word);
    if (tempText.getLocalBounds().size.x > maxWidth) {
        wrappedText += '\n';
    }
    wrappedText += word;
    return wrappedText;
}

struct NPC {
    std::string id; // eg: "blacksmith"
    sf::CircleShape shape;
    sf::Text text;
    sf::Text name;
    sf::Color colour;
    bool speaking = false;
    std::chrono::steady_clock::time_point speakTime;

        NPC(const std::string& id,
        const sf::Font& font,
        sf::Vector2f pos,
        sf::Color colour,
        float radius = 50.f)
    : id(id),
      shape(radius), text(font, "", 20), name(font, id, 20)
    {
        shape.setFillColor(colour);
        shape.setPosition(pos);
        // Place text above NPC
        text.setPosition({
            shape.getPosition().x + shape.getRadius() - 80,
            shape.getPosition().y + shape.getRadius() - 120
        });
        // Centre text
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.size / 2.0f);
        // Place name text below NPC
        name.setPosition({
            shape.getPosition().x + shape.getRadius(),
            shape.getPosition().y + shape.getRadius() + 80
        });
        // Centre name text
        sf::FloatRect nameBounds = name.getLocalBounds();
        name.setOrigin(nameBounds.size / 2.0f);
    }
};

int main() {
    // Create game window and camera
    sf::RenderWindow window(sf::VideoMode({800, 600}), "NPC Chatbot");
    window.setFramerateLimit(60);
    sf::View camera;
    camera.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    window.setView(camera);

    // Create a background
    const sf::Texture backgroundTex("assets/images/Village.png");
    sf::Sprite background(backgroundTex);
    float scaleX = static_cast<float>(window.getSize().x) / backgroundTex.getSize().x;
    float scaleY = static_cast<float>(window.getSize().y) / backgroundTex.getSize().y;
    background.setScale({scaleX * 2, scaleY * 2});

    // Set font for text
    sf::Font font("assets/fonts/ARIAL.ttf");

    // Create player
    sf::CircleShape player(50);
    player.setFillColor(sf::Color::Green);
    player.setPosition({650, 550});
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
    std::vector<NPC> npcs;
    npcs.emplace_back("blacksmith", font, sf::Vector2f{550, 350}, sf::Color::Black);
    npcs.emplace_back("professor", font, sf::Vector2f{750, 800}, sf::Color::Cyan);
    npcs.emplace_back("shopkeeper", font, sf::Vector2f{1040, 380}, sf::Color::Red);
    npcs.emplace_back("thief", font, sf::Vector2f{320, 700}, sf::Color::Magenta);

    float talkRadius = 300.0f; // Only talk if player is this close
    NPC* closeNPC = nullptr; // Store closest NPC

    // Game loop
    while (window.isOpen()) {
        // Process events
        std::vector<sf::Event> events;
        while (const std::optional event = window.pollEvent())
        {
            sf::Event ev = *event;
            events.push_back(ev);
            // Close window properly
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

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
        inputText.setString(wrapText(playerInput, font, inputText.getCharacterSize(), 200.f)); // Set player text and wrap it so it does not fall off the screen
            
        // Loop through NPCs
        for (auto& npc : npcs) {
            // Set up player to NPC distance
            sf::Vector2f npcPos = npc.shape.getPosition();
            sf::Vector2f playerPos = player.getPosition();
            float distance = std::sqrt(std::pow(playerPos.x - npcPos.x, 2) + std::pow(playerPos.y - npcPos.y, 2)); // Calculate distance from player
            // If close to player (Can't type or chat unless close to an NPC)
            if (distance <= talkRadius) { 
                closeNPC = &npc;
            } else {
                closeNPC = nullptr;
            }
        }

        // Process events
        for (auto& event : events) {
            if (!closeNPC) break; // Don't bother if there is no NPC nearby
            NPC& npc = *closeNPC;

            // Check for player input or backspace
            if (const auto* textEntered = event.getIf<sf::Event::TextEntered>()) {
                // If backspace then remove from input
                if (static_cast<char>(textEntered->unicode) == '\b' && !playerInput.empty()) {
                    playerInput.pop_back();
                }
                // If a valid letter then add to input
                else if (textEntered->unicode < 128) {
                    playerInput += static_cast<char>(textEntered->unicode); }
            }
            // Check for Enter key
            if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
                // If enter then send player input to npc
                if (keyPressed->scancode == sf::Keyboard::Scan::Enter) {
                    std::string reply = getNPCResponse(playerInput, npc.id);
                    npc.text.setString(wrapText(reply, font, npc.text.getCharacterSize(), 200.f)); // Set NPC text and wrap it so it does not fall off the screen
                    playerInput.clear();
                    inputText.setString(""); // Clear the input display
                    npc.speaking = true;
                    npc.speakTime = std::chrono::steady_clock::now();
                }
            }
            // Check if enough time has passed to hide the text, if player doesn't move the text may stay for a while longer
            if (npc.speaking == true) {
                auto now = std::chrono::steady_clock::now();
                float passedTime = std::chrono::duration<float>(now - npc.speakTime).count();
                if (passedTime > 5.0f) {
                    npc.speaking = false;
                }
            }
        }
        // Draw and display the game window
        window.clear(sf::Color::Black);
        window.draw(background);
        window.draw(player);
        window.draw(inputText);
        camera.setCenter(player.getPosition());
        window.setView(camera);
        for (auto& npc : npcs) {
            window.draw(npc.shape);
            if (npc.speaking == true) {
                window.draw(npc.text);
            }
            window.draw(npc.name);
        }
        window.display();
    }
    return 0;
}
