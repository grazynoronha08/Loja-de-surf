#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// LISTA DUPLAMENTE ENCADEADA – PRODUTOS À VENDA
typedef struct NO {
    int codigo;
    char tipo[20];
    char descricao[100];
    float preco;
    struct NO* prox;
    struct NO* ant;
} NO;

NO* inicio = NULL;
NO* fim = NULL;
int tam = 0;

void add(int codigo, char* tipo, char* descricao, float preco) {
    NO* novo = malloc(sizeof(NO));
    novo->codigo = codigo;
    strcpy(novo->tipo, tipo);
    strcpy(novo->descricao, descricao);
    novo->preco = preco;
    novo->prox = NULL;
    novo->ant = NULL;

    if (inicio == NULL) {
        inicio = fim = novo;
    } else if (preco < inicio->preco) {
        novo->prox = inicio;
        inicio->ant = novo;
        inicio = novo;
    } else {
        NO* aux = inicio;
        while (aux->prox && aux->prox->preco <= preco){
            aux = aux->prox;
        }
        novo->prox = aux->prox;
        novo->ant = aux;
        if (aux->prox){  // se tiver um prox (meio)
            aux->prox->ant = novo;
        }else {          // se o prox == NULL (fim)
            fim = novo;
        }
        aux->prox = novo;
    }
    tam++;
}

NO* remover(int codigo) {
    NO* lixo = inicio;
    while (lixo && lixo->codigo != codigo){
        lixo = lixo->prox;
    }
    if (!lixo) {  // lixo == NULL
        return NULL;
    }
    if (lixo == inicio) {
        inicio = lixo->prox;
        if (inicio){  // inicio != NULL
            inicio->ant = NULL;
        } else {
            fim = NULL;
        }    
    } else if (lixo == fim) {
        fim = lixo->ant;
        fim->prox = NULL;
    } else {
        lixo->ant->prox = lixo->prox;
        lixo->prox->ant = lixo->ant;
    }

    tam--;
    return lixo;
}

/// VISUALIZAÇÃO DE PRODUTOS (REVENDA)
void mostrar_por_categoria(char* tipo) {
    NO* aux = inicio;
    printf("\nCategoria: %s\n", tipo);

    while (aux) {  // aux != NULL
        if (strcmp(aux->tipo, tipo) == 0) {
            printf("Codigo: %d | %s | R$ %.2f\n", aux->codigo, aux->descricao, aux->preco);
        }
        aux = aux->prox;
    }
}

void mostrar_por_preco(float min, float max) {
    NO* aux = inicio;
    printf("\nProdutos entre %.2f e %.2f\n", min, max);

    while (aux) { // aux != NULL
        if (aux->preco >= min && aux->preco <= max) {
            printf("Codigo: %d | %s | R$ %.2f\n", aux->codigo, aux->descricao, aux->preco);
        }
        aux = aux->prox;
    }
}

/// LISTA SIMPLES DE CÓDIGOS (DENTRO DA CAIXA)
typedef struct NO_COD_PROD {
    int codigo;
    struct NO_COD_PROD* prox;
} NO_COD_PROD;

typedef struct LISTA_COD_PROD {
    NO_COD_PROD* inicio;
    NO_COD_PROD* fim;
    int tam;
} LISTA_COD_PROD;

LISTA_COD_PROD* constroi_lista() {
    LISTA_COD_PROD* l = malloc(sizeof(LISTA_COD_PROD));
    l->inicio = NULL;
    l->fim = NULL;
    l->tam = 0;
    return l;
}

void add_lista_cod_prod(LISTA_COD_PROD* lista, int codigo) {
    NO_COD_PROD* novo = malloc(sizeof(NO_COD_PROD));
    novo->codigo = codigo;
    novo->prox = NULL;
    if (!lista->inicio){  // lista->inicio == NULL (vazia)
        lista->inicio = lista->fim = novo;
    } else {   //add no fim
        lista->fim->prox = novo;
        lista->fim = novo;
    }
    lista->tam++;
}

int remover_da_lista_codigos(LISTA_COD_PROD* lista, int codigo) {
    NO_COD_PROD *atual = lista->inicio;
    NO_COD_PROD *ant = NULL;
    while (atual) {  // atual != NULL
        if (atual->codigo == codigo) {
            if (!ant) { // ant == NULL (remove no inicio)
                lista->inicio = atual->prox;
            } else {  // se for no meio vai 1 no para frente
            ant->prox = atual->prox;  
            }
            if (atual == lista->fim) {  // remove no fim
                lista->fim = ant;
            }
            free(atual);
            lista->tam--;
            return 1;
        }
        ant = atual;
        atual = atual->prox;
    }
    return 0;
}

/// PILHAS DE CAIXAS (ESTOQUE)
typedef struct CAIXA {
    int codigo_caixa;
    LISTA_COD_PROD* lista_cod_prod;
} CAIXA;

typedef struct NO_PILHA {
    CAIXA* caixa;
    struct NO_PILHA* prox;
} NO_PILHA;

typedef struct PILHA {
    NO_PILHA* topo;
    int tam;
} PILHA;

PILHA* constroi_pilha() {
    PILHA* p = malloc(sizeof(PILHA));
    p->topo = NULL;
    p->tam = 0;
    return p;
}

CAIXA* constroi_caixa(int codigo) {
    CAIXA* c = malloc(sizeof(CAIXA));
    c->codigo_caixa = codigo;
    c->lista_cod_prod = constroi_lista();
    return c;
}

void add_pilha(PILHA* pilha, CAIXA* caixa) {
    NO_PILHA* novo = malloc(sizeof(NO_PILHA));
    novo->caixa = caixa;
    novo->prox = pilha->topo;
    pilha->topo = novo;
    pilha->tam++;
}

void remover_pilha(PILHA* pilha) {
    if (pilha->topo) {
        NO_PILHA* lixo = pilha->topo;
        pilha->topo = lixo->prox;
        free(lixo);
        pilha->tam--;
    }
}

void adicionar_estoque(PILHA* pilha, int codigo, int limite, int cod_caixa) { // add caixa na pilha
    if (!pilha->topo || pilha->topo->caixa->lista_cod_prod->tam == limite) { // se o topo == NULL ou a caixa estiver cheia 
        CAIXA* nova = constroi_caixa(cod_caixa);
        add_lista_cod_prod(nova->lista_cod_prod, codigo);
        add_pilha(pilha, nova);
    } else {
        add_lista_cod_prod(pilha->topo->caixa->lista_cod_prod, codigo);
    }
}

/// REMOÇÃO DA PILHA COM EQUILÍBRIO
int remover_pilha_com_equilibrio(PILHA* pilha, int codigo) {
    if (!pilha->topo){
        return 0;
    }
    PILHA* aux = constroi_pilha();
    CAIXA* origem = NULL;  
    while (pilha->topo) { 
        CAIXA* c = pilha->topo->caixa; // recebe caixa do topo
        remover_pilha(pilha); // remover caixa da pilha principal
        if (remover_da_lista_codigos(c->lista_cod_prod, codigo)) { // se conseguir remover produto
            origem = c; // origem recebe caixa do produto removido
            break; 
        }
        add_pilha(aux, c); // nao encotrou na caixa do topo mova c para pilha aux
    }
    if (!origem) { // origem == NULL
        return 0;
    }
    if (aux->topo) {
        CAIXA* equilibrio = aux->topo->caixa; //recebe caixa do topo da pilha auxiliar
        remover_pilha(aux); // remove essa caixa da pilha auxiliar
        NO_COD_PROD* mov = equilibrio->lista_cod_prod->inicio; // pegar inicio da lista da caixa de equilíbrio
        if (mov) {
            equilibrio->lista_cod_prod->inicio = mov->prox; // inicio da lista da caixa vai receber o prox (remove antigo inicio)
            equilibrio->lista_cod_prod->tam--; 
            add_lista_cod_prod(origem->lista_cod_prod, mov->codigo); // add antigo inicio em origem
            free(mov); 
        }
        while (aux->topo) {  // reempilhando as pilhas
            CAIXA* c = aux->topo->caixa;
            remover_pilha(aux);
            add_pilha(pilha, c);
        }
        add_pilha(pilha, origem); // ordem correta
        add_pilha(pilha, equilibrio);
    } else {
        add_pilha(pilha, origem);
    }
    free(aux);
    return 1;
}

/// COMPRA DE PRODUTO
NO* comprar_produto(int codigo, PILHA* p_leash, PILHA* p_parafina, PILHA* p_quilha, PILHA* p_deck) {
    NO* prod = inicio;
    while (prod && prod->codigo != codigo){ //procura codigo
        prod = prod->prox;
    }
    if (!prod){
         return NULL;
    }
    if (strcmp(prod->tipo, "Leash") == 0){ 
        remover_pilha_com_equilibrio(p_leash, codigo);
    }else if (strcmp(prod->tipo, "Parafina") == 0){
        remover_pilha_com_equilibrio(p_parafina, codigo);
    }else if (strcmp(prod->tipo, "Quilha") == 0){
        remover_pilha_com_equilibrio(p_quilha, codigo);
    }else if (strcmp(prod->tipo, "Deck") == 0){
        remover_pilha_com_equilibrio(p_deck, codigo);
    }
    return remover(codigo);
}

/// FILA DE ENTREGA
typedef struct PEDIDO {
    NO* produto;
    char nome_cliente[100];
    char cpf[15];
    char cep[10];
    char rua[100];
    int numero;
    char complemento[50];
    struct PEDIDO* prox;
} PEDIDO;

PEDIDO* inicio_fila = NULL;
PEDIDO* fim_fila = NULL;

void criar_pedido(NO* prod, char* nome, char* cpf, char* cep, char* rua, int numero, char* compl) {
    PEDIDO* p = malloc(sizeof(PEDIDO));
    p->produto = prod;
    strcpy(p->nome_cliente, nome);
    strcpy(p->cpf, cpf);
    strcpy(p->cep, cep);
    strcpy(p->rua, rua);
    p->numero = numero;
    strcpy(p->complemento, compl);
    p->prox = NULL;

    if (!inicio_fila)
        inicio_fila = fim_fila = p;
    else {
        fim_fila->prox = p;
        fim_fila = p;
    }
}

int entregador_atual = 1; // variável goblal para controlar o rodízio

void sair_para_entrega() {
    if (!inicio_fila) {
        printf("\nNenhum pedido para entrega.\n");
        return;
    }

    PEDIDO* p = inicio_fila;
    inicio_fila = p->prox;
    if (!inicio_fila){
        fim_fila = NULL;
    }
    printf("\nProduto saindo para entrega...\n");
    printf("Produto: %s | R$ %.2f\n", p->produto->descricao, p->produto->preco);
    printf("Cliente: %s | CPF: %s\n", p->nome_cliente, p->cpf);
    printf("Endereco: %s, %d (%s) - CEP %s\n", p->rua, p->numero, p->complemento, p->cep);
    printf("Entregador %d esta levando este pedido.\n", entregador_atual);

    // Atualizar para o próximo entregador (rodízio 1 → 2 → 3 → 1)
    entregador_atual++;
    if (entregador_atual > 3) {
        entregador_atual = 1;
    }
    free(p->produto);
    free(p);
}

int main() {

    /// INICIALIZACAO DAS PILHAS
    PILHA* p_leash    = constroi_pilha();
    PILHA* p_parafina = constroi_pilha();
    PILHA* p_quilha   = constroi_pilha();
    PILHA* p_deck     = constroi_pilha();

    int codigo_prod = 1;
    int codigo_caixa = 1;

    /// PROCESSO DE RECEBIMENTO
    /// CADASTRO DE 100 PRODUTOS
    for (int i = 0; i < 25; i++) {
        add(codigo_prod, "Parafina", "Parafina Wax Base", 10 + i);
        adicionar_estoque(p_parafina, codigo_prod++, 50, codigo_caixa++);

        add(codigo_prod, "Leash", "Leash Surf Pro", 80 + i);
        adicionar_estoque(p_leash, codigo_prod++, 25, codigo_caixa++);

        add(codigo_prod, "Quilha", "Quilha Performance", 120 + i);
        adicionar_estoque(p_quilha, codigo_prod++, 10, codigo_caixa++);

        add(codigo_prod, "Deck", "Deck Antiderrapante", 150 + i);
        adicionar_estoque(p_deck, codigo_prod++, 5, codigo_caixa++);
    }
    printf("\n=== RECEBIMENTO DE 100 PRODUTOS REALIZADO ===\n");

    /// IMPRESSAO DO ESTOQUE
    NO_PILHA* np;
    printf("\n--- ESTOQUE DE PARAFINAS ---\n");
    np = p_parafina->topo;
    while (np) {
        printf("Caixa %d | Quantidade: %d\n", np->caixa->codigo_caixa, np->caixa->lista_cod_prod->tam);
        np = np->prox;
    }

    printf("\n--- ESTOQUE DE LEASHES ---\n");
    np = p_leash->topo;
    while (np) {
        printf("Caixa %d | Quantidade: %d\n", np->caixa->codigo_caixa, np->caixa->lista_cod_prod->tam);
        np = np->prox;
    }

    printf("\n--- ESTOQUE DE QUILHAS ---\n");
    np = p_quilha->topo;
    while (np) {
        printf("Caixa %d | Quantidade: %d\n", np->caixa->codigo_caixa, np->caixa->lista_cod_prod->tam);
        np = np->prox;
    }

    printf("\n--- ESTOQUE DE DECKS ---\n");
    np = p_deck->topo;
    while (np) {
        printf("Caixa %d | Quantidade: %d\n", np->caixa->codigo_caixa, np->caixa->lista_cod_prod->tam);
        np = np->prox;
    }

    /// PROCESSO DE REVENDA (VISUALIZACAO)
    printf("\n=== VISUALIZACAO POR CATEGORIA ===\n");
    mostrar_por_categoria("Parafina");
    mostrar_por_categoria("Leash");

    printf("\n=== VISUALIZACAO POR PRECO ===\n");
    mostrar_por_preco(90, 140);

    /// COMPRA DE 20 PRODUTOS
    printf("\n=== COMPRA DE 20 PRODUTOS ===\n");
    for (int i = 1; i <= 20; i++) {
        NO* vendido = comprar_produto( i, p_leash, p_parafina, p_quilha, p_deck);
        if (vendido) {
            criar_pedido(vendido, "Cliente Teste", "12345678900", "12345678", "Rua do Surf", 100 + i, "Apto 1");
            printf("Produto %d vendido com sucesso.\n", i);
        }
    }

    /// PROCESSO DE ENTREGA
    printf("\n=== SAIDA PARA ENTREGA (10 PRODUTOS) ===\n");
    for (int i = 0; i < 10; i++) {
        sair_para_entrega();
    }
    printf("\n=== SIMULACAO FINALIZADA COM SUCESSO ===\n");

    return 0;
}