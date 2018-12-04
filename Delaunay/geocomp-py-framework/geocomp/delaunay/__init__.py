# -*- coding: utf-8 -*-

"""Algoritmos para construir a triangulação de Delaunay

Algoritmos disponiveis:
- Incremental Probabilistico
- Incremental

algoritmo otimo = executa em tempo O(n lg(n)), n = numero de pontos
"""
from . import incr_prob

# cada entrada deve ter:
#  [ 'nome-do-modulo', 'nome-da-funcao', 'nome do algoritmo' ]
children = [
	( 'incr_prob', 'IncrProb', 'Incremental\nProbabilistico' ),
	( 'incr_prob', 'Incr', 'Incremental' )
]

#children = algorithms

#__all__ = [ 'graham', 'gift' ]
__all__ = [a[0] for a in children]
