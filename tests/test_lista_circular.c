#undef NDEBUG
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "josefo.h"
#include "lista_circular.h"

static int testes_passaram = 0;

#define RODAR(teste)                          \
    do {                                      \
        teste();                              \
        testes_passaram++;                    \
        printf("  [ok] %s\n", #teste);        \
    } while (0)

#define TAM(v) (sizeof(v) / sizeof((v)[0]))

static uint32_t semente = 88172645u;
static uint32_t aleatorio(void)
{
    semente ^= semente << 13;
    semente ^= semente >> 17;
    semente ^= semente << 5;
    return semente;
}

typedef struct {
    const int *esperado;
    size_t n;
    size_t i;
} Comparacao;

static void comparar(int valor, void *contexto)
{
    Comparacao *c = contexto;
    assert(c->i < c->n);
    assert(valor == c->esperado[c->i]);
    c->i++;
}

/* Confere conteudo (uma volta exata), tamanho, primeiro e ultimo, e que o
 * ponteiro 'ultimo' esta correto (inserir no fim deve ir para o fim). */
static void conferir(ListaCircular *l, const int *esperado, size_t n)
{
    Comparacao c = {esperado, n, 0};
    lista_percorrer(l, comparar, &c);
    assert(c.i == n);
    assert(lista_tamanho(l) == n);
    assert(lista_vazia(l) == (n == 0));
    for (size_t i = 0; i < n; i++) {
        int v;
        assert(lista_obter(l, i, &v) && v == esperado[i]);
    }
    int v;
    assert(!lista_obter(l, n, &v));
    assert(lista_inserir_fim(l, 999));
    assert(lista_obter(l, n, &v) && v == 999);
    assert(lista_remover_posicao(l, n, &v) && v == 999);
    assert(lista_tamanho(l) == n);
}

static ListaCircular *lista_de(const int *v, size_t n)
{
    ListaCircular *l = lista_criar();
    assert(l != NULL);
    for (size_t i = 0; i < n; i++) {
        assert(lista_inserir_fim(l, v[i]));
    }
    return l;
}

static void teste_lista_vazia(void)
{
    ListaCircular *l = lista_criar();
    assert(l != NULL);
    int v = 3;
    assert(!lista_remover_inicio(l, &v));
    assert(!lista_remover_posicao(l, 0, &v));
    assert(!lista_remover_valor(l, 1));
    assert(!lista_buscar(l, 1, NULL));
    assert(v == 3);
    lista_girar(l, 5);
    conferir(l, NULL, 0);
    lista_destruir(l);
}

static void teste_um_elemento(void)
{
    ListaCircular *l = lista_criar();
    assert(l != NULL);
    assert(lista_inserir_inicio(l, 1));
    const int e[] = {1};
    conferir(l, e, 1);
    lista_girar(l, 3);
    lista_girar(l, -3);
    conferir(l, e, 1);
    int v;
    assert(lista_remover_inicio(l, &v) && v == 1);
    conferir(l, NULL, 0);
    assert(lista_inserir_fim(l, 2));
    assert(lista_remover_valor(l, 2));
    conferir(l, NULL, 0);
    lista_destruir(l);
}

static void teste_inserir(void)
{
    ListaCircular *l = lista_criar();
    assert(l != NULL);
    assert(lista_inserir_fim(l, 20));
    assert(lista_inserir_inicio(l, 10));
    assert(lista_inserir_fim(l, 40));
    assert(lista_inserir_posicao(l, 2, 30));
    assert(lista_inserir_posicao(l, 0, 0));
    assert(lista_inserir_posicao(l, 5, 50));
    assert(!lista_inserir_posicao(l, 7, 99));
    const int e[] = {0, 10, 20, 30, 40, 50};
    conferir(l, e, TAM(e));
    lista_destruir(l);
}

static void teste_remover(void)
{
    const int base[] = {1, 2, 3, 4, 5, 6};
    ListaCircular *l = lista_de(base, TAM(base));
    int v;
    assert(lista_remover_inicio(l, &v) && v == 1);
    assert(lista_remover_posicao(l, 4, &v) && v == 6); /* ultimo: atualiza ponteiro */
    assert(lista_remover_posicao(l, 1, &v) && v == 3);
    assert(!lista_remover_posicao(l, 3, &v));
    const int e1[] = {2, 4, 5};
    conferir(l, e1, TAM(e1));
    assert(lista_remover_valor(l, 5)); /* o ultimo, por valor */
    assert(!lista_remover_valor(l, 5));
    const int e2[] = {2, 4};
    conferir(l, e2, TAM(e2));
    lista_destruir(l);
}

static void teste_buscar(void)
{
    const int base[] = {7, 3, 7, 9};
    ListaCircular *l = lista_de(base, TAM(base));
    size_t pos = 99;
    assert(lista_buscar(l, 7, &pos) && pos == 0);
    assert(lista_buscar(l, 9, &pos) && pos == 3);
    assert(!lista_buscar(l, 4, &pos) && pos == 3);
    assert(lista_remover_valor(l, 7));
    const int e[] = {3, 7, 9};
    conferir(l, e, TAM(e));
    lista_destruir(l);
}

static void teste_girar(void)
{
    const int base[] = {1, 2, 3, 4, 5};
    ListaCircular *l = lista_de(base, TAM(base));
    lista_girar(l, 2);
    const int e1[] = {3, 4, 5, 1, 2};
    conferir(l, e1, TAM(e1));
    lista_girar(l, -2);
    conferir(l, base, TAM(base));
    lista_girar(l, 5); /* volta completa */
    conferir(l, base, TAM(base));
    lista_girar(l, 13); /* 13 mod 5 = 3 */
    const int e2[] = {4, 5, 1, 2, 3};
    conferir(l, e2, TAM(e2));
    lista_girar(l, -6); /* 6 para a direita = 1 para a direita */
    const int e3[] = {3, 4, 5, 1, 2};
    conferir(l, e3, TAM(e3));
    lista_girar(l, LLONG_MIN); /* nao pode estourar */
    lista_girar(l, LLONG_MAX);
    /* LLONG_MIN + LLONG_MAX = -1: no total, um passo para a direita. */
    const int e4[] = {2, 3, 4, 5, 1};
    conferir(l, e4, TAM(e4));
    lista_destruir(l);
}

static void teste_aleatorio_contra_modelo(void)
{
    enum { MAX = 400 };
    static int modelo[MAX];
    size_t n = 0;
    ListaCircular *l = lista_criar();
    assert(l != NULL);

    for (int op = 0; op < 20000; op++) {
        uint32_t r = aleatorio() % 7;
        int valor = (int)(aleatorio() % 40);
        int v;
        if (r <= 1 && n < MAX) {
            size_t pos = aleatorio() % (n + 1);
            assert(lista_inserir_posicao(l, pos, valor));
            memmove(&modelo[pos + 1], &modelo[pos], (n - pos) * sizeof modelo[0]);
            modelo[pos] = valor;
            n++;
        } else if (r == 2 && n < MAX) {
            assert(lista_inserir_inicio(l, valor));
            memmove(&modelo[1], &modelo[0], n * sizeof modelo[0]);
            modelo[0] = valor;
            n++;
        } else if (r == 3 && n > 0) {
            size_t pos = aleatorio() % n;
            assert(lista_remover_posicao(l, pos, &v) && v == modelo[pos]);
            memmove(&modelo[pos], &modelo[pos + 1], (n - pos - 1) * sizeof modelo[0]);
            n--;
        } else if (r == 4) {
            size_t i = 0;
            while (i < n && modelo[i] != valor) i++;
            assert(lista_remover_valor(l, valor) == (i < n));
            if (i < n) {
                memmove(&modelo[i], &modelo[i + 1], (n - i - 1) * sizeof modelo[0]);
                n--;
            }
        } else if (r == 5 && n > 0) {
            long long k = (long long)(aleatorio() % 2000) - 1000;
            lista_girar(l, k);
            size_t passos = (size_t)(((k % (long long)n) + (long long)n) % (long long)n);
            static int tmp[MAX];
            for (size_t i = 0; i < n; i++) {
                tmp[i] = modelo[(i + passos) % n];
            }
            memcpy(modelo, tmp, n * sizeof modelo[0]);
        } else if (n < MAX) {
            assert(lista_inserir_fim(l, valor));
            modelo[n++] = valor;
        }
        if (op % 101 == 0) {
            conferir(l, modelo, n);
        }
    }
    conferir(l, modelo, n);
    lista_destruir(l);
}

static void teste_josefo_casos_conhecidos(void)
{
    size_t ordem[7];
    size_t s = 0;
    assert(josefo_simular(7, 3, ordem, &s));
    const size_t esperado[] = {3, 6, 2, 7, 5, 1, 4};
    for (size_t i = 0; i < 7; i++) {
        assert(ordem[i] == esperado[i]);
    }
    assert(s == 4);
    assert(josefo_simular(41, 3, NULL, &s) && s == 31); /* caso historico */
    assert(josefo_formula(41, 3) == 31);
    assert(josefo_simular(1, 5, NULL, &s) && s == 1);
    assert(josefo_simular(5, 1, NULL, &s) && s == 5);  /* k = 1: sobra o ultimo */
    assert(josefo_simular(10, 1000003, NULL, &s));     /* k muito maior que n */
    assert(s == josefo_formula(10, 1000003));
}

static void teste_josefo_simulacao_contra_formulas(void)
{
    for (size_t n = 1; n <= 120; n++) {
        for (size_t k = 1; k <= 25; k++) {
            size_t s = 0;
            assert(josefo_simular(n, k, NULL, &s));
            assert(s == josefo_formula(n, k));
            assert(s == josefo_recursivo(n, k));
        }
        size_t s2 = 0;
        assert(josefo_simular(n, 2, NULL, &s2));
        assert(s2 == josefo_k2(n));
    }
}

/* A ordem de eliminacao deve ser uma permutacao de 1..n. */
static void teste_josefo_ordem_e_permutacao(void)
{
    enum { N = 500 };
    static size_t ordem[N];
    static bool visto[N + 1];
    assert(josefo_simular(N, 17, ordem, NULL));
    for (size_t i = 0; i < N; i++) {
        assert(ordem[i] >= 1 && ordem[i] <= N && !visto[ordem[i]]);
        visto[ordem[i]] = true;
    }
    assert(ordem[N - 1] == josefo_formula(N, 17));
}

static void teste_josefo_entradas_invalidas(void)
{
    size_t s = 42;
    assert(!josefo_simular(0, 3, NULL, &s));
    assert(!josefo_simular(5, 0, NULL, &s));
    assert(s == 42);
    assert(josefo_formula(0, 3) == 0);
    assert(josefo_formula(3, 0) == 0);
    assert(josefo_recursivo(0, 3) == 0);
    assert(josefo_k2(0) == 0);
    /* A formula funciona com n grande, onde a simulacao seria cara. */
    assert(josefo_formula(1000000, 2) == josefo_k2(1000000));
}

static void teste_ponteiros_nulos(void)
{
    int v;
    assert(!lista_inserir_inicio(NULL, 1));
    assert(!lista_inserir_fim(NULL, 1));
    assert(!lista_inserir_posicao(NULL, 0, 1));
    assert(!lista_remover_inicio(NULL, &v));
    assert(!lista_remover_posicao(NULL, 0, &v));
    assert(!lista_remover_valor(NULL, 1));
    assert(!lista_buscar(NULL, 1, NULL));
    assert(!lista_obter(NULL, 0, &v));
    assert(lista_tamanho(NULL) == 0);
    assert(lista_vazia(NULL));
    lista_girar(NULL, 1);
    lista_percorrer(NULL, NULL, NULL);
    lista_destruir(NULL);
}

int main(void)
{
    puts("Testes da lista circular e do problema de Josefo:");
    RODAR(teste_lista_vazia);
    RODAR(teste_um_elemento);
    RODAR(teste_inserir);
    RODAR(teste_remover);
    RODAR(teste_buscar);
    RODAR(teste_girar);
    RODAR(teste_aleatorio_contra_modelo);
    RODAR(teste_josefo_casos_conhecidos);
    RODAR(teste_josefo_simulacao_contra_formulas);
    RODAR(teste_josefo_ordem_e_permutacao);
    RODAR(teste_josefo_entradas_invalidas);
    RODAR(teste_ponteiros_nulos);
    printf("%d testes passaram\n", testes_passaram);
    return 0;
}
