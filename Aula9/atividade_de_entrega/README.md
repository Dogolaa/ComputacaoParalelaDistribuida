# Sistema de Gestão de Veículos - Cliente/Servidor

Este projeto implementa um sistema de gestão de veículos utilizando o modelo cliente/servidor em Java. O servidor é multithreaded, capaz de atender a múltiplos clientes simultaneamente, controlando o acesso concorrente a um banco de dados em memória.

O sistema suporta as operações básicas de CRUD (Create, Read, Update, Delete) sobre os registros de veículos. Além do cliente interativo para um usuário, o projeto também inclui um cliente de teste de carga para avaliar o desempenho e a capacidade do servidor (Fase 3 do trabalho).

## Tecnologias Utilizadas

- **Java 11 (ou superior)**
- **Java Sockets** para comunicação em rede (TCP/IP).
- **Java Concurrency** (Threads, ExecutorService, AtomicInteger) para o servidor multithreaded e o cliente de teste.
- **Java Object Serialization** para a troca de objetos entre cliente e servidor.

## Estrutura do Projeto

- `Servidor.java`: Ponto de entrada principal do servidor. Ele inicializa o serviço e aguarda conexões de clientes.
- `Cliente.java`: O cliente interativo, que fornece um menu de console para o usuário final.
- `ClienteDeTeste.java`: Um cliente não-interativo projetado para realizar testes de carga e estresse no servidor.
- `GerenciadorCliente.java`: Classe `Runnable` que lida com toda a lógica de comunicação para um único cliente em uma thread dedicada.
- `BancoDeDados.java`: Simula o banco de dados em memória utilizando um `ConcurrentHashMap` para garantir a segurança em ambiente concorrente (thread-safe).
- `Veiculo.java`: Classe de modelo que representa um veículo.
- `Requisicao.java` / `Resposta.java`: Objetos utilizados para padronizar a comunicação (protocolo) entre cliente e servidor.
- `Operacao.java`: Enum que define as operações possíveis no sistema.

## Pré-requisitos

Para compilar e executar este projeto, você precisará ter o **Java Development Kit (JDK) versão 11 ou superior** instalado e configurado no seu sistema.

## Como Compilar

Todos os arquivos-fonte (`.java`) estão na mesma pasta, simplificando o processo de compilação.

1.  Abra um terminal (ou prompt de comando) na pasta raiz do projeto.
2.  Execute o seguinte comando para compilar todos os arquivos Java:

    ```bash
    javac *.java
    ```

3.  Este comando irá gerar os arquivos `.class` necessários para a execução.

    _Nota: Você poderá ver um aviso sobre "deprecated API" ao compilar. Isso é esperado e não impede a execução. É apenas uma nota do compilador sobre boas práticas em versões mais recentes do Java._

## Como Executar

A execução é dividida em dois cenários: o uso normal com o cliente interativo e a execução dos testes de carga. **Para ambos os cenários, você precisará de dois terminais abertos.**

### 1. Execução Normal (Uso Interativo)

Este modo permite que um usuário utilize o sistema através de um menu no console.

**Passo 1: Iniciar o Servidor**

- No **primeiro terminal**, execute o comando abaixo para iniciar o servidor. Ele ficará ativo, aguardando conexões de clientes.

  ```bash
  java Servidor
  ```

  Você verá a mensagem: `Servidor iniciado. Aguardando clientes na porta 54323...`

**Passo 2: Iniciar o Cliente Interativo**

- Abra um **SEGUNDO terminal**, na mesma pasta. Execute o comando para iniciar o cliente:

  ```bash
  java Cliente
  ```

  O menu interativo e personalizado aparecerá, e você poderá começar a usar o sistema.

### 2. Execução do Teste de Carga (Fase 3)

Este modo simula múltiplos usuários acessando o servidor simultaneamente para medir seu desempenho.

**Passo 1: Iniciar o Servidor**

- O primeiro passo é o mesmo: inicie o servidor no **primeiro terminal**.

  ```bash
  java Servidor
  ```

**Passo 2: Iniciar o Cliente de Teste**

- Abra um **SEGUNDO terminal**, na mesma pasta. Execute o comando para iniciar o cliente de teste:

  ```bash
  java ClienteDeTeste
  ```

  O cliente de teste começará a executar automaticamente, sem a necessidade de interação. Ele primeiro irá popular o banco de dados e depois iniciar o teste de carga. Ao final, um relatório de desempenho com métricas como requisições por segundo e tempo médio de resposta será exibido no console.
