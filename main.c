#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define RED 1
#define BLACK 0
#define MAX_PALAVRA 50

//Criação da árvore red-black ArvoreRB, a qual vai receber as informações de cada nodo em
//char parr, com o limite de 50 caracteres.
typedef struct arvoreRB {
    char parr[MAX_PALAVRA];
    int qtdOcorrencias;
    int cor;
    struct arvoreRB *esq;
    struct arvoreRB *dir;
} ArvoreRB;

struct docAleatorio {
    char* nome;
    double TFidf;
    struct docAleatorio *pr;
} typedef DocAleatorio;

typedef struct repetePalv {
    ArvoreRB* palavra;
    struct repetePalv *p;
} RepetePalv;

//Verifica se o nó é vermelho de acordo com as especificações das árvores red-black e
//retorna nó vermelho caso o anterior for preto. Do contrário e se não tiver nó
// na iteração anterior, retorna preto.
int eh_no_vermelho (ArvoreRB *no) {
    if(!no) return BLACK;
    return(no->cor == RED);
}

//Insere novo nó
ArvoreRB* novo_no (char* palavra) {
    ArvoreRB *arv = (ArvoreRB*) malloc(sizeof(ArvoreRB));
    strcpy(arv->parr, palavra);
    arv->esq = arv->dir = NULL;
    arv->cor = RED;
    arv->qtdOcorrencias = 1;
    return arv;
}

//Insere "folhas" à direita e à esquerda
ArvoreRB* direcao_dir(ArvoreRB* dir) {
    ArvoreRB *esq = dir->esq;
    dir->esq = esq->dir;
    esq->dir = dir;
    esq->cor = dir->cor;
    dir->cor = RED;
    return (esq);
}

ArvoreRB* direcao_esq(ArvoreRB* esq) {
    ArvoreRB *dir = esq->dir;
    esq->dir = dir->esq;
    dir->esq = esq;
    dir->cor = esq->cor;
    esq->cor = RED;
    return (dir);
}

//Inverte cores dos nós caso seja necessário de acordo com as regras da red-black
void inverte_cor (ArvoreRB *arv) {
    arv->cor = !(arv->cor);
    arv->esq->cor = !(arv->esq->cor);
    arv->dir->cor = !(arv->dir->cor);
}

ArvoreRB* insere_ArvoreRB (ArvoreRB *arv, char* palavra) {
    if (arv == NULL) return novo_no(palavra);
    if (strcmp(palavra, arv->parr) < 0)
        arv->esq = insere_ArvoreRB(arv->esq, palavra);
    else if (strcmp(palavra, arv->parr) > 0)
        arv->dir = insere_ArvoreRB(arv->dir, palavra);
    else {
        arv->qtdOcorrencias++;
        return arv;
    }

    if (eh_no_vermelho(arv->dir) && !eh_no_vermelho(arv->esq)) {
        arv = direcao_esq(arv);
    }
    if (eh_no_vermelho(arv->esq) && eh_no_vermelho(arv->esq->esq)) {
        arv = direcao_dir(arv);
    } else if (eh_no_vermelho(arv->esq) && eh_no_vermelho(arv->dir)) {
        inverte_cor(arv);
    }
    return arv;
}

//Ordena árvore
void in_order(ArvoreRB *arv){
    if(!arv)
        return;
    in_order(arv->esq);
    printf("%d ", arv->parr);
    in_order(arv->dir);
}

void print(ArvoreRB *arv, int espaco) {
    if (arv == NULL)
        return;
    int i;
    espaco += 10;
    print(arv->dir, espaco);
    for(i=10;i<espaco;i++) printf(" ");
    if(arv->cor) printf(" ");
    printf("%s %d\n", arv->parr, arv->qtdOcorrencias);
    print(arv->esq, espaco);
}

//Seleciona a inserção do arquivo na árvore rb
ArvoreRB* arquivoArvoreRB(ArvoreRB* arv, int* result, char* arq) {
    int tr = 0;
    (*result) = 0;
    char palavra[MAX_PALAVRA];
    *palavra = '\0';
    char armz[1024];
    FILE *file = fopen(arq, "r");
    while (fgets(armz, 1024, file)) {
        for(int i = 0; armz[i] != '\0'; ++i) {
            if (armz[i] >= 'A' && armz[i] <= 'Z') {
                armz[i]+= ' ';
            }
            if (armz[i] >= 'a' && armz[i] <= 'z') {
                tr = 1;
                strncat(palavra, &(armz[i]), 1);
            } else {
                if (tr && strlen(palavra) > 2) {
                    (*result)++;
                    arv = insere_ArvoreRB(arv, palavra);
                    arv->cor = BLACK;
                }
                *palavra= '\0';
                tr = 0;
            }
        }
        if (tr && strlen(palavra) > 2) {
            (*result)++;
            arv = insere_ArvoreRB(arv, palavra);
            arv->cor = BLACK;
        }
        *palavra= '\0';
        tr = 0;
    }
    return arv;
}

//Manipulação das palavras dos arquivos
//Função para obter as palavras mais repetidas no arquivo de texto
void repetePalavra(RepetePalv** lista, ArvoreRB* arv, int num) {
    if (arv == NULL) return;
    repetePalavra(lista, arv->esq, num);

    int i = 0;
    RepetePalv *at = *lista;
    RepetePalv *ant = *lista;
    while (at != NULL && arv->qtdOcorrencias <= at->palavra->qtdOcorrencias) {
        ant = at;
        at = at->p;
        i++;
    }
    if (i < num) {
        RepetePalv *el = (RepetePalv*)malloc(sizeof(RepetePalv));
        el->palavra = arv;
        if (at == *lista) {
            el->p = *lista;
            *lista = el;
        } else {
            el->p = ant->p;
            ant->p = el;
        }
        while (i < num-2 && at != NULL) {
            at = at->p;
            i++;
        }
        if (at != NULL) at->p = NULL;
    }
    repetePalavra(lista, arv->dir, num);
}

RepetePalv** repetePalavras (ArvoreRB* arv, int num) {
    RepetePalv** lista = (RepetePalv**)malloc(sizeof(RepetePalv*));
    RepetePalv* el = (RepetePalv*)malloc(sizeof(RepetePalv));
    el->palavra = arv;
    el->p = NULL;
    *lista = el;
    repetePalavra(lista, arv, num);
    return lista;
}

//Função que abrange a frequência das palavras no arquivo de texto
int freqPalavra (ArvoreRB* arv, char* palav) {
    if (arv == NULL) return 0;
    if (!strcmp(arv->parr, palav)) return arv->qtdOcorrencias;

    int f = freqPalavra(arv->esq, palav);
    if (f) return f;
    f = freqPalavra(arv->dir, palav);
    if (f) return f;
}

int main(int argc, char** argv) {
    //Exibe o número de ocorrência das N palavras que mais aparecem em ARQUIVO, em
    //ordem decrescente de ocorrência.
    //--freq
    if(argc >= 4) {
        if (strcmp(argv[1], "--freq") == 0) {
            ArvoreRB *arv = NULL;
            int t = 0;
            arv = arquivoArvoreRB(arv, &t, argv[3]);

            int num = atoi(argv[2]);
            if (num > 0) {
                RepetePalv** lista = repetePalavras(arv, num);
                printf("Palavras mais frequentes no arquivo: \n");
                RepetePalv *el = *lista;
                for (int i = 1; el != NULL; i++) {
                    printf("%d - %s (%d)\n", i, el->palavra->parr, el->palavra->qtdOcorrencias);
                    el = el->p;
                }
            } else printf("Parâmetros inválidos!");
        }

        //Exibe o número de ocorrências de PALAVRA em ARQUIVO.
        //--freq-word
        if (strcmp(argv[1], "--freq-word") == 0) {
            ArvoreRB* arv = NULL;
            int t = 0;
            arv = arquivoArvoreRB(arv, &t, argv[3]);

            if (strlen(argv[2]) > 2) {
                int f = freqPalavra(arv, argv[2]);
                printf("Quantidade de vezes que a palavra \"%s\" aparece: %d\n", argv[2], f);
            } else printf("Parâmetros inválidos!");
        }

        //Exibe uma listagem dos ARQUIVOS mais relevantes encontrados pela busca por TERMO.
        //--search
        if (strcmp(argv[1], "--search") == 0) {
            int docnum = argc-3;
            ArvoreRB** arvLista = (ArvoreRB**)malloc(sizeof(ArvoreRB*) * docnum);
            int* listat = (int*)malloc(sizeof(int) * docnum);
            //Cálculo de relevância do texto para um termo de busca
            double* tf = (double*)malloc(sizeof(double) * docnum);
            double* tfidf = (double*)malloc(sizeof(double) * docnum);
            int qtdPalavras = 0;
            for (int i = 0; i < docnum; i++) {
                arvLista[i] = NULL;
                arvLista[i] = arquivoArvoreRB(arvLista[i], &(listat[i]), argv[i+3]);
                if (arvLista[i] == NULL) {
                    printf("Parâmetro inválido!");
                    return 0;
                }
                double TFtotal = 0;
                char palavras[MAX_PALAVRA];
                strcpy(palavras, argv[2]);
                char* pl = strtok(palavras, " ");
                int j = 0;
                while (pl != NULL) {
                    int f = freqPalavra(arvLista[i], pl);
                    TFtotal += (f/100.0) / (listat[i]/100.0);
                    pl = strtok(NULL, " ");
                    j++;
                }
                tf[i] = TFtotal/j;
                if(tf[i] > 0) qtdPalavras++;
            }
            double idf = log10((docnum/100.0) / (qtdPalavras/100.0)) + 0.001;
            for (int i = 0; i < docnum; i++) {
                tfidf[i] = tf[i] * idf;
            }
            DocAleatorio** al = (DocAleatorio**)malloc(sizeof(DocAleatorio*));
            DocAleatorio* el = (DocAleatorio*)malloc(sizeof(DocAleatorio));

            el->nome = argv[3];
            el->TFidf = tfidf[0];
            el->pr = NULL;
            *al = el;

            for(int i = 0; i < docnum; i++) {
                DocAleatorio* el = (DocAleatorio*)malloc(sizeof(DocAleatorio));
                el->nome = argv[i+3];
                el->TFidf = tfidf[i];
                DocAleatorio *an, *at = *al;
                while(at != NULL && el->TFidf <= at->TFidf) {
                    an = at;
                    at = at->pr;
                }
                if (at == (*al)) {
                    el->pr = *al;
                    *al = el;
                } else {
                    el->pr = an->pr;
                    an->pr = el;
                }
            }
            //Apresenta uma listagem dos documentos mais relevantes para um dado termo de busca através do tfidf
            int i = 1;
            DocAleatorio* at = *al;
            printf("Lista dos documentos mais relevantes para o termo \"%s\": \n", argv[2]);
            while (at != NULL) {
                printf("%d  %s\n", i, at->nome);
                at = at->pr;
                i++;
            }
        }
    } else printf("Parâmetros inválidos!");
}
