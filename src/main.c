#include <stdio.h>
#include <stdlib.h>

#include "arvore.h"

static void imprimir_chave(int chave, void *contexto)
{
    (void)contexto;
    printf(" %d", chave);
}

static void mostrar_percursos(const Arvore *a)
{
    printf("  pre-ordem: ");
    arvore_pre_ordem(a, imprimir_chave, NULL);
    printf("\n  em-ordem:  ");
    arvore_em_ordem(a, imprimir_chave, NULL);
    printf("\n  pos-ordem: ");
    arvore_pos_ordem(a, imprimir_chave, NULL);
    printf("\n  em largura:");
    arvore_em_largura(a, imprimir_chave, NULL);
    putchar('\n');
}

static void mostrar_resumo(const Arvore *a)
{
    int min = 0, max = 0;
    arvore_minimo(a, &min);
    arvore_maximo(a, &max);
    printf("  nos=%zu altura=%d minimo=%d maximo=%d\n",
           arvore_contar_nos(a), arvore_altura(a), min, max);
}

static void remover(Arvore *a, int chave, const char *caso)
{
    printf("\nRemovendo %d (%s): %s\n", chave, caso,
           arvore_remover(a, chave) ? "ok" : "nao encontrado");
    printf("  em-ordem:  ");
    arvore_em_ordem(a, imprimir_chave, NULL);
    printf("\n  em largura:");
    arvore_em_largura(a, imprimir_chave, NULL);
    putchar('\n');
}

int main(void)
{
    Arvore *a = arvore_criar();
    if (a == NULL) {
        fprintf(stderr, "erro: memoria insuficiente\n");
        return EXIT_FAILURE;
    }

    const int chaves[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 65};
    const size_t n = sizeof chaves / sizeof chaves[0];

    puts("== Arvore Binaria de Busca ==");
    printf("Inserindo:");
    for (size_t i = 0; i < n; i++) {
        if (!arvore_inserir(a, chaves[i])) {
            fprintf(stderr, "\nerro ao inserir %d\n", chaves[i]);
            arvore_destruir(a);
            return EXIT_FAILURE;
        }
        printf(" %d", chaves[i]);
    }
    printf("\nInserir 40 de novo: %s\n\n",
           arvore_inserir(a, 40) ? "inserido" : "rejeitado (chave repetida)");

    mostrar_percursos(a);
    mostrar_resumo(a);

    printf("\nBuscar 45: %s\n", arvore_buscar(a, 45) ? "encontrado" : "nao encontrado");
    printf("Buscar 99: %s\n", arvore_buscar(a, 99) ? "encontrado" : "nao encontrado");

    remover(a, 30, "dois filhos -> sucessor 35");
    remover(a, 20, "folha");
    remover(a, 60, "um filho");
    remover(a, 50, "raiz com dois filhos -> sucessor 65");
    remover(a, 99, "inexistente");

    putchar('\n');
    mostrar_resumo(a);

    arvore_destruir(a);
    return EXIT_SUCCESS;
}
