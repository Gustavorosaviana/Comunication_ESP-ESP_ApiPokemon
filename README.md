# 🎮 Projeto ESP32 – Pokédex com MQTT e API Pokémon

**Autor:** Gustavo Rosa Viana  
**Projeto:** Interação entre ESP32  
**Versão:** 0.1  
**Data:** 05/11/2026

---

## 1. Sobre o projeto

Este projeto consiste no desenvolvimento de uma Pokédex utilizando o microcontrolador ESP32. O sistema permite selecionar diferentes Pokémon por meio de botões físicos, consultar suas informações em uma API externa e apresentar os dados em um display LCD.

Além disso, o projeto utiliza o protocolo MQTT para realizar a comunicação entre o ESP32 e uma aplicação externa, permitindo enviar informações sobre o Pokémon selecionado, acompanhar o estado da conexão e receber comandos remotamente.

O objetivo é integrar conceitos de programação embarcada, comunicação entre dispositivos, redes e consumo de APIs.

## 2. Tecnologias e ferramentas utilizadas

### ESP32

O ESP32 é o microcontrolador responsável pelo funcionamento de todo o projeto. Ele controla os botões, o display LCD, os LEDs e realiza as comunicações pela internet.

Entre suas principais responsabilidades estão:
- Conectar-se à rede Wi-Fi.
- Estabelecer comunicação com o broker MQTT.
- Realizar requisições HTTP para a API Pokémon.
- Interpretar informações recebidas em JSON.
- Controlar a interface do usuário por meio dos botões e do LCD.

### Wi-Fi

O Wi-Fi é a tecnologia utilizada para conectar o ESP32 à internet.

Por meio dessa conexão, o microcontrolador consegue acessar a API Pokémon e estabelecer comunicação com o servidor MQTT.

A conexão é realizada utilizando o nome da rede (SSID) e sua senha, definidos no código.

### MQTT

O MQTT (Message Queuing Telemetry Transport) é um protocolo de comunicação leve, bastante utilizado em projetos de Internet das Coisas (IoT).

Seu funcionamento é baseado no modelo Publisher/Subscriber (publicador e assinante).

Nesse projeto, o ESP32 desempenha duas funções:

- **Publisher:** envia informações sobre o Pokémon selecionado e o estado da conexão.
- **Subscriber:** recebe mensagens e comandos enviados por outros dispositivos.

A comunicação acontece por meio de tópicos, que funcionam como canais de envio e recebimento de mensagens.

Os tópicos utilizados são:

| Tópico | Função |
|---|---|
| `senai134/esp32/eduardo-gustavo/comando` | Receber comandos externos. |
| `senai134/esp32/eduardo-gustavo/status` | Publicar informações da conexão. |
| `senai134/esp32/eduardo-gustavo/pokemon` | Publicar informações do Pokémon. |

### Broker EMQX

O EMQX é o broker MQTT utilizado no projeto.

O broker funciona como um intermediário entre os dispositivos conectados. Ele recebe as mensagens publicadas e encaminha essas informações para os clientes inscritos nos respectivos tópicos.

O endereço utilizado é:

`broker.emqx.io`

Porta: `1883`.

É importante destacar que esse é um broker público, portanto, os tópicos utilizados não devem ser considerados privados.

### MQTTX

O MQTTX é uma ferramenta utilizada para visualizar, testar e gerenciar comunicações MQTT.

Por meio dele, é possível estabelecer uma conexão com o mesmo broker utilizado pelo ESP32, acompanhar as mensagens publicadas e enviar comandos para os tópicos em que o microcontrolador está inscrito.

No projeto, o MQTTX permite:
- Visualizar os dados dos Pokémon selecionados.
- Acompanhar o estado da conexão do ESP32.
- Consultar informações como endereço IP e identificação do usuário.
- Enviar comandos em JSON para controlar o LED RGB.

### HTTP

O HTTP (Hypertext Transfer Protocol) é o protocolo utilizado para realizar a comunicação entre o ESP32 e a API Pokémon.

O projeto utiliza o método HTTP GET, que permite solicitar informações de um servidor.

Quando um Pokémon é selecionado, o ESP32 realiza uma requisição para o endereço da API, utilizando o ID correspondente.

### JSON

O JSON (JavaScript Object Notation) é um formato utilizado para organizar e transmitir informações.

Nesse projeto, ele é utilizado tanto na comunicação com a API quanto nas mensagens MQTT.

A biblioteca ArduinoJson permite:
- Desserializar: transformar o JSON recebido em informações que o programa consegue utilizar.
- Serializar: transformar as informações do programa em uma mensagem JSON para transmissão.

### API Pokémon – PokéAPI

A PokéAPI é uma API pública e gratuita que disponibiliza informações sobre os Pokémon.

O endereço utilizado no projeto é:

`https://pokeapi.co/api/v2/pokemon/`

A consulta é realizada utilizando o ID do Pokémon. Por exemplo:

`https://pokeapi.co/api/v2/pokemon/25`

Essa requisição retorna diversas informações, como nome, identificação, tipos, habilidades e outras características.

Entretanto, o projeto utiliza filtros para selecionar apenas os dados necessários, principalmente nome, ID e tipos do Pokémon.

## 3. Componentes utilizados

- ESP32.
- Display LCD I2C 20x4.
- Módulo de comunicação Wi-Fi integrado ao ESP32.
- Três botões físicos:
  - Botão para cima.
  - Botão para baixo.
  - Botão de confirmação (ENTER).
- LED RGB integrado ao ESP32.
- Bibliotecas ArduinoJson, PubSubClient, LiquidCrystal_I2C, WiFi e HTTPClient.

## 4. Lógica de funcionamento do projeto

O funcionamento do sistema pode ser dividido em algumas etapas.

### 4.1 Inicialização

Ao ligar o ESP32, o programa inicia a comunicação serial, inicializa o display LCD e liga sua iluminação.

Em seguida, executa a função de conexão, responsável por conectar o dispositivo à rede Wi-Fi e ao broker MQTT.

Depois das conexões, os botões são inicializados e o display apresenta a tela inicial da Pokédex.

### 4.2 Seleção do Pokémon

O usuário utiliza os botões físicos para navegar entre os Pokémon.

- **Botão para cima:** aumenta o ID do Pokémon.
- **Botão para baixo:** diminui o ID do Pokémon.
- **Botão ENTER:** confirma a seleção e solicita os dados completos.

O sistema trabalha com IDs entre 1 e 1025.

Durante a navegação, o ESP32 consulta a PokéAPI para descobrir o nome correspondente ao ID selecionado e atualiza as informações no LCD.

### 4.3 Consulta à API

Quando o usuário seleciona um Pokémon, o ESP32 realiza uma requisição HTTP GET para a PokéAPI.

A resposta é recebida no formato JSON e processada pela biblioteca ArduinoJson.

O programa utiliza um filtro para evitar o processamento de informações desnecessárias, extraindo somente os dados importantes para o projeto.

### 4.4 Exibição das informações

Após receber e interpretar os dados, o ESP32 apresenta no LCD:

- Nome do Pokémon.
- Número da Pokédex.
- Primeiro tipo.
- Segundo tipo, quando disponível.

Caso aconteça algum erro durante a consulta ou interpretação dos dados, o sistema apresenta uma mensagem de erro.

### 4.5 Comunicação MQTT

Depois de confirmar a seleção, o ESP32 organiza os dados do Pokémon em um documento JSON.

As informações são publicadas no tópico destinado aos Pokémon.

Dessa maneira, o MQTTX consegue receber e visualizar os dados enviados pelo microcontrolador.

Além disso, o ESP32 publica periodicamente informações sobre seu estado de conexão.

### 4.6 Recebimento de comandos

O ESP32 permanece inscrito no tópico de comandos MQTT.

Quando recebe uma mensagem, o programa interpreta seu conteúdo em JSON e verifica o valor da variável `estadoLed`.

- Se o valor for `true`, o LED RGB é ligado.
- Se o valor for `false`, o LED RGB é desligado.

Assim, é possível controlar o LED remotamente utilizando o MQTTX.

## 5. Funções declaradas no projeto

Nesta seção estão descritas as funções responsáveis pelas principais operações do sistema.

### `pedirParaApi(int id)`

Responsável por realizar a comunicação HTTP com a PokéAPI.

Recebe como parâmetro o ID do Pokémon, monta o endereço da requisição, executa o método GET e retorna a resposta recebida do servidor em formato JSON.

Caso a requisição não seja bem-sucedida, apresenta o código HTTP de erro no Monitor Serial.

### `buscarNomePokemon(int id)`

Utilizada durante a navegação entre os Pokémon.

Recebe o ID selecionado, solicita os dados à API e utiliza a biblioteca ArduinoJson para extrair somente o nome do Pokémon.

O nome encontrado é armazenado na variável `nomePokemon`, permitindo sua apresentação na tela inicial.

### `montarTelalcd(int id)`

Responsável por montar a tela de informações completas do Pokémon.

Recebe o ID, consulta a API e interpreta os dados recebidos.

Extrai o nome, o ID e os tipos do Pokémon, armazenando os tipos nas variáveis correspondentes.

Depois, apresenta essas informações no display LCD.

Retorna `true` quando consegue processar os dados corretamente e `false` quando ocorre algum erro.

### `aoReceber(char* topico, byte* payload, unsigned int tamanho)`

É a função de callback do MQTT.

Ela é executada automaticamente quando o ESP32 recebe uma mensagem em um tópico no qual está inscrito.

Sua responsabilidade é reconstruir a mensagem recebida, apresentar o tópico e o conteúdo no Monitor Serial e encaminhar o JSON para a função `tratarJson()`.

Também realiza alterações no LED RGB como indicação visual do recebimento.

### `tratarJson(String mensagem)`

Responsável por interpretar as mensagens JSON recebidas pelo MQTT.

Utiliza a biblioteca ArduinoJson para realizar a desserialização e extrair a variável `estadoLed`.

Com base no valor recebido, determina se o LED RGB deve ser ligado ou desligado.

Caso o JSON apresente algum erro de interpretação, a função informa o problema no Monitor Serial.

### `telaInicial()`

Responsável pela organização e atualização da interface inicial do LCD.

Apresenta o ID selecionado, o nome do Pokémon e a orientação para utilizar o botão ENTER.

É chamada durante a inicialização e sempre que o usuário altera o Pokémon selecionado.

### `statusConexao()`

Responsável por monitorar e publicar informações sobre o estado da conexão do ESP32.

A cada cinco segundos, organiza informações em JSON, incluindo:

- Estado da conexão MQTT.
- Identificação do usuário.
- Endereço IP do ESP32.
- Timestamp.
- Mensagem de erro.

Quando o MQTT está conectado, publica essas informações no tópico de status.

Também apresenta os dados enviados no Monitor Serial.

### `conexao()`

Responsável por estabelecer as conexões necessárias para o funcionamento do projeto.

Primeiramente, conecta o ESP32 à rede Wi-Fi.

Após conseguir acesso à rede, configura o broker MQTT, define a função de callback e tenta estabelecer a conexão com o servidor.

Se a conexão MQTT for bem-sucedida, o ESP32 realiza a inscrição no tópico de comandos.

Durante esse processo, o display LCD e o LED RGB apresentam indicações visuais sobre o estado das conexões.

### `mqttPokemon()`

Responsável por organizar e publicar as informações completas do Pokémon selecionado.

Cria um documento JSON contendo:

- Usuário.
- Nome do Pokémon.
- ID da Pokédex.
- Primeiro tipo.
- Segundo tipo.

Depois, realiza a serialização do documento e publica a mensagem no tópico MQTT destinado aos Pokémon.

## 6. Funções principais do Arduino

### `setup()`

É executada uma única vez quando o ESP32 é ligado ou reiniciado.

Sua função é preparar os componentes e os recursos necessários para o funcionamento do sistema.

Nessa etapa, são inicializados:
- Comunicação serial.
- Display LCD.
- Conexão Wi-Fi e MQTT.
- Botões físicos.
- Tela inicial.

### `loop()`

É executada continuamente enquanto o ESP32 estiver ligado.

Nela acontece o funcionamento principal do projeto.

O programa mantém o processamento das mensagens MQTT, atualiza o estado da conexão, verifica os botões pressionados e executa as ações correspondentes.

É nessa função que acontece a navegação entre os Pokémon e a confirmação da escolha.

## 7. Bibliotecas utilizadas

| Biblioteca | Finalidade |
|---|---|
| `Arduino.h` | Recursos básicos de programação do Arduino. |
| `WiFi.h` | Conexão do ESP32 com redes Wi-Fi. |
| `PubSubClient.h` | Comunicação utilizando o protocolo MQTT. |
| `ArduinoJson.h` | Criação e interpretação de mensagens JSON. |
| `Botao.h` | Controle e leitura dos botões físicos. |
| `LiquidCrystal_I2C.h` | Comunicação e controle do display LCD I2C. |
| `HTTPClient.h` | Realização de requisições HTTP. |

## 8. Objetivos de aprendizagem

O desenvolvimento desse projeto permite aplicar conhecimentos de diferentes áreas da automação e programação:

- Programação de microcontroladores ESP32.
- Comunicação entre dispositivos utilizando MQTT.
- Conexão com redes Wi-Fi.
- Consumo de APIs externas utilizando HTTP.
- Manipulação e interpretação de arquivos JSON.
- Integração entre hardware e software.
- Utilização de ferramentas de monitoramento MQTT.
- Desenvolvimento de interfaces utilizando displays LCD e botões.

## 9. Conclusão

O projeto demonstra como um microcontrolador ESP32 pode integrar diferentes tecnologias para desenvolver um sistema conectado à internet.

A utilização da PokéAPI permite obter informações externas, enquanto o protocolo MQTT possibilita a comunicação com outras aplicações e dispositivos.

A integração entre Wi-Fi, HTTP, JSON e MQTT demonstra na prática conceitos importantes da Internet das Coisas (IoT).

Além de apresentar informações dos Pokémon em um display LCD, o sistema permite monitoramento e controle remoto, tornando o projeto uma aplicação prática de comunicação entre dispositivos inteligentes.
