from flask import Flask, request, jsonify, send_from_directory

app = Flask(__name__, static_folder=".")

# Lista para armazenar todos os dados recebidos
traffic_data_list = []

# Rota para servir o HTML da interface
@app.route('/')
def serve_html():
    return send_from_directory(app.static_folder, "interface.html")

# Rota para obter todos os dados recebidos
@app.route('/api/dados', methods=['GET'])
def get_traffic_data():
    return jsonify(traffic_data_list)

# Rota para atualizar o estado do semáforo
@app.route('/api/dados', methods=['POST'])
def update_traffic_state():
    global traffic_data_list
    data = request.get_json()

    # Validar se os dados enviados estão corretos
    if "state" in data and "touchCount" in data and "totalTouchTime" in data:
        traffic_data_list.append(data)  # Adicionar os dados recebidos à lista
        response = {"message": "Dados recebidos com sucesso!", "next_state": "NEXT"}
    else:
        response = {"message": "Erro: Dados inválidos recebidos."}

    return jsonify(response)

# Inicializar o servidor
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)
