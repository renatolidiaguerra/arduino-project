
// #### 1. **Sensor**
//    - Sensores são entidades que monitoram e relatam um valor. Eles não têm estado próprio de "ligado" ou "desligado".

//    - `battery` - Representa o nível de carga de uma bateria.
//    - `humidity` - Representa a umidade como uma porcentagem.
//    - `temperature` - Representa a temperatura em graus Celsius ou Fahrenheit.
//    - `pressure` - Representa a pressão em hPa, mbar ou mmHg.
//    - `illuminance` - Representa a iluminação em lx ou lm.
//    - `timestamp` - Representa uma marca de tempo.
//    - `signal_strength` - Representa a força do sinal (como WiFi ou Zigbee) em dB.
//    - `power` - Representa o consumo de energia em W.
//    - `energy` - Representa a quantidade de energia consumida em kWh.
//    - `carbon_dioxide` - Representa a concentração de CO2 em ppm.
//    - `carbon_monoxide` - Representa a concentração de CO em ppm.
//    - `volatile_organic_compounds` - Representa a concentração de compostos orgânicos voláteis em ppm.
//    - `voltage` - Representa a tensão em V.
//    - `current` - Representa a corrente elétrica em A.
//    - `frequency` - Representa a frequência em Hz.
//    - `power_factor` - Representa o fator de potência como um número decimal entre 0 e 1.

// #### 2. **Binary Sensor**
//    - Sensores binários são entidades que têm um estado de "ligado" ou "desligado".

//    - `battery` - Indica se a bateria está baixa.
//    - `cold` - Indica se está frio.
//    - `connectivity` - Indica o estado de conectividade (conectado/desconectado).
//    - `door` - Indica o estado de uma porta (aberta/fechada).
//    - `garage_door` - Indica o estado de uma porta de garagem.
//    - `gas` - Indica a detecção de gás.
//    - `heat` - Indica se está quente.
//    - `light` - Indica a presença de luz.
//    - `lock` - Indica o estado de uma trava (travado/destravado).
//    - `moisture` - Indica a presença de umidade.
//    - `motion` - Indica a presença de movimento.
//    - `occupancy` - Indica a ocupação de um espaço.
//    - `opening` - Indica o estado de abertura (aberto/fechado).
//    - `plug` - Indica se uma tomada está conectada.
//    - `power` - Indica o consumo de energia.
//    - `presence` - Indica a presença de alguém.
//    - `problem` - Indica a presença de um problema.
//    - `safety` - Indica o estado de segurança.
//    - `smoke` - Indica a presença de fumaça.
//    - `sound` - Indica a presença de som.
//    - `vibration` - Indica a presença de vibração.
//    - `window` - Indica o estado de uma janela (aberta/fechada).

// #### 4. **Switch**
//    - Representa dispositivos que podem ser "ligados" ou "desligados", como tomadas inteligentes.

//    **Atributos:**
//    - `is_on` - Estado de ligado/desligado.

// #### 13. **Input Boolean**
//    - Representa um interruptor booleano definido pelo usuário.

//    **Atributos:**
//    - `is_on` - Estado de ligado/desligado.

// #### 16. **Button**
//    - Representa um botão que pode ser pressionado para acionar uma ação.

//    **Atributos:**
//    - `state` - Estado de pressionado ou não.
