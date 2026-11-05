#ifndef ARVORE_H
#define ARVORE_H

/*
 * Arvore Binaria de Busca (ABB) de inteiros, sem chaves repetidas.
 *
 * Invariante: para todo no x, as chaves da subarvore esquerda sao menores
 * que x e as da subarvore direita sao maiores que x.
 *
 * Nas complexidades, h e a altura da arvore: O(log n) se ela estiver
 * balanceada e O(n) no pior caso (insercao de chaves ja ordenadas).
 * Nenhuma operacao usa recursao, entao arvores degeneradas nao estouram a
 * pilha de chamadas.
 */

#include <stdbool.h>
#include <stddef.h>

/* Tipo opaco: os nos ficam escondidos em src/arvore.c. */
typedef struct Arvore Arvore;

/* Funcao chamada para cada chave nos percursos. */
typedef void (*ArvoreVisitante)(int chave, void *contexto);

/* Cria uma arvore vazia. Retorna NULL se faltar memoria. O(1) */
Arvore *arvore_criar(void);

/* Libera todos os nos e a arvore. Aceita NULL. O(n) tempo, O(1) espaco */
void arvore_destruir(Arvore *a);

/* Insere 'chave' (iterativo). Retorna false se a chave ja existir ou se
 * faltar memoria. O(h) */
bool arvore_inserir(Arvore *a, int chave);

/* true se 'chave' estiver na arvore (iterativo). O(h) */
bool arvore_buscar(const Arvore *a, int chave);

/* Remove 'chave'. Trata os 3 casos (folha, um filho, dois filhos -> sucessor).
 * Retorna false se a chave nao existir. O(h) */
bool arvore_remover(Arvore *a, int chave);

/* Menor / maior chave em *saida. Retornam false se a arvore estiver vazia. O(h) */
bool arvore_minimo(const Arvore *a, int *saida);
bool arvore_maximo(const Arvore *a, int *saida);

/* Altura em arestas: arvore vazia = -1, apenas a raiz = 0.
 * Calculada por niveis (fila). Retorna -2 se faltar memoria. O(n) */
int arvore_altura(const Arvore *a);

/* Numero de nos (mantido a cada insercao/remocao). O(1) */
size_t arvore_contar_nos(const Arvore *a);

/* Percursos em profundidade (pilha explicita) e em largura (fila).
 * Chamam visitante(chave, contexto) para cada no. Retornam false se a ou
 * visitante forem NULL ou se faltar memoria para o vetor auxiliar (nesse
 * caso nenhum no e visitado). O(n) tempo, O(n) espaco auxiliar. */
bool arvore_pre_ordem(const Arvore *a, ArvoreVisitante visitante, void *contexto);
bool arvore_em_ordem(const Arvore *a, ArvoreVisitante visitante, void *contexto);
bool arvore_pos_ordem(const Arvore *a, ArvoreVisitante visitante, void *contexto);
bool arvore_em_largura(const Arvore *a, ArvoreVisitante visitante, void *contexto);

#endif /* ARVORE_H */
