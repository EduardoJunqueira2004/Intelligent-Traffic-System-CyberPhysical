# Intelligent-Traffic-System-CyberPhysical
Este repositório contém a implementação de um sistema ciberfísico para monitorização e gestão de tráfego urbano. O projeto inclui um semáforo inteligente, integração de sensores e comunicação com uma API via MQTT.

# Intelligent Traffic System - CyberPhysical Project

Este repositório contém a implementação de um sistema ciberfísico para monitorização e gestão de tráfego urbano. O projeto inclui um semáforo inteligente, integração de sensores e comunicação com uma API via MQTT.

## 📜 Descrição do Projeto

O **Intelligent Traffic System** foi projetado para simular e monitorizar o fluxo de tráfego em tempo real, utilizando um **ESP32** como microcontrolador principal. O sistema é composto por:
- Um semáforo de três luzes (vermelho, amarelo e verde) controlado de forma dinâmica.
- Um sensor de toque para deteção de peões.
- Comunicação Cliente/Servidor via **Wi-Fi** para enviar e receber comandos e dados do semáforo.

O projeto aborda conceitos de sistemas ciberfísicos, integração de sensores e IoT.

---

## 🔧 Componentes e Circuito
### Hardware Utilizado:
- ESP32
- Sensor de toque
- LEDs (vermelho, amarelo e verde)
- Breadboard e jumpers

### Diagrama do Circuito:
(Adiciona aqui uma imagem ou diagrama do circuito)

---

## 💻 Funcionalidades
- **Gestão de Tráfego**:
  - Alternância automática entre estados do semáforo (vermelho, amarelo e verde).
  - Controle manual do semáforo através do sensor de toque.
- **Recolha de Dados**:
  - Registo do número de interações com o sensor.
  - Registo do tempo em cada estado do semáforo.
- **Envio de Dados**:
  - Envio dos registos para uma API (servidor HTTP) em formato JSON.
- **Comandos Remotos**:
  - Receção de comandos do servidor para alterar os estados do semáforo.

---

## 🚀 Como Usar
1. Clone este repositório:
   ```bash
   git clone https://github.com/teu-usuario/Intelligent-Traffic-System-CyberPhysical.git
