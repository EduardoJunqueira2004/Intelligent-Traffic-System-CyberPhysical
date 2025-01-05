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
    # Receber o JSON do ESP32
    data = request.get_json()
    if "state" in data:
        traffic_light_state["state"] = data["state"]
    if "touchCount" in data:
        traffic_light_state["touchCount"] = data["touchCount"]
    if "totalTouchTime" in data:
        traffic_light_state["totalTouchTime"] = data["totalTouchTime"]
    
    return jsonify({"message": "Estado atualizado com sucesso!"})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)
