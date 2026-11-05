# Lista Circular e o Problema de Josefo em C

[![CI](https://github.com/sidsidoso/lista-circular-c/actions/workflows/ci.yml/badge.svg)](https://github.com/sidsidoso/lista-circular-c/actions/workflows/ci.yml)
![Linguagem](https://img.shields.io/badge/linguagem-C11-blue.svg)
[![Licença: MIT](https://img.shields.io/badge/licen%C3%A7a-MIT-green.svg)](LICENSE)

Implementação de uma **lista circular simplesmente encadeada** de inteiros em
C11 que guarda **apenas um ponteiro para o último nó** — o suficiente para
inserir no início e no fim em O(1) — com rotação de *k* posições e, como
aplicação, o clássico **problema de Josefo**, resolvido por simulação na lista
e conferido com a fórmula recursiva/iterativa J(n, k).
Projeto desenvolvido para a disciplina de **Estrutura de Dados**.

---

## Sumário

- [Conceito](#conceito)
- [Aplicação: problema de Josefo](#aplicação-problema-de-josefo)
- [Operações e complexidade](#operações-e-complexidade)
- [Decisões de implementação e otimizações](#decisões-de-implementação-e-otimizações)
- [Estrutura de pastas](#estrutura-de-pastas)
- [Como compilar, executar e testar](#como-compilar-executar-e-testar)
- [Exemplo de saída](#exemplo-de-saída)
- [Referências](#referências)

---

## Conceito

Em uma lista **circular** o último nó não aponta para `NULL`, e sim de volta
para o primeiro. Guardando só o ponteiro `ultimo`, o primeiro elemento é
`ultimo->prox` — as duas pontas ficam a **um passo** de distância.

```c
typedef struct No {
    int valor;
    struct No *prox;
} No;

struct ListaCircular {
    No *ultimo;      /* primeiro = ultimo->prox */
    size_t tamanho;
};
```

```
                 ultimo
                   |
                   v
  +----+   +----+   +----+
  | 10 |-->| 20 |-->| 30 |--+
  +----+   +----+   +----+  |
    ^                       |
    +-----------------------+     primeiro = ultimo->prox = 10
```

**Inserir no início**: o novo nó entra entre `ultimo` e o primeiro.
**Inserir no fim**: é o mesmo encaixe, seguido de `ultimo = ultimo->prox`.

```
 inserir_inicio(5):   ultimo(30) -> [5] -> 10 -> 20 -> 30
 inserir_fim(40):     ultimo(30) -> [40] -> 10 -> 20 -> 30   (encaixa como no início)
                      ultimo = 40                            (só avança o ponteiro)
                      10 -> 20 -> 30 -> 40 -> (10)
```

**Girar *k* posições** não move nenhum nó: basta avançar `ultimo` *k* vezes
(com *k* reduzido módulo *n*). Girar para a direita (k negativo) equivale a
girar `n − (|k| mod n)` para a esquerda.

```
 girar(2):   [10 20 30 40 50]   ->   [30 40 50 10 20]
             ultimo: 50 -> 10 -> 20
```

## Aplicação: problema de Josefo

*n* pessoas numeradas de 1 a *n* formam um círculo; a contagem começa na
pessoa 1 e cada *k*-ésima pessoa é eliminada, recomeçando a contagem na
seguinte. Quem sobra?

**Simulação na lista** (`josefo_simular`): a pessoa da vez é sempre a primeira
da lista. A cada rodada, `lista_girar(k − 1)` leva a *k*-ésima pessoa para a
frente e `lista_remover_inicio` a elimina — ambas as operações já existem na
lista, sem nenhum código especial.

```
 n = 7, k = 3
 [1 2 3 4 5 6 7]  girar(2) -> [3 4 5 6 7 1 2]  remove 3 -> [4 5 6 7 1 2]
 [4 5 6 7 1 2]    girar(2) -> [6 7 1 2 4 5]    remove 6 -> [7 1 2 4 5]
 ...                                                     sobrevivente: 4
```

**Fórmula** (`josefo_formula` iterativa, `josefo_recursivo` recursiva). Após a
primeira eliminação sobra um círculo de *n − 1* pessoas começando *k* posições
à frente, o que dá a recorrência (posições base 0):

```
 J(1, k) = 0
 J(n, k) = (J(n − 1, k) + k) mod n          sobrevivente = J(n, k) + 1
```

Para **k = 2** existe forma fechada (`josefo_k2`): escrevendo
`n = 2^m + L` com `0 ≤ L < 2^m`, o sobrevivente é `2L + 1`.

Os testes comparam a simulação com as três fórmulas para todo
`1 ≤ n ≤ 120` e `1 ≤ k ≤ 25`, e conferem que a ordem de eliminação é uma
permutação de 1..n.

## Operações e complexidade

| Operação                       | Função                     | Tempo                 | Espaço extra |
|--------------------------------|----------------------------|-----------------------|--------------|
| Criar                          | `lista_criar`              | O(1)                  | O(1)         |
| Destruir                       | `lista_destruir`           | O(n)                  | O(1)         |
| Inserir no início              | `lista_inserir_inicio`     | **O(1)**              | O(1)         |
| Inserir no fim                 | `lista_inserir_fim`        | **O(1)**              | O(1)         |
| Inserir na posição *k*         | `lista_inserir_posicao`    | O(k)                  | O(1)         |
| Remover do início              | `lista_remover_inicio`     | O(1)                  | O(1)         |
| Remover da posição *k*         | `lista_remover_posicao`    | O(k)                  | O(1)         |
| Remover por valor              | `lista_remover_valor`      | O(n)                  | O(1)         |
| Buscar valor                   | `lista_buscar`             | O(n)                  | O(1)         |
| Obter posição *k*              | `lista_obter`              | O(k); 1º e último O(1)| O(1)         |
| Girar *k* posições             | `lista_girar`              | O(k mod n)            | O(1)         |
| Tamanho / vazia                | `lista_tamanho`, `lista_vazia` | O(1)              | O(1)         |
| Percorrer com callback         | `lista_percorrer`          | O(n)                  | O(1)         |
| Josefo por simulação           | `josefo_simular`           | O(n · min(k, n))      | O(n)         |
| Josefo pela recorrência        | `josefo_formula`           | O(n)                  | O(1)         |
| Josefo recursivo               | `josefo_recursivo`         | O(n)                  | O(n) (pilha) |
| Josefo k = 2 (forma fechada)   | `josefo_k2`                | O(log n)              | O(1)         |

Espaço total da lista: **O(n)** — um nó por elemento e um único ponteiro de
controle.

## Decisões de implementação e otimizações

- **Só o ponteiro `ultimo`**: dá acesso O(1) ao último *e* ao primeiro
  (`ultimo->prox`), economizando o ponteiro de cabeça. `inserir_fim` reutiliza
  `inserir_inicio` e apenas avança `ultimo`.
- **Rotação sem mover nós**: `lista_girar` reduz *k* módulo *n* (no máximo
  n − 1 passos) e só desloca o ponteiro `ultimo`. O caso negativo é tratado
  sem calcular `-k`, evitando estouro com `LLONG_MIN`.
- **Remoção pelo nó anterior**: todas as remoções usam `remover_depois(ant)`,
  que atualiza `ultimo` quando necessário e trata o caso de nó único (o
  círculo de um elemento aponta para si mesmo).
- **Destruição segura**: o círculo é quebrado (`ultimo->prox = NULL`) antes de
  liberar, para o laço terminar naturalmente.
- **Josefo reaproveitando a API**: a simulação é só `girar(k − 1)` +
  `remover_inicio`; reduzir `k − 1` módulo o tamanho atual mantém o custo
  O(n · min(k, n)) mesmo para *k* enorme.
- **Fórmula sem estouro**: `(r + k mod i) mod i` nunca passa de `2i`, então
  funciona com *k* próximo de `SIZE_MAX`. A versão recursiva fica como
  referência didática (profundidade O(n)); a iterativa é a recomendada.
- **Tipo opaco**, validação de posições e verificação de todo `malloc`.
- Testes com **20 000 operações aleatórias** (inserções, remoções e rotações)
  comparadas a um vetor-modelo.

## Estrutura de pastas

```
lista-circular-c/
├── .github/workflows/ci.yml       # integração contínua (testes com sanitizers + demo)
├── include/lista_circular.h       # API da lista circular
├── include/josefo.h               # API do problema de Josefo
├── src/lista_circular.c           # implementação da lista
├── src/josefo.c                   # simulação e fórmulas de Josefo
├── src/main.c                     # programa de demonstração
├── tests/test_lista_circular.c    # testes unitários com assert
├── Makefile
├── LICENSE
└── README.md
```

## Como compilar, executar e testar

### Linux / macOS (com `make`)

```bash
make            # compila demo e testes em build/
make run        # executa a demonstração
make test       # executa os testes
make test SANITIZE=1   # testes com AddressSanitizer + UBSan
make clean      # remove build/
```

### Windows (MinGW-w64 / GCC, sem `make`)

```bat
mkdir build
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude src/lista_circular.c src/josefo.c src/main.c -o build/demo.exe
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude src/lista_circular.c src/josefo.c tests/test_lista_circular.c -o build/test_lista_circular.exe
build\demo.exe
build\test_lista_circular.exe
```

(Com o MinGW, `mingw32-make` também funciona no lugar de `make`.)

Saída esperada dos testes:

```
Testes da lista circular e do problema de Josefo:
  [ok] teste_lista_vazia
  [ok] teste_um_elemento
  [ok] teste_inserir
  [ok] teste_remover
  [ok] teste_buscar
  [ok] teste_girar
  [ok] teste_aleatorio_contra_modelo
  [ok] teste_josefo_casos_conhecidos
  [ok] teste_josefo_simulacao_contra_formulas
  [ok] teste_josefo_ordem_e_permutacao
  [ok] teste_josefo_entradas_invalidas
  [ok] teste_ponteiros_nulos
12 testes passaram
```

## Exemplo de saída

Saída real de `make run` (o valor entre parênteses mostra que, depois do
último, a lista volta ao primeiro):

```
== Lista circular (ponteiro so para o ultimo no) ==
inserir_fim 10..50:        [5] 10 -> 20 -> 30 -> 40 -> 50 -> (10)
inserir_inicio 5:          [6] 5 -> 10 -> 20 -> 30 -> 40 -> 50 -> (5)
inserir_posicao(3, 25):    [7] 5 -> 10 -> 20 -> 25 -> 30 -> 40 -> 50 -> (5)
buscar 40: posicao 5
ultimo (O(1)): 50
girar(2):                  [7] 20 -> 25 -> 30 -> 40 -> 50 -> 5 -> 10 -> (20)
girar(-3):                 [7] 50 -> 5 -> 10 -> 20 -> 25 -> 30 -> 40 -> (50)
girar(15):                 [7] 5 -> 10 -> 20 -> 25 -> 30 -> 40 -> 50 -> (5)
remover_inicio: saiu 5
remover_valor(25):         [5] 10 -> 20 -> 30 -> 40 -> 50 -> (10)
remover_posicao(2): saiu 30
lista final:               [4] 10 -> 20 -> 40 -> 50 -> (10)

== Josefo passo a passo: n = 7, k = 3 ==
circulo inicial:           [7] 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> (1)
elimina 3:                 [6] 4 -> 5 -> 6 -> 7 -> 1 -> 2 -> (4)
elimina 6:                 [5] 7 -> 1 -> 2 -> 4 -> 5 -> (7)
elimina 2:                 [4] 4 -> 5 -> 7 -> 1 -> (4)
elimina 7:                 [3] 1 -> 4 -> 5 -> (1)
elimina 5:                 [2] 1 -> 4 -> (1)
elimina 1:                 [1] 4 -> (4)
sobrevivente: 4

== Josefo classico: n = 41, k = 3 ==
ordem de eliminacao:
  3 6 9 12 15 18 21 24 27 30 33 36 39 1
  5 10 14 19 23 28 32 37 41 7 13 20 26 34
  40 8 17 29 38 11 25 2 22 4 35 16 31
simulacao: 31 | formula iterativa: 31 | recursiva: 31

== k = 2: simulacao x forma fechada 2L + 1 ==
  n           1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
  simulacao   1  1  3  1  3  5  7  1  3  5  7  9 11 13 15  1
  2L + 1      1  1  3  1  3  5  7  1  3  5  7  9 11 13 15  1

formula para n = 1000000, k = 7: sobrevivente 206072
```

## Referências

- GRAHAM, R. L.; KNUTH, D. E.; PATASHNIK, O. *Matemática Concreta*. 2. ed. Rio de Janeiro: LTC, 1995. (Seção 1.3 — O problema de Josefo.)
- CORMEN, T. H.; LEISERSON, C. E.; RIVEST, R. L.; STEIN, C. *Algoritmos: Teoria e Prática*. 3. ed. Rio de Janeiro: Elsevier, 2012. (Cap. 10.2 — Listas ligadas; Problema 14-2 — Permutação de Josephus.)
- SEDGEWICK, R. *Algorithms in C*, Parts 1–4. 3. ed. Addison-Wesley, 1998. (Seção 3.3 — Listas circulares e o problema de Josefo.)
- CELES, W.; CERQUEIRA, R.; RANGEL, J. L. *Introdução a Estruturas de Dados: com técnicas de programação em C*. 2. ed. Elsevier, 2016.

## Licença

Distribuído sob a licença MIT. Veja [LICENSE](LICENSE).
