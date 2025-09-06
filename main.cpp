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
    std::string wrapped;
    std::string word;
    sf::Text tempText(font, "", characterSize);
    std::string currentLine;
    std::istringstream words(text);

    // Loop through all words 
    while (words >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        tempText.setString(testLine);

        if (tempText.getLocalBounds().size.x > maxWidth) {
            // Add current line and start new line
            if (!wrapped.empty()) wrapped += '\n';
            wrapped += currentLine;
            currentLine = word;
        } else {
            currentLine = testLine;
        }
    }
    // Add any remaining words
    if (!currentLine.empty()) {
        if (!wrapped.empty()) wrapped += '\n';
        wrapped += currentLine;
    }

    return wrapped;
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
        // Colour text black
        text.setFillColor(sf::Color::Black);
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
            
        // Loop through NPCs
        for (auto& npc : npcs) {
            // Set up player to NPC distance
            sf::Vector2f npcPos = npc.shape.getPosition();
            sf::Vector2f playerPos = player.getPosition();
            float distance = std::sqrt(std::pow(playerPos.x - npcPos.x, 2) + std::pow(playerPos.y - npcPos.y, 2)); // Calculate distance from player
            // If close to player (Can't type or chat unless close to an NPC)
            if (distance <= talkRadius) { 
                closeNPC = &npc;
            }
        }

        // Process events
        for (auto& event : events) {
            if (!closeNPC) continue; // Don't bother if there is no NPC nearby
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
        }

        // Check if enough time has passed to hide the text
        for (auto& npc : npcs) {
            if (npc.speaking) {
                auto now = std::chrono::steady_clock::now();
                float passedTime = std::chrono::duration<float>(now - npc.speakTime).count();
                if (passedTime > 7.0f) { // Show text for about 7 seconds
                    npc.speaking = false;
                }
            }
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
        // Colour text
        inputText.setFillColor(sf::Color::Black);

        // Draw and display the game window
        window.clear(sf::Color::Black);
        window.draw(background);
        window.draw(player);
        // Draw text inside a speech bubble
        std::string trimmedInput = playerInput;
        // Remove whitespace
        trimmedInput.erase(std::remove_if(trimmedInput.begin(), trimmedInput.end(), ::isspace), trimmedInput.end());
        if (!trimmedInput.empty()) {
            sf::FloatRect textBoundsP = inputText.getLocalBounds();
            float padding = 5.f; // Add white space around text

            // Create speech bubble
            sf::RectangleShape bubble;
            bubble.setFillColor(sf::Color::White);
            bubble.setOutlineColor(sf::Color::Black);
            bubble.setOutlineThickness(2.f);
            // Size it to fit the text
            bubble.setSize({textBoundsP.size.x + padding * 2, textBoundsP.size.y + padding * 2});

            // Center text horizontally
            inputText.setOrigin({textBoundsP.size.x / 2.f, 0.f});
            // Calculate how far above the NPC to place the bubble
            float baseOffset = player.getRadius() + 10.f;
            float extraOffset = textBoundsP.size.y / 2.f;
            // Position the text above the NPC
            inputText.setPosition(
                {player.getPosition().x + player.getRadius(),
                player.getPosition().y - baseOffset - extraOffset}
            );
            // Recalculate bounds
            textBoundsP = inputText.getLocalBounds();
            // Position so that it fully covers the text
            bubble.setOrigin({bubble.getSize().x / 2.f, 0.f});
            bubble.setPosition({inputText.getPosition().x, inputText.getPosition().y + textBoundsP.position.y - padding});
            // Draw speech bubble, then text
            window.draw(bubble);
            window.draw(inputText);
        }
        camera.setCenter(player.getPosition());
        window.setView(camera);
        for (auto& npc : npcs) {
            window.draw(npc.shape);
            // Draw text inside a speech bubble
           if (npc.speaking) {
                sf::FloatRect textBounds = npc.text.getLocalBounds();
                float padding = 5.f; // Add white space around text

                // Create speech bubble
                sf::RectangleShape bubble;
                bubble.setFillColor(sf::Color::White);
                bubble.setOutlineColor(sf::Color::Black);
                bubble.setOutlineThickness(2.f);
                // Size it to fit the text
                bubble.setSize({textBounds.size.x + padding * 2, textBounds.size.y + padding * 2});

                // Center text horizontally
                npc.text.setOrigin({textBounds.size.x / 2.f, 0.f});
                // Calculate how far above the NPC to place the bubble
                float baseOffset = npc.shape.getRadius() + 10.f;
                float extraOffset = textBounds.size.y / 2.f;
                // Position the text above the NPC
                npc.text.setPosition(
                    {npc.shape.getPosition().x + npc.shape.getRadius(),
                    npc.shape.getPosition().y - baseOffset - extraOffset}
                );
                // Recalculate bounds
                textBounds = npc.text.getLocalBounds();
                // Position so that it fully covers the text
                bubble.setOrigin({bubble.getSize().x / 2.f, 0.f});
                bubble.setPosition({npc.text.getPosition().x, npc.text.getPosition().y + textBounds.position.y - padding});
                // Draw speech bubble, then text
                window.draw(bubble);
                window.draw(npc.text);
            }
            window.draw(npc.name);
        }
        window.display();
    }
    return 0;
}
