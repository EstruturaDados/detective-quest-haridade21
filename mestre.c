#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOME 64
#define MAX_PISTA 128
#define HASH_SIZE 101

// ---------------------- ESTRUTURAS ----------------------

// Nó da árvore binária que representa uma sala da mansão.
typedef struct Sala {
    char nome[MAX_NOME];
    char pista[MAX_PISTA]; // pista estática associada à sala (pode ser "")
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// Nó da BST que armazena pistas coletadas (ordenadas alfabeticamente).
typedef struct PistaNode {
    char pista[MAX_PISTA];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// Nó para tabela hash que mapeia chave string -> valor string (pista -> suspeito)
typedef struct HashNodeStr {
    char key[MAX_PISTA];     // pista
    char value[MAX_NOME];    // suspeito
    struct HashNodeStr *next;
} HashNodeStr;

// Nó para tabela hash que mapeia suspeito -> contador de pistas (int)
typedef struct HashNodeInt {
    char key[MAX_NOME];      // suspeito
    int value;               // contador
    struct HashNodeInt *next;
} HashNodeInt;

// Tabelas hash (encadeamento)
HashNodeStr* hashPistaToSuspeito[HASH_SIZE];
HashNodeInt* hashSuspeitoCount[HASH_SIZE];

// ---------------------- FUNÇÕES AUXILIARES HASH ----------------------

unsigned int hashString(const char *s) {
    // djb2 simplificado
    unsigned long hash = 5381;
    int c;
    while ((c = *s++))
        hash = ((hash << 5) + hash) + (unsigned char)c;
    return (unsigned int)(hash % HASH_SIZE);
}

// Insere associação pista -> suspeito na tabela hash.
// Função exigida: inserirNaHash()
/*
 inserirNaHash(key, suspect):
 - key: string da pista
 - suspect: string do suspeito
 - insere na tabela hash hashPistaToSuspeito.
 - sobrescreve se a chave já existir.
*/
void inserirNaHash(const char *key, const char *suspect) {
    unsigned int idx = hashString(key);
    HashNodeStr *cur = hashPistaToSuspeito[idx];
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            // sobrescreve o suspeito caso já exista
            strncpy(cur->value, suspect, MAX_NOME-1);
            cur->value[MAX_NOME-1] = '\0';
            return;
        }
        cur = cur->next;
    }
    // criar novo nó
    HashNodeStr *novo = (HashNodeStr*) malloc(sizeof(HashNodeStr));
    if (!novo) { perror("malloc"); exit(1); }
    strncpy(novo->key, key, MAX_PISTA-1); novo->key[MAX_PISTA-1] = '\0';
    strncpy(novo->value, suspect, MAX_NOME-1); novo->value[MAX_NOME-1] = '\0';
    novo->next = hashPistaToSuspeito[idx];
    hashPistaToSuspeito[idx] = novo;
}

// Busca suspeito pela pista. Retorna NULL se não encontrada.
// Função exigida: encontrarSuspeito()
/*
 encontrarSuspeito(pista) -> retorna ponteiro para string do suspeito (static) ou NULL.
*/
const char* encontrarSuspeito(const char *pista) {
    unsigned int idx = hashString(pista);
    HashNodeStr *cur = hashPistaToSuspeito[idx];
    while (cur) {
        if (strcmp(cur->key, pista) == 0)
            return cur->value;
        cur = cur->next;
    }
    return NULL;
}

// Insere ou atualiza contador do suspeito na tabela hash de contagem.
// Se não existir, insere com valor 1 (ou incremento fornecido).
void incrementarContadorSuspeito(const char *suspeito, int incremento) {
    unsigned int idx = hashString(suspeito);
    HashNodeInt *cur = hashSuspeitoCount[idx];
    while (cur) {
        if (strcmp(cur->key, suspeito) == 0) {
            cur->value += incremento;
            return;
        }
        cur = cur->next;
    }
    // criar novo
    HashNodeInt *novo = (HashNodeInt*) malloc(sizeof(HashNodeInt));
    if (!novo) { perror("malloc"); exit(1); }
    strncpy(novo->key, suspeito, MAX_NOME-1); novo->key[MAX_NOME-1] = '\0';
    novo->value = incremento;
    novo->next = hashSuspeitoCount[idx];
    hashSuspeitoCount[idx] = novo;
}

// Busca contador do suspeito (0 se não existir)
int buscarContadorSuspeito(const char *suspeito) {
    unsigned int idx = hashString(suspeito);
    HashNodeInt *cur = hashSuspeitoCount[idx];
    while (cur) {
        if (strcmp(cur->key, suspeito) == 0)
            return cur->value;
        cur = cur->next;
    }
    return 0;
}

// ---------------------- CRIAÇÃO DE SALAS ----------------------

// Cria dinamicamente um cômodo (Sala) com nome e pista.
// Função exigida: criarSala()
/*
 criarSala(nome, pista):
 - nome: identificador do cômodo
 - pista: string da pista ("" se não houver)
 - retorna ponteiro para Sala alocada dinamicamente.
*/
Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) { perror("malloc"); exit(1); }
    strncpy(s->nome, nome, MAX_NOME-1); s->nome[MAX_NOME-1] = '\0';
    strncpy(s->pista, pista, MAX_PISTA-1); s->pista[MAX_PISTA-1] = '\0';
    s->esquerda = s->direita = NULL;
    return s;
}

// ---------------------- BST DE PISTAS ----------------------

// Cria nó de pista
PistaNode* criarPistaNode(const char *pista) {
    PistaNode *p = (PistaNode*) malloc(sizeof(PistaNode));
    if (!p) { perror("malloc"); exit(1); }
    strncpy(p->pista, pista, MAX_PISTA-1); p->pista[MAX_PISTA-1] = '\0';
    p->esquerda = p->direita = NULL;
    return p;
}

// Insere nova pista na BST (alfabética).
// Função exigida: inserirPista() / adicionarPista()
/*
 inserirPista(raiz, pista, coletadaFlag):
 - raiz: ponteiro para raiz atual da BST.
 - pista: string da pista a inserir.
 - coletadaFlag (ponteiro int): se >0, será incrementado quando pista inserida (para contar quantas pistas novas foram inseridas).
 - retorna nova raiz.
 - Não insere duplicatas (strings idênticas).
*/
PistaNode* inserirPista(PistaNode *raiz, const char *pista, int *coletadaFlag) {
    if (raiz == NULL) {
        if (coletadaFlag) *coletadaFlag = 1;
        return criarPistaNode(pista);
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista, coletadaFlag);
    } else if (cmp > 0) {
        raiz->direita = inserirPista(raiz->direita, pista, coletadaFlag);
    } else {
        // mesma pista, não inserir duplicata
        if (coletadaFlag) *coletadaFlag = 0;
    }
    return raiz;
}

// Exibe pistas (in-order => alfabético)
void exibirPistas(PistaNode *raiz) {
    if (!raiz) return;
    exibirPistas(raiz->esquerda);
    printf(" - %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}

// ---------------------- EXPLORAÇÃO DA MANSÃO ----------------------

// Função exigida: explorarSalas()
// Navega a partir da sala atual e coleta pistas automaticamente ao entrar.
void explorarSalas(Sala *inicio, PistaNode **raizPistas) {
    Sala *atual = inicio;
    char escolha;
    while (1) {
        printf("\nVocê está em: %s\n", atual->nome);
        if (strlen(atual->pista) > 0) {
            printf(" -> Pista encontrada: \"%s\"\n", atual->pista);
            // tenta inserir na BST de pistas; se inseriu (nova), então incrementa contador do suspeito
            int inseriu = 0;
            *raizPistas = inserirPista(*raizPistas, atual->pista, &inseriu);
            if (inseriu) {
                const char *sus = encontrarSuspeito(atual->pista);
                if (sus != NULL) {
                    incrementarContadorSuspeito(sus, 1);
                    printf("    (a pista aponta para o suspeito: %s)\n", sus);
                } else {
                    printf("    (pista sem associação a suspeitos)\n");
                }
            } else {
                printf("    (pista já coletada anteriormente)\n");
            }
        } else {
            printf(" -> Nenhuma pista neste cômodo.\n");
        }

        // opções ao jogador
        printf("\nEscolhas: (e) esquerda   (d) direita   (s) sair\nOpção: ");
        if (scanf(" %c", &escolha) != 1) {
            // limpar stdin
            int c; while ((c=getchar()) != '\n' && c != EOF);
            escolha = 'x';
        }

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esquerda) atual = atual->esquerda;
            else printf("Não há sala à esquerda.\n");
        } else if (escolha == 'd' || escolha == 'D') {
            if (atual->direita) atual = atual->direita;
            else printf("Não há sala à direita.\n");
        } else if (escolha == 's' || escolha == 'S') {
            printf("Exploração encerrada pelo jogador.\n");
            break;
        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    }
}

// ---------------------- JULGAMENTO FINAL ----------------------

// Função exigida: verificarSuspeitoFinal()
// Verifica se existem pelo menos 2 pistas que apontam para o suspeito acusado.
/*
 verificarSuspeitoFinal(suspeito):
 - retorna 1 se há >= 2 pistas, 0 caso contrário.
 - também imprime mensagem explicativa.
*/
int verificarSuspeitoFinal(const char *suspeito) {
    int contador = buscarContadorSuspeito(suspeito);
    printf("\nVerificando acusação contra: %s\n", suspeito);
    printf("Pistas que apontam para %s: %d\n", suspeito, contador);
    if (contador >= 2) {
        printf("Resultado: Há evidências suficientes. O suspeito %s é considerado CULPADO!\n", suspeito);
        return 1;
    } else {
        printf("Resultado: Evidências insuficientes. %s NÃO pode ser considerado culpado com base nas pistas.\n", suspeito);
        return 0;
    }
}

// ---------------------- LIMPAR MEMÓRIA ----------------------

void liberarPistasBST(PistaNode *r) {
    if (!r) return;
    liberarPistasBST(r->esquerda);
    liberarPistasBST(r->direita);
    free(r);
}
void liberarSalas(Sala *r) {
    if (!r) return;
    liberarSalas(r->esquerda);
    liberarSalas(r->direita);
    free(r);
}
void liberarHashPistaToSuspeito() {
    for (int i=0;i<HASH_SIZE;i++) {
        HashNodeStr *cur = hashPistaToSuspeito[i];
        while (cur) {
            HashNodeStr *aux = cur->next;
            free(cur);
            cur = aux;
        }
        hashPistaToSuspeito[i] = NULL;
    }
}
void liberarHashSuspeitoCount() {
    for (int i=0;i<HASH_SIZE;i++) {
        HashNodeInt *cur = hashSuspeitoCount[i];
        while (cur) {
            HashNodeInt *aux = cur->next;
            free(cur);
            cur = aux;
        }
        hashSuspeitoCount[i] = NULL;
    }
}

// ---------------------- FUNÇÃO MAIN ----------------------

int main() {
    // Inicialização das hashes para NULL
    for (int i=0;i<HASH_SIZE;i++) {
        hashPistaToSuspeito[i] = NULL;
        hashSuspeitoCount[i] = NULL;
    }

    printf("=== DETECTIVE QUEST: JULGAMENTO FINAL ===\n");
    printf("Explore a mansão, colete pistas e acuse quem você acha culpado.\n");

    // Montagem fixa do mapa (árvore binária de salas)
    // Cada sala tem uma pista estática definida aqui (pode ser string vazia).
    Sala *hall = criarSala("Hall de Entrada", "Pegadas de lama no tapete");
    Sala *salaEstar = criarSala("Sala de Estar", "Copo de vinho quebrado");
    Sala *cozinha = criarSala("Cozinha", "Faca faltando no bloco");
    Sala *biblioteca = criarSala("Biblioteca", "Livro rasgado com iniciais P.A.");
    Sala *jardim = criarSala("Jardim", "");
    Sala *adega = criarSala("Adega", "Garrafa com rótulo de vinícola X");
    Sala *escritorio = criarSala("Escritório", "Carta rasgada com assinatura S.");

    // Construindo a árvore (fixa)
    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;
    cozinha->esquerda = adega;
    cozinha->direita = escritorio;

    // Popula tabela hash de pistas -> suspeitos (associações codificadas)
    // Ex.: "Pegadas de lama no tapete" -> "Pedro"
    inserirNaHash("Pegadas de lama no tapete", "Pedro");
    inserirNaHash("Copo de vinho quebrado", "Ana");
    inserirNaHash("Faca faltando no bloco", "Carlos");
    inserirNaHash("Livro rasgado com iniciais P.A.", "Pedro");
    inserirNaHash("Garrafa com rótulo de vinícola X", "Ana");
    inserirNaHash("Carta rasgada com assinatura S.", "Sofia");

    // BST de pistas coletadas (inicialmente vazia)
    PistaNode *raizPistas = NULL;

    // Exploração interativa a partir do hall
    explorarSalas(hall, &raizPistas);

    // Exibir lista final de pistas coletadas
    printf("\n===== PISTAS COLETADAS (ORDENADAS) =====\n");
    if (!raizPistas) {
        printf("Nenhuma pista foi coletada.\n");
    } else {
        exibirPistas(raizPistas);
    }

    // Solicita ao jogador indicar quem é o culpado
    char escolhaSuspeito[MAX_NOME];
    printf("\nInforme o nome do suspeito que deseja acusar (ex: Pedro, Ana, Carlos, Sofia):\n> ");
    // limpar buffer antes de fgets
    int c; while ((c=getchar()) != '\n' && c != EOF);
    if (fgets(escolhaSuspeito, sizeof(escolhaSuspeito), stdin) == NULL) {
        printf("Entrada inválida.\n");
        escolhaSuspeito[0] = '\0';
    } else {
        // remove newline
        size_t L = strlen(escolhaSuspeito);
        if (L>0 && escolhaSuspeito[L-1]=='\n') escolhaSuspeito[L-1] = '\0';
    }
    // Normaliza espaços finais e iniciais (remover)
    // Simplificação: assume usuário digitou corretamente o nome (case-sensitive).
    if (strlen(escolhaSuspeito) == 0) {
        printf("Nenhum suspeito informado. Encerrando.\n");
    } else {
        verificarSuspeitoFinal(escolhaSuspeito);
    }

    // Limpeza de memória
    liberarSalas(hall);
    liberarPistasBST(raizPistas);
    liberarHashPistaToSuspeito();
    liberarHashSuspeitoCount();

    printf("\nObrigado por jogar Detective Quest - Desfecho concluído.\n");
    return 0;
}
