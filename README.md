# Sistema de Cadastro de Relatos sobre Catástrofes Naturais (C)

## Alunos

* Lucas Kenzo Nishiwaki - RM561325
* Guilherme Morais de Assis - RM564198

## Disciplina

**Data Structures and Algorithms**

Curso: Ciência da Computação – 1º semestre

## 1. Objetivo do Projeto

O projeto tem como objetivo desenvolver um programa em linguagem C que permita o cadastro e a consulta de relatos sobre catástrofes naturais. A ideia é simular um sistema simples onde usuários podem registrar informações como nome, tipo de catástrofe, localização, data e hora do ocorrido.

Além disso, também implementamos uma busca por relatos que aconteceram próximos a um determinado ponto geográfico, e salvamos os dados em arquivo `.txt` para que fiquem guardados.

## 2. Funcionalidades do Sistema

* **Cadastro de Relatos:**
  O sistema coleta várias informações do relator (nome, CPF, telefone, e-mail), endereço completo, tipo da catástrofe, data, hora, localização (latitude/longitude) e uma descrição.

* **Listagem de Relatos:**
  Mostra todos os relatos cadastrados no sistema de forma organizada.

* **Consulta por Raio de 10 km:**
  O usuário informa coordenadas (latitude e longitude), e o sistema mostra os relatos que aconteceram até 10 km de distância. Usamos a fórmula de Haversine para calcular essa distância.

* **Ordenação:**
  Os relatos podem ser ordenados de 3 formas:

  * Por ID (crescente)
  * Por data (mais antiga primeiro)
  * Por tipo de catástrofe (ordem alfabética)
    Utilizamos o algoritmo **Bubble Sort**, que é simples e ideal para fins de aprendizado.

* **Validação de Dados:**
  Verificamos se os dados inseridos estão no formato correto, como CPF com 11 dígitos, e-mail com `@`, UF com 2 letras, entre outros.

* **Salvar e Carregar Arquivo:**
  Os relatos são salvos automaticamente em um arquivo `relatos_c.txt`. Quando o programa inicia, ele tenta carregar os dados salvos anteriormente.

* **Interface no Console:**
  Todo o funcionamento é por menu no terminal, com opções como Cadastrar, Listar, Consultar, Ordenar, Salvar e Sair.

## 3. Estrutura do Código

O programa foi feito em um único arquivo `.c` com as seguintes partes principais:

* Definição da `struct Relato`, que armazena todas as informações.
* Variáveis globais para o controle dos relatos e IDs.
* Funções para:

  * Cadastrar um novo relato
  * Listar relatos
  * Consultar por raio
  * Ordenar relatos
  * Validar entradas do usuário
  * Calcular distância entre coordenadas
  * Salvar e carregar os relatos de um arquivo
  * Exibir menus
* A função `main()` controla o menu principal e o fluxo geral do programa.

## 4. Exemplo do Menu

```
===== Sistema de Relatos de Catastrofes (C) =====
Relatos: 0/100

    1 - Cadastrar
    2 - Listar
    3 - Consultar Raio (10.0 km)
    4 - Ordenar
    5 - Salvar
    0 - Sair
==============================================
Escolha:
```

## 5. Observações

* O cálculo da distância geográfica usa a fórmula de Haversine, que considera a Terra como uma esfera (é uma aproximação).
* O limite de relatos é 100 (usamos um array fixo).
* Em compiladores online, a gravação de arquivos pode não funcionar corretamente entre sessões.
* Fizemos validações básicas nos campos, mas elas podem ser melhoradas com mais tempo (por exemplo, tratar anos bissextos na data).
