<a id="readme-topo"></a>

<h3 align="center">Data Logger de Baixo Consumo - MCU ESP32</h3>

  <p align="center">
    Coleta temperatura e luminosidade, grava em LittleFS, sincroniza via NTP, envia dados por HTTP usando Wi-Fi e entra em deep-sleep após cada ciclo.
    <br/>
    <a><strong>Relatório Técnico »</strong></a>
    <br/>
    <br/>
    <a>esp32-wroom-32u</a>
    &middot;
    <a>wokwi</a>
    &middot;
    <a>platformIO</a>
	&middot;
	<a>arduino-esp32</a>
	&middot;
	<a>C++</a>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Índice de Conteúdo</summary>
  <ol>
    <li>
      <a href="#sobre-o-projeto">Sobre o Projeto</a>
      <ul>
        <li><a href="#funcionalidades">Funcionalidades</a></li>
      </ul>
    </li>
    <li>
      <a href="#tecnologias-usadas">Tecnologias Usadas</a>
      <ul>
        <li><a href="#softwares">Softwares</a></li>
        <li><a href="#hardwares">Hardwares</a></li>
      </ul>
    </li>
    <li><a href="#estutura-do-repositorio">Estrutura do Repositório</a></li>
    <li><a href="#demonstracao">Demonstração</a></li>
    <li><a href="#instalacao">Instalação</a></li>
    <li><a href="#license">Licença</a></li>
    <li><a href="#autor">Autor</a></li>
  </ol>
</details>

<h2 id="sobre-o-projeto"> Sobre o Projeto</h2>

O projeto `datalogger-lowpower-esp32` é um sistema embarcado voltado para o estudo de firmware usando técnicas de gerenciamento de energia, persistência de dados e sincronização NTP no ambiente do ESP32. Desenvolvido para operar em ciclos curtos de atividade, o dispositivo desperta, realiza medições, grava os dados e retorna ao modo de hibernação profunda.

A arquitetura enfatiza três pilares fundamentais em dispositivos IoT:

-   Eficiência Energética — uso do modo deep-sleep e temporizadores RTC para minimizar consumo.
-   Confiabilidade Temporal — sincronização NTP com fallback para RTC e offset persistente.
-   Integridade de Dados — verificação CRC32 ou hash, estrutura de arquivo robusta e envio controlado via HTTP.

Sua estrutura modular permite adaptação a sensores reais ou virtuais (via Wokwi).

<h3 id="funcionalidades"> Funcionalidades</h3>

-   ⏰ Acorda periodicamente (a cada 5 minutos) via timer RTC ou botão físico (GPIO).

-   🎚️ Leitura de sensores de temperatura (sensor virtual NTC) e luminosidade (sensor virtual LDR).

-   🌐 Sincronização de tempo via NTP, com fallback para relógio RTC com offset salvo.

-   💾 Gravação de dados em LittleFS, formato CSV com cabeçalho de versão.

-   ✅ Integridade assegurada por CRC32 por registro ou hash no trailer do arquivo.

-   📤 Envio de dados via HTTP POST, quando rede Wi-Fi disponível (mock ou endpoint real).

-   💤 Modo Deep-Sleep automático após gravação ou envio, garantindo baixo consumo.

-   🔁 Retenção RTC de variáveis: número de boots, último timestamp válido e falhas de upload.

<p align="right">(<a href="#readme-topo">voltar para o topo</a>)</p>

<h2 id="tecnologias">Tecnologia Usadas</h2>

<h3>Software</h3>

-   **Linguagem C++** — utilizada para programação de baixo nível para controle do hardware.
-   **PlatformIO e VS Code** — ambiente de desenvolvimento integrado para compilação, gerenciamento de bibliotecas e monitoramento serial.

-   **Simulador Wokwi** — ambiente de simulação online que reproduz o comportamento elétrico e lógico do ESP32, sensores e periféricos.

-   **LittleFS** — sistema de arquivos utilizado para registro local dos dados coletados em formato CSV.

-   **NTP (Network Time Protocol)** — protocolo de sincronização temporal utilizado para garantir precisão dos timestamps durante a coleta de dados.

-   **Biblioteca auxiliar Arduino-ESP32** — utilizadas para operações de rede, serialização de dados e controle de energia.

<h3>Hardware</h3>

-   **ESP32 (WROOM 32U)** — microcontrolador de 32 bits com conectividade Wi-Fi e Bluetooth, suporte nativo a deep-sleep e armazenamento em flash.

-   **Sensores Virtuais (Wokwi)** — sensores _simulados_ de temperatura (NTC) e luminosidade (LDR), configurados para testes de coleta e armazenamento de dados.

-   **Botão de Wake-up (GPIO)** — acionador _virtual_ utilizado para despertar o microcontrolador do modo de baixo consumo.

-   **Alimentação via USB** — permite tanto medições de consumo energético no hardware físico quanto a execução em ambiente virtual.

<p align="right">(<a href="#readme-topo">voltar para o topo</a>)</p>
