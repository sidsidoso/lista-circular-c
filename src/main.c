#include <stdio.h>
#include <stdlib.h>

#include "josefo.h"
#include "lista_circular.h"

static void imprimir_valor(int valor, void *contexto)
{
    (void)contexto;
    printf("%d -> ", valor);
}

static void mostrar(const char *rotulo, const ListaCircular *l)
{
    int primeiro = 0;
    printf("%-26s [%zu] ", rotulo, lista_tamanho(l));
    lista_percorrer(l, imprimir_valor, NULL);
    if (lista_obter(l, 0, &primeiro)) {
        printf("(%d)\n", primeiro); /* volta ao primeiro: e um circulo */
    } else {
        puts("(vazia)");
    }
}

static int erro_memoria(ListaCircular *l)
{
    fprintf(stderr, "erro: memoria insuficiente\n");
    lista_destruir(l);
    return EXIT_FAILURE;
}

/* Simula Josefo passo a passo, mostrando o circulo a cada eliminacao. */
static int josefo_passo_a_passo(int n, long long k)
{
    ListaCircular *roda = lista_criar();
    if (roda == NULL) {
        return erro_memoria(NULL);
    }
    for (int i = 1; i <= n; i++) {
        if (!lista_inserir_fim(roda, i)) {
            return erro_memoria(roda);
        }
    }
    printf("\n== Josefo passo a passo: n = %d, k = %lld ==\n", n, k);
    mostrar("circulo inicial:", roda);
    while (lista_tamanho(roda) > 1) {
        int sai;
        char rotulo[40];
        lista_girar(roda, k - 1);
        lista_remover_inicio(roda, &sai);
        snprintf(rotulo, sizeof rotulo, "elimina %d:", sai);
        mostrar(rotulo, roda);
    }
    int sobrevivente = 0;
    lista_obter(roda, 0, &sobrevivente);
    printf("sobrevivente: %d\n", sobrevivente);
    lista_destruir(roda);
    return EXIT_SUCCESS;
}

int main(void)
{
    ListaCircular *l = lista_criar();
    if (l == NULL) {
        return erro_memoria(NULL);
    }

    puts("== Lista circular (ponteiro so para o ultimo no) ==");
    bool ok = true;
    for (int v = 10; v <= 50; v += 10) {
        ok = ok && lista_inserir_fim(l, v);
    }
    mostrar("inserir_fim 10..50:", l);
    ok = ok && lista_inserir_inicio(l, 5);
    mostrar("inserir_inicio 5:", l);
    ok = ok && lista_inserir_posicao(l, 3, 25);
    mostrar("inserir_posicao(3, 25):", l);
    if (!ok) {
        return erro_memoria(l);
    }

    size_t pos;
    if (lista_buscar(l, 40, &pos)) {
        printf("buscar 40: posicao %zu\n", pos);
    }
    int ultimo;
    if (lista_obter(l, lista_tamanho(l) - 1, &ultimo)) {
        printf("ultimo (O(1)): %d\n", ultimo);
    }

    lista_girar(l, 2);
    mostrar("girar(2):", l);
    lista_girar(l, -3);
    mostrar("girar(-3):", l);
    lista_girar(l, 15); /* 15 mod 7 = 1 */
    mostrar("girar(15):", l);

    int v;
    lista_remover_inicio(l, &v);
    printf("remover_inicio: saiu %d\n", v);
    lista_remover_valor(l, 25);
    mostrar("remover_valor(25):", l);
    lista_remover_posicao(l, 2, &v);
    printf("remover_posicao(2): saiu %d\n", v);
    mostrar("lista final:", l);
    lista_destruir(l);

    if (josefo_passo_a_passo(7, 3) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    size_t ordem[41];
    size_t sobrevivente;
    if (!josefo_simular(41, 3, ordem, &sobrevivente)) {
        return erro_memoria(NULL);
    }
    puts("\n== Josefo classico: n = 41, k = 3 ==");
    printf("ordem de eliminacao:");
    for (size_t i = 0; i < 41; i++) {
        printf("%s%zu", i % 14 == 0 ? "\n  " : " ", ordem[i]);
    }
    printf("\nsimulacao: %zu | formula iterativa: %zu | recursiva: %zu\n",
           sobrevivente, josefo_formula(41, 3), josefo_recursivo(41, 3));

    puts("\n== k = 2: simulacao x forma fechada 2L + 1 ==");
    printf("  %-10s", "n");
    for (size_t n = 1; n <= 16; n++) {
        printf("%3zu", n);
    }
    printf("\n  %-10s", "simulacao");
    for (size_t n = 1; n <= 16; n++) {
        size_t s = 0;
        if (!josefo_simular(n, 2, NULL, &s)) {
            return erro_memoria(NULL);
        }
        printf("%3zu", s);
    }
    printf("\n  %-10s", "2L + 1");
    for (size_t n = 1; n <= 16; n++) {
        printf("%3zu", josefo_k2(n));
    }
    putchar('\n');

    printf("\nformula para n = 1000000, k = 7: sobrevivente %zu\n",
           josefo_formula(1000000, 7));
    return EXIT_SUCCESS;
}
