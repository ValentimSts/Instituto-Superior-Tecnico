# Descrição do Problema

Uma árvore genealógica consiste num grafo dirigido em que cada nó representa uma pessoa e os vizinhos directos de um nó correspondem aos seus filhos, podendo existir 
nós orfãos, nós com um progenitor e nós com dois progenitores. Dados dois nós ``P1`` e ``P2`` de uma árvore genealógica:
 - ``P1`` diz-se **ancestral** de ``P2`` se é possível atingir ``P2`` a partir de ``P1`` na árvore genealógica;
 - ``P3`` diz-se um **ancestral comum mais próximo** de ``P1`` e ``P2``, se é **ancestral** de ``P1`` e ``P2``, e não existe um nó ``P4`` descendente de ``P3`` que seja também **ancestral** de ``P1`` e ``P2``.

Observamos que a definição de **ancestral comum mais próximo** permite que dois nós tenham vários **ancestrais comuns mais próximos**. Por exemplo, no grafo abaixo, ``v2`` e ``v4`` são ambos **ancestrais comuns mais próximos** de ``v5`` e ``v6``.

![imagem](https://user-images.githubusercontent.com/75221226/150774926-461a1e22-5871-4e15-ad7a-81246985a843.png)

Dado um grafo dirigido ``G = (V,E)`` e dois vértices ``v1``,``v2`` ∈V , pretende determinar-se:
- (1) se G forma uma árvore genealógica válida e, caso forme, (2) o conjunto de **ancestrais comuns mais próximos** entre ``v1`` e ``v2``.

# Input

O ficheiro de entrada contém a informação relativa à árvore genealógica a ser processada e aos vértices ``v1`` e ``v2``, cujos **ancestrais comuns mais próximos** devem ser calculados, e é definido da seguinte forma:

 - uma linha contendo os identificadores dos vértices ``v1`` e ``v2``;
 - uma linha contendo dois inteiros: o número ``n`` de **vértices (n ≥1)**, e o número ``m`` de **arcos (m ≥0)**;
 - uma lista de ``m`` linhas, em que cada linha contém os identificadores dos vértices ``x`` e ``y``, indicando que ``y`` é **filho** de ``x``.

Os identificadores dos vértices são **números inteiros entre 1 e n**.

# Output

O programa deverá escrever no output ``0`` se o grafo não formar uma árvore válida. Caso contrário, deverá escrever no output a sequência dos identificadores de todos os **ancestrais comuns mais próximos**, ordenados por **valor crescente e separados por um espaço em branco** (termine com um espaço em branco no fim para facilitar). Caso não exista nenhum, deverá escrever no output ``-``.

# Exemplo

## Input 1
```
5 6
8 9
1 2
1 3
2 6
2 7
3 8
4 3
4 5
7 5
8 6
```
## Output 1
```
2 4
```
## Input 2
```
5 2
8 9
1 2
1 3
2 6
2 7
3 8
4 3
4 5
7 5
8 6
```
## Output 2
```
2
```
## Input 3
```
2 4
8 9
1 2
1 3
2 6
2 7
3 8
4 3
4 5
7 5
8 6
```
## Output 3
```
-
```
