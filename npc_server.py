from flask import Flask, request, jsonify
from openai import OpenAI
import os
from dotenv import load_dotenv

load_dotenv()

app = Flask(__name__)

# Set up gpt-oss-20b model
client = OpenAI(
    base_url="https://router.huggingface.co/v1",
    api_key = os.getenv("HF_API_KEY") # Will need to log into hugging face and get a free api key and make sure no can access it
)

NPC_PROFILES = {
    "blacksmith": {
        "name": "James",
        "personality": "Kind-hearted, loves talking about weapons and craftsmanship."
    },
    "professor": {
        "name": "Henry",
        "personality": "Curious and interested in history, always quoting ancient texts and overexplaining."
    },
    "thief": {
        "name": "Emily",
        "personality": "Sarcastic, street-smart, and in it for themself."
    },
    "shopkeeper": {
        "name": "Bill",
        "personality": "Little patience and came here to set up a shop to sell items they have collected over the years."
    }
}

# List of messages over time
# Keep separate histories for each NPC
npc_histories = {npc: [] for npc in NPC_PROFILES}

# Function to set up requirements and NPC personality
def build_system_messages(profile):
    return [
        {
            "role": "system",
            "content": (
                "You are in a large but empty world and prefer to speak with short responses. "
                "You must only reply using only ASCII characters, which means no emojis, no smart punctuation, or non-ASCII symbols. "
                "Avoid curly quotes, accented letters, and special characters. No dashes, no apostrophes. Keep it simple and readable. "
                "Ensure any text you give is compatible with the free version of ARIAL.TTF. "
                "Ensure responses are no longer than 20 words and don't give advice."
            )
        },
        {
            "role": "system",
            "content": f"You are {profile['name']}, an NPC in a fantasy world. "
                       f"Your personality: {profile['personality']}. Stay in character at all times."
        }
    ]

# Function to get a reply from AI
def generate_reply(message, npc_id):
    profile = NPC_PROFILES.get(npc_id)
    history = npc_histories[npc_id] # Stores previous messages for this NPC

    # Build system prompts and save new messages to history 
    message = message.lower()
    messages = build_system_messages(profile) + history + [
        {"role": "user", "content": message}
    ]
    history.append({"role": "user", "content": message})
    response = client.chat.completions.create(
        model="openai/gpt-oss-20b:fireworks-ai",
        messages=messages
    )
    history.append({"role": "assistant", "content": response.choices[0].message.content})
    print(response.choices[0].message.content)
    return (response.choices[0].message.content)
    
# Send reply to frontend
@app.route("/chat", methods=["POST"])
def chat():
    data = request.get_json()
    message = data.get("message", "")
    npc_id = data.get("npc_id", "")
    reply = generate_reply(message, npc_id)
    print(jsonify({"reply": reply}))
    return jsonify({"reply": reply})

if __name__ == "__main__":
    app.run(port=5005)
