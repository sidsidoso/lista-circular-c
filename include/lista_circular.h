#ifndef LISTA_CIRCULAR_H
#define LISTA_CIRCULAR_H

/*
 * Lista circular simplesmente encadeada de inteiros.
 *
 * A lista guarda APENAS um ponteiro para o ultimo no. Como o ultimo aponta de
 * volta para o primeiro (ultimo->prox == primeiro), as duas pontas ficam a um
 * passo de distancia: inserir no inicio e no fim custam O(1).
 * Posicoes sao indices base 0, contados a partir do primeiro elemento.
 */

#include <stdbool.h>
#include <stddef.h>

/* Tipo opaco: os nos e a lista ficam escondidos em src/lista_circular.c. */
typedef struct ListaCircular ListaCircular;

/* Funcao chamada para cada elemento em lista_percorrer. */
typedef void (*ListaVisitante)(int valor, void *contexto);

/* Cria uma lista vazia. Retorna NULL se faltar memoria. O(1) */
ListaCircular *lista_criar(void);

/* Libera todos os nos e a propria lista. Aceita NULL. O(n) */
void lista_destruir(ListaCircular *l);

/* Insere antes do primeiro elemento. Retorna false se faltar memoria. O(1) */
bool lista_inserir_inicio(ListaCircular *l, int valor);

/* Insere depois do ultimo elemento. Retorna false se faltar memoria. O(1) */
bool lista_inserir_fim(ListaCircular *l, int valor);

/* Insere de modo que o novo elemento ocupe a posicao 'pos' (0..tamanho).
 * Retorna false se pos > tamanho ou faltar memoria. O(pos) */
bool lista_inserir_posicao(ListaCircular *l, size_t pos, int valor);

/* Remove o primeiro elemento e grava em *saida (se saida != NULL).
 * Retorna false se a lista estiver vazia. O(1) */
bool lista_remover_inicio(ListaCircular *l, int *saida);

/* Remove o elemento da posicao 'pos' (0..tamanho-1).
 * Retorna false se a posicao for invalida. O(pos) */
bool lista_remover_posicao(ListaCircular *l, size_t pos, int *saida);

/* Remove a primeira ocorrencia de 'valor'. Retorna false se nao existir. O(n) */
bool lista_remover_valor(ListaCircular *l, int valor);

/* Procura a primeira ocorrencia de 'valor'; grava a posicao em *posicao
 * (se posicao != NULL). Retorna false se nao encontrar. O(n) */
bool lista_buscar(const ListaCircular *l, int valor, size_t *posicao);

/* Le o elemento da posicao 'pos'. Retorna false se a posicao for invalida.
 * O(pos); o primeiro e o ultimo elemento saem em O(1). */
bool lista_obter(const ListaCircular *l, size_t pos, int *saida);

/* Gira a lista k posicoes para a esquerda: o elemento da posicao (k mod n)
 * passa a ser o primeiro. k negativo gira para a direita.
 * So move o ponteiro 'ultimo' (nenhum no e realocado). O(min(k mod n, n)) */
void lista_girar(ListaCircular *l, long long k);

/* Numero de elementos. O(1) */
size_t lista_tamanho(const ListaCircular *l);

/* true se a lista nao tiver elementos. O(1) */
bool lista_vazia(const ListaCircular *l);

/* Chama visitante(valor, contexto) uma vez para cada elemento, do primeiro
 * ao ultimo (da uma unica volta no circulo). O(n) */
void lista_percorrer(const ListaCircular *l, ListaVisitante visitante, void *contexto);

#endif /* LISTA_CIRCULAR_H */
