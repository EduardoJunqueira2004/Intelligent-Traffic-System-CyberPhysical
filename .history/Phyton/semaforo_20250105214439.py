from flask import Flask, request, jsonify

app = Flask(__name__)

# Estado inicial do semáforo
traffic_light_state = {
    "state": "OFF",
    "touchCount": 0,
    "totalTouchTime": 0
}

@app.route('/dados', methods=['GET'])
def get_traffic_data():
    # Retornar o estado atual do semáforo
    return jsonify(traffic_light_state)

@app.route('/dados', methods=['POST'])
def update_traffic_state():
    global traffic_light_state
    data = request.get_json()

    if "state" in data:
        if data["state"] == "NEXT":
            # Alternar para o próximo estado
            if traffic_light_state["state"] == "RED":
                traffic_light_state["state"] = "GREEN"
            elif traffic_light_state["state"] == "GREEN":
                traffic_light_state["state"] = "YELLOW"
            elif traffic_light_state["state"] == "YELLOW":
                traffic_light_state["state"] = "RED"
            else:
                traffic_light_state["state"] = "OFF"
    
    response = {"message": "Estado atualizado com sucesso!", "next_state": "NEXT"}
    return jsonify(response)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)
