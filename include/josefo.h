#ifndef JOSEFO_H
#define JOSEFO_H

/*
 * Problema de Josefo (Flavio Josefo): n pessoas, numeradas de 1 a n, estao
 * em circulo. Comecando a contar pela pessoa 1, cada k-esima pessoa e
 * eliminada e a contagem recomeca na seguinte. Quem sobra?
 *
 * Tres solucoes:
 *  - josefo_simular:   simula o circulo com a lista circular (girar + remover);
 *  - josefo_formula:   recorrencia J(1) = 0, J(i) = (J(i-1) + k) mod i, iterativa;
 *  - josefo_recursivo: a mesma recorrencia escrita de forma recursiva.
 * Para k = 2 ha ainda a forma fechada josefo_k2.
 */

#include <stdbool.h>
#include <stddef.h>

/* Simula a eliminacao em uma ListaCircular.
 * Se 'ordem' != NULL, deve ter espaco para n posicoes e recebe a ordem de
 * eliminacao (a ultima posicao e o sobrevivente). Se 'sobrevivente' != NULL,
 * recebe o numero (1..n) de quem sobra.
 * Retorna false se n == 0, k == 0, n > INT_MAX ou faltar memoria.
 * Tempo O(n * min(k, n)), espaco O(n). */
bool josefo_simular(size_t n, size_t k, size_t *ordem, size_t *sobrevivente);

/* Sobrevivente (1..n) pela recorrencia, iterativa. Retorna 0 se n == 0 ou
 * k == 0. Tempo O(n), espaco O(1). */
size_t josefo_formula(size_t n, size_t k);

/* Mesma recorrencia, recursiva (didatica: usa O(n) de pilha, entao serve
 * apenas para n pequeno). Retorna 0 se n == 0 ou k == 0. */
size_t josefo_recursivo(size_t n, size_t k);

/* Forma fechada para k = 2: escrevendo n = 2^m + L (0 <= L < 2^m),
 * o sobrevivente e 2L + 1. Retorna 0 se n == 0. O(log n) */
size_t josefo_k2(size_t n);

#endif /* JOSEFO_H */
