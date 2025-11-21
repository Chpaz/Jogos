#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#ifdef _WIN32
    #include <process.h>
    #define getpid _getpid
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
#endif

// Jogo 2048 - Definições e estruturas
#define grade 4
#define alvo 2048

typedef struct {
    int grid[grade][grade];
    int score;
    int moved;
} Game;

// Jogo 2048 - Inicialização do jogo
void initGame(Game *game) {
    for (int i = 0; i < grade; i++) {
        for (int j = 0; j < grade; j++) {
            game->grid[i][j] = 0;
        }
    }
    game->score = 0;
    game->moved = 0;
}

// Jogo 2048 - Exibição da grade
void printGrid(Game *game) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    printf("\n=== JOGO 2048 ===\n");
    printf("Pontuação: %d\n\n", game->score);
    printf("┌──────┬──────┬──────┬──────┐\n");

    for (int i = 0; i < grade; i++) {
        printf("│");
        for (int j = 0; j < grade; j++) {
            if (game->grid[i][j] == 0) {
                printf("      │");
            } else {
                printf(" %4d │", game->grid[i][j]);
            }
        }
        printf("\n");
        if (i < grade - 1) {
            printf("├──────┼──────┼──────┼──────┤\n");
        }
    }
    printf("└──────┴──────┴──────┴──────┘\n");
    printf("\nControles: W(↑) A(←) S(↓) D(→) Q(sair)\n");
}

// Jogo 2048 - Adiciona peça aleatória
void addRandomTile(Game *game) {
    int emptyCells[grade * grade][2];
    int emptyCount = 0;

    for (int i = 0; i < grade; i++) {
        for (int j = 0; j < grade; j++) {
            if (game->grid[i][j] == 0) {
                emptyCells[emptyCount][0] = i;
                emptyCells[emptyCount][1] = j;
                emptyCount++;
            }
        }
    }

    if (emptyCount > 0) {
        int randomIndex = rand() % emptyCount;
        int row = emptyCells[randomIndex][0];
        int col = emptyCells[randomIndex][1];
        game->grid[row][col] = (rand() % 10 < 9) ? 2 : 4;
    }
}

// Jogo 2048 - Deslizar peças para esquerda
void slideLeft(int row[grade], int *score) {
    int temp[grade] = {0};
    int pos = 0;

    for (int i = 0; i < grade; i++) {
        if (row[i] != 0) {
            temp[pos++] = row[i];
        }
    }

    for (int i = 0; i < grade - 1; i++) {
        if (temp[i] != 0 && temp[i] == temp[i + 1]) {
            temp[i] *= 2;
            *score += temp[i];
            temp[i + 1] = 0;
        }
    }

    for (int i = 0; i < grade; i++) {
        row[i] = 0;
    }
    pos = 0;
    for (int i = 0; i < grade; i++) {
        if (temp[i] != 0) {
            row[pos++] = temp[i];
        }
    }
}

// Jogo 2048 - Inverter linha
void reverseRow(int row[grade]) {
    for (int i = 0; i < grade / 2; i++) {
        int temp = row[i];
        row[i] = row[grade - 1 - i];
        row[grade - 1 - i] = temp;
    }
}

// Jogo 2048 - Transpor matriz
void transpose(Game *game) {
    for (int i = 0; i < grade; i++) {
        for (int j = i + 1; j < grade; j++) {
            int temp = game->grid[i][j];
            game->grid[i][j] = game->grid[j][i];
            game->grid[j][i] = temp;
        }
    }
}

// Jogo 2048 - Verificar se houve mudança
int hasChanged(Game *oldGame, Game *newGame) {
    for (int i = 0; i < grade; i++) {
        for (int j = 0; j < grade; j++) {
            if (oldGame->grid[i][j] != newGame->grid[i][j]) {
                return 1;
            }
        }
    }
    return 0;
}

// Jogo 2048 - Copiar jogo
void copyGame(Game *dest, Game *src) {
    for (int i = 0; i < grade; i++) {
        for (int j = 0; j < grade; j++) {
            dest->grid[i][j] = src->grid[i][j];
        }
    }
    dest->score = src->score;
}

// Jogo 2048 - Movimento para esquerda
void moveLeft(Game *game) {
    Game oldGame;
    copyGame(&oldGame, game);

    for (int i = 0; i < grade; i++) {
        slideLeft(game->grid[i], &game->score);
    }

    game->moved = hasChanged(&oldGame, game);
}

// Jogo 2048 - Movimento para direita
void moveRight(Game *game) {
    Game oldGame;
    copyGame(&oldGame, game);

    for (int i = 0; i < grade; i++) {
        reverseRow(game->grid[i]);
        slideLeft(game->grid[i], &game->score);
        reverseRow(game->grid[i]);
    }

    game->moved = hasChanged(&oldGame, game);
}

// Jogo 2048 - Movimento para cima
void moveUp(Game *game) {
    Game oldGame;
    copyGame(&oldGame, game);

    transpose(game);
    for (int i = 0; i < grade; i++) {
        slideLeft(game->grid[i], &game->score);
    }
    transpose(game);

    game->moved = hasChanged(&oldGame, game);
}

// Jogo 2048 - Movimento para baixo
void moveDown(Game *game) {
    Game oldGame;
    copyGame(&oldGame, game);

    transpose(game);
    for (int i = 0; i < grade; i++) {
        reverseRow(game->grid[i]);
        slideLeft(game->grid[i], &game->score);
        reverseRow(game->grid[i]);
    }
    transpose(game);

    game->moved = hasChanged(&oldGame, game);
}

// Jogo 2048 - Verificar vitória
int hasWon(Game *game) {
    for (int i = 0; i < grade; i++) {
        for (int j = 0; j < grade; j++) {
            if (game->grid[i][j] == alvo) {
                return 1;
            }
        }
    }
    return 0;
}

// Jogo 2048 - Verificar se ainda pode mover
int canMove(Game *game) {
    for (int i = 0; i < grade; i++) {
        for (int j = 0; j < grade; j++) {
            if (game->grid[i][j] == 0) {
                return 1;
            }
        }
    }

    for (int i = 0; i < grade; i++) {
        for (int j = 0; j < grade; j++) {
            if (i < grade - 1 && game->grid[i][j] == game->grid[i + 1][j]) {
                return 1;
            }
            if (j < grade - 1 && game->grid[i][j] == game->grid[i][j + 1]) {
                return 1;
            }
        }
    }

    return 0;
}

// Jogo 2048 - Salvar pontuação
void saveScore(Game *game, char *result) {
    if (!result || !game) return;

    FILE *file = fopen("placar.txt", "w");
    if (file) {
        fprintf(file, "Resultado: %.50s\n", result);
        fprintf(file, "Pontuação Final: %d\n", game->score);
        fclose(file);
    }
}

// Jogo 2048 - Função principal
int executar_2048() {
    srand((unsigned int)(time(NULL) ^ getpid()));
    Game game;
    char input;
    int gameRunning = 1;
    int wonGame = 0;

    initGame(&game);
    addRandomTile(&game);
    addRandomTile(&game);

    printf("Bem-vindo ao 2048!\n");
    printf("Combine os números para chegar ao 2048!\n");
    printf("Pressione ENTER para começar...");
    getchar();

    while (gameRunning) {
        printGrid(&game);

        if (hasWon(&game) && !wonGame) {
            printf("\n🎉 PARABÉNS! VOCÊ CHEGOU AO 2048! 🎉\n");
            printf("Você pode continuar jogando ou pressionar Q para sair.\n");
            wonGame = 1;
            saveScore(&game, "Vitória");
        }

        if (!canMove(&game)) {
            printf("\n💀 GAME OVER! Não há mais movimentos possíveis!\n");
            printf("Pontuação final: %d\n", game.score);
            saveScore(&game, "Derrota");
            printf("Pressione ENTER para voltar ao menu...");
            getchar();
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            break;
        }

        printf("Seu movimento: ");
        scanf(" %c", &input);
        input = tolower(input);

        game.moved = 0;

        switch (input) {
            case 'w':
                moveUp(&game);
                break;
            case 'a':
                moveLeft(&game);
                break;
            case 's':
                moveDown(&game);
                break;
            case 'd':
                moveRight(&game);
                break;
            case 'q':
                printf("Voltando ao menu principal...\n");
                printf("Pontuação final: %d\n", game.score);
                saveScore(&game, wonGame ? "Vitória" : "Desistência");
                printf("Pressione ENTER para voltar ao menu...");
                getchar();
                #ifdef _WIN32
                    system("cls");
                #else
                    system("clear");
                #endif
                gameRunning = 0;
                break;
            default:
                printf("Movimento inválido! Use W/A/S/D ou Q para sair.\n");
                continue;
        }

        if (game.moved && gameRunning) {
            addRandomTile(&game);
        }
    }

    printf("Obrigado por jogar!\n");
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Jogo da Forca - Definições e variáveis globais
#define MAX_TAM 20

int vitorias = 0;
int derrotas = 0;

struct FORCA {
    char palavra[MAX_TAM];
    char categoria[MAX_TAM];
    char dica[MAX_TAM];
};

char vetaux[MAX_TAM];
char letrasTentadas[26];
int letrasTentadasCount;
int acertos, erros, tentativasRestantes;

// Jogo da Forca - Desenhar boneco
void desenharForca(int erros) {
    printf("_________\n");
    printf("|       |\n");
    printf("|       %c\n", erros >= 1 ? 'O' : ' ');
    printf("|      %c%c%c\n", erros >= 3 ? '/' : ' ', erros >= 2 ? '|' : ' ', erros >= 4 ? '\\' : ' ');
    printf("|      %c %c\n", erros >= 5 ? '/' : ' ', erros >= 6 ? '\\' : ' ');
    printf("|\n");
    printf("=========\n");
}

// Jogo da Forca - Inicializar vetor auxiliar
void setVetAux(char palavra[]) {
    int len = strlen(palavra);
    for (int i = 0; i < len; i++) {
        vetaux[i] = (palavra[i] == ' ') ? ' ' : '_';
    }
    vetaux[len] = '\0';
}

// Jogo da Forca - Mostrar progresso da palavra
void mostrarProgresso(char palavra[]) {
    printf("Palavra: ");
    for (int i = 0; i < strlen(palavra); i++) {
        printf("%c ", vetaux[i]);
    }
    printf("\n");
}

// Jogo da Forca - Mostrar letras tentadas
void mostrarLetrasTentadas() {
    printf("Letras tentadas: ");
    for (int i = 0; i < letrasTentadasCount; i++) {
        printf("%c ", letrasTentadas[i]);
    }
    printf("\n");
}

// Jogo da Forca - Verificar se letra já foi usada
int letraJaUsada(char letra) {
    for (int i = 0; i < letrasTentadasCount; i++) {
        if (letrasTentadas[i] == letra)
            return 1;
    }
    return 0;
}

// Jogo da Forca - Limpar tela
void limparTela() {
    system("clear || cls");
}

// Jogo da Forca - Função principal do jogo
int jogar() {
    struct FORCA jogo;
    char letra;
    acertos = erros = letrasTentadasCount = 0;
    tentativasRestantes = 6;

    printf("Digite a palavra secreta: ");
    fgets(jogo.palavra, MAX_TAM, stdin);
    jogo.palavra[strcspn(jogo.palavra, "\n")] = 0;

    printf("Digite a categoria: ");
    fgets(jogo.categoria, MAX_TAM, stdin);
    jogo.categoria[strcspn(jogo.categoria, "\n")] = 0;

    printf("Digite a dica: ");
    fgets(jogo.dica, MAX_TAM, stdin);
    jogo.dica[strcspn(jogo.dica, "\n")] = 0;

    limparTela();
    printf("---- Começando o jogo ----\n");

    setVetAux(jogo.palavra);
    int tamanho = strlen(jogo.palavra);

    while (tentativasRestantes > 0 && acertos < tamanho) {
        printf("\nCategoria: %s\n", jogo.categoria);
        printf("Dica: %s\n", jogo.dica);
        desenharForca(erros);
        mostrarProgresso(jogo.palavra);
        mostrarLetrasTentadas();

        printf("Digite uma letra: ");
        scanf(" %c", &letra);
        getchar();
        letra = tolower(letra);

        if (!isalpha(letra)) {
            printf("Digite apenas letras!\n");
            continue;
        }

        if (letraJaUsada(letra)) {
            printf("Você já tentou essa letra!\n");
            continue;
        }

        letrasTentadas[letrasTentadasCount++] = letra;

        int acertou = 0;
        for (int i = 0; i < tamanho; i++) {
            if (tolower(jogo.palavra[i]) == letra && vetaux[i] == '_') {
                vetaux[i] = jogo.palavra[i];
                acertos++;
                acertou = 1;
            }
        }

        if (!acertou) {
            erros++;
            tentativasRestantes--;
            printf("Errou! Tentativas restantes: %d\n", tentativasRestantes);
        } else {
            printf("Boa! Você acertou uma letra.\n");
        }

        limparTela();
    }

    desenharForca(erros);

    if (acertos == tamanho) {
        printf("Parabéns! Você venceu. A palavra era: %s\n", jogo.palavra);
        vitorias++;
    } else {
        printf("Você perdeu! A palavra era: %s\n", jogo.palavra);
        derrotas++;
    }

    printf("\n[1] Voltar ao menu\n[2] Voltar ao menu principal\nEscolha: ");
    int escolha;
    scanf("%d", &escolha);
    getchar();

    if (escolha == 2) {
        printf("Voltando ao menu principal...\n");
        limparTela();
        return 2; // Retorna 2 para indicar volta ao menu principal
    } else {
        limparTela();
        return 1; // Retorna 1 para continuar no menu da forca
    }
}

// Jogo da Forca - Menu principal
int menu() {
    int opcao;
    while (1) {
        limparTela();
        printf("JOGO DA FORCA\n");
        printf("Vitórias: %d | Derrotas: %d\n\n", vitorias, derrotas);
        printf("1 - Jogar\n2 - Instruções\n3 - Voltar ao menu principal\nSua escolha: ");
        scanf("%d", &opcao);
        getchar();

        limparTela();

        switch (opcao) {
            case 1: {
                int resultado = jogar();
                if (resultado == 2) {
                    printf("Voltando ao menu principal...\n");
                    return 0; // Retorna ao menu principal
                }
                // Se resultado == 1, continua no loop do menu da forca
                break;
            }
            case 2:
                printf("Instruções:\nVocê deve adivinhar a palavra secreta.\nPode errar até 6 vezes.\nBoa sorte!\n\n");
                printf("Pressione ENTER para continuar...");
                getchar();
                break;
            case 3:
                printf("Voltando ao menu principal...\n");
                return 0;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }


    }
}

// Jogo da Forca - Função principal
int executar_forca() {
    menu();
    return 0;
}

// Campo Minado - Declaração das funções
int executar_campo_minado();
void calcular_minas_vizinhas();

// Menu principal - Escolha de jogo
int main() {
    int escolha;

    while (1) {
        printf("=== ESCOLHA SEU JOGO ===\n");
        printf("1 - Jogo 2048\n");
        printf("2 - Jogo da Forca\n");
        printf("3 - Campo Minado\n");
        printf("4 - Jogo Snake\n");
        printf("9 - Créditos\n");
        printf("0 - Sair\n");
        printf("Digite sua escolha: ");
        scanf("%d", &escolha);
        getchar();

        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        switch (escolha) {
            case 1:
                executar_2048();
                #ifdef _WIN32
                    system("cls");
                #else
                    system("clear");
                #endif
                break;
            case 2:
                executar_forca();
                #ifdef _WIN32
                    system("cls");
                #else
                    system("clear");
                #endif
                break;
            case 3:
                executar_campo_minado();
                #ifdef _WIN32
                    system("cls");
                #else
                    system("clear");
                #endif
                break;
            case 4:
                executar_snake();
                #ifdef _WIN32
                    system("cls");
                #else
                    system("clear");
                #endif
                break;
            case 9:
                printf("Créditos:\nCriado por Christian Paz\nAlguns jogos teste que eu criei no meu tempo livre. Espero que gostem :)\nContato: chrishickmannpaz@gmail.com (por coincidência também é meu pix)\n\n");
                printf("Pressione ENTER para continuar...");
                getchar();
                #ifdef _WIN32
                    system("cls");
                #else
                    system("clear");
                #endif
                break;
            case 0:
                printf("Saindo...\n");
                return 0;
            default:
                printf("Opção inválida!\n");
                printf("Pressione ENTER para continuar...");
                getchar();
                #ifdef _WIN32
                    system("cls");
                #else
                    system("clear");
                #endif
                break;
        }
    }

    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LINHAS 10
#define COLUNAS 10
#define MINAS 15

// Campo Minado - Estrutura da célula
typedef struct {
    bool tem_mina;
    bool revelado;
    bool marcado;
    int minas_vizinhas;
} Celula;

// Campo Minado - Variáveis globais
Celula campo[LINHAS][COLUNAS];
int minas_restantes;
bool bombas_geradas = false;  // Flag para controlar se as bombas já foram geradas

// Campo Minado - Calcular distância entre duas posições
int calcular_distancia(int linha1, int coluna1, int linha2, int coluna2) {
    int dx = abs(linha1 - linha2);
    int dy = abs(coluna1 - coluna2);
    return (dx > dy) ? dx : dy;  // Distância de Chebyshev (máximo entre dx e dy)
}

// Campo Minado - Verificar se posição está na zona segura
bool esta_na_zona_segura(int linha, int coluna, int linha_inicial, int coluna_inicial) {
    int distancia = calcular_distancia(linha, coluna, linha_inicial, coluna_inicial);
    return distancia < 2;  // Zona segura de 1 casa ao redor da posição inicial
}

// Campo Minado - Gerar bombas após primeira jogada
void gerar_bombas(int linha_inicial, int coluna_inicial) {
    if (bombas_geradas) return;  // Se já foram geradas, não gerar novamente

    srand(time(NULL));
    int minas_colocadas = 0;
    int tentativas = 0;
    const int MAX_TENTATIVAS = 1000;  // Evitar loop infinito

    printf("\n🎯 Gerando campo minado seguro...\n");

    while (minas_colocadas < MINAS && tentativas < MAX_TENTATIVAS) {
        int linha = rand() % LINHAS;
        int coluna = rand() % COLUNAS;
        tentativas++;

        // Verificar se a posição é válida para colocar uma bomba
        if (!campo[linha][coluna].tem_mina && 
            !esta_na_zona_segura(linha, coluna, linha_inicial, coluna_inicial)) {

            int distancia = calcular_distancia(linha, coluna, linha_inicial, coluna_inicial);

            // Colocar bombas a uma distância de 2-5 casas da posição inicial
            if (distancia >= 2 && distancia <= 5) {
                campo[linha][coluna].tem_mina = true;
                minas_colocadas++;
            }
        }
    }

    // Se não conseguiu colocar todas as bombas na faixa ideal, colocar as restantes em qualquer lugar seguro
    while (minas_colocadas < MINAS && tentativas < MAX_TENTATIVAS * 2) {
        int linha = rand() % LINHAS;
        int coluna = rand() % COLUNAS;
        tentativas++;

        if (!campo[linha][coluna].tem_mina && 
            !esta_na_zona_segura(linha, coluna, linha_inicial, coluna_inicial)) {
            campo[linha][coluna].tem_mina = true;
            minas_colocadas++;
        }
    }

    printf("✅ %d bombas colocadas com segurança!\n", minas_colocadas);
    bombas_geradas = true;

    // Calcular minas vizinhas após gerar as bombas
    calcular_minas_vizinhas();
}

// Campo Minado - Calcular minas vizinhas
void calcular_minas_vizinhas() {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (!campo[i][j].tem_mina) {
                int contador = 0;

                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        int ni = i + di;
                        int nj = j + dj;

                        if (ni >= 0 && ni < LINHAS && nj >= 0 && nj < COLUNAS) {
                            if (campo[ni][nj].tem_mina) {
                                contador++;
                            }
                        }
                    }
                }

                campo[i][j].minas_vizinhas = contador;
            }
        }
    }
}

// Campo Minado - Inicializar campo (sem gerar bombas)
void inicializar_campo() {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            campo[i][j].tem_mina = false;
            campo[i][j].revelado = false;
            campo[i][j].marcado = false;
            campo[i][j].minas_vizinhas = 0;
        }
    }

    minas_restantes = MINAS;
    bombas_geradas = false;  // Reset da flag
}

// Campo Minado - Exibir o campo de jogo
void mostrar_campo(bool mostrar_minas) {
    printf("\n   ");
    for (int j = 0; j < COLUNAS; j++) {
        printf("%2d ", j);
    }
    printf("\n");

    for (int i = 0; i < LINHAS; i++) {
        printf("%2d ", i);

        for (int j = 0; j < COLUNAS; j++) {
            if (campo[i][j].marcado && !campo[i][j].revelado && !mostrar_minas) {
                printf(" M ");
            } else if (!campo[i][j].revelado && !mostrar_minas) {
                printf(" . ");
            } else if (campo[i][j].tem_mina) {
                if (mostrar_minas) {
                    // Se é a bomba que explodiu (revelada), mostra explosão
                    if (campo[i][j].revelado) {
                        printf(" 💥 ");
                    } else {
                        // Outras bombas não reveladas mostram emoji de bomba
                        printf(" 💣 ");
                    }
                } else {
                    printf(" . ");
                }
            } else if (!campo[i][j].revelado && mostrar_minas) {
                printf(" . ");
            } else if (campo[i][j].minas_vizinhas == 0) {
                printf("   ");
            } else {
                printf(" %d ", campo[i][j].minas_vizinhas);
            }
        }
        printf("\n");
    }

    printf("\nMinas restantes: %d\n", minas_restantes);
}

// Campo Minado - Revelar área recursivamente
void revelar_area(int linha, int coluna) {
    if (linha < 0 || linha >= LINHAS || coluna < 0 || coluna >= COLUNAS) {
        return;
    }

    if (campo[linha][coluna].revelado || campo[linha][coluna].marcado) {
        return;
    }

    campo[linha][coluna].revelado = true;

    if (campo[linha][coluna].minas_vizinhas == 0 && !campo[linha][coluna].tem_mina) {
        for (int di = -1; di <= 1; di++) {
            for (int dj = -1; dj <= 1; dj++) {
                revelar_area(linha + di, coluna + dj);
            }
        }
    }
}

// Campo Minado - Verificar condição de vitória
bool verificar_vitoria() {
    int celulas_restantes = 0;

    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            if (!campo[i][j].revelado && !campo[i][j].tem_mina) {
                celulas_restantes++;
            }
        }
    }

    return celulas_restantes == 0;
}

// Campo Minado - Marcar/desmarcar célula com bandeira
void marcar_celula(int linha, int coluna) {
    if (linha < 0 || linha >= LINHAS || coluna < 0 || coluna >= COLUNAS) {
        return;
    }

    if (campo[linha][coluna].revelado) {
        return;
    }

    if (campo[linha][coluna].marcado) {
        campo[linha][coluna].marcado = false;
        minas_restantes++;
    } else {
        campo[linha][coluna].marcado = true;
        minas_restantes--;
    }
}

// Campo Minado - Limpar tela
void limpar_console() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Campo Minado - Exibir instruções do jogo
void mostrar_instrucoes() {
    printf("=== CAMPO MINADO INTELIGENTE ===\n");
    printf("Campo: %dx%d | Minas: %d\n\n", LINHAS, COLUNAS, MINAS);
    printf("COMO JOGAR:\n");
    printf("• Digite [linha] [coluna] para revelar uma célula\n");
    printf("• Digite 'm' [linha] [coluna] para marcar/desmarcar\n");
    printf("• Digite 'q' para voltar ao menu principal\n\n");
    printf("SÍMBOLOS:\n");
    printf("• '.' = célula não revelada\n");
    printf("• 'M' = célula marcada (mina)\n");
    printf("• '💥' = mina explodida (game over)\n");
    printf("• Números = quantidade de minas vizinhas\n\n");
}

// Campo Minado - Função principal executar
int executar_campo_minado() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    printf("=== BEM-VINDO AO CAMPO MINADO ===\n");
    printf("Objetivo: Encontre todas as células sem minas!\n");
    printf("Pressione ENTER para começar...");
    getchar();

    int resultado = jogar_campo_minado();

    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    return resultado;
}

// Campo Minado - Função principal do jogo
int jogar_campo_minado() {
    char comando;
    int linha, coluna;
    bool jogo_ativo = true;
    bool primeira_rodada = true;

    inicializar_campo();

    while (jogo_ativo) {
        if (primeira_rodada) {
            mostrar_instrucoes();
            primeira_rodada = false;
        } else {
            limpar_console();
        }

        mostrar_campo(false);

        printf("\nComando (linha coluna / m linha coluna / q para sair): ");

        if (scanf(" %c", &comando) != 1) {
            printf("Comando inválido!\n");
            continue;
        }

        if (comando == 'q' || comando == 'Q') {
            printf("Voltando ao menu principal...\n");
            break;
        }

        if (comando == 'm' || comando == 'M') {
            if (scanf("%d %d", &linha, &coluna) != 2) {
                printf("Coordenadas inválidas!\n");
                continue;
            }

            if (linha < 0 || linha >= LINHAS || coluna < 0 || coluna >= COLUNAS) {
                printf("Coordenadas fora do campo!\n");
                continue;
            }

            marcar_celula(linha, coluna);
        } else {
            // Tentar ler como números (assumindo que o primeiro caractere é o dígito da linha)
            ungetc(comando, stdin);  // Voltar o caractere para o buffer

            if (scanf("%d %d", &linha, &coluna) != 2) {
                printf("Coordenadas inválidas!\n");
                continue;
            }

            if (linha < 0 || linha >= LINHAS || coluna < 0 || coluna >= COLUNAS) {
                printf("Coordenadas fora do campo!\n");
                continue;
            }

            if (campo[linha][coluna].marcado) {
                printf("Célula marcada! Use 'm %d %d' para desmarcar primeiro.\n", linha, coluna);
                continue;
            }

            if (campo[linha][coluna].revelado) {
                printf("Célula já revelada!\n");
                continue;
            }

            // Se é a primeira jogada, gerar bombas com zona segura
            if (!bombas_geradas) {
                gerar_bombas(linha, coluna);
                printf("Primeira jogada segura garantida! 🛡️\n");
                printf("Pressione ENTER para continuar...");
                getchar();
            }

            if (campo[linha][coluna].tem_mina) {
                // Game Over (isso nunca deve acontecer na primeira jogada)
                campo[linha][coluna].revelado = true;
                limpar_console();
                mostrar_campo(true);
                printf("\n💥 BOOM! Você pisou em uma mina! 💥\n");
                printf("GAME OVER! Mais sorte na próxima vez!\n\n");

                int escolha_game_over;
                while (1) {
                    printf("1 - Jogar novamente\n");
                    printf("2 - Voltar ao menu principal\n");
                    printf("Digite sua escolha: ");
                    scanf("%d", &escolha_game_over);
                    getchar();

                    if (escolha_game_over == 1) {
                        // Reiniciar o jogo
                        inicializar_campo();
                        primeira_rodada = true;
                        break;
                    } else if (escolha_game_over == 2) {
                        jogo_ativo = false;
                        break;
                    } else {
                        printf("Opção inválida! Tente novamente.\n\n");
                    }
                }
            } else {
                revelar_area(linha, coluna);

                if (verificar_vitoria()) {
                    limpar_console();
                    mostrar_campo(false);
                    printf("\n🎉 PARABÉNS! VOCÊ VENCEU! 🎉\n");
                    printf("Todas as células seguras foram reveladas!\n");
                    printf("Excelsior!\n\n");

                    int escolha_vitoria;
                    while (1) {
                        printf("1 - Jogar novamente\n");
                        printf("2 - Voltar ao menu principal\n");
                        printf("Digite sua escolha: ");
                        scanf("%d", &escolha_vitoria);
                        getchar();

                        if (escolha_vitoria == 1) {
                            // Reiniciar o jogo
                            inicializar_campo();
                            primeira_rodada = true;
                            break;
                        } else if (escolha_vitoria == 2) {
                            jogo_ativo = false;
                            break;
                        } else {
                            printf("Opção inválida! Tente novamente.\n\n");
                        }
                    }
                }
            }
        }
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Snake - Definições e estruturas
#define SNAKE_WIDTH 40
#define SNAKE_HEIGHT 20
#define MAX_SNAKE_LENGTH 800

typedef struct {
    int x, y;
} SnakePoint;

typedef struct {
    SnakePoint body[MAX_SNAKE_LENGTH];
    int length;
    int direction; // 0=cima, 1=direita, 2=baixo, 3=esquerda
} SnakeGame;

typedef struct {
    SnakePoint position;
} SnakeFood;

SnakeGame snake_game;
SnakeFood snake_food;
int snake_score = 0;
int snake_gameOver = 0; // 0=jogando, 1=morreu, 2=desistiu

// Snake - Configurar terminal para entrada não-bloqueante
void setupSnakeTerminal() {
    #ifndef _WIN32
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 0;  // Não esperar por caracteres
    term.c_cc[VTIME] = 0; // Não esperar tempo
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    #endif
}

// Snake - Restaurar configurações do terminal
void restoreSnakeTerminal() {
    #ifndef _WIN32
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    #endif
}

// Snake - Limpar tela
void clearSnakeScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Snake - Inicializar jogo
void initSnakeGame() {
    srand((unsigned int)(time(NULL) ^ getpid()));

    // Inicializar cobra
    snake_game.length = 3;
    snake_game.direction = 1; // direita
    snake_game.body[0].x = SNAKE_WIDTH / 2;
    snake_game.body[0].y = SNAKE_HEIGHT / 2;
    snake_game.body[1].x = snake_game.body[0].x - 1;
    snake_game.body[1].y = snake_game.body[0].y;
    snake_game.body[2].x = snake_game.body[1].x - 1;
    snake_game.body[2].y = snake_game.body[1].y;

    // Posicionar comida
    snake_food.position.x = rand() % SNAKE_WIDTH;
    snake_food.position.y = rand() % SNAKE_HEIGHT;

    snake_score = 0;
    snake_gameOver = 0;
}

// Snake - Verificar entrada do teclado
void handleSnakeInput() {
    #ifdef _WIN32
    if (_kbhit()) {
        char key = _getch();
        switch (key) {
            case 'w':
            case 'W':
                if (snake_game.direction != 2) snake_game.direction = 0; // cima
                break;
            case 'd':
            case 'D':
                if (snake_game.direction != 3) snake_game.direction = 1; // direita
                break;
            case 's':
            case 'S':
                if (snake_game.direction != 0) snake_game.direction = 2; // baixo
                break;
            case 'a':
            case 'A':
                if (snake_game.direction != 1) snake_game.direction = 3; // esquerda
                break;
            case 'q':
            case 'Q':
                snake_gameOver = 2; // Desistência
                break;
        }
    }
    #else
    char key;
    ssize_t bytes_read = read(STDIN_FILENO, &key, 1);
    if (bytes_read > 0) {
        switch (key) {
            case 'w':
            case 'W':
                if (snake_game.direction != 2) snake_game.direction = 0; // cima
                break;
            case 'd':
            case 'D':
                if (snake_game.direction != 3) snake_game.direction = 1; // direita
                break;
            case 's':
            case 'S':
                if (snake_game.direction != 0) snake_game.direction = 2; // baixo
                break;
            case 'a':
            case 'A':
                if (snake_game.direction != 1) snake_game.direction = 3; // esquerda
                break;
            case 'q':
            case 'Q':
                snake_gameOver = 2; // Desistência
                break;
        }
    }
    #endif
}

// Snake - Mover cobra
void moveSnake() {
    SnakePoint newHead = snake_game.body[0];

    // Mover cabeça baseado na direção
    switch (snake_game.direction) {
        case 0: newHead.y--; break; // cima
        case 1: newHead.x++; break; // direita
        case 2: newHead.y++; break; // baixo
        case 3: newHead.x--; break; // esquerda
    }

    // Verificar colisão com bordas
    if (newHead.x < 0 || newHead.x >= SNAKE_WIDTH || 
        newHead.y < 0 || newHead.y >= SNAKE_HEIGHT) {
        snake_gameOver = 1; // Morte por colisão com borda
        return;
    }

    // Verificar colisão com próprio corpo
    for (int i = 0; i < snake_game.length; i++) {
        if (newHead.x == snake_game.body[i].x && newHead.y == snake_game.body[i].y) {
            snake_gameOver = 1; // Morte por colisão com próprio corpo
            return;
        }
    }

    // Mover corpo
    for (int i = snake_game.length - 1; i > 0; i--) {
        snake_game.body[i] = snake_game.body[i - 1];
    }
    snake_game.body[0] = newHead;

    // Verificar se comeu a comida
    if (newHead.x == snake_food.position.x && newHead.y == snake_food.position.y) {
        snake_score += 10;
        snake_game.length++;

        // Gerar nova comida
        do {
            snake_food.position.x = rand() % SNAKE_WIDTH;
            snake_food.position.y = rand() % SNAKE_HEIGHT;
        } while (0); // Simplificado - poderia verificar se não está no corpo da cobra
    }
}

// Snake - Desenhar jogo
void drawSnake() {
    clearSnakeScreen();

    printf("\n=== SNAKE ===\n");
    printf("Pontuação: %d | Tamanho: %d\n\n", snake_score, snake_game.length);

    // Desenhar bordas superiores
    printf("┌");
    for (int i = 0; i < SNAKE_WIDTH; i++) printf("─");
    printf("┐\n");

    // Desenhar área do jogo
    for (int y = 0; y < SNAKE_HEIGHT; y++) {
        printf("│");
        for (int x = 0; x < SNAKE_WIDTH; x++) {
            int isSnake = 0;
            int isHead = 0;

            // Verificar se é parte da cobra
            for (int i = 0; i < snake_game.length; i++) {
                if (snake_game.body[i].x == x && snake_game.body[i].y == y) {
                    isSnake = 1;
                    if (i == 0) isHead = 1;
                    break;
                }
            }

            if (isHead) {
                printf("●"); // Cabeça da cobra
            } else if (isSnake) {
                printf("█"); // Corpo da cobra
            } else if (snake_food.position.x == x && snake_food.position.y == y) {
                printf("♦"); // Comida
            } else {
                printf(" ");
            }
        }
        printf("│\n");
    }

    // Desenhar bordas inferiores
    printf("└");
    for (int i = 0; i < SNAKE_WIDTH; i++) printf("─");
    printf("┘\n");

    // Mostrar informações
    printf("\nControles: WASD para mover, Q para sair\n");
}

// Snake - Salvar pontuação
void saveSnakeScore(char *result) {
    if (!result) return;

    FILE *file = fopen("placar.txt", "w");
    if (file) {
        fprintf(file, "Jogo: Snake\n");
        fprintf(file, "Resultado: %.50s\n", result);
        fprintf(file, "Pontuação Final: %d\n", snake_score);
        fprintf(file, "Tamanho Final: %d\n", snake_game.length);
        fclose(file);
    }
}

// Snake - Função principal
int executar_snake() {
    printf("=== BEM-VINDO AO SNAKE ===\n");
    printf("Objetivo: Coma as comidas e cresça o máximo possível!\n");
    printf("Controles: WASD para movimento, Q para sair\n");
    printf("Pressione ENTER para começar...");
    getchar();

    setupSnakeTerminal();
    initSnakeGame();

    while (snake_gameOver == 0) {
        drawSnake();
        handleSnakeInput();
        
        // Se o jogador pressionou Q, sair do loop
        if (snake_gameOver != 0) {
            break;
        }
        
        moveSnake();
        
        // Se morreu durante o movimento, sair do loop
        if (snake_gameOver != 0) {
            break;
        }

        #ifdef _WIN32
            Sleep(150); // 150ms de delay no Windows
        #else
            usleep(150000); // 150ms de delay no Linux
        #endif
    }

    // Restaurar terminal ANTES de mostrar mensagens
    restoreSnakeTerminal();

    clearSnakeScreen();
    
    // Verificar se saiu por morte ou por desistência
    if (snake_gameOver == 1) {
        printf("\n🐍 GAME OVER - SNAKE! 🐍\n");
        printf("Pontuação final: %d\n", snake_score);
        printf("Tamanho final da cobra: %d\n", snake_game.length);

        if (snake_score > 0) {
            saveSnakeScore("Derrota");
            printf("Sua pontuação foi salva!\n");
        }

        // Opções simples sem loop
        int escolha;
        printf("\n1 - Jogar novamente\n");
        printf("2 - Voltar ao menu principal\n");
        printf("Digite sua escolha: ");
        
        scanf("%d", &escolha);
        getchar(); // Limpar buffer
        
        if (escolha == 1) {
            // Jogar novamente
            clearSnakeScreen();
            return executar_snake();
        } else {
            // Voltar ao menu principal
            clearSnakeScreen();
            return 0;
        }
    } else {
        // Saiu por desistência (Q)
        printf("Voltando ao menu principal...\n");
        printf("Pontuação final: %d\n", snake_score);
        
        if (snake_score > 0) {
            saveSnakeScore("Desistência");
            printf("Sua pontuação foi salva!\n");
        }
        
        printf("Pressione ENTER para continuar...");
        getchar();
        clearSnakeScreen();
    }

    return 0;
}
