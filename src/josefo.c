#include "josefo.h"

#include <limits.h>

#include "lista_circular.h"

bool josefo_simular(size_t n, size_t k, size_t *ordem, size_t *sobrevivente)
{
    if (n == 0 || k == 0 || n > (size_t)INT_MAX) {
        return false;
    }
    ListaCircular *roda = lista_criar();
    if (roda == NULL) {
        return false;
    }
    for (size_t i = 1; i <= n; i++) {
        if (!lista_inserir_fim(roda, (int)i)) {
            lista_destruir(roda);
            return false;
        }
    }

    /* A pessoa da vez e sempre a primeira da lista. Girar k-1 posicoes poe a
     * k-esima pessoa na frente; remover_inicio a elimina e a seguinte vira a
     * primeira, exatamente onde a contagem recomeca. */
    size_t passo_k = k - 1;
    size_t i = 0;
    while (lista_tamanho(roda) > 1) {
        size_t m = lista_tamanho(roda);
        lista_girar(roda, (long long)(passo_k % m));
        int eliminado;
        lista_remover_inicio(roda, &eliminado);
        if (ordem != NULL) {
            ordem[i] = (size_t)eliminado;
        }
        i++;
    }
    int ultimo = 0;
    lista_remover_inicio(roda, &ultimo);
    if (ordem != NULL) {
        ordem[i] = (size_t)ultimo;
    }
    if (sobrevivente != NULL) {
        *sobrevivente = (size_t)ultimo;
    }
    lista_destruir(roda);
    return true;
}

size_t josefo_formula(size_t n, size_t k)
{
    if (n == 0 || k == 0) {
        return 0;
    }
    /* r = posicao (base 0) do sobrevivente em um circulo de i pessoas. */
    size_t r = 0;
    for (size_t i = 2; i <= n; i++) {
        r = (r + k % i) % i; /* k % i antes evita estouro com k enorme */
    }
    return r + 1;
}

static size_t josefo_rec_base0(size_t n, size_t k)
{
    if (n == 1) {
        return 0;
    }
    return (josefo_rec_base0(n - 1, k) + k % n) % n;
}

size_t josefo_recursivo(size_t n, size_t k)
{
    if (n == 0 || k == 0) {
        return 0;
    }
    return josefo_rec_base0(n, k) + 1;
}

size_t josefo_k2(size_t n)
{
    if (n == 0) {
        return 0;
    }
    /* Maior potencia de 2 que nao passa de n. */
    size_t potencia = 1;
    while (potencia <= n / 2) {
        potencia *= 2;
    }
    return 2 * (n - potencia) + 1;
}
