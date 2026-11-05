#include "arvore.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct NoArvore {
    int chave;
    struct NoArvore *esq;
    struct NoArvore *dir;
} NoArvore;

struct Arvore {
    NoArvore *raiz;
    size_t tamanho;
};

Arvore *arvore_criar(void)
{
    Arvore *a = malloc(sizeof *a);
    if (a == NULL) {
        return NULL;
    }
    a->raiz = NULL;
    a->tamanho = 0;
    return a;
}

/*
 * Destruicao iterativa com O(1) de memoria extra: enquanto o no atual tiver
 * filho esquerdo, faz uma rotacao a direita (o filho sobe). Quando nao houver
 * filho esquerdo, libera o no e segue para a direita. Cada rotacao deixa um
 * no a menos fora do "caminho direito", logo o total e O(n).
 */
void arvore_destruir(Arvore *a)
{
    if (a == NULL) {
        return;
    }
    NoArvore *atual = a->raiz;
    while (atual != NULL) {
        if (atual->esq != NULL) {
            NoArvore *e = atual->esq;
            atual->esq = e->dir;
            e->dir = atual;
            atual = e;
        } else {
            NoArvore *d = atual->dir;
            free(atual);
            atual = d;
        }
    }
    free(a);
}

bool arvore_inserir(Arvore *a, int chave)
{
    if (a == NULL) {
        return false;
    }
    /* 'elo' aponta para o ponteiro que recebera o novo no. */
    NoArvore **elo = &a->raiz;
    while (*elo != NULL) {
        if (chave == (*elo)->chave) {
            return false; /* chave repetida */
        }
        elo = chave < (*elo)->chave ? &(*elo)->esq : &(*elo)->dir;
    }

    NoArvore *novo = malloc(sizeof *novo);
    if (novo == NULL) {
        return false;
    }
    novo->chave = chave;
    novo->esq = NULL;
    novo->dir = NULL;
    *elo = novo;
    a->tamanho++;
    return true;
}

bool arvore_buscar(const Arvore *a, int chave)
{
    if (a == NULL) {
        return false;
    }
    const NoArvore *n = a->raiz;
    while (n != NULL) {
        if (chave == n->chave) {
            return true;
        }
        n = chave < n->chave ? n->esq : n->dir;
    }
    return false;
}

bool arvore_remover(Arvore *a, int chave)
{
    if (a == NULL) {
        return false;
    }
    NoArvore **elo = &a->raiz;
    while (*elo != NULL && (*elo)->chave != chave) {
        elo = chave < (*elo)->chave ? &(*elo)->esq : &(*elo)->dir;
    }
    if (*elo == NULL) {
        return false;
    }

    NoArvore *alvo = *elo;
    if (alvo->esq != NULL && alvo->dir != NULL) {
        /* Caso 3: dois filhos. O sucessor (menor chave da subarvore direita)
         * nao tem filho esquerdo; copiamos sua chave e removemos o sucessor. */
        NoArvore **elo_suc = &alvo->dir;
        while ((*elo_suc)->esq != NULL) {
            elo_suc = &(*elo_suc)->esq;
        }
        NoArvore *suc = *elo_suc;
        alvo->chave = suc->chave;
        *elo_suc = suc->dir;
        free(suc);
    } else {
        /* Casos 1 e 2: folha ou um filho -> o filho (ou NULL) sobe. */
        *elo = alvo->esq != NULL ? alvo->esq : alvo->dir;
        free(alvo);
    }
    a->tamanho--;
    return true;
}

bool arvore_minimo(const Arvore *a, int *saida)
{
    if (a == NULL || a->raiz == NULL) {
        return false;
    }
    const NoArvore *n = a->raiz;
    while (n->esq != NULL) {
        n = n->esq;
    }
    if (saida != NULL) {
        *saida = n->chave;
    }
    return true;
}

bool arvore_maximo(const Arvore *a, int *saida)
{
    if (a == NULL || a->raiz == NULL) {
        return false;
    }
    const NoArvore *n = a->raiz;
    while (n->dir != NULL) {
        n = n->dir;
    }
    if (saida != NULL) {
        *saida = n->chave;
    }
    return true;
}

size_t arvore_contar_nos(const Arvore *a)
{
    return a != NULL ? a->tamanho : 0;
}

/*
 * Vetor auxiliar de ponteiros usado como pilha (percursos em profundidade)
 * ou como fila (percurso em largura). Nenhuma das duas estruturas precisa
 * de mais de n posicoes, entao alocamos n de uma vez: nao ha realocacao no
 * meio do percurso e uma falha de memoria acontece antes de visitar qualquer no.
 */
static const NoArvore **alocar_auxiliar(size_t n)
{
    if (n > SIZE_MAX / sizeof(NoArvore *)) {
        return NULL;
    }
    return malloc(n * sizeof(NoArvore *));
}

bool arvore_pre_ordem(const Arvore *a, ArvoreVisitante visitante, void *contexto)
{
    if (a == NULL || visitante == NULL) {
        return false;
    }
    if (a->raiz == NULL) {
        return true;
    }
    const NoArvore **pilha = alocar_auxiliar(a->tamanho);
    if (pilha == NULL) {
        return false;
    }

    size_t topo = 0;
    pilha[topo++] = a->raiz;
    while (topo > 0) {
        const NoArvore *n = pilha[--topo];
        visitante(n->chave, contexto);
        /* Empilha a direita primeiro para visitar a esquerda antes. */
        if (n->dir != NULL) {
            pilha[topo++] = n->dir;
        }
        if (n->esq != NULL) {
            pilha[topo++] = n->esq;
        }
    }
    free(pilha);
    return true;
}

bool arvore_em_ordem(const Arvore *a, ArvoreVisitante visitante, void *contexto)
{
    if (a == NULL || visitante == NULL) {
        return false;
    }
    if (a->raiz == NULL) {
        return true;
    }
    const NoArvore **pilha = alocar_auxiliar(a->tamanho);
    if (pilha == NULL) {
        return false;
    }

    size_t topo = 0;
    const NoArvore *atual = a->raiz;
    while (atual != NULL || topo > 0) {
        while (atual != NULL) { /* desce o maximo para a esquerda */
            pilha[topo++] = atual;
            atual = atual->esq;
        }
        atual = pilha[--topo];
        visitante(atual->chave, contexto);
        atual = atual->dir;
    }
    free(pilha);
    return true;
}

bool arvore_pos_ordem(const Arvore *a, ArvoreVisitante visitante, void *contexto)
{
    if (a == NULL || visitante == NULL) {
        return false;
    }
    if (a->raiz == NULL) {
        return true;
    }
    const NoArvore **pilha = alocar_auxiliar(a->tamanho);
    if (pilha == NULL) {
        return false;
    }

    size_t topo = 0;
    const NoArvore *atual = a->raiz;
    const NoArvore *ultimo = NULL; /* ultimo no visitado */
    while (atual != NULL || topo > 0) {
        if (atual != NULL) {
            pilha[topo++] = atual;
            atual = atual->esq;
        } else {
            const NoArvore *t = pilha[topo - 1];
            if (t->dir != NULL && ultimo != t->dir) {
                atual = t->dir; /* subarvore direita ainda nao visitada */
            } else {
                visitante(t->chave, contexto);
                ultimo = t;
                topo--;
            }
        }
    }
    free(pilha);
    return true;
}

bool arvore_em_largura(const Arvore *a, ArvoreVisitante visitante, void *contexto)
{
    if (a == NULL || visitante == NULL) {
        return false;
    }
    if (a->raiz == NULL) {
        return true;
    }
    /* Fila simples: cada no entra uma unica vez, entao n posicoes bastam
     * e nao e preciso "dar a volta" no vetor. */
    const NoArvore **fila = alocar_auxiliar(a->tamanho);
    if (fila == NULL) {
        return false;
    }

    size_t inicio = 0, fim = 0;
    fila[fim++] = a->raiz;
    while (inicio < fim) {
        const NoArvore *n = fila[inicio++];
        visitante(n->chave, contexto);
        if (n->esq != NULL) {
            fila[fim++] = n->esq;
        }
        if (n->dir != NULL) {
            fila[fim++] = n->dir;
        }
    }
    free(fila);
    return true;
}

/* Altura por niveis: percorre em largura contando quantos niveis existem. */
int arvore_altura(const Arvore *a)
{
    if (a == NULL || a->raiz == NULL) {
        return -1;
    }
    const NoArvore **fila = alocar_auxiliar(a->tamanho);
    if (fila == NULL) {
        return -2;
    }

    size_t inicio = 0, fim = 0;
    int niveis = 0;
    fila[fim++] = a->raiz;
    while (inicio < fim) {
        size_t fim_do_nivel = fim;
        while (inicio < fim_do_nivel) {
            const NoArvore *n = fila[inicio++];
            if (n->esq != NULL) {
                fila[fim++] = n->esq;
            }
            if (n->dir != NULL) {
                fila[fim++] = n->dir;
            }
        }
        niveis++;
    }
    free(fila);
    return niveis - 1;
}
