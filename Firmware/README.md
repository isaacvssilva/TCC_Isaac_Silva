# Firmware - Miniestação Portátil Automatizada

Este diretório contém todo o firmware necessário para o funcionamento da miniestação automatizada, baseadas na Raspberry Pi Pico Zero (RP2040) e na comunicação sem fio LoRa.  

O sistema foi projetado para baixo consumo de energia e monitoramento ambiental, coletando dados de temperatura, umidade e precipitação, com transmissão periódica através da tecnologia LoRa.

---

## 📂 Estrutura do Diretório

A pasta `Firmware/` está organizada de forma modular para facilitar testes independentes e integração final:

📂 Firmware/ *(Código para os módulos individuais)*  
├── 📂 sht30-sensor/ *(Código para leitura do sensor de temperatura e umidade SHT30 via I2C)*  
│ └── `README.md` *(Documentação específica do sensor SHT30)*  
├── 📂 pluviometro-hall/ *(Código para medição de precipitação com sensor Hall A3144)*  
│ └── `README.md` *(Documentação do pluviômetro)*  
├── 📂 lora-lorawan/ *(Comunicação sem fio via LoRaWAN (SX1276))*  
│ └── `README.md` *(Configuração e uso do módulo LoRaWAN)*  
├── 📂 DS3231/ *(Testes de RTC e gerenciamento de baixo consumo)*  
│ └── `README.md` *(Implementação do Deep Sleep e Wake-up periódico)*  
├── 📂 final-integration/ *(Versão final do firmware, integrando todos os módulos do projeto)*
│ └── `README.md` *(Explicação sobre a versão final do firmware e integração dos módulos)*  
└── 📜 README.md *(Documentação geral do firmware)*  

---

## Sobre o Firmware

O firmware foi desenvolvido utilizando a RP2040 Zero (Waveshare), programado no ambiente PlatformIO com o framework Arduino, mas explorando funcionalidades do SDK do RP2040 para otimizar desempenho e eficiência energética.  

Cada módulo foi testado individualmente antes da integração final, garantindo precisão nos dados coletados, confiabilidade na transmissão LoRaWAN e eficiência energética.  

---

## Principais Funcionalidades

✔ Coleta de dados ambientais: Temperatura, umidade e precipitação.  
✔ Comunicação via LoRaWAN: Transmissão de dados para um servidor remoto.  
✔ Modo de baixo consumo: Implementação de Deep Sleep para maximizar a autonomia.  
✔ Modularidade: Sensores e módulos podem ser adicionados conforme a necessidade.  

---
