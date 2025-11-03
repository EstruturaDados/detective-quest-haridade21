#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------- ESTRUTURA DA SALA (ÁRVORE BINÁRIA) ----------
// Cada sala representa um cômodo da mansão, com um nome e uma pista opcional.
typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// ---------- ESTRUTURA DA PISTA (BST) ----------
// Cada nó da árvore BST contém uma pista coletada, organizada alfabeticamente.
typedef struct PistaNode {
    char pista[100];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// ---------- FUNÇÃO: criarSala ----------
// Cria uma sala dinamicamente com nome e pista.
Sala* criarSala(const char *nome, const char *pista) {
Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memória para a sala!\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// ---------- FUNÇÃO: criarPistaNode ----------
// Cria um nó de pista dinamicamente.
PistaNode* criarPistaNode(const char *pista) {
    PistaNode *nova = (PistaNode*) malloc(sizeof(PistaNode));
    if (nova == NULL) {
        printf("Erro ao alocar memória para a pista!\n");
        exit(1);
    }
    strcpy(nova->pista, pista);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// ---------- FUNÇÃO: inserirPista ----------
// Insere uma nova pista na árvore BST em ordem alfabética.
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) {
        return criarPistaNode(pista);
    }
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);
    // pistas iguais são ignoradas
    return raiz;
}

// ---------- FUNÇÃO: exibirPistas ----------
// Exibe todas as pistas coletadas em ordem alfabética (in-order traversal).
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf("🔍 %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}

// ---------- FUNÇÃO: explorarSalasComPistas ----------
// Permite que o jogador explore a mansão e colete pistas automaticamente.
void explorarSalasComPistas(Sala *atual, PistaNode **raizPistas) {
    char escolha;

    while (1) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Exibe pista, se houver
        if (strlen(atual->pista) > 0) {
            printf("Você encontrou uma pista: \"%s\"\n", atual->pista);
            *raizPistas = inserirPista(*raizPistas, atual->pista);
        } else {
            printf("Nenhuma pista neste cômodo.\n");
        }

        // Opções de movimento
        printf("\nEscolha seu próximo passo:\n");
        printf("(e) Ir para a esquerda\n");
        printf("(d) Ir para a direita\n");
        printf("(s) Sair da exploração\n");
        printf("Opção: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esquerda != NULL)
                atual = atual->esquerda;
            else
                printf("Não há sala à esquerda!\n");
        }
        else if (escolha == 'd' || escolha == 'D') {
            if (atual->direita != NULL)
                atual = atual->direita;
            else
                printf("Não há sala à direita!\n");
        }
        else if (escolha == 's' || escolha == 'S') {
            printf("\nVocê decidiu encerrar a exploração.\n");
            break;
        }
        else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

// ---------- FUNÇÃO: liberarArvoreSalas ----------
void liberarArvoreSalas(Sala *raiz) {
    if (raiz == NULL) return;
    liberarArvoreSalas(raiz->esquerda);
    liberarArvoreSalas(raiz->direita);
    free(raiz);
}

// ---------- FUNÇÃO: liberarArvorePistas ----------
void liberarArvorePistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    liberarArvorePistas(raiz->esquerda);
    liberarArvorePistas(raiz->direita);
    free(raiz);
}

// ---------- FUNÇÃO PRINCIPAL ----------
int main() {
    printf("=== DETECTIVE QUEST: COLETA DE PISTAS ===\n");
    printf("Você está prestes a explorar a mansão misteriosa...\n");

    // Mapa fixo da mansão (árvore binária)
    Sala *hall = criarSala("Hall de Entrada", "Pegadas de lama no tapete.");
    Sala *salaEstar = criarSala("Sala de Estar", "Um copo de vinho derramado.");
    Sala *cozinha = criarSala("Cozinha", "Uma faca afiada com manchas.");
    Sala *biblioteca = criarSala("Biblioteca", "Um livro aberto em uma página marcada.");
    Sala *jardim = criarSala("Jardim", "");
    Sala *adega = criarSala("Adega", "Uma garrafa faltando na prateleira.");
    Sala *escritorio = criarSala("Escritório", "Uma carta rasgada sobre a mesa.");

    // Estrutura da mansão (árvore binária)
    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;
    cozinha->esquerda = adega;
    cozinha->direita = escritorio;

    // Árvore de pistas inicialmente vazia
    PistaNode *raizPistas = NULL;

    // Exploração
    explorarSalasComPistas(hall, &raizPistas);

    // Exibição final das pistas
    printf("\n===== PISTAS COLETADAS =====\n");
    if (raizPistas == NULL)
        printf("Nenhuma pista foi coletada.\n");
    else
        exibirPistas(raizPistas);

    // Liberação de memória
    liberarArvoreSalas(hall);
    liberarArvorePistas(raizPistas);

    printf("\nFim da investigação.\n");
    return 0;
}
