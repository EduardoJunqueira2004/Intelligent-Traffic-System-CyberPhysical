from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS

app = Flask(__name__, static_folder=".")
CORS(app)

# Estado inicial do semáforo
traffic_light_state = {
    "state": "OFF",
    "touchCount": 0,
    "totalTouchTime": 0
}

# Rota para servir o HTML da interface
@app.route('/')
def serve_html():
    return send_from_directory(".", "interface.html")  # Certifique-se de que o HTML está na mesma pasta que este script

# Rota para obter os dados do semáforo
@app.route('/api/dados', methods=['GET'])
def get_traffic_data():
    return jsonify(traffic_light_state)

# Rota para atualizar o estado do semáforo
@app.route('/api/dados', methods=['POST'])
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

# Inicializar o servidor
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)
