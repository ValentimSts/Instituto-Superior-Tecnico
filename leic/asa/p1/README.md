# Descrição do Problema

O projecto é composto pelos dois problemas que se descrevem em baixo.

## Problema 1
Dada uma sequência ```x =〈x0,x1, ...,xk〉```de inteiros, pretende calcular-se o tamanho da maior subsequência **estritamente crescente** de x, bem
como o número de **subsequências estritamente crescentes de tamanho máximo**.
Por exemplo:
- a sequência ```x =〈1,2,6,3,7〉``` tem duas subsequências **estritamente crescentes de tamanho máximo** igual a ```4```: ```s1 =〈1,2,6,7〉``` e ```s2 =〈1,2,3,7〉```.

## Problema 2
Dadas duas sequências ```x =〈x0,x1, ...,xk〉```e ```y =〈y0,y1, ...,yl〉```de inteiros, pretende calcular-se o tamanho da maior **subsequência comum estritamente
crescente** entre ```x``` e ```y```.
Porexemplo:
- as sequências ```x =〈1,2,6,3,7〉```e ```y =〈1,2,4,7,3〉```têm duas **subsequências comuns estritamente crescentes de tamanho máximo** igual a ```3```: ```s1=〈1,2,3〉```
  e ```s2=〈1,2,7〉```.

# Input

O ficheiro de entrada contém a informação relativa ao problema a resolver e às sequências de inteiros correspondentes, e é definido da seguinte forma:

- uma linha contendo um inteiro que indica o problema a resolver: o inteiro ***n*** corresponde ao problema ***n***;
- ***n*** linhas  contendo  cada  uma  delas  uma  sequência  de  inteiros  separados  por  um  único espaço em branco e não contendo qualquer outro
caractér, a não ser o fim de linha.

# Output

O  programa  deverá  escrever  no  output  dois  inteiros ***t*** e ***c*** separados  por  um  espaço,  onde ***t*** corresponde  ao  **tamanho  da  maior  subsequência**  que  respeita  as  restrições  do  problema e ***c*** corresponde ao **número de subsequências de tamanho máximo**.

# Exemplo

## Input
```
1
1 2 6 3 7
```
## Output
```
4 2
```
## Input
```
2
1 2 6 3 7
1 2 4 7 3
```
## Output
```
3
```
