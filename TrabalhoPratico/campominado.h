#pragma once

#include <stdbool.h>

typedef struct campo_minado_private_t* campo_minado_private;

typedef struct
{
	double fracao_minas_corretamente_marcadas;
	double fracao_minas_incorretamente_marcadas;
	double fracao_casas_abertas;
} campo_minado_estatisticas_t;

typedef struct
{
	// Valores possiveis para cada casa do tabuleiro:
	// '*': casa contendo mina (aparece apenas quando perde o jogo)
	// 'P': casa marcada pelo jogador como contendo mina
	// ':': casa nao aberta
	// '0' - '8': casa aberta, indicando quantidade de minas ao redor
	signed char** tabuleiro;
	int linhas, colunas, minas;
	campo_minado_private _private;
} campo_minado_t;

typedef enum
{
	JOGADA_GANHOU,
	JOGADA_NAO_EXPLODIU,
	JOGADA_EXPLODIU,
	JOGADA_POSICAO_INVALIDA
} status_jogada_t;

typedef enum
{
	MINA_MARCAR,
	MINA_DESMARCAR
} marcacao_mina_t;

typedef enum
{
	ERRO_MAIS_MINAS_QUE_CASAS = 1,
	ERRO_LINHA_OU_COLUNA_INVALIDA = 2,
	ERRO_TENTATIVA_DE_MARCAR_CASA_INVALIDA = 3,
} erros_t;

void campo_minado_inicializa(campo_minado_t* c, int linhas, int colunas, int minas);
status_jogada_t campo_minado_faz_jogada(campo_minado_t* c, int linha, int coluna);
void campo_minado_marca_desmarca_mina(campo_minado_t* c, int linha, int coluna, marcacao_mina_t marcacao);
void campo_minado_destroi(campo_minado_t* c);
void campo_minado_imprime_tabuleiro(campo_minado_t* c);

#ifdef UNIT_TEST
// Funcoes auxiliares apenas para testes, nao devem ser empregadas pelo usuario
void campo_minado_inicializa_com_tabuleiro_solucao(campo_minado_t* c, int linhas, int colunas, int minas, char** tabuleiro);
#endif