#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "campominado.h"
#include "ia.h"

#define MODO_ESTATISTICAS 1
#define MODO_DEBUG 2

// Escolha o modo de operacao do jogo descomentando uma das linhas abaixo
// e comentando a outra
#define MODO MODO_DEBUG
//#define MODO MODO_ESTATISTICAS

#define NUM_REPETICOES 10000

#if MODO != MODO_DEBUG && MODO != MODO_ESTATISTICAS
#error Voce deve selecionar um dos modos (MODO_ESTATISTICAS OU MODO_DEBUG)
#endif

struct campo_minado_private_t
{
	bool inicializado;
	campo_minado_estatisticas_t estatisticas;
	signed char** tabuleiro_solucao;
};

typedef struct
{
	int linhas, colunas, minas;
} parametros_t;

void campo_minado_erro_fatal(erros_t erro)
{
	const char* msg_erro[4] =
	{
		"",
		"ERRO_MAIS_MINAS_QUE_CASAS",
		"ERRO_LINHA_OU_COLUNA_INVALIDA",
		"ERRO_TENTATIVA_DE_MARCAR_CASA_INVALIDA"
	};

	if ((int)erro >= 1 && (int)erro <= 3)
		printf("Seu programa cometeu um erro fatal, abortando: %s\n", msg_erro[(int)erro]);
	else
		printf("Erro desconhecido\n");

	exit((int)erro);
}

int main()
{
	parametros_t parametros[3] =
	{
		{ .linhas =  8, .colunas =  8, .minas = 10 },
		{ .linhas = 16, .colunas = 16, .minas = 40 },
		{ .linhas = 16, .colunas = 30, .minas = 99 },
	};

#if MODO == MODO_ESTATISTICAS
	campo_minado_estatisticas_t estatisticas[3] =
	{
		{ .fracao_minas_corretamente_marcadas = 0.0, .fracao_minas_incorretamente_marcadas = 0.0, .fracao_casas_abertas = 0.0 },
		{ .fracao_minas_corretamente_marcadas = 0.0, .fracao_minas_incorretamente_marcadas = 0.0, .fracao_casas_abertas = 0.0 },
		{ .fracao_minas_corretamente_marcadas = 0.0, .fracao_minas_incorretamente_marcadas = 0.0, .fracao_casas_abertas = 0.0 },
	};

	const char* nomes_modos[3] =
	{
		"iniciante",
		"intermediario",
		"avancado"
	};

	double fracao_vitorias[3] = { 0.0, 0.0, 0.0 };
#endif
	int i;
#if MODO == MODO_ESTATISTICAS
	int j;
#endif

#if MODO == MODO_DEBUG
	const char* resultado_jogada[] =
	{
		"Ganhou o jogo", // JOGADA_GANHOU,
		"Casa aberta", // JOGADA_NAO_EXPLODIU,
		"Explosao de mina", // JOGADA_EXPLODIU,
		"Posicao invalida (erro de programacao)" // JOGADA_POSICAO_INVALIDA
	};

	// Escolha a dificuldade do jogo descomentando uma das
	// linhas abaixo e comentando as outras duas
	//i = 0; // Modo iniciante
	//i = 1; // Modo intermediario
	i = 2; // Modo avancado
#endif

	srand((unsigned int)time(NULL));

#if MODO == MODO_ESTATISTICAS
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < NUM_REPETICOES; j++)
		{
#endif
			campo_minado_t c;
			int num_jogada = 0;

			campo_minado_inicializa(&c, parametros[i].linhas, parametros[i].colunas, parametros[i].minas);

			while (1)
			{
				bool abortar = false;
				int linha = -1, coluna = -1;
				status_jogada_t status_jogada;

				campo_minado_ia(&c, &linha, &coluna);

				status_jogada = campo_minado_faz_jogada(&c, linha, coluna);

#if MODO == MODO_DEBUG
				printf("\njogada #%d: abrir linha %d e coluna %d\nResultado da jogada: %s\n\nNova configuracao do tabuleiro:\n\n", num_jogada++, linha, coluna, resultado_jogada[status_jogada]);
				campo_minado_imprime_tabuleiro(&c);
				printf("\n");
				system("pause");
#endif
				switch (status_jogada)
				{
					case JOGADA_EXPLODIU:
						abortar = true;
						break;

					case JOGADA_GANHOU:
#if MODO == MODO_ESTATISTICAS
						fracao_vitorias[i]++;
#endif
						abortar = true;
						break;

					case JOGADA_POSICAO_INVALIDA:
						campo_minado_erro_fatal(ERRO_LINHA_OU_COLUNA_INVALIDA);
						break;

					default:
						break;
				}

				if (abortar)
				{
#if MODO == MODO_ESTATISTICAS
					if (c._private->estatisticas.fracao_casas_abertas < 0 || c._private->estatisticas.fracao_casas_abertas > 1)
						i++;
					estatisticas[i].fracao_casas_abertas += c._private->estatisticas.fracao_casas_abertas;
					estatisticas[i].fracao_minas_corretamente_marcadas += c._private->estatisticas.fracao_minas_corretamente_marcadas;
					estatisticas[i].fracao_minas_incorretamente_marcadas += c._private->estatisticas.fracao_minas_incorretamente_marcadas;
#endif
					break;
				}
			}

			campo_minado_destroi(&c);
#if MODO == MODO_ESTATISTICAS
		}

		fracao_vitorias[i] /= NUM_REPETICOES;
		estatisticas[i].fracao_casas_abertas /= NUM_REPETICOES;
		estatisticas[i].fracao_minas_corretamente_marcadas /= NUM_REPETICOES;
		estatisticas[i].fracao_minas_incorretamente_marcadas /= NUM_REPETICOES;

		printf(
			"Estatisticas modo %s:\nVitorias: %.2lf%%\nCasas abertas: %.2lf%%\nMinas corretamente marcadas: %.2lf%%\nMinas incorretamente marcadas %.2lf%%\n\n",
			nomes_modos[i],
			fracao_vitorias[i]*100.0,
			estatisticas[i].fracao_casas_abertas*100.0,
			estatisticas[i].fracao_minas_corretamente_marcadas*100.0,
			estatisticas[i].fracao_minas_incorretamente_marcadas*100.0
		);
	}
#endif
	return 0;
}