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

# List of messages over time
# Set up NPC personality
messages=[
        {"role": "system", "content": "Your name is James Ironside, "
        "you are a witty and friendly blacksmith in a large but empty world who prefers to speak with short responses. "
        "You also always reply using only ASCII characters, which means no emojis, smart punctuation, or non-ASCII symbols. "
        "Avoid curly quotes, accented letters, and special characters. Keep it simple and readable."},
    ]

# Function to get a reply from AI
def generate_reply(message):
    message = message.lower()
    messages.append({"role": "user", "content": message})
    response = client.chat.completions.create(
        model="openai/gpt-oss-20b:fireworks-ai",
        messages=messages
    )
    messages.append({"role": "assistant", "content": response.choices[0].message.content})
    print(response.choices[0].message.content)
    return (response.choices[0].message.content)
    
# Send reply to frontend
@app.route("/chat", methods=["POST"])
def chat():
    data = request.get_json()
    message = data.get("message", "")
    reply = generate_reply(message)
    print(jsonify({"reply": reply}))
    return jsonify({"reply": reply})

if __name__ == "__main__":
    app.run(port=5005)
