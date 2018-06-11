#include <ctype.h>
#include <stdlib.h>
#include "campominado.h"

void campo_minado_ia(campo_minado_t* c, int* linha, int* coluna)
{
	// Exemplo de uma inteligencia artifical extremamente simploria para o jogo.
	// Apenas sorteia uma casa aleatoria, ainda nao aberta, e retorna a linha e
	// coluna correspondentes.
	while (1)
	{
		*linha = rand() % c->linhas;
		*coluna = rand() % c->colunas;

		if (c->tabuleiro[*linha][*coluna] == ':')
			return;
	}
}
