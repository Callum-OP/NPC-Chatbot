from flask import Flask, request, jsonify

app = Flask(__name__)

def generate_reply(message):
    message = message.lower()
    if "hello" in message:
        return "Hello, there."
    elif "good day" in message:
        return "Yes it is a good day."
    elif "bye" in message:
        return "Farewell."
    else:
        return "I do not understand. Could you rephrase that?"

@app.route("/chat", methods=["POST"])
def chat():
    data = request.get_json()
    player_message = data.get("message", "")
    reply = generate_reply(player_message)
    return jsonify({"reply": reply})

if __name__ == "__main__":
    app.run(port=5005)
