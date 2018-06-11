#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "campominado.h"

struct campo_minado_private_t
{
	bool inicializado;
	campo_minado_estatisticas_t estatisticas;
	signed char** tabuleiro_solucao;
};

void campo_minado_erro_fatal(erros_t erro);

static void campo_minado_preenche_quantidade_minas(campo_minado_t* c)
{
	int i, j;

	for (i = 0; i < c->linhas; i++)
	{
		for (j = 0; j < c->colunas; j++)
		{
			char num_minas_adjacentes = 0;

			if (c->_private->tabuleiro_solucao[i][j] == -1)
				continue;

			if (i > 0 && j > 0 && c->_private->tabuleiro_solucao[i-1][j-1] == -1)
			{
				num_minas_adjacentes++;
			}

			if (i > 0 && c->_private->tabuleiro_solucao[i - 1][j] == -1)
			{
				num_minas_adjacentes++;
			}

			if (i > 0 && j < c->colunas - 1 && c->_private->tabuleiro_solucao[i - 1][j + 1] == -1)
			{
				num_minas_adjacentes++;
			}

			if (j > 0 && c->_private->tabuleiro_solucao[i][j - 1] == -1)
			{
				num_minas_adjacentes++;
			}

			if (j < c->colunas - 1 && c->_private->tabuleiro_solucao[i][j + 1] == -1)
			{
				num_minas_adjacentes++;
			}

			if (i < c->linhas - 1 && j > 0 && c->_private->tabuleiro_solucao[i + 1][j - 1] == -1)
			{
				num_minas_adjacentes++;
			}

			if (i < c->linhas - 1 && c->_private->tabuleiro_solucao[i + 1][j] == -1)
			{
				num_minas_adjacentes++;
			}

			if (i < c->linhas - 1 && j < c->colunas - 1 && c->_private->tabuleiro_solucao[i + 1][j + 1] == -1)
			{
				num_minas_adjacentes++;
			}

			c->_private->tabuleiro_solucao[i][j] = num_minas_adjacentes;
		}
	}
}

static void campo_minado_sorteia_minas(campo_minado_t* c, int linha_primeira_jogada, int coluna_primeira_jogada)
{
	int* posicoes_sorteadas[2];
	int i, j, k = 0;

	// Aloca vetores com numero de posicoes = linhas*colunas - 1 (ou seja, sem
	// incluir a posicao da primeira jogada)
	posicoes_sorteadas[0] = malloc((c->linhas * c->colunas - 1) * sizeof(int));
	posicoes_sorteadas[1] = malloc((c->linhas * c->colunas - 1) * sizeof(int));

	// Preenche vetores com posicoes em ordem para posterior embaralhamento
	for (i = 0; i < c->linhas; i++)
	{
		for (j = 0; j < c->colunas; j++)
		{
			if (i != linha_primeira_jogada || j != coluna_primeira_jogada)
			{
				posicoes_sorteadas[0][k] = i;
				posicoes_sorteadas[1][k++] = j;
			}
		}
	}

	// Embaralha primeiros "minas" elementos dos vetores e, a cada elemento
	// sorteado, marca minas nas posicoes correspondentes
	for (i = 0; i < c->minas; i++)
	{
		int posicao[2], indice;

		indice = i + rand () % (c->linhas * c->colunas - 1 - i);

		for (j = 0; j < 2; j++)
		{
			posicao[j] = posicoes_sorteadas[j][indice];
			posicoes_sorteadas[j][indice] = posicoes_sorteadas[j][i];
			posicoes_sorteadas[j][i] = posicao[j];
		}

		c->_private->tabuleiro_solucao[posicao[0]][posicao[1]] = -1;
	}

	campo_minado_preenche_quantidade_minas(c);

	free(posicoes_sorteadas[0]);
	free(posicoes_sorteadas[1]);
}

void campo_minado_inicializa(campo_minado_t* c, int linhas, int colunas, int minas)
{
	int i, j;

	if (minas >= linhas * colunas)
		campo_minado_erro_fatal(ERRO_MAIS_MINAS_QUE_CASAS);

	c->linhas = linhas;
	c->colunas = colunas;
	c->minas = minas;

	c->tabuleiro = malloc(linhas * sizeof(char*));
	c->_private = malloc(sizeof(struct campo_minado_private_t));
	c->_private->tabuleiro_solucao = malloc(linhas * sizeof(char*));

	for (i = 0; i < linhas; i++)
	{
		c->tabuleiro[i] = malloc(colunas * sizeof(char));
		c->_private->tabuleiro_solucao[i] = malloc(colunas * sizeof(char));

		for (j = 0; j < colunas; j++)
		{
			c->_private->tabuleiro_solucao[i][j] = 0;
			c->tabuleiro[i][j] = ':';
		}
	}

	// Aguarda primeira jogada antes de inicializar as minas, de forma a garantir
	// que o usuario nao perdera' o jogo na primeira jogada
	c->_private->inicializado = false;
}

static void campo_minado_calcula_estatisticas(campo_minado_t* c)
{
	int i, j;

	c->_private->estatisticas.fracao_minas_corretamente_marcadas = 0.0;
	c->_private->estatisticas.fracao_minas_incorretamente_marcadas = 0.0;
	c->_private->estatisticas.fracao_casas_abertas = 0.0;

	for (i = 0; i < c->linhas; i++)
	{
		for (j = 0; j < c->colunas; j++)
		{
			switch (c->tabuleiro[i][j])
			{
				case 'P':
					if (c->_private->tabuleiro_solucao[i][j] == -1)
						c->_private->estatisticas.fracao_minas_corretamente_marcadas++;
					else
						c->_private->estatisticas.fracao_minas_incorretamente_marcadas++;
					break;

				case ':':
					break;

				default:
					c->_private->estatisticas.fracao_casas_abertas++;
					break;
			}
		}
	}

	c->_private->estatisticas.fracao_minas_corretamente_marcadas /= c->minas;
	c->_private->estatisticas.fracao_minas_incorretamente_marcadas /= c->minas;
	c->_private->estatisticas.fracao_casas_abertas /= c->linhas * c->colunas - c->minas;
}

static void campo_minado_marca_minas(campo_minado_t* c)
{
	int i, j;

	for (i = 0; i < c->linhas; i++)
	{
		for (j = 0; j < c->colunas; j++)
		{
			if (c->_private->tabuleiro_solucao[i][j] == -1)
				c->tabuleiro[i][j] = '*';
			else
				c->tabuleiro[i][j] = c->_private->tabuleiro_solucao[i][j] + '0';
		}
	}
}

static void campo_minado_marca_posicoes_liberadas(campo_minado_t* c, int linha, int coluna)
{
	if (c->tabuleiro[linha][coluna] != ':')
		return;

	c->tabuleiro[linha][coluna] = c->_private->tabuleiro_solucao[linha][coluna] + '0';

	if (c->_private->tabuleiro_solucao[linha][coluna] == 0)
	{
		if (linha > 0 && coluna > 0)
		{
			campo_minado_marca_posicoes_liberadas(c, linha - 1, coluna - 1);
		}

		if (linha > 0)
		{
			campo_minado_marca_posicoes_liberadas(c, linha - 1, coluna);
		}

		if (linha > 0 && coluna < c->colunas - 1)
		{
			campo_minado_marca_posicoes_liberadas(c, linha - 1, coluna + 1);
		}

		if (coluna > 0)
		{
			campo_minado_marca_posicoes_liberadas(c, linha, coluna - 1);
		}

		if (coluna < c->colunas - 1)
		{
			campo_minado_marca_posicoes_liberadas(c, linha, coluna + 1);
		}

		if (linha < c->linhas - 1 && coluna > 0)
		{
			campo_minado_marca_posicoes_liberadas(c, linha + 1, coluna - 1);
		}

		if (linha < c->linhas - 1)
		{
			campo_minado_marca_posicoes_liberadas(c, linha + 1, coluna);
		}

		if (linha < c->linhas - 1 && coluna < c->colunas - 1)
		{
			campo_minado_marca_posicoes_liberadas(c, linha + 1, coluna + 1);
		}
	}
}

static bool campo_minado_ganhou_jogo(campo_minado_t* c)
{
	int i, j;

	for (i = 0; i < c->linhas; i++)
	{
		for (j = 0; j < c->colunas; j++)
		{
			if (c->_private->tabuleiro_solucao[i][j] != -1)
			{
				if (c->tabuleiro[i][j] >= '0' && c->tabuleiro[i][j] <= '8')
				{
					continue;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return true;
}

status_jogada_t campo_minado_faz_jogada(campo_minado_t* c, int linha, int coluna)
{
	if (linha < 0 || linha > c->linhas - 1 || coluna < 0 || coluna > c->colunas - 1)
	{
		return JOGADA_POSICAO_INVALIDA;
	}
	
	if (!c->_private->inicializado)
	{
		campo_minado_sorteia_minas(c, linha, coluna);
		c->_private->inicializado = true;
	}

	if (c->_private->tabuleiro_solucao[linha][coluna] == -1)
	{
		campo_minado_calcula_estatisticas(c);

		campo_minado_marca_minas(c);

		return JOGADA_EXPLODIU;
	}
	else
	{
		campo_minado_marca_posicoes_liberadas(c, linha, coluna);

		if (campo_minado_ganhou_jogo(c))
		{
			campo_minado_calcula_estatisticas(c);

			campo_minado_marca_minas(c);

			return JOGADA_GANHOU;
		}
		else
		{
			return JOGADA_NAO_EXPLODIU;
		}
	}
}

void campo_minado_marca_desmarca_mina(campo_minado_t* c, int linha, int coluna, marcacao_mina_t marcacao)
{
	if (linha < 0 || linha > c->linhas - 1 || coluna < 0 || coluna > c->colunas - 1)
	{
		campo_minado_erro_fatal(ERRO_LINHA_OU_COLUNA_INVALIDA);
	}

	if (c->tabuleiro[linha][coluna] == ':')
	{
		if (marcacao == MINA_MARCAR)
		{
			c->tabuleiro[linha][coluna] = 'P';
		}
	}
	else if (c->tabuleiro[linha][coluna] == 'P')
	{
		if (marcacao == MINA_DESMARCAR)
		{
			c->tabuleiro[linha][coluna] = ':';
		}
	}
	else
	{
		campo_minado_erro_fatal(ERRO_TENTATIVA_DE_MARCAR_CASA_INVALIDA);
	}
}

void campo_minado_destroi(campo_minado_t* c)
{
	int i;

	for (i = 0; i < c->linhas; i++)
	{
		free(c->tabuleiro[i]);
		free(c->_private->tabuleiro_solucao[i]);
	}

	free(c->tabuleiro);
	free(c->_private->tabuleiro_solucao);
	free(c->_private);
}

void campo_minado_imprime_tabuleiro(campo_minado_t* c)
{
	int i, j;

	printf("\\C ");
	for (j = 0; j < c->colunas; j++)
	{
		printf("%d ", j/10);
	}

	printf("\nL\\ ");
	for (j = 0; j < c->colunas; j++)
	{
		printf("%d ", j % 10);
	}

	printf("\n  \\\n");

	for (i = 0; i < c->linhas; i++)
	{
		printf("%02d ", i);
		for (j = 0; j < c->colunas; j++)
		{
			printf("%c ", c->tabuleiro[i][j] != '0' ? c->tabuleiro[i][j] : ' ');
		}

		printf("\n");
	}
}

#ifdef UNIT_TEST
void campo_minado_inicializa_com_tabuleiro_solucao(campo_minado_t* c, int linhas, int colunas, int minas, char** tabuleiro_solucao)
{
	int i, j;

	campo_minado_inicializa(c, linhas, colunas, minas);

	for (i = 0; i < linhas; i++)
	{
		for (j = 0; j < colunas; j++)
		{
			c->_private->tabuleiro_solucao[i][j] = tabuleiro_solucao[i][j];
		}
	}

	campo_minado_preenche_quantidade_minas(c);

	c->_private->inicializado = true;
}
#endif
