#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------- ESTRUTURA DA SALA ----------
// Cada sala representa um nó da árvore binária.
typedef struct Sala {
    char nome[50];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// ---------- FUNÇÃO: criarSala ----------
// Cria uma sala de forma dinâmica com o nome informado.
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// ---------- FUNÇÃO: explorarSalas ----------
// Permite que o jogador explore a mansão a partir de uma sala.
// O jogador escolhe ir para a esquerda (e), direita (d) ou sair (s).
void explorarSalas(Sala *atual) {
    char escolha;

    while (1) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Caso o jogador esteja em um cômodo sem saídas (nó-folha)
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Você chegou ao fim do caminho! Não há mais portas.\n");
            break;
        }

        printf("Deseja ir para (e) esquerda, (d) direita ou (s) sair? ");
        scanf(" %c", &escolha);

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esquerda != NULL) {
                atual = atual->esquerda;
            } else {
                printf("Não há sala à esquerda!\n");
            }
        } 
        else if (escolha == 'd' || escolha == 'D') {
            if (atual->direita != NULL) {
                atual = atual->direita;
            } else {
                printf("Não há sala à direita!\n");
            }
        } 
        else if (escolha == 's' || escolha == 'S') {
            printf("Você decidiu sair da exploração.\n");
            break;
        } 
        else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

// ---------- FUNÇÃO: liberarArvore ----------
// Libera a memória alocada para todas as salas da árvore.
void liberarArvore(Sala *raiz) {
    if (raiz == NULL) return;
    liberarArvore(raiz->esquerda);
    liberarArvore(raiz->direita);
    free(raiz);
}

// ---------- FUNÇÃO PRINCIPAL ----------
int main() {
    // Montagem automática do mapa da mansão
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *jardim = criarSala("Jardim");
    Sala *adega = criarSala("Adega");
    Sala *escritorio = criarSala("Escritorio");

    // Conectando as salas (árvore binária)sd
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->esquerda = adega;
    cozinha->direita = escritorio;

    // Início da exploração
    printf("=== DETECTIVE QUEST ===\n");
    printf("Você está prestes a explorar a mansão misteriosa...\n");

    explorarSalas(hall);

    // Libera memória
    liberarArvore(hall);

    printf("\nFim da exploração.\n");
    return 0;
}
