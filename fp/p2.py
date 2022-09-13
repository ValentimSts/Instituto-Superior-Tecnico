# ist199343 - Valentim Costa Santos - JOGO DO MOINHO #


                            # TAD posicao #
# Representado por uma lista com dois elementos, o primeiro a coluna 'a', 'b' ou 'c'
# e o segundo, a linha '1', '2' ou '3'. Exp: ['a','1']

# (Construtor)
# - cria_posicao: str x str --> posicao
# - cria_copia_posicao: posicao --> posicao

# (Seletores)
# - obter_pos_c: posicao --> str
# - obter_pos_l: posicao --> str

# (Reconhecedor)
# - eh_posicao: universal --> booleano

# (Teste)
# - posicoes_iguais: posicao x posicao --> booleano

# (Transformador)
# - posicao_para_str : posicao --> str

# (Funcao de alto nivel associada ao TAD posicao)
# - obter_posicoes_adjacentes: posicao --> tuplo de posicoes

def cria_posicao(c, l):
    """
    Recebe duas cadeias de carateres correspondentes a coluna c e a linha l de 
    uma posicao e devolve a posicao correspondente. 
    O construtor verifica a validade dos seus argumentos, gerando um ValueError 
    caso os seus argumentos sejam invalidos
    """
    col = ('a','b','c')
    linha = ('1','2','3')
    if type(c)!=str or c not in col or type(l)!=str or l not in linha:
        raise ValueError ("cria_posicao: argumentos invalidos")
    return [c, l]
        

def cria_copia_posicao(p):
    """
    Recebe uma posicao e devolve a copia dessa mesma posicao
    """
    pos_nova = list.copy(p)
    return pos_nova


def obter_pos_c(p):
    """
    Devolve a componente coluna c da posicao p
    """
    return p[0]


def obter_pos_l(p):
    """
    Devolve a componente linha l da posicao p
    """
    return p[1]


def eh_posicao(arg):
    """
    Devolve True caso o seu argumento seja um TAD posicao e False caso contrario
    """
    return type(arg)==list and len(arg)==2 and type(arg[0])==str and \
           type(arg[1])==str and arg[0] in ('a','b','c') and arg[1] in \
           ('1','2','3')


def posicoes_iguais(p1, p2):
    """
    Devolve True apenas se p1 e p2 sao posicoes e sao iguais
    """
    return eh_posicao(p1) and eh_posicao(p2) and p1[0]==p2[0] and p1[1]==p2[1]


def posicao_para_str(p):
    """
    Devolve a cadeia de caracteres 'cl' que representa o seu argumento, sendo
    os valores c e l as componentes coluna e linha de p
    """
    return str(p[0])+str(p[1])


def obter_posicoes_adjacentes(p):
    """
    Devolve um tuplo com as posicoes adjacentes a posicao p de acordo com a
    ordem de leitura do tabuleiro.
    """
    l_p = obter_pos_l(p) # linha de p
    c_p = obter_pos_c(p) # coluna de p
    linha = [str(int(obter_pos_l(p))-1),l_p,str(int(obter_pos_l(p))+1)]
    col = [chr(ord(obter_pos_c(p))-1),c_p,chr(ord(obter_pos_c(p))+1)]
    res = ()
    
    if pos_type(p)=='diagonal':
        for l in linha:
            for c in col:
                if eh_posicao([c,l]) and not posicoes_iguais([c, l], p):
                    res += (cria_posicao(c, l), )
    
    elif pos_type(p)=='lateral':
        for l in linha:
            for c in col:
                if eh_posicao([c,l]) and not posicoes_iguais([c, l], p)\
                   and posicao_para_str([c, l]) not in ('b1','a2','c2','b3'):
                    res += (cria_posicao(c, l), )        
    return res    



                             # TAD peca #
# Representado por uma string 'X'(jogador X), 'O'(jogador O) ou ' '(nao ha jogador)

# (Construtor)
# - cria_peca: str --> peca
# - cria_copia_peca: peca --> peca

# (Reconhecedor)
# - eh_peca: universal --> booleano

# (Teste)
# - pecas_iguais: peca x peca --> booleano

# (Transformador)
# - peca_para_str : peca --> str

# (Funcao de alto nivel associada ao TAD peca)
# - peca_para_inteiro: peca --> int

def cria_peca(s):
    """
    Recebe uma cadeia de carateres correspondente ao identificador de um dos 
    dois jogadores ('X' ou 'O') ou a uma peca livre (' ') e devolve a peca 
    correspondente
    """
    if type(s)==str and s in ('X','O',' '):
        return s
    raise ValueError ("cria_peca: argumento invalido")


def cria_copia_peca(j):
    """
    Recebe uma peca e devolve uma copia nova da peca
    """
    peca_nova = cria_peca(j)
    return peca_nova


def eh_peca(arg):
    """
    Devolve True caso o argumento seja um TAD peca e False caso contrario
    """
    return type(arg)==str and arg in ('X','O',' ')


def pecas_iguais(j1, j2):
    """
    Devolve True apenas se p1 e p2 sao pecas e sao iguais
    """
    return eh_peca(j1) and eh_peca(j2) and j1==j2


def peca_para_str(j):
    """
     Devolve a cadeia de caracteres que representa o jogador dono da peca, isto
     e, '[X]', '[O]' ou '[ ]'
     """
    if eh_peca(j):
        return '['+j+']'


def peca_para_inteiro(j):
    """
    devolve um inteiro valor 1, -1 ou 0, dependendo se a peca e do jogador
    'X', 'O' ou livre, respetivamente
    """
    if eh_peca(j):
        if pecas_iguais(j, cria_peca('X')):
            return 1
        elif pecas_iguais(j, cria_peca('O')):
            return -1
        return 0


                           # TAD tabuleiro #
# Representado por um dicionario com 9 entradas, cada uma representada pela 
# string da posicao correspondente. Cada key do dicionario contem o inteiro 
# correspondente a peca que ocupa essa mesma posicao

# (Contrutor)
# - cria_tabuleiro: {} --> tabuleiro
# - cria_copia_tabuleiro: tabuleiro --> tabuleiro

# (Seletores)
# - obter_peca: tabuleiro x posicao --> peca
# - obter_vetor: tabuleiro x str --> tuplo de pecas

# (Modificadores)
# - coloca_peca: tabuleiro x peca x posicao --> tabuleiro
# - remove_peca: tabuleiro x posicao --> tabuleiro
# - move_peca: tabuleiro x posicao x posicao --> tabuleiro

# (Reconhecedor)
# - eh_tabuleiro: universal --> booleano
# - eh_posicao_livre: tabuleiro x posicao --> booleano

# (Teste)
# - tabuleiros_iguais: tabuleiro x tabuleiro --> booleano

# (Transformador)
# - tabuleiro_para_str : tabuleiro --> str
# - tuplo_para_tabuleiro: tuplo --> tabuleiro

# (Funcoes de alto nivel associadas ao TAD tabulero)
# - obter_ganhador : tabuleiro --> peca
# - obter_posicoes_livres: tabuleiro --> tuplo de posicoes
# - obter_posicoes_jogador: tabuleiro x peca --> tuplo de posicoes
def cria_tabuleiro():
    """
    Devolve um tabuleiro de jogo do moinho de 3x3 sem posicoes ocupadas por
    pecas de jogador
    """
    return {'a1':0,'b1':0,'c1':0,'a2':0,'b2':0,'c2':0,'a3':0,'b3':0,'c3':0}
    # 0 --> ' ' / 1 --> 'X' / -1 --> 'O'


def cria_copia_tabuleiro(t):
    """
    Recebe um tabuleiro e devolve uma copia nova do tabuleiro
    """
    tab_novo = dict.copy(t)
    return tab_novo


def obter_peca(t, p):
    """
    Devolve a peca na posicao p do tabuleiro. Se a posicao nao estiver ocupada,
    devolve uma peca livre.
    """
    return inteiro_para_peca(t[posicao_para_str(p)])


def obter_vetor(t, s):
    """
    Devolve todas as pecas da linha ou coluna especificada pelo seu argumento
    """
    res = ()
    if type(s)==str and s in ('a','b','c') or s in ('1','2','3'):
        for i in identifica_vetor(t, s):
            res += (inteiro_para_peca(t[i]), )
        return res
    

def coloca_peca(t, j, p):
    """
    modifica destrutivamente o tabuleiro t colocando a peca j na posicao p, e 
    devolve o proprio tabuleiro
    """
    if eh_peca(j) and eh_posicao(p):
        t[posicao_para_str(p)]=peca_para_inteiro(j)
        return t
    

def remove_peca(t, p):
    """
    Modifica destrutivamente o tabuleiro t removendo a peca da posicao p, e 
    devolve o proprio tabuleiro
    """
    if eh_posicao(p):
        t[posicao_para_str(p)]=0  
        return t

    
def move_peca(t, p1, p2):
    """
    Modifica destrutivamente o tabuleiro t movendo a peca que se encontra na 
    posicao p1 para a posicao p2, e devolve o proprio tabuleiro
    """
    if eh_posicao(p1) and eh_posicao(p2):
        if posicoes_iguais(p1, p2):
            t[posicao_para_str(p1)]=t[posicao_para_str(p1)]
        else:
            t[posicao_para_str(p2)]=t[posicao_para_str(p1)]
            t[posicao_para_str(p1)]=0
        return t


def eh_tabuleiro(t):
    """
    Devolve True caso o seu argumento seja um TAD tabuleiro e False caso 
    contrario. Um tabuleiro valido pode ter um maximo de 3 pecas de cada 
    jogador, nao pode conter mais de 1 peca mais de um jogador que do
    contrario, e apenas pode haver um ganhador em simultaneo
    """
    
    return type(t)==dict and len(t)==9 and numero_pecas(t)['X']<=3 and \
           numero_pecas(t)['O']<=3 and \
           ((numero_pecas(t)['X']-numero_pecas(t)['O'])<=1 and \
            (numero_pecas(t)['O']-numero_pecas(t)['X'])<=1) and \
           verifica_ganhador(t)!=2


def eh_posicao_livre(t, p):
    """
    Devolve True apenas no caso da posicao p do tabuleiro corresponder a uma 
    posicao livre
    """
    return t[posicao_para_str(p)]==0


def tabuleiros_iguais(t1, t2):
    """
    Devolve True apenas se t1 e t2 sao tabuleiros e sao iguais
    """
    return eh_tabuleiro(t1) and eh_tabuleiro(t2) and t1==t2


def tabuleiro_para_str(t):
    """
    Devolve a cadeia de caracteres que representa o tabuleiro
    """
    return '   a   b   c\n1 {}-{}-{}\n   | \ | / |\n2 {}-{}-{}\n   | / | \ |\n3 {}-{}-{}'\
           .format(*[peca_para_str(inteiro_para_peca(t[p])) for p in ordena_pos()])


def tuplo_para_tabuleiro(t):
    """
    Devolve o tabuleiro que e representado pelo tuplo t com 3 tuplos, cada um
    deles contendo 3 valores inteiros iguais a 1, -1 ou 0
    """
    tab = cria_tabuleiro()
    for linha in [0,1,2]:
        for coluna in [0,1,2]:
            coloca_peca(tab, inteiro_para_peca(t[linha][coluna]), tuplo_pos(linha, coluna))
    return tab


def obter_ganhador(t):
    """
    Devolve uma peca do jogador que tenha as suas 3 pecas em linha na vertical 
    ou na horizontal no tabuleiro. Se nao existir nenhum ganhador, devolve uma
    peca livre
    """
    for linha in ('1','2','3'):
        for col in ('a','b','c'):
            if sum(vetor_para_tuplo(obter_vetor(t, linha)))==3 or \
               sum(vetor_para_tuplo(obter_vetor(t, col)))==3:
                return cria_peca('X')
            elif sum(vetor_para_tuplo(obter_vetor(t, linha)))==-3 or \
                 sum(vetor_para_tuplo(obter_vetor(t, col)))==-3:
                return cria_peca('O')
    return cria_peca(' ')
        

def obter_posicoes_livres(t):
    """
    Devolve um tuplo com as posicoes nao ocupadas pelas pecas de qualquer
    um dos dois jogadores na ordem de leitura do tabuleiro
    """
    pos_livres = ()
    for pos in ordena_pos():
        if eh_posicao_livre(t, str_para_posicao(pos)):
            pos_livres += (str_para_posicao(pos), )
    return pos_livres
                
                
def obter_posicoes_jogador(t, j):
    """
    Devolve um tuplo com as posicoes ocupadas pelas pecas j de um dos dois 
    jogadores na ordem de leitura do tabuleiro
    """
    def posicoes_jog_aux(ind, i, tab, j): # fucncao auxiliar da recursao
        if len(tab)==0:
            return ind
        elif tab[0]==peca_para_inteiro(j):
            return posicoes_jog_aux(ind+[i], i+1, tab[1:], j)
        return posicoes_jog_aux(ind, i+1, tab[1:], j)
    
    # retorna uma lista com os indices das posicoes em que o jogador j se encontra
    ind_pos_jog = posicoes_jog_aux([], 1, tab_completo(tab_para_tuplo(t)), j)
    posicoes = ()
    for i in ind_pos_jog:
        posicoes += (str_para_posicao(ordena_pos()[i-1]), )
    return posicoes
        
        
         
# - - - - - - - - - - - - - # Funcoes adicionais # - - - - - - - - - - - - - #


def obter_movimento_manual(t, p):
    """
    - obter_movimento_manual: tabuleiro x peca --> tuplo de posicoes
    
    Funcao auxiliar que recebe um tabuleiro e uma peca de um jogador, e devolve 
    um tuplo com uma ou duas posicoes que representam uma posicao ou um 
    movimento introduzido manualmente pelo jogador
    """
    posicoes = ()
    if numero_pecas(t)[p]<3: # fase de colocacao

        posicao = str(input("Turno do jogador. Escolha uma posicao: "))
        if posicao in ordena_pos():
            pos = str_para_posicao(posicao)
            
            if not eh_posicao_livre(t, pos):
                raise ValueError ("obter_movimento_manual: escolha invalida")
            
            posicoes += (pos, )
        else:
            raise ValueError ("obter_movimento_manual: escolha invalida")
    
    
    else: # fase de movimento
        mov = str(input("Turno do jogador. Escolha um movimento: "))
        
        if len(mov)!=4 or mov[0:2] not in ordena_pos() or mov[2:] not in ordena_pos():
            raise ValueError ("obter_movimento_manual: escolha invalida")
        p_i = str_para_posicao(mov[0:2]) # posicao inicial 
        p_f = str_para_posicao(mov[2:]) # posicao final  
        
        if jogador_bloqueado(t, p): # caso em que se pode jogar na mesma posicao
            if posicoes_iguais(p_f,p_i):
                posicoes += (str_para_posicao(p_i), str_para_posicao(p_f), )
            else:
                raise ValueError ("obter_movimento_manual: escolha invalida")
        
        elif eh_posicao(p_i) and eh_posicao(p_f) and p_i in obter_posicoes_jogador(t, p)\
             and p_f in obter_posicoes_adjacentes(p_i) and eh_posicao_livre(t, p_f):
            
            posicoes += (p_i, p_f, )
        else:
            raise ValueError ("obter_movimento_manual: escolha invalida")
    return posicoes


def obter_movimento_auto(t, p, dif):
    """
    - obter_movimento_auto: tabuleiro x peca x str --> tuplo de posicoes
    
    Funcao auxiliar que recebe um tabuleiro, uma peca de um jogador e uma cadeia 
    deccarateres representando o nivel de dificuldade do jogo, e devolve um 
    tuplo com uma ou duas posicoes que representam uma posicao ou um movimento 
    escolhido automaticamente
    """
    if dif=='facil':
        return dif_facil(t, p)
    
    elif dif=='normal':
        return dif_normal(t, p)
    
    return dif_dificil(t, p)
        
        
def moinho(jogador, dif):
    """
    - moinho: str x str --> str
    
    Funcao principal que permite jogar um jogo completo do jogo do moinho de um 
    jogador contra o computador. A funcao recebe duas cadeias de caracteres e 
    devolve a representacao externa da peca ganhadora ('[X]' ou '[O]'). O primeiro 
    argumento corresponde a representacao externa da peca com que deseja jogar 
    o jogador humano, e o segundo argumento selecciona o nivel de dificuldade do jogo
    """
    if jogador == peca_para_str('X'): jog = cria_peca('X')
    elif jogador == peca_para_str('O'): jog = cria_peca('O')
    
    if jogador not in (peca_para_str('X'),peca_para_str('O')) or dif not in \
       ('facil','normal','dificil'):
        raise ValueError ("moinho: argumentos invalidos")
    
    jog_inicial = cria_peca('X')      # o jogador 'X' comeca sempre primeiro
    t_jogo = cria_tabuleiro()         # tabuleiro de jogo
    
    print ("Bem-vindo ao JOGO DO MOINHO. Nivel de dificuldade {}.".format(dif))
    print (tabuleiro_para_str(t_jogo))
    
    while pecas_iguais(obter_ganhador(t_jogo), cria_peca(' ')):
        
        if pecas_iguais(jog, jog_inicial):
            pos = obter_movimento_manual(t_jogo, jog_inicial)
            
            if numero_pecas(t_jogo)[jog_inicial]<3: # fase de colocacao
                t_jogo = coloca_peca(t_jogo, jog_inicial, pos[0])
            
            else: # fase de movimento
                t_jogo = move_peca(t_jogo, pos[0], pos[1])
        
        else:
            print ("Turno do computador ({}):".format(dif))
            pos = obter_movimento_auto(t_jogo, jog_inicial, dif)
            
            if numero_pecas(t_jogo)[jog_inicial]<3: # fase de colocacao
                t_jogo = coloca_peca(t_jogo, jog_inicial, pos[0])
            
            else: # fase de movimento
                t_jogo = move_peca(t_jogo, pos[0], pos[1])
        
        print (tabuleiro_para_str(t_jogo))
        jog_inicial = inteiro_para_peca(-peca_para_inteiro(jog_inicial))
        
    return peca_para_str(obter_ganhador(t_jogo))
    

#-----------------------------------------------------------------------------#
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/ (Funcoes Auxiliares) \/\/\/\/\/\/\/\/\/\/\/\/\/\#
#-----------------------------------------------------------------------------#


def pos_type(p):
    """
    funcao auxiliar: - posicao: posicao --> str 
    
    Devolve uma cadeia de caracteres a indicar a posicao que ocupa, 'lateral'
    se for uma posicao que se encontre numa das laterais do tabuleiro,
    'diagonal' se for uma posicao que tenha diagonais associadas a mesma
    """
    if posicao_para_str(p) in ('a1','c1','b2','a3','c3'): return 'diagonal'
    return 'lateral'


def inteiro_para_peca(p):
    """
    funcao auxiliar: - inteiro_para_peca: inteiro --> str
    
    Recebe um inteiro 1, -1 ou 0 e devolve a str da peca correspondente, 'X', 
    'O' e ' ' respetivamente
    """
    if p in [1,-1,0]:
        if p==1: return 'X'
        elif p==-1: return 'O'
        return ' '
    
    
def identifica_vetor(t, v):
    """
    funcao auxiliar: - identifica_vetor: tabuleiro x str --> tuplo de pecas
    
    Recebe um tabuleior e uma str da coluna ou linha pretendida e devolve 
    um tuplo com as pecas dessa mesma coluna ou linha
    """
    if type(v)==str and v in ('a','b','c') or v in ('1','2','3'):
        if v=='a': 
            return ('a1','a2','a3')
        elif v=='b':
            return ('b1','b2','b3')
        elif v=='c':
            return ('c1','c2','c3')
        elif v=='1':
            return ('a1','b1','c1')
        elif v=='2':
            return ('a2','b2','c2')
        elif v=='3':
            return ('a3','b3','c3')
        

def numero_pecas(t):
    """
    funcao auxiliar: - numero_pecas: tabuleiro --> dict
    
    Recebe um tabuleiro e devolve um dicionario com a quantidade de pecas 
    de cada tipo que estao no tabuleiro ('X', 'O' e ' ')
    """
    keys = {cria_peca('X'):0, cria_peca('O'):0, cria_peca(' '): 0}
    for key in t:
        if t[key]==peca_para_inteiro(cria_peca('X')):
            keys[cria_peca('X')]+=1
            
        elif t[key]==peca_para_inteiro(cria_peca('O')):
            keys[cria_peca('O')]+=1
            
        elif t[key]==peca_para_inteiro(cria_peca(' ')):
            keys[cria_peca(' ')]+=1    
    return keys


def ordena_pos():
    """
    funcao auxiliar: - ordena_pos: {} --> tuplo
    
    Devolve todas as posicoes do tabuleiro na ordem de leitura correta
    """
    return ('a1','b1','c1','a2','b2','c2','a3','b3','c3')


def str_para_posicao(p):
    """
    funcao auxiliar: - str_para_posicao: str --> posicao
    
    Recebe a str de uma posicao e retorna essa mesma posicao na sua
    representacao TAD
    """
    return cria_posicao(str(p[0]),str(p[1]))


def verifica_ganhador(t):
    """
    funcao auxiliar: - verifica_ganhador: tabuleiro --> tuplo
    
    Recebe um tabuleiro e devolve um tuplo com os ganhadores do tabuleiro
    correspondente (1 se 'X' ganha, -1 se 'O' ganha e () se ninguem ganha)
    """
    v = 0
    for l in ('1','2','3'):
        linha = sum(vetor_para_tuplo(obter_vetor(t, l)))
        if linha in [3,-3]:
            v += 1
    for c in ('a','b','c'):
        col = sum(vetor_para_tuplo(obter_vetor(t, c)))            
        if col in [3,-3]:
            v += 1
    return v 


def tab_completo(t):
    """
    funcao auxiliar: - tab_completo: tuplo --> tuplo
    
    Recebe um tuplo de tuplos, representando o tabuleiro de jogo, e devolve um 
    unico tuplo com todas as pecas de jogo consoante o tuplo original
    """
    tab_c = ()
    for linha in t:
        for coluna in linha:
            tab_c += (coluna, )
    return tab_c


def tuplo_pos(l, c):
    """
    funcao auxiliar: - tuplo_pos: int x int --> posicao
    
    Recebe dois inteiros correspondentes a linha e coluna de um tuplo e devolve
    a sua posicao em TAD
    """
    linha = l+1
    if c == 0: coluna = 'a'
    elif c == 1: coluna = 'b'
    elif c == 2: coluna = 'c'
    return cria_posicao(coluna, str(linha))


def tab_para_tuplo(t):
    """
    funcao auxiliar: - tab_para_tuplo: tabuleiro --> tuplo
    
    Recebe um tabuleiro e devolve o tuplo de tuplos corresponte
    """
    if eh_tabuleiro(t):
        t_pos = tab_completo((obter_vetor(t,'1'),obter_vetor(t,'2'),obter_vetor(t,'3')))
        tuplo = ()
        for pos in t_pos:
            tuplo += (peca_para_inteiro(pos), )
        return ((tuplo[0],tuplo[1],tuplo[2]),(tuplo[3],tuplo[4],tuplo[5]),\
                (tuplo[6],tuplo[7],tuplo[8]))


def vetor_para_tuplo(v):
    """
    funcao auxiliar: - vetor_para_tuplo: tuplo (vetor de pecas) --> tuplo
    
    Recebe um vetor de posicoes e devolve o tuplo correspondente, consoante
    as pecas do vetor
    """
    tuplo = ()
    for peca in v:
        tuplo += (peca_para_inteiro(peca), )
    return tuplo
    
    
def jogador_bloqueado(t, p):
    """
    funcao auxiliar: - jogador_bloqueado: tabuleiro x peca --> booleano
    
    Recebe um tabuleiro e uma peca (jogador) e retorna True caso esse jogador 
    nao consiga fazer qualquer movimento por estarem todas as suas pecas
    bloqueadas pelo jogador contrario. Retorna False caso contrario 
    """
    res = ()
    for pos in obter_posicoes_jogador(t, p):
        for adj in obter_posicoes_adjacentes(pos):
            if eh_posicao_livre(t, adj):
                res += (1, )
    return len(res)==0

    
#-----------------------------------------------------------------------------#
#\/\/\/\/\/\/\/\/\/\/\ (Funcoes Estrategias Auxiliares) \/\/\/\/\/\/\/\/\/\/\/#
#-----------------------------------------------------------------------------#


def dif_facil(t, p):
    """
    funcao auxiliar: - dif_facil: tab x peca --> posicao
    
    Recebe um tabuleiro e uma peca e retorna a posicao ideal de acordo com a 
    dificuldade facil
    """
    caso_especial = ()
    # fase de colocacao
    if numero_pecas(t)[p]<3:
        if len(vitoria(t, p))!=0:
            return vitoria(t, p)
        
        elif len(bloqueio(t, p))!=0:
            return bloqueio(t, p)
        
        elif len(posicoes_vazias(t))>1:
            caso_especial += (posicoes_vazias(t)[0], )
            return caso_especial
        return posicoes_vazias(t)
    
    # fase de movimento
    mov = ()
    for pos in obter_posicoes_jogador(t, p):
        for adj in obter_posicoes_adjacentes(pos):
            if eh_posicao_livre(t, adj):
                mov += ((pos, adj), )
                
    if len(mov)==0:  # nao ha posicoes para onde se movimentar 
        # joga na mesma posicao
        return (obter_posicoes_jogador(t, p)[0],obter_posicoes_jogador(t, p)[0]) 
    return mov[0]                               
            


def dif_normal(t, p):
    """
    funcao auxiliar: - dif_normal: tab x peca --> posicao
    
    Recebe um tabuleiro e uma peca e retorna a posicao ideal de acordo com a 
    dificuldade normal
    """
    # fase de colocacao
    if numero_pecas(t)[p]<3:
        return dif_facil(t, p)
    
    #fase de movimento
    mov = 0
    for pos in obter_posicoes_jogador(t, p):
        for adj in obter_posicoes_adjacentes(pos):
            if eh_posicao_livre(t, adj):
                mov += 1   
    if mov==0:
        # joga na mesma posicao 
        return (obter_posicoes_jogador(t, p)[0],obter_posicoes_jogador(t, p)[0])
    return minimax(t, p, 1, ())[1][0]


def dif_dificil(t, p):
    """
    funcao auxiliar: - dif_dificil: tab x peca --> posicao
    
    Recebe um tabuleiro e uma peca e retorna a posicao ideal de acordo com a 
    dificuldade dificil
    """
    # fase de colocacao
    if numero_pecas(t)[p]<3:
        return dif_facil(t, p)
    
    # fase de movimento
    mov = 0
    for pos in obter_posicoes_jogador(t, p):
        for adj in obter_posicoes_adjacentes(pos):
            if eh_posicao_livre(t, adj):
                mov += 1   
    if mov==0:
        # joga na mesma posicao
        return (obter_posicoes_jogador(t, p)[0],obter_posicoes_jogador(t, p)[0])
    return minimax(t, p, 5, ())[1][0]
                   

def posicoes_vazias(t): # centro vazio / canto vazio / lateral vazio
    """
    funcao auxiliar: - posicoes_vazias: tab --> posicao
    
    Recebe um tabuleiro e retorna, se estiverem livres, as posicoes do centro,
    primeiro canto e primeira lateral, por esta mesma ordem
    """
    pos_livres = obter_posicoes_livres(t)
    pos = ()
    canto = ()
    lateral = ()
    
    if ['b','2'] in pos_livres: # centro vazio
        pos += ((['b','2']),)
        return pos
    
    else:
        for p in pos_livres:
            if posicao_para_str(p) in ('a1','c1','a3','c3'): # canto vazio
                canto += (p, )
            elif posicao_para_str(p) in ('b1','a2','c2','b3'): # lateral vazia
                lateral += (p, )
        if len(canto)!=0:
            return canto
        return lateral


def vitoria(t, p):
    """
    funcao auxiliar: - vitoria: tab x peca --> posicao
    
    Recebe um tabuleiro e uma peca e retorna a posicao de vitoria para essa 
    mesma peca (se estiverem na fase de colocacao) ou entao, retorna 
    um tuplo com as posicoes inicial e final do movimento (se estiverem na fase
    de movimento), isto tudo apenas se existir a possibilidade de vitoria
    """
    posicoes = obter_posicoes_livres(t)
    pos_ocupadas = obter_posicoes_jogador(t, p)
    pos_v = ()
    
    if numero_pecas(t)[p]<3:
        # fase de colocacao
        for pos in posicoes:
            if pecas_iguais(obter_ganhador(coloca_peca(cria_copia_tabuleiro(t), p, pos)),p):
                pos_v += (pos, )
        return pos_v
    
    for p_o in pos_ocupadas:
        # fase de movimento
        for pos in obter_posicoes_adjacentes(p_o):
            if eh_posicao_livre(t, pos) and \
               pecas_iguais(obter_ganhador(move_peca(cria_copia_tabuleiro(t), p_o, pos)),p):
                pos_v = (p_o, pos)
    if len(pos_v)!=0: 
        return (pos_v[0], pos_v[1])
    
    return pos_v


def bloqueio(t, p):
    """
    funcao auxiliar: - bloqueio: tab x peca --> posicao 
    
    Recebe um tabuleiro e uma peca e retorna a posicao para a qual se bloqueia
    a vitoria do jogador oposto (se estiverem na fase de colocacao) ou entao
    devolve um tuplo com as posicoes de movimento para as quais se bloqueia o 
    adversario (se estiverem na fase de movimento)
    """ 
    jog_oposto = inteiro_para_peca(-peca_para_inteiro(p))
    vit = vitoria(t, jog_oposto)
    bloq = ()
    if len(vit)!=0:
        return vit
    
    else:
        for pos in obter_posicoes_livres(t):
            teste = coloca_peca(cria_copia_tabuleiro(t), jog_oposto, pos)
            if pecas_iguais(obter_ganhador(teste), cria_peca(jog_oposto)):
                bloq += (pos, )
        return bloq


def minimax(t, peca, depth, seq_mov):
    """
    funcao auxliar: - minimax: tab x peca x int x tuplo --> tuplo
    
    Recebe um tabuleiro, uma peca, um nivel de profundidade (int) e um tuplo com
    a sequencia de movimentos a serem avaliados. A funcao explora todos os
    movimentos legais do jogador escolhido (peca), chamando a funcao recursiva 
    com, o tabuleiro modificado com um dos movimentos e o jogador adversario como 
    novos parametros, o algoritmo devolvera o movimento que mais favoreca o 
    jogador do turno atual. A recursao finaliza quando existe um ganhador ou 
    quando se atinge um nivel maximo de profundidade da recursao
    """
    if depth==0 or not pecas_iguais(obter_ganhador(t), cria_peca(' ')):
        return (peca_para_inteiro(obter_ganhador(t)), seq_mov)
    
    melhor_res = -peca_para_inteiro(peca)
    melhor_seq_mov = ()
    
    for p in obter_posicoes_jogador(t, peca):
        for adj in obter_posicoes_adjacentes(p):
            
            if eh_posicao_livre(t, adj):
                novo_tab = move_peca(cria_copia_tabuleiro(t), p, adj)
                jog_oposto = inteiro_para_peca(-peca_para_inteiro(peca))
                
                m_max = minimax(novo_tab, jog_oposto, depth-1, seq_mov+((p,adj),))
                novo_res = m_max[0]
                nova_seq_mov = m_max[1]
                
                if melhor_seq_mov==() or (pecas_iguais(peca,cria_peca('X'))\
                                          and novo_res > melhor_res)\
                   or (pecas_iguais(peca,cria_peca('O')) and novo_res < melhor_res):
                    melhor_res = novo_res 
                    melhor_seq_mov = nova_seq_mov
                    
    return (melhor_res, melhor_seq_mov)