# Árvore Binária de Busca (ABB) em C

[![CI](https://github.com/sidsidoso/arvore-binaria-busca-c/actions/workflows/ci.yml/badge.svg)](https://github.com/sidsidoso/arvore-binaria-busca-c/actions/workflows/ci.yml)
![Linguagem](https://img.shields.io/badge/linguagem-C11-blue.svg)
[![Licença: MIT](https://img.shields.io/badge/licen%C3%A7a-MIT-green.svg)](LICENSE)

Implementação de uma **Árvore Binária de Busca** de inteiros em C11 com
inserção, busca e remoção **iterativas**, mínimo, máximo, altura, contagem de
nós e os quatro percursos clássicos (**pré-ordem, em-ordem, pós-ordem e em
largura**). Projeto desenvolvido para a disciplina de **Estrutura de Dados**.

---

## Sumário

- [Conceito](#conceito)
- [Operações e complexidade](#operações-e-complexidade)
- [Decisões de implementação e otimizações](#decisões-de-implementação-e-otimizações)
- [Estrutura de pastas](#estrutura-de-pastas)
- [Como compilar, executar e testar](#como-compilar-executar-e-testar)
- [Exemplo de saída](#exemplo-de-saída)
- [Referências](#referências)

---

## Conceito

Uma **árvore binária** é formada por nós, cada um com no máximo dois filhos
(esquerdo e direito). Na **árvore binária de busca** vale a propriedade:

> para todo nó *x*, as chaves da subárvore **esquerda** são **menores** que
> *x* e as da subárvore **direita** são **maiores** que *x*.

Assim, cada comparação descarta uma subárvore inteira — como em uma busca
binária. Árvore usada na demonstração (inserção de
`50 30 70 20 40 60 80 35 45 65`):

```
                 50
              /      \
            30        70
           /  \      /  \
         20    40  60    80
              /  \   \
            35   45   65
```

### Percursos

| Percurso     | Ordem de visita                     | Resultado na árvore acima        |
|--------------|-------------------------------------|----------------------------------|
| Pré-ordem    | raiz, esquerda, direita             | 50 30 20 40 35 45 70 60 65 80    |
| Em-ordem     | esquerda, raiz, direita             | 20 30 35 40 45 50 60 65 70 80    |
| Pós-ordem    | esquerda, direita, raiz             | 20 35 45 40 30 65 60 80 70 50    |
| Em largura   | nível por nível (fila)              | 50 30 70 20 40 60 80 35 45 65    |

O percurso **em-ordem** de uma ABB sempre produz as chaves **ordenadas**.

### Remoção — os 3 casos

```
 Caso 1: folha (o nó é simplesmente removido)

       40                    40
      /  \       ==>        /
    35   [45]             35

 Caso 2: um filho (o filho ocupa o lugar do nó)

       70                    70
      /  \       ==>        /  \
   [60]   80              65    80
      \
       65

 Caso 3: dois filhos (a chave é trocada pela do sucessor em-ordem)

      [30]                   35
      /  \                  /  \
    20    40     ==>      20    40
         /  \                     \
       35    45                    45
       ^
       sucessor = menor chave da subárvore direita
```

No caso 3 a chave do nó é substituída pela do **sucessor em-ordem** (o menor
nó da subárvore direita). O sucessor nunca tem filho esquerdo, então removê-lo
cai no caso 1 ou 2.

## Operações e complexidade

*h* = altura da árvore: O(log n) se balanceada, O(n) no pior caso
(chaves inseridas já ordenadas geram uma árvore "degenerada", igual a uma lista).

| Operação             | Função                | Tempo  | Espaço extra |
|----------------------|-----------------------|--------|--------------|
| Criar                | `arvore_criar`        | O(1)   | O(1)         |
| Destruir             | `arvore_destruir`     | O(n)   | O(1)         |
| Inserir              | `arvore_inserir`      | O(h)   | O(1)         |
| Buscar               | `arvore_buscar`       | O(h)   | O(1)         |
| Remover              | `arvore_remover`      | O(h)   | O(1)         |
| Mínimo / Máximo      | `arvore_minimo`, `arvore_maximo` | O(h) | O(1)  |
| Altura               | `arvore_altura`       | O(n)   | O(n)         |
| Contar nós           | `arvore_contar_nos`   | O(1)   | O(1)         |
| Pré/em/pós-ordem     | `arvore_pre_ordem`, `arvore_em_ordem`, `arvore_pos_ordem` | O(n) | O(n)* |
| Em largura           | `arvore_em_largura`   | O(n)   | O(n)         |

\* Os percursos em profundidade usam no máximo *h* + 1 posições da pilha
(pré-ordem: até *n*), mas o vetor auxiliar é alocado com *n* posições de uma
só vez para nunca precisar realocar durante o percurso.

Espaço total da estrutura: **O(n)**.

## Decisões de implementação e otimizações

- **Nenhuma recursão.** Inserção, busca e remoção são iterativas; os percursos
  usam **pilha explícita** (profundidade) ou **fila** (largura). Assim uma
  árvore degenerada com dezenas de milhares de nós não estoura a pilha de
  chamadas (há um teste com 20 000 chaves em ordem crescente).
- **Ponteiro para ponteiro (`NoArvore **elo`)** na inserção e na remoção:
  o ponteiro que aponta para o nó (seja a raiz ou um filho) é alterado
  diretamente, sem precisar guardar o "pai" nem tratar a raiz à parte.
- **Remoção com sucessor**: no caso de dois filhos, copia-se a chave do
  sucessor e remove-se o nó sucessor (que tem no máximo um filho), evitando
  reorganizar ponteiros do nó original.
- **Destruição com rotações** (O(n) tempo, **O(1)** memória): enquanto o nó
  atual tem filho esquerdo, faz-se uma rotação à direita; sem filho esquerdo,
  o nó é liberado e segue-se para a direita.
- **Fila interna simples** no percurso em largura e na altura: um vetor de *n*
  ponteiros com índices `inicio`/`fim`. Como cada nó entra na fila uma única
  vez, não é preciso buffer circular.
- **Altura por níveis** (em arestas: vazia = −1, só raiz = 0), calculada
  processando a fila nível a nível.
- **Contagem de nós em O(1)**: o tamanho é atualizado em toda inserção/remoção.
- **Chaves repetidas são rejeitadas** (`arvore_inserir` retorna `false`).
- **Tipo opaco**, verificação de todo `malloc` e percursos com
  *callback* + contexto (`void *`).

## Estrutura de pastas

```
arvore-binaria-busca-c/
├── .github/workflows/ci.yml   # integração contínua (testes com sanitizers + demo)
├── include/arvore.h           # API pública (struct opaca + documentação)
├── src/arvore.c               # implementação
├── src/main.c                 # programa de demonstração
├── tests/test_arvore.c        # testes unitários com assert
├── Makefile
├── LICENSE
└── README.md
```

## Como compilar, executar e testar

### Linux / macOS (com `make`)

```bash
make            # compila demo e testes em build/
make run        # executa a demonstração
make test       # executa os testes
make test SANITIZE=1   # testes com AddressSanitizer + UBSan
make clean      # remove build/
```

### Windows (MinGW-w64 / GCC, sem `make`)

```bat
mkdir build
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude src/arvore.c src/main.c -o build/demo.exe
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude src/arvore.c tests/test_arvore.c -o build/test_arvore.exe
build\demo.exe
build\test_arvore.exe
```

Saída esperada dos testes:

```
Testes da arvore binaria de busca:
  [ok] teste_arvore_vazia
  [ok] teste_um_elemento
  [ok] teste_inserir_e_buscar
  [ok] teste_percursos
  [ok] teste_minimo_maximo_altura
  [ok] teste_remover_folha
  [ok] teste_remover_um_filho
  [ok] teste_remover_dois_filhos
  [ok] teste_remover_tudo
  [ok] teste_arvore_degenerada
  [ok] teste_aleatorio
  [ok] teste_ponteiros_nulos
12 testes passaram
```

## Exemplo de saída

Saída real de `make run`:

```
== Arvore Binaria de Busca ==
Inserindo: 50 30 70 20 40 60 80 35 45 65
Inserir 40 de novo: rejeitado (chave repetida)

  pre-ordem:  50 30 20 40 35 45 70 60 65 80
  em-ordem:   20 30 35 40 45 50 60 65 70 80
  pos-ordem:  20 35 45 40 30 65 60 80 70 50
  em largura: 50 30 70 20 40 60 80 35 45 65
  nos=10 altura=3 minimo=20 maximo=80

Buscar 45: encontrado
Buscar 99: nao encontrado

Removendo 30 (dois filhos -> sucessor 35): ok
  em-ordem:   20 35 40 45 50 60 65 70 80
  em largura: 50 35 70 20 40 60 80 45 65

Removendo 20 (folha): ok
  em-ordem:   35 40 45 50 60 65 70 80
  em largura: 50 35 70 40 60 80 45 65

Removendo 60 (um filho): ok
  em-ordem:   35 40 45 50 65 70 80
  em largura: 50 35 70 40 65 80 45

Removendo 50 (raiz com dois filhos -> sucessor 65): ok
  em-ordem:   35 40 45 65 70 80
  em largura: 65 35 70 40 80 45

Removendo 99 (inexistente): nao encontrado
  em-ordem:   35 40 45 65 70 80
  em largura: 65 35 70 40 80 45

  nos=6 altura=3 minimo=35 maximo=80
```

## Referências

- CORMEN, T. H.; LEISERSON, C. E.; RIVEST, R. L.; STEIN, C. *Algoritmos: Teoria e Prática*. 3. ed. Rio de Janeiro: Elsevier, 2012. (Cap. 12 — Árvores de busca binária; Cap. 22.2 — Busca em largura.)
- SEDGEWICK, R.; WAYNE, K. *Algorithms*. 4. ed. Addison-Wesley, 2011. (Seção 3.2 — Binary Search Trees; remoção de Hibbard.)
- KNUTH, D. E. *The Art of Computer Programming, Vol. 3: Sorting and Searching*. 2. ed. Addison-Wesley, 1998. (Seção 6.2.2.)
- SZWARCFITER, J. L.; MARKENZON, L. *Estruturas de Dados e seus Algoritmos*. 3. ed. LTC, 2010.

## Licença

Distribuído sob a licença MIT. Veja [LICENSE](LICENSE).
