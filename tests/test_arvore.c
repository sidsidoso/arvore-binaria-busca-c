#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "arvore.h"

static int testes_passaram = 0;

#define RODAR(teste)                          \
    do {                                      \
        teste();                              \
        testes_passaram++;                    \
        printf("  [ok] %s\n", #teste);        \
    } while (0)

#define TAM(v) (sizeof(v) / sizeof((v)[0]))
#define MAX_COLETA 64

/* Coleta as chaves visitadas em um vetor, para comparacao. */
typedef struct {
    int v[MAX_COLETA];
    size_t n;
} Coleta;

static void coletar(int chave, void *contexto)
{
    Coleta *c = contexto;
    assert(c->n < MAX_COLETA);
    c->v[c->n++] = chave;
}

typedef bool (*Percurso)(const Arvore *, ArvoreVisitante, void *);

static void conferir_percurso(const Arvore *a, Percurso p, const int *esperado, size_t n)
{
    Coleta c = {.n = 0};
    assert(p(a, coletar, &c));
    assert(c.n == n);
    for (size_t i = 0; i < n; i++) {
        assert(c.v[i] == esperado[i]);
    }
}

/*
 *              50
 *           /      \
 *         30        70
 *        /  \      /  \
 *      20    40  60    80
 *           /  \   \
 *         35   45   65
 */
static Arvore *arvore_exemplo(void)
{
    static const int chaves[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 65};
    Arvore *a = arvore_criar();
    assert(a != NULL);
    for (size_t i = 0; i < TAM(chaves); i++) {
        assert(arvore_inserir(a, chaves[i]));
    }
    return a;
}

static void teste_arvore_vazia(void)
{
    Arvore *a = arvore_criar();
    assert(a != NULL);
    int v = 7;
    assert(arvore_contar_nos(a) == 0);
    assert(arvore_altura(a) == -1);
    assert(!arvore_buscar(a, 1));
    assert(!arvore_remover(a, 1));
    assert(!arvore_minimo(a, &v));
    assert(!arvore_maximo(a, &v));
    assert(v == 7);
    conferir_percurso(a, arvore_pre_ordem, NULL, 0);
    conferir_percurso(a, arvore_em_ordem, NULL, 0);
    conferir_percurso(a, arvore_pos_ordem, NULL, 0);
    conferir_percurso(a, arvore_em_largura, NULL, 0);
    arvore_destruir(a);
}

static void teste_um_elemento(void)
{
    Arvore *a = arvore_criar();
    assert(a != NULL);
    assert(arvore_inserir(a, 10));
    assert(arvore_contar_nos(a) == 1);
    assert(arvore_altura(a) == 0);
    int v;
    assert(arvore_minimo(a, &v) && v == 10);
    assert(arvore_maximo(a, &v) && v == 10);
    const int e[] = {10};
    conferir_percurso(a, arvore_pre_ordem, e, 1);
    conferir_percurso(a, arvore_pos_ordem, e, 1);

    assert(arvore_remover(a, 10)); /* raiz folha */
    assert(arvore_contar_nos(a) == 0);
    assert(arvore_altura(a) == -1);
    assert(!arvore_buscar(a, 10));
    arvore_destruir(a);
}

static void teste_inserir_e_buscar(void)
{
    Arvore *a = arvore_exemplo();
    assert(arvore_contar_nos(a) == 10);
    assert(!arvore_inserir(a, 40)); /* repetida */
    assert(arvore_contar_nos(a) == 10);

    const int presentes[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 65};
    for (size_t i = 0; i < TAM(presentes); i++) {
        assert(arvore_buscar(a, presentes[i]));
    }
    assert(!arvore_buscar(a, 0));
    assert(!arvore_buscar(a, 55));
    assert(!arvore_buscar(a, 100));
    arvore_destruir(a);
}

static void teste_percursos(void)
{
    Arvore *a = arvore_exemplo();
    const int pre[] = {50, 30, 20, 40, 35, 45, 70, 60, 65, 80};
    const int em[] = {20, 30, 35, 40, 45, 50, 60, 65, 70, 80};
    const int pos[] = {20, 35, 45, 40, 30, 65, 60, 80, 70, 50};
    const int larg[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 65};
    conferir_percurso(a, arvore_pre_ordem, pre, TAM(pre));
    conferir_percurso(a, arvore_em_ordem, em, TAM(em));
    conferir_percurso(a, arvore_pos_ordem, pos, TAM(pos));
    conferir_percurso(a, arvore_em_largura, larg, TAM(larg));
    assert(!arvore_em_ordem(a, NULL, NULL));
    arvore_destruir(a);
}

static void teste_minimo_maximo_altura(void)
{
    Arvore *a = arvore_exemplo();
    int v;
    assert(arvore_minimo(a, &v) && v == 20);
    assert(arvore_maximo(a, &v) && v == 80);
    assert(arvore_altura(a) == 3);
    assert(arvore_inserir(a, 66));
    assert(arvore_altura(a) == 4);
    arvore_destruir(a);
}

static void teste_remover_folha(void)
{
    Arvore *a = arvore_exemplo();
    assert(arvore_remover(a, 20));
    assert(arvore_remover(a, 45));
    assert(!arvore_buscar(a, 20) && !arvore_buscar(a, 45));
    const int em[] = {30, 35, 40, 50, 60, 65, 70, 80};
    conferir_percurso(a, arvore_em_ordem, em, TAM(em));
    assert(arvore_contar_nos(a) == 8);
    arvore_destruir(a);
}

static void teste_remover_um_filho(void)
{
    Arvore *a = arvore_exemplo();
    assert(arvore_remover(a, 60)); /* so tem filho direito (65) */
    const int larg[] = {50, 30, 70, 20, 40, 65, 80, 35, 45};
    conferir_percurso(a, arvore_em_largura, larg, TAM(larg));
    arvore_destruir(a);
}

static void teste_remover_dois_filhos(void)
{
    Arvore *a = arvore_exemplo();
    assert(arvore_remover(a, 30)); /* sucessor = 35 */
    const int larg1[] = {50, 35, 70, 20, 40, 60, 80, 45, 65};
    conferir_percurso(a, arvore_em_largura, larg1, TAM(larg1));

    assert(arvore_remover(a, 50)); /* raiz; sucessor = 60, que tem filho direito */
    const int larg2[] = {60, 35, 70, 20, 40, 65, 80, 45};
    conferir_percurso(a, arvore_em_largura, larg2, TAM(larg2));

    assert(arvore_remover(a, 70)); /* sucessor = 80, filho direito direto */
    const int larg3[] = {60, 35, 80, 20, 40, 65, 45};
    conferir_percurso(a, arvore_em_largura, larg3, TAM(larg3));
    assert(!arvore_remover(a, 70));
    assert(arvore_contar_nos(a) == 7);
    arvore_destruir(a);
}

static void teste_remover_tudo(void)
{
    Arvore *a = arvore_exemplo();
    const int ordem[] = {50, 20, 70, 45, 30, 65, 80, 35, 40, 60};
    for (size_t i = 0; i < TAM(ordem); i++) {
        assert(arvore_remover(a, ordem[i]));
        assert(arvore_contar_nos(a) == TAM(ordem) - i - 1);
    }
    assert(arvore_altura(a) == -1);
    assert(arvore_inserir(a, 1)); /* reutilizavel depois de esvaziar */
    arvore_destruir(a);
}

/* Insercao em ordem crescente gera uma arvore degenerada (uma "lista").
 * Como nada e recursivo, nao ha risco de estourar a pilha. */
static void teste_arvore_degenerada(void)
{
    Arvore *a = arvore_criar();
    assert(a != NULL);
    const int n = 20000;
    for (int i = 0; i < n; i++) {
        assert(arvore_inserir(a, i));
    }
    assert(arvore_altura(a) == n - 1);
    assert(arvore_contar_nos(a) == (size_t)n);
    int v;
    assert(arvore_minimo(a, &v) && v == 0);
    assert(arvore_maximo(a, &v) && v == n - 1);
    assert(arvore_buscar(a, n - 1));
    assert(arvore_remover(a, 0));
    assert(arvore_remover(a, n / 2));
    assert(arvore_contar_nos(a) == (size_t)n - 2);
    arvore_destruir(a);
}

/* Contexto que verifica se o em-ordem sai estritamente crescente. */
typedef struct {
    bool primeiro;
    int anterior;
    size_t contagem;
} Ordenacao;

static void verificar_crescente(int chave, void *contexto)
{
    Ordenacao *o = contexto;
    assert(o->primeiro || chave > o->anterior);
    o->primeiro = false;
    o->anterior = chave;
    o->contagem++;
}

static void teste_aleatorio(void)
{
    Arvore *a = arvore_criar();
    assert(a != NULL);
    srand(12345);
    static bool presente[2000];
    size_t esperado = 0;
    for (int i = 0; i < 20000; i++) {
        int k = rand() % 2000;
        if (rand() % 3 == 0) {
            assert(arvore_remover(a, k) == presente[k]);
            if (presente[k]) {
                esperado--;
            }
            presente[k] = false;
        } else {
            assert(arvore_inserir(a, k) == !presente[k]);
            if (!presente[k]) {
                esperado++;
            }
            presente[k] = true;
        }
    }
    assert(arvore_contar_nos(a) == esperado);
    for (int k = 0; k < 2000; k++) {
        assert(arvore_buscar(a, k) == presente[k]);
    }
    Ordenacao o = {true, 0, 0};
    assert(arvore_em_ordem(a, verificar_crescente, &o));
    assert(o.contagem == esperado);
    arvore_destruir(a);
}

static void teste_ponteiros_nulos(void)
{
    assert(!arvore_inserir(NULL, 1));
    assert(!arvore_buscar(NULL, 1));
    assert(!arvore_remover(NULL, 1));
    assert(!arvore_minimo(NULL, NULL));
    assert(!arvore_maximo(NULL, NULL));
    assert(arvore_altura(NULL) == -1);
    assert(arvore_contar_nos(NULL) == 0);
    assert(!arvore_pre_ordem(NULL, coletar, NULL));
    assert(!arvore_em_largura(NULL, coletar, NULL));
    arvore_destruir(NULL);
}

int main(void)
{
    puts("Testes da arvore binaria de busca:");
    RODAR(teste_arvore_vazia);
    RODAR(teste_um_elemento);
    RODAR(teste_inserir_e_buscar);
    RODAR(teste_percursos);
    RODAR(teste_minimo_maximo_altura);
    RODAR(teste_remover_folha);
    RODAR(teste_remover_um_filho);
    RODAR(teste_remover_dois_filhos);
    RODAR(teste_remover_tudo);
    RODAR(teste_arvore_degenerada);
    RODAR(teste_aleatorio);
    RODAR(teste_ponteiros_nulos);
    printf("%d testes passaram\n", testes_passaram);
    return 0;
}
