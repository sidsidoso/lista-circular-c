#include "lista_circular.h"

#include <stdlib.h>

typedef struct No {
    int valor;
    struct No *prox;
} No;

/* Apenas o ultimo no e guardado; o primeiro e sempre ultimo->prox. */
struct ListaCircular {
    No *ultimo;
    size_t tamanho;
};

ListaCircular *lista_criar(void)
{
    ListaCircular *l = malloc(sizeof *l);
    if (l != NULL) {
        l->ultimo = NULL;
        l->tamanho = 0;
    }
    return l;
}

void lista_destruir(ListaCircular *l)
{
    if (l == NULL) {
        return;
    }
    if (l->ultimo != NULL) {
        No *atual = l->ultimo->prox; /* primeiro */
        l->ultimo->prox = NULL;      /* quebra o circulo: o laco para no fim */
        while (atual != NULL) {
            No *prox = atual->prox;
            free(atual);
            atual = prox;
        }
    }
    free(l);
}

/* Liga 'novo' logo depois de 'antes'. Com a lista vazia, o no forma sozinho
 * um circulo de tamanho 1. Nao altera l->ultimo. */
static void ligar_depois(ListaCircular *l, No *antes, No *novo)
{
    if (antes == NULL) {
        novo->prox = novo;
        l->ultimo = novo;
    } else {
        novo->prox = antes->prox;
        antes->prox = novo;
    }
    l->tamanho++;
}

static No *novo_no(int valor)
{
    No *no = malloc(sizeof *no);
    if (no != NULL) {
        no->valor = valor;
    }
    return no;
}

bool lista_inserir_inicio(ListaCircular *l, int valor)
{
    if (l == NULL) {
        return false;
    }
    No *no = novo_no(valor);
    if (no == NULL) {
        return false;
    }
    /* Inserir depois do ultimo = virar o novo primeiro. */
    ligar_depois(l, l->ultimo, no);
    return true;
}

bool lista_inserir_fim(ListaCircular *l, int valor)
{
    if (!lista_inserir_inicio(l, valor)) {
        return false;
    }
    /* O truque da lista circular: o novo primeiro vira o ultimo apenas
     * avancando o ponteiro 'ultimo' uma posicao. */
    l->ultimo = l->ultimo->prox;
    return true;
}

/* No anterior a posicao 'pos' (0..tamanho). Para pos == 0 e o ultimo. */
static No *anterior_a(const ListaCircular *l, size_t pos)
{
    No *ant = l->ultimo;
    while (pos-- > 0) {
        ant = ant->prox;
    }
    return ant;
}

bool lista_inserir_posicao(ListaCircular *l, size_t pos, int valor)
{
    if (l == NULL || pos > l->tamanho) {
        return false;
    }
    if (pos == l->tamanho) {
        return lista_inserir_fim(l, valor); /* O(1) */
    }
    No *no = novo_no(valor);
    if (no == NULL) {
        return false;
    }
    ligar_depois(l, anterior_a(l, pos), no);
    return true;
}

/* Remove o no seguinte a 'ant' e devolve seu valor. */
static int remover_depois(ListaCircular *l, No *ant)
{
    No *alvo = ant->prox;
    int valor = alvo->valor;
    if (alvo == ant) {            /* era o unico no */
        l->ultimo = NULL;
    } else {
        ant->prox = alvo->prox;
        if (alvo == l->ultimo) {
            l->ultimo = ant;
        }
    }
    free(alvo);
    l->tamanho--;
    return valor;
}

bool lista_remover_inicio(ListaCircular *l, int *saida)
{
    return lista_remover_posicao(l, 0, saida);
}

bool lista_remover_posicao(ListaCircular *l, size_t pos, int *saida)
{
    if (l == NULL || pos >= l->tamanho) {
        return false;
    }
    int v = remover_depois(l, anterior_a(l, pos));
    if (saida != NULL) {
        *saida = v;
    }
    return true;
}

bool lista_remover_valor(ListaCircular *l, int valor)
{
    if (l == NULL || l->ultimo == NULL) {
        return false;
    }
    No *ant = l->ultimo;
    for (size_t i = 0; i < l->tamanho; i++, ant = ant->prox) {
        if (ant->prox->valor == valor) {
            remover_depois(l, ant);
            return true;
        }
    }
    return false;
}

bool lista_buscar(const ListaCircular *l, int valor, size_t *posicao)
{
    if (l == NULL || l->ultimo == NULL) {
        return false;
    }
    const No *atual = l->ultimo->prox;
    for (size_t i = 0; i < l->tamanho; i++, atual = atual->prox) {
        if (atual->valor == valor) {
            if (posicao != NULL) {
                *posicao = i;
            }
            return true;
        }
    }
    return false;
}

bool lista_obter(const ListaCircular *l, size_t pos, int *saida)
{
    if (l == NULL || pos >= l->tamanho) {
        return false;
    }
    if (saida != NULL) {
        const No *no = pos == l->tamanho - 1 ? l->ultimo : anterior_a(l, pos)->prox;
        *saida = no->valor;
    }
    return true;
}

void lista_girar(ListaCircular *l, long long k)
{
    if (l == NULL || l->tamanho < 2) {
        return;
    }
    size_t n = l->tamanho;
    size_t passos;
    if (k >= 0) {
        passos = (size_t)((unsigned long long)k % n);
    } else {
        /* -k pode estourar para LLONG_MIN: calcula o modulo sem negar. */
        unsigned long long magnitude = (unsigned long long)(-(k + 1)) + 1u;
        size_t r = (size_t)(magnitude % n);
        passos = r == 0 ? 0 : n - r;
    }
    /* Girar 'passos' para a esquerda = avancar o ponteiro 'ultimo'. */
    No *ultimo = l->ultimo;
    while (passos-- > 0) {
        ultimo = ultimo->prox;
    }
    l->ultimo = ultimo;
}

size_t lista_tamanho(const ListaCircular *l)
{
    return l == NULL ? 0 : l->tamanho;
}

bool lista_vazia(const ListaCircular *l)
{
    return lista_tamanho(l) == 0;
}

void lista_percorrer(const ListaCircular *l, ListaVisitante visitante, void *contexto)
{
    if (l == NULL || visitante == NULL || l->ultimo == NULL) {
        return;
    }
    const No *atual = l->ultimo;
    do {
        atual = atual->prox;
        visitante(atual->valor, contexto);
    } while (atual != l->ultimo);
}
