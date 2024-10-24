#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> //essa aqui é pro delay, caso o usuário escreva uma opção inválida e pra ficar mais dinâmico

typedef struct { //AQUI ARMAZENA as estruturas do jogo, a dificuldade em relação as minas e o tamanho..
    int tamanho; // o tamanho do campo minado
    int minas; // quantidade de minas no campo minado
} Dificuldade;

Dificuldade escolherDificuldade() { // pro usuário escolher o Nível que ele quer jogar
    Dificuldade dificuldade;
    int opcao;
    int primeiraTentativa = 1;

    while (1) { // coloquei um while pro usuário tentar novamente caso ele coloque um número inválido
         if (primeiraTentativa) {
            printf("bem-vindo ao campo minado do Samuel :)\n");
            sleep(2);
            primeiraTentativa = 0; // IF pra aparecer mensagem de boas vindas só uma vez, caso o usuário erre 
        }
        
        printf("Escolha seu nível de jogo:\n");
        sleep(1); //delayzinho de 1 segundo pra ficar mais dinâmico
        printf("1. Fácil\n");
        sleep(1);
        printf("2. Médio\n");
        sleep(1);
        printf("3. Difícil\n");
        sleep(1);
        scanf("%d", &opcao);

        switch (opcao) { // aqui o usuário DECIDE o nível do jogo, tamanho das minas e do campo em relação a dificuldade
            case 1:
                dificuldade.tamanho = 10;
                dificuldade.minas = 3;
                return dificuldade; 
            case 2:
                dificuldade.tamanho = 20;
                dificuldade.minas = 6;
                return dificuldade;
            case 3:
                dificuldade.tamanho = 30;
                dificuldade.minas = 9;
                return dificuldade;
            default:
                printf("\nOps, digite apenas 1, 2 ou 3.\n");
                sleep(3); // os segundos de delay, até aparecer as altermativas de novo
                break;
        }
    }
}

int **alocMatriz(int tamanho) { // FUNÇÂO pra ALOCAR a memória da Matriz
    int **matriz = malloc(tamanho * sizeof(int *));  // Aqui alocando um espaço na memória para 'tamanho' ponteiros. E esses ponteiros vão representar cada linha da matriz
    for (int i = 0; i < tamanho; i++) { // Agora, o cod percorre cada linha e aloca espaço para as colunas. E pra cada linha, aloca um 'tamanho' inteiro.
        matriz[i] = malloc(tamanho * sizeof(int)); // Isso é como dizer "aqui estão os espaços para cada coluna nessa linha"
    }
    return matriz;
}

void liberarMatriz(int **matriz, int tamanho) { // FUNÇÃO para LIBERAR a memória alocada!
    for (int i = 0; i < tamanho; i++) { // Percorre cada linha da matriz
        free(matriz[i]); //  Libera a memória da linha i. Aqui é onde dizemos "tchau" para essa linha!
    }
    free(matriz); // Agora liberamos a memória da matriz em si. É como dizer "valeu por tudo, matriz!".
}

// Função para inicializar o campo minado
void inicializarCampo(int tamanho, int **campo, int minas) {
    // Inicializa todas as células do campo como 0
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++) {
            campo[i][j] = 0;
        }
    }

    // Coloca as minas aleatoriamente no campo
    srand(time(NULL)); // Semente para geração de números aleatórios
    for (int i = 0; i < minas; i++) {
        int x, y;
        // Gera coordenadas até encontrar uma célula vazia
        do {
            x = rand() % tamanho; // linha aleatória
            y = rand() % tamanho; // coluna aleatória
        } while (campo[x][y] == -1); // Garante que a célula não tenha mina
        campo[x][y] = -1; // Coloca a mina na posição aleatória
    }

    // Atualiza a contagem de minas vizinhas para cada célula
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++) {
            if (campo[i][j] == -1) continue; // Ignora células com minas
            int contador = 0; // Contador de minas vizinhas
            // Verifica as 8 células vizinhas
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    if (di == 0 && dj == 0) continue; // Ignora a própria célula
                    int ni = i + di; // nova linha
                    int nj = j + dj; // nova colun
                    // Verifica se a nova posição está dentro dos limites do campo
                    if (ni >= 0 && ni < tamanho && nj >= 0 && nj < tamanho && campo[ni][nj] == -1) {
                        contador++; // Incrementa o contador se houver uma mina
                    }
                }
            }
            campo[i][j] = contador; // aqui aualiza a célula com a contagem de minas vizinhas
        }
    }
}

// Função para mostrar o campo minado pro usuário
void mostrarCampo(int tamanho, int **campo, int **revelado) {
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++) {
            // Exibe o número de minas vizinhas ou 'x' se a célula não foi revelada
            if (revelado[i][j]) {
                printf("%d ", campo[i][j]);
            } else {
                printf("X "); // 'x' para células não reveladas
            }
        }
        printf("\n"); // Nova linha após cada linha do campo
    }
}

// Função para mostrar o campo completo sem 'x'
void mostrarCampoCompleto(int tamanho, int **campo) {
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++) {
            if (campo[i][j] == -1) {
                printf("-1 "); // Exibe -1 para minas
            } else {
                printf("%d ", campo[i][j]); // Exibe contagem de minas vizinhas
            }
        }
        printf("\n"); // Nova linha após cada linha do campo
    }
}

void solicitarCoordenadas(int tamanho, int **campo, int **revelado, int *descobertas, int totalCelulas) {
    int x, y;
    printf("Digite as coordenadas: ");
    scanf("%d,%d", &x, &y);

    // Converte para índice de 0
    x--; 
    y--;

    // verifica se as coordenadas estão dentro dos limites do campo
    if (x >= 0 && x < tamanho && y >= 0 && y < tamanho) {
        if (campo[x][y] == -1) { // Se o usuário descobrir uma mina
            printf("game Over\n");
            mostrarCampoCompleto(tamanho, campo); // Mostra a matriz completa
            exit(0); // Encerra o programa
        } else {
            revelado[x][y] = 1; // Marca a célula como revelada
            (*descobertas)++; // Incrementa o contador de células descobertas

            // Verifica se todas as células sem minas foram descobertas
            if (*descobertas == totalCelulas) {
                printf("Parabéns, você é fera!\n");
                mostrarCampoCompleto(tamanho, campo); // mostra a matriz completa
                exit(0); // Encerra o programa
            } else {
                mostrarCampo(tamanho, campo, revelado); // atualiza a exibição do campo
            }
        }
    } else {
        printf("Ops, coordenadas inválidas!\n"); // mensagem de erro caso a coordenada seja inválida ou seja inexstitente
    }
}

int main() {
    // aqui chama a função para escolher a dificuldade do jogo. O usuário vai escolher se quer jogar fácil, médio ou difícil
    Dificuldade dificuldade = escolherDificuldade(); 

    // aqui ta alocando espaço na memória para o campo do jogo e para as células que vão ser mostradas ao usuário
    int **campo = alocMatriz(dificuldade.tamanho);
    int **revelado = alocMatriz(dificuldade.tamanho);
    
    // Aaqui vai preparar a matriz "revelado", todas as células estão como não reveladas (0).
    for (int i = 0; i < dificuldade.tamanho; i++) {
        for (int j = 0; j < dificuldade.tamanho; j++) {
            revelado[i][j] = 0; // aqui diz que todas as células estão escondidas, tipo "não olha aqui!"
        }
    }

    // a função que vai inicializar o campo minado, colocando as minas e contando as vizinhas
    inicializarCampo(dificuldade.tamanho, campo, dificuldade.minas);
    
    // calcula quantas células o jogado precisa descobrir para ganhar. É o total de células menos as minas
    int totalCelulas = (dificuldade.tamanho * dificuldade.tamanho) - dificuldade.minas; 
    int descobertas = 0; // ainda ninguém descobriu nada, então começamos com zero.

    // aqui mostra o campo inicial para o jogador. Aqui ele vê tudo escondido, sem saber onde estão as minas
    mostrarCampo(dificuldade.tamanho, campo, revelado);
    
    // Este é o loop principal do jogo. Enquanto o jogador não descobrir todas as células seguras, o jogo continua
    while (descobertas < totalCelulas) {
        // aquiu pede pro jogador escolher as coordenadas que ele quer revelar. 
        solicitarCoordenadas(dificuldade.tamanho, campo, revelado, &descobertas, totalCelulas);
    }

    // Quando o jogador descobre todas as células sem minas ou acaba perdendo, precisa liberar a memória que foi alocada
    liberarMatriz(campo, dificuldade.tamanho);
    liberarMatriz(revelado, dificuldade.tamanho);

    return 0; // the end
}