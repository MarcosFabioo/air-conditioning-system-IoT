# Sistema de controle de Ar-Condicionado

O Sistema de Controle de Ar-Condicionado é uma solução IoT desenvolvida por [Ruan Gustavo](mailto:ruan.gustavo@escolar.ifrn.edu.br) e [Marcos Fábio](mailto:marcos.fabio@escolar.ifrn.edu.br) para realizar a tarefa manual de ligar/desligar os aparelhos de ar-condicionado no Instituto Federal do Rio Grande do Norte.

## Sumário

- [Visão geral](#visão-geral)
- [Tecnologias](#tecnologias)
- [Funcionalidades](#funcionalidades)

## Visão geral

Na IFRN, gerenciar (ligar/desligar) os aparelhos de ar-condicionado é uma tarefa manual e demorada devido a quantidade de salas e aparelhos. Este projeto visa automatizar e simplificar o processo, fornecendo uma interface administrativa para controlar os aparelhos remotamente. Este microserviço IoT implantado ao aparelho de ar-condicionado por meio do `ESP8266` se comunica com o back-end através do protocolo MQTT e este, por sua vez, com o aplicativo front-end a fim de fornecer um controle e gerenciamento dos dispositivos.

## Tecnologias

- **Plataforma ESP8266:** uma plataforma de desenvolvimento para a criação de dispositivos conectados à internet baseados no chip ESP8266, que possui um processador de baixo custo e baixo consumo de energia, além de conexão Wi-Fi integrada.

- **Biblioteca ESP8266WiFi:** uma biblioteca de software que permite a conexão do dispositivo ESP8266 a uma rede Wi-Fi.

- **Biblioteca PubSubClient:** uma biblioteca de software que implementa o protocolo MQTT (Message Queuing Telemetry Transport), que é um protocolo de comunicação leve para dispositivos conectados à internet.

- **IRrecv.h e IRsend.h:** bibliotecas para decodificação e envio de sinais infravermelhos (IR).

- **Arduino.h:** biblioteca padrão do Arduino.

- **IRremoteESP8266.h e IRutils.h:** bibliotecas para controle de dispositivos IR utilizando o chip ESP8266.

- **Bibliotecas para controle de diversos modelos de ar condicionado**, como ir_Coolix.h, ir_Daikin.h, ir_Fujitsu.h, ir_Gree.h, ir_Haier.h, ir_Hitachi.h, ir_Kelvinator.h, ir_Midea.h, ir_Mitsubishi.h, ir_Panasonic.h, ir_Samsung.h, ir_Tcl.h, ir_Teco.h, ir_Toshiba.h, ir_Vestel.h e ir_Whirlpool.h.

- **Linguagem de programação C++:** é a linguagem utilizada para escrever o código-fonte do programa.

- **Ambiente de desenvolvimento integrado (IDE) Arduino:** é uma plataforma de software que fornece uma interface gráfica para o desenvolvimento de programas para microcontroladores, como o ESP8266.

## Funcionalidades

- **conexão com rede Wi-Fi:** a conexão Wi-Fi é estabelecida e configurada para utilizar o protocolo STA (Station mode).

- **Conexão MQTT:** envio e recebimento de mensagens por meio do protocolo MQTT.

- **Manipulação de sinais infravermelhos (IR):** recebimento, decodificação e utilização de sinais infravermelhos (IR) de diferentes marcas de ar condicionado.

- **Controle remoto dos aparelhos de ar-condicionado:** Ligar e desligar os aparelhos de ar-condicionado remotamente usando o protocolo MQTT. 
