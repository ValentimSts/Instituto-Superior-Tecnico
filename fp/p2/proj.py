                           
                           
                           # TAD posicao #
# Representado por uma lista com dois elementos (x,y), inteiros nao negativos

# (Construtor)
# - cria_posicao: int x int --> posicao
# - cria_copia_posicao: posicao --> posicao

# (Seletores)
# - obter_pos_x: posicao --> int
# - obter_pos_y: posicao --> int

# (Reconhecedor)
# - eh_posicao: universal --> booleano

# (Teste)
# - posicoes_iguais: posicao x posicao --> booleano

# (Transformador)
# - posicao_para_str : posicao --> str

# (Funcao de alto nivel associada ao TAD posicao)
# - obter_posicoes_adjacentes: posicao --> tuplo de posicoes
# - ordenar_posicoes: tuplo --> tuplo

def cria_posicao(x,y):
    """
    recebe os valores correspondentes as coordenadas de uma
    posicao e devolve a posicao correspondente.
    O construtor verifica a validade dos seus argumentos, gerando 
    um ValueError caso os seus argumentos nao sejam validos.
    """
    if type(x) == int and type(y) == int and x >= 0 and y >= 0:
        return [x,y]
    raise ValueError("cria posicao: argumentos invalidos")


def cria_copia_posicao(pos):
    """ 
    recebe uma posicao e devolve uma copia nova da posicao
    """
    new_pos = pos[:]
    return new_pos


def obter_pos_x(pos):
    """
    devolve a componente x da posicao p.
    """
    return pos[0]


def obter_pos_y(pos):
    """
    devolve a componente y da posicao p.
    """
    return pos[1]


def eh_posicao(pos):
    """
    devolve True caso o seu argumento seja um TAD posicao e
    False caso contrario.
    """
    return type(pos) == list and len(pos) == 2 and type(pos[0]) == int and \
           type(pos[1]) == int  and pos[0] >= 0 and pos[1] >= 0


def posicoes_iguais(pos1, pos2):
    """
    devolve True apenas se p1 e p2 sao posicoes e sao iguais.
    """
    return eh_posicao(pos1) and eh_posicao(pos2) and \
           obter_pos_x(pos1) == obter_pos_x(pos2) and \
           obter_pos_y(pos1) == obter_pos_y(pos2)


def posicao_para_str(pos):
    return str(tuple(pos))


def obter_posicoes_adjacentes(pos):
    """
    devolve um tuplo com as posicoes adjacentes a posicao
    pos, comecando pela posicao acima de pos e seguindo no sentido horario.
    """
    x = obter_pos_x(pos)
    y = obter_pos_y(pos)

    res = ()

    if eh_posicao([x, y-1]):
        res += ((x, y-1), )
    
    if eh_posicao([x+1, y]):
        res += ((x+1, y), )

    if eh_posicao([x, y+1]):
        res += ((x, y+1), )
    
    if eh_posicao([x-1, y]):
        res += ((x-1, y), )
    
    return res


def ordenar_posicoes(t):
    """
    devolve um tuplo contendo as mesmas posicoes do tuplo fornecido como argumento, 
    ordenadas de acordo com a ordem de leitura do prado.
    """
    temp = list(t)
    temp.sort(key = lambda x: x[0])
    temp.sort(key = lambda x: x[1])

    return tuple(temp)



                             # TAD animal #
# Representado por uma lista 

# (Construtor)
# - cria_animal: str x int x int --> animal
# - cria_copia_animal: animal --> animal

# (Seletores)
# - obter_especie: animal --> str
# - obter_freq_reproducao: animal --> int
# - obter_freq_alimentacao: animal --> int
# - obter_idade: animal --> int
# - obter_fome: animal --> int

# (Modificadores)
# - aumenta_idade: animal --> animal
# - reset_idade: animal --> animal
# - aumenta_fome: animal --> animal
# - reset_fome: animal --> animal

# (Reconhecedor)
# - eh_animal: universal --> booleano
# - eh_predador: universal --> booleano
# - eh_presa: universal --> booleano

# (Teste)
# - animais_iguais: animal x animal --> booleano

# (Transformador)
# - animal para char : animal --> str
# - animal para str : animal --> str 

# (Funcoes de alto nivel associadas ao TAD animal)
# - eh animal fertil: animal --> booleano
# - eh animal faminto: animal --> booleano
# - reproduz animal: animal --> animal

def cria_animal(s, r, a):
    """
    recebe uma cadeia de caracteres s nao vazia correspondente a especie do animal e dois 
    valores inteiros correspondentes a frequencia de reproducao r (maior do que 0) e a frequencia
    de alimentacao a (maior ou igual que 0); e devolve o animal. Animais com frequencia 
    de alimentacao maior que 0 sao considerados predadores, caso contrario sao considerados
    presas.
     O construtor verifica a validade dos seus argumentos, gerando um ValueError, caso
    os seus argumentos nao sejam validos.
    """
    if type(s) == str and type(r) == int and r > 0 and type(a) == int and a >= 0:
        if a > 0:
            # predator [name, age, r, hunger, a]
            return [s, 0, r, 0, a]
        #prey [name, age, a, r]
        return [s, 0, a, r]

    raise ValueError("cria animal: argumentos invalidos")


def cria_copia_animal(animal):
    """
    recebe um animal (predador ou presa) e devolve uma
    nova copia do animal
    """
    new_animal = animal[:]
    return new_animal


def obter_especie(animal):
    """
    devolve a cadeia de caracteres correspondente a especie do
    animal.
    """
    return animal[0]


def obter_freq_reproducao(animal):
    """
    devolve a frequencia de reproducao do animal.
    """
    if len(animal) == 4:
        return animal[3] # prey
    return animal[2] # predator


def obter_freq_alimentacao(animal):
    """
    devolve a frequencia de alimentacao do animal (as presas devolvem sempre 0).
    """
    if len(animal) == 4:
        return 0 # prey
    return animal[4] # predator


def obter_idade(animal):
    """
    devolve a idade do animal.
    """
    return animal[1]


def obter_fome(animal):
    """
    devolve a fome do animal (as presas devolvem sempre 0).
    """
    if len(animal) == 4:
        return 0 # prey
    return animal[3] # predator


def aumenta_idade(animal):
    """
    modifica destrutivamente o animal, incrementando o 
    valor da sua idade em uma unidade, e devolve o proprio animal.
    """
    animal[1] += 1
    return animal


def reset_idade(animal):
    """
    modifica destrutivamente o animal, definindo o valor da sua
    idade igual a 0, e devolve o proprio animal.
    """
    animal[1] = 0
    return animal


def aumenta_fome(animal):
    """
    modifica destrutivamente o animal predador, incrementando o valor da 
    sua fome em uma unidade, e devolve o proprio animal. Esta
    operacao nao modifica os animais presa.
    """
    if len(animal) == 4:
        return animal # prey

    animal[3] += 1
    return animal # predator


def reset_fome(animal):
    """
    modifica destrutivamente o animal predador, definindo o valor
    da sua fome igual a 0, e devolve o proprio animal. Esta operacao nao modifica
    os animais presa.
    """
    if len(animal) == 4:
        return animal # prey

    animal[3] = 0
    return animal # predator


def eh_animal(animal):
    """
    devolve True caso o seu argumento seja um TAD animal e False caso contrario.
    """
    return type(animal) == list and type(animal[0]) == str and type(animal[1]) == int and animal[1] >= 0 and \
           (
               (
                len(animal) == 4 and (type(animal[x]) == int for x in range(2,4)) and \
                animal[2] >= 0 and animal[3] > 0
               )
                or
               (
                len(animal) == 5 and (type(animal[x]) == int for x in range(2,5)) and \
                animal[2] > 0 and animal[3] >= 0 and animal[4] >= 0
               )
           )
           

def eh_predador(arg):
    """
    devolve True caso o seu argumento seja um TAD animal do tipo predador 
    e False caso contrario.
    """
    return eh_animal(arg) and len(arg) == 5


def eh_presa(arg):
    """
    devolve True caso o seu argumento seja um TAD animal do tipo presa 
    e False caso contrario.
    """
    return eh_animal(arg) and len(arg) == 4


def animais_iguais(animal1, animal2):
    """
    devolve True apenas se animal1 e aanimal2  sao animais e sao iguais.
    """
    return eh_animal(animal1) and eh_animal(animal2) and \
           ((eh_predador(animal1) and eh_predador(animal2)) or \
            (eh_presa(animal1) and eh_presa(animal2))) and\
           obter_especie(animal1) == obter_especie(animal2) and \
           obter_freq_alimentacao(animal1) == obter_freq_alimentacao(animal2) and \
           obter_fome(animal1) == obter_fome(animal2) and \
           obter_freq_reproducao(animal1) == obter_freq_reproducao(animal2) and \
           obter_idade(animal1) == obter_idade(animal2)


def animal_para_char(animal):
    """
    devolve a cadeia de caracteres dum unico elemento correspondente ao primeiro 
    caracter da especie do animal passado por argumento, em maiuscula para 
    animais predadores e em minuscula para animais presa.
    """
    especie = obter_especie(animal)

    if eh_predador(animal):
        return especie[0].upper()
    return especie[0].lower()


def animal_para_str(animal):
    """
    devolve a cadeia de caracteres que representa o animal.
    """
    if eh_predador(animal):
        return "{} [{}/{};{}/{}]".format(*[comp for comp in animal])

    return animal[0] + "[" + str(animal[2]) + "/" + str(animal[3]) + "]"


def eh_animal_fertil(animal):
    """
    """
    return obter_idade(animal) == obter_freq_reproducao(animal)


def eh_animal_faminto(animal):
    """
    """
    return obter_fome(animal) == obter_freq_alimentacao(animal)


def reproduz_animal(animal):
    """
    """
    animal = reset_idade(animal)
    new_animal = cria_copia_animal(animal)
    new_animal = reset_fome(new_animal)
    return new_animal




                             # TAD prado #
# Representado por uma lista 

# (Construtor)


# (Seletores)


# (Modificadores)


# (Reconhecedor)


# (Teste)


# (Transformador)


# (Funcoes de alto nivel associadas ao TAD animal)


def cria_prado(d, r, a, p):
    """
    """
    if eh_posicao(d) and obter_pos_x(d) >= 2 and obter_pos_y(d) >= 2 and \
       type(r) == tuple and len(r) >= 0 and (eh_posicao(x) for x in r) and \
       type(a) == tuple and len(a) >= 1 and (eh_animal(x) for x in a) and \
       type(p) == tuple and len(p) == len(a) and (eh_posicao(x) for x in p):

        # 0: posicao montanha / 1: posicoes dos rochedos
        # 2: animais no prado / 3: posicoes dos animais
       return [d, list(r), list(a), list(p)]

    raise ValueError("criaprado:  argumentos invalidos")



def cria_copia_prado(m):
    """
    """
    new_prado = m[:]
    return new_prado


def obter_tamanho_x(m):
    """
    """
    return m[0][0]+1


def obter_tamanho_y(m):
    """
    """
    return m[0][1]+1


def obter_numero_predadores(m):
    """
    """
    n = 0
    for animal in m[2]:
        if eh_predador(animal):
            n += 1
    return n


def obter_numero_presas(m):
    """
    """
    n = 0
    for animal in m[2]:
        if eh_presa(animal):
            n += 1
    return n
    

def obter_posicao_animais(m):
    """
    """
    return ordenar_posicoes(m[3])


def obter_animal(m, p):
    """
    """
    return m[2][m[3].index(p)]


def eliminar_animal(m, p):
    """
    """
    index = m[3].index(p)

    m[2].remove(m[2][index])
    m[3].remove(p)

    return m


def eliminar_animal_por_nome(m, animal):
    """
    """
    index = m[2].index(animal)

    m[3].remove(m[3][index])
    m[2].remove(animal)
    
    return m

def mover_animal(m, p1, p2):
    """
    """
    index = m[3].index(p1)

    aumenta_idade(m[2][index])
    m[3][index] = p2

    if p2 in m[3]:
        m = eliminar_animal_por_nome(m, obter_animal(m, p2))

    return m
    

def inserir_animal(m, a, p):
    """
    """
    m[2].append(a)
    m[3].append(p)

    return m


def eh_prado(m):
    """
    """
    return type(m) == list and \
           eh_posicao(m[0]) and obter_pos_x(m[0]) >= 2 and obter_pos_y(m[0]) >= 2 and \
           (type(m[x]) == list for x in range(1, 4)) and \
           len(m[1]) >= 0 and (eh_posicao(x) for x in m[1]) and \
           len(m[2]) >= 1 and (eh_animal(x) for x in m[2]) and \
           len(m[3]) == len(m[2]) and (eh_posicao(x) for x in m[3])


def eh_posicao_animal(m, p):
    """
    """
    return p in m[3]


# funcao auxiliar
def eh_obstaculo(m, p):
    """
    """
    return p in m[1]


# funcao auxiliar
def eh_montanha(m, p):
    """
    """
    return obter_pos_x(p) == 0 or obter_pos_x(p) == obter_tamanho_x(m)-1 or \
           obter_pos_y(p) == 0 or obter_pos_y(p) == obter_tamanho_y(m)-1


def eh_posicao_obstaculo(m, p):
    """
    """
    return eh_obstaculo(m, p) or eh_montanha(m, p)


def eh_posicao_livre(m, p):
    """
    """
    return not eh_posicao_animal(m, p) and not eh_posicao_obstaculo(m, p)


def prados_iguais(m1, m2):
    """
    """
    return eh_prado(m1) and eh_prado(m2) and posicoes_iguais(m1[0], m2[0]) and \
           (posicoes_iguais(x, y) for x in m1[1] for y in m2[1]) and \
           (animais_iguais(x, y) for x in m1[2] for y in m2[2]) and \
           (posicoes_iguais(x, y) for x in m1[3] for y in m2[3])


# funcao auxiliar
def cantos_prado(m):
    """
    """
    Nx = obter_tamanho_x(m)-1
    Ny = obter_tamanho_y(m)-1

    return (cria_posicao(0, 0), cria_posicao(0, Ny),
            cria_posicao(Nx, 0), cria_posicao(Nx, Ny))


# funcao auxiliar
def pos_para_string(m, p):
    """
    """
    if eh_posicao_animal(m, p):
        return animal_para_char(obter_animal(m, p))

    if eh_posicao_obstaculo(m, p):

        if eh_montanha(m, p):

            if p in cantos_prado(m):
                if obter_pos_x(p) == obter_tamanho_x(m)-1:
                    return "+\n"
                return '+'

            elif obter_pos_y(p) == 0 or obter_pos_y(p) == obter_tamanho_y(m)-1:
                return '-'
        
            if obter_pos_x(p) == obter_tamanho_x(m)-1:
                return "|\n"
            return '|'
        
        return '@'
    
    return '.'


def prado_para_str(m):
    """
    """
    res = ""
    for y in range(0, obter_tamanho_y(m)):
        for x in range(0, obter_tamanho_x(m)):
            res += pos_para_string(m, cria_posicao(x, y))
    # o resultado vem com um '\n' a mais e por isso retornamos a string
    # sem o ultimo caracter
    return res[:-1]


def obter_posicoes_possiveis(m, p, type):
    """
    """
    pos_possiveis = []
    pos_adj = ordenar_posicoes(obter_posicoes_adjacentes(p))

    for pos in pos_adj:

        temp = list(pos)

        if type == "presa":
            if eh_posicao_livre(m, temp):
                pos_possiveis.append(temp)
        
        if type == "predador":
            if eh_posicao_livre(m, temp) or (eh_posicao_animal(m, temp) and eh_presa(obter_animal(m, temp))):
                if eh_posicao_animal(m, temp) and eh_presa(obter_animal(m, temp)):
                    return [temp]
                pos_possiveis.append(temp)
            
    return pos_possiveis



def obter_valor_numerico(m, p):
    """
    """
    return (obter_tamanho_x(m) * obter_pos_y(p)) + obter_pos_x(p)



def obter_movimento(m, p):
    """
    """
    if eh_presa(obter_animal(m, p)):
        mov_poss = obter_posicoes_possiveis(m, p, "presa")
    else:
        mov_poss = obter_posicoes_possiveis(m, p, "predador")

    if len(mov_poss) == 0:
        return p

    temp = []
    for pos in mov_poss:
        temp.append(list(map(lambda i, j: i - j, p, pos)))

    temp.sort(key = lambda x: x[0])
    temp.sort(key = lambda x: x[1])
    
    return [obter_pos_x(p)-obter_pos_x(temp[0]), obter_pos_y(p)-obter_pos_y(temp[0])]



def geracao(m):
    """
    """
    new_animals = []
    del_animals = []

    for y in range(0, obter_tamanho_y(m)):
        for x in range(0, obter_tamanho_x(m)):

            pos = cria_posicao(x,y)

            if eh_posicao_livre(m, pos) or eh_posicao_obstaculo(m, pos):
                continue

            if eh_posicao_animal(m, pos):

                animal = obter_animal(m, pos)

                if eh_presa(animal):
                    if eh_animal_fertil(animal):
                        new_animal = reproduz_animal(animal)
                        new_animals.append([new_animal, pos])
                        mover_animal(m, pos, obter_movimento(m, pos))
                    else:
                        mover_animal(m, pos, obter_movimento(m, pos))
                
                else:
                    if eh_animal_faminto(animal):
                        del_animals.append(pos)
                    else:
                        mover_animal(m, pos, obter_movimento(m, pos))
    
    for pair in new_animals:
        inserir_animal(m, pair[0], pair[1])
    
    for pos in del_animals:
        eliminar_animal(m, pos)
    

            






if __name__ == '__main__':
    dim = cria_posicao(11, 4)
    obs = (cria_posicao(4,2), cria_posicao(5,2))
    an1 = tuple(cria_animal('sheep', 2, 0) for i in range(3))
    an2 = (cria_animal('wolf', 20, 15),)
    pos = tuple(cria_posicao(p[0],p[1]) for p in ((2,2),(4,3),(10,2),(3,2)))
    prado = cria_prado(dim, obs, an1+an2, pos)
    print(prado_para_str(prado))
    print(prado_para_str(geracao(prado)))
    print(prado_para_str(geracao(prado)))
    print(prado_para_str(geracao(prado)))


