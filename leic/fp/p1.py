# Valentim Costa Santos - ist199343 #

def eh_tabuleiro(tab):
    """diz se o tabuleiro escolhido e aceite"""
    aceites = ('1','0','-1')
    a = 0
    if isinstance(tab, tuple):  # ve se o tab e um tuplo
        for linha in tab:
            if isinstance(linha, tuple):  #ve se a linha e um tuplo 
                for elemento in linha:
                    if isinstance(elemento, int):  # ve se o elemento e um int 
                        if str(elemento) in aceites:  # ve se o elemento e aceite
                            a = a + 1
                    else:
                        return False 
            else:
                return False 
        if a == 9:
            return True
        else:
            return False
    else:
        return False
    
# --------------------------------------------------------- #
    
def eh_posicao(n): 
    """verifica se a posicao escolhida pertence ao tabuleiro, 1 a 9 inclusive"""
    aceites = ('1','2','3','4','5','6','7','8','9')
    if str(n) in aceites:  # ve se a posicao escolhida e aceite 
        return True
    else:
        return False
    
# --------------------------------------------------------- #
    
def obter_coluna(tab, c):
    """obtem uma coluna, de 1 a 3, do tabuleiro, consoante o 'c' escolhido"""
    coluna = ()
    if eh_tabuleiro(tab):
        if str(c) in ('1','2','3'):  # ve se a coluna escolhida e uma das 3 possiveis 
            for linha in tab:
                coluna = coluna + (linha[c-1], )                
            return coluna
        else:
            return ValueError ("obter_coluna: algum dos argumentos e invalido")
    else:
        raise ValueError ("obter_coluna: algum dos argumentos e invalido")
    
# --------------------------------------------------------- #
    
def obter_linha(tab, l):
    """obtem uma linha do tabuleiro, de 1 a 3, consoante o 'l' escolhido"""
    if eh_tabuleiro(tab):
        if str(l) in ('1','2','3'):  # ve se a linha e uma das 3 possiveis
            return tab[l-1]
        else:
            raise ValueError ("obter_linha: algum dos argumentos e invalido")
    else:
        raise ValueError ("obter_linha: algum dos argumentos e invalido")
    
# --------------------------------------------------------- #
    
def obter_diagonal(tab, d):
    """devolve a diagonal escolhida. se d==1, devolve a diagonal principal,
    da esquerda para a direita e a comecar no topo do tabuleiro, se d==2,
    devolve a diagonal secundaria do tabuleiro, da esquerda para a direita a
    comecar em baixo do tabuleiro """
    diagonal = ()
    if eh_tabuleiro(tab) and str(d) in ('1','2'):  # ve se a diagonal e uma das 2 possiveis 
        if d == 1:
            for elementos in range(len(tab)):
                diagonal = diagonal + (tab[elementos][elementos], )
            return diagonal
        if d == 2:
            diagonal = (tab[2][0],tab[1][1],tab[0][2])
        return diagonal
    else:
        raise ValueError ("obter_diagonal: algum dos argumentos e invalido") 
                
# --------------------------------------------------------- #

def tabuleiro_str(tab):
    """devolve o tabuleiro de jogo feito numa cadeia de caracteres"""
    if eh_tabuleiro(tab):                
        l = ' '
        a = 0
        for linha in tab:            
            for index in range(3):                
                if linha[index] == 0:  # se nesta pos estiver um 0, retorna "nada", ' '
                    elemento = ' '
                elif linha[index] == 1:  # se nesta pos estiver um 1, retorna 'X'
                    elemento = 'X'
                elif linha[index] == -1:  # se nesta pos estiver um -1, retorna 'O'
                    elemento = 'O'
                if index < 2:
                    l = l + elemento + ' | ' # para os elementos antes da segunda pos, poe uma ' | ' a frente
                if index == 2:
                    a = a + 1  # conta as linhas ja desenhadas 
                    if a == 3:
                        l = l + elemento + ' '  # se ja estivermos na 3 linha nao adiciona o separador (------------)
                    elif a != 3:
                        l = l + elemento + ' \n-----------\n ' 
        return l 
    else:
        raise ValueError ("tabuleiro_str: o argumento e invalido")

# --------------------------------------------------------- #

def eh_posicao_livre(tab, n):
    """devolve True se a posicao escolhida for ocupada por um 0, ou seja, uma
    posicao livre"""
    if eh_tabuleiro(tab):
        if isinstance(n, int) and eh_posicao(n): # ve se a posicao (n) e um int e e posicao aceite
            tab_c = tab_completo(tab)
            for i in tab_c:
                if tab_c[n-1] == 0:  # ve se a posicao escolhida e uma posicao livre
                    return True 
                else:
                    return False
        else:
            raise ValueError ("eh_posicao_livre: algum dos argumentos e invalido")
    else:
        raise ValueError ("eh_posicao_livre: algum dos argumentos e invalido")
    
# --------------------------------------------------------- #

def obter_posicoes_livres(tab):
    """retorna um tuplo com todas as posicoes livres do tabuleiro"""
    if eh_tabuleiro(tab):
        pos = ()
        for n in range(1, 10):
            if eh_posicao_livre(tab, n):  # ve se cada pos do tab e uma posicao livre
                pos = pos + (n, )  # se essa pos for livre, adiciona ao tuplo
        return pos
    else:
        raise ValueError ("obter_posicoes_livres: o argumento e invalido")
    
# --------------------------------------------------------- #

def jogador_ganhador(tab):
    """diz se para o tabuleiro escolhido existe vencedor e qual e, se 'X' e o
    vencedor, retorna 1, se 'O' e o vencedor, retorna -1, se nao esxite 
    vencedor retorna 0"""
    soma = 0
    if eh_tabuleiro(tab): # ve se a soma das colunas/linha/diagonais da 3 ou -3, ou seja ha vencedor 
        for i in range(1 ,4):
            if sum(obter_linha(tab, i)) == 3 or sum(obter_coluna(tab, i)) == 3:
                return 1
            elif sum(obter_diagonal(tab, 1)) == 3 or sum(obter_diagonal(tab, 2)) == 3:
                return 1
            elif sum(obter_linha(tab, i)) == -3 or sum(obter_coluna(tab, i)) == -3:
                return -1
            elif sum(obter_diagonal(tab, 1)) == -3 or sum(obter_diagonal(tab, 2)) == -3:
                return -1 
            else:
                vencedor = 0
        return vencedor
    else:
        raise ValueError ("jogador_ganhador: o argumento e invalido")
    
# --------------------------------------------------------- #

def marcar_posicao(tab, jogador, pos):
    """marca numa posicao livre um 1, se o jogador escolhido for 1 (X), ou -1, 
    se o jogador escolhido for -1 (O)"""
    if eh_tabuleiro(tab):
        if str(jogador) in ('1','-1') and eh_posicao(pos) and eh_posicao_livre(tab, pos):
            tab_c = tab_completo(tab)
            if pos in obter_posicoes_livres(tab):
                if jogador == 1:  # cria um tab novo, e organiza-o em tuplos dentro de tuplos 
                    tab_novo = tab_c[ :pos-1] + (1, ) + tab_c[pos: ]
                if jogador == -1:
                    tab_novo = tab_c[ :pos-1] + (-1, ) + tab_c[pos: ]
            tab_final = ((tab_novo[ :3]),(tab_novo[3:6]),(tab_novo[6: ]))        
            return tab_final
        else:
            raise ValueError ("marcar_posicao: algum dos argumentos e invalido")        
    else:
        raise ValueError ("marcar_posicao: algum dos argumentos e invalido")

# --------------------------------------------------------- #

def escolher_posicao_manual(tab):
    """pede ao jogador para escolher uma posicao do tabuleiro, e escreve 1 na
    mesma, se o jogador for 1 (X), ou -1 (O), se o jogador for -1"""
    if eh_tabuleiro(tab):
        n = eval(input("Turno do jogador. Escolha uma posicao livre: "))
        if eh_posicao(n):  # ve se a posicao escolhida e valida 
            if n in obter_posicoes_livres(tab):  # ve se a posicao escolhida esta livre
                return n
            else:
                raise ValueError ("escolher_posicao_manual: a posicao introduzida e invalida")
        else:
            raise ValueError ("escolher_posicao_manual: a posicao introduzida e invalida")
    else:
        raise ValueError ("escolher_posicao_manual: o argumento e invalido")

# --------------------------------------------------------- #

def escolher_posicao_auto(tab, jogador, estrategia):
    """retorna a posicao escolhida automaticamente, pelo computador, consoante
    a estrategia escolhida"""
    if eh_tabuleiro(tab):
        if estrategia == 'basico':
            return posicao_basico(tab, jogador) # retorna o resultado da 'posicao_basico'
        elif estrategia == 'normal': 
            return posicao_normal(tab, jogador) # retorna o resultado da 'posicao_normal'
        elif estrategia == 'perfeito':
            return posicao_perfeito(tab, jogador) # retorna o resultado da 'posicao_perfeito'
        else:
            raise ValueError ("escolher_posicao_auto: algum dos argumentos e invalido")
    else:
        raise ValueError ("escolher_posicao_auto: algum dos argumentos e invalido")
        
# --------------------------------------------------------- #

def jogo_do_galo(jogador, estrategia):
    """funcao final, permite jogar o jogo do galo contra o computador, caso o 
    jogador escolha o 'X' este joga primeiro contra o computador que sera o 'O', 
    o contrario acontece caso o jogador escolha jogar com o 'O'"""
    tab_jogo = ((0,0,0),(0,0,0),(0,0,0))
    print("Bem-vindo ao JOGO DO GALO.\nO jogador joga com '"+ jogador + "'.")
    if jogador == 'X':
            player = 1 # se o jogador for 'X', o player (nos) jogamos com 1
    elif jogador == 'O': 
        player = -1 # se o jogador for 'O', o player (nos) jogamos com -1
    computador = -(player) # o computador joga sempre com o oposto do jogador
    while jogador_ganhador(tab_jogo) == 0 or len(obter_posicoes_livres(tab_jogo)) == 0:
        # enquanto nao houver vencedor ou ja nao existirem posicoes livres, executa o codigo
        if computador == -1: # se o computador jogar com 'O' o jogador comeca primeiro 
            pos = escolher_posicao_manual(tab_jogo)
            tab_jogo = marcar_posicao(tab_jogo, player, pos)
            print(tabuleiro_str(tab_jogo))
            # ve se ja houve algum vencedor ou se ja acabaram as posicoes livres 
            if jogador_ganhador(tab_jogo) == 1:
                return ('X')
            elif jogador_ganhador(tab_jogo) == -1:
                return ('O')
            elif len(obter_posicoes_livres(tab_jogo)) == 0 and jogador_ganhador(tab_jogo) == 0:
                return ('EMPATE') 
                     
            print("Turno do computador ("+estrategia+"):")
            posicao_auto = (escolher_posicao_auto(tab_jogo, computador, estrategia))
            tab_jogo = (marcar_posicao(tab_jogo, computador, posicao_auto))
            print((tabuleiro_str(tab_jogo)))
            # ve se ja houve algum vencedor ou se ja acabaram as posicoes livres 
            if jogador_ganhador(tab_jogo) == 1:
                return ('X')
            elif jogador_ganhador(tab_jogo) == -1:
                return ('O')
            elif len(obter_posicoes_livres(tab_jogo)) == 0 and jogador_ganhador(tab_jogo) == 0:
                return ('EMPATE')        
                   
        elif computador == 1: # se o computador jogar com 'X', o computador comeca primeiro
            print("Turno do computador ("+estrategia+"):")
            posicao_auto = (escolher_posicao_auto(tab_jogo, computador, estrategia))
            tab_jogo = (marcar_posicao(tab_jogo, computador, posicao_auto))
            print((tabuleiro_str(tab_jogo)))
            # ve se ja houve algum vencedor ou se ja acabaram as posicoes livres 
            if jogador_ganhador(tab_jogo) == 1:
                return ('X')
            elif jogador_ganhador(tab_jogo) == -1:
                return ('O')
            elif len(obter_posicoes_livres(tab_jogo)) == 0 and jogador_ganhador(tab_jogo) == 0:
                return ('EMPATE')         
            
            pos = escolher_posicao_manual(tab_jogo)
            tab_jogo = marcar_posicao(tab_jogo, player, pos)
            print(tabuleiro_str(tab_jogo))
            # ve se ja houve algum vencedor ou se ja acabaram as posicoes livres 
            if jogador_ganhador(tab_jogo) == 1:
                return ('X')
            elif jogador_ganhador(tab_jogo) == -1:
                return ('O')
            elif len(obter_posicoes_livres(tab_jogo)) == 0 and jogador_ganhador(tab_jogo) == 0:
                return ('EMPATE') 

# =================( funcoes auxiliares )================== #

def tab_completo(tab):
    """funcao auxiliar: retorna o tabuleiro completo num so tuplo"""
    tab_c = ()
    if eh_tabuleiro(tab):
        for linha in tab:
            for elemento in linha:
                tab_c = tab_c + (elemento, ) # retorna o tabuleiro num so tuplo
        return tab_c
    else:
        raise ValueError ("tab_completo: algum dos argumentos e invalido")

# --------------------------------------------------------- #

def posicao_basico(tab, jogador): # executa todas as estrategias 'basicas' por ordem
    """funcao auxiliar: retorna o resultado da estrategia 'basico' """
    if eh_tabuleiro(tab) and str(jogador) in ('1','-1'):
        if centro(tab, jogador): # estrategia 5
            return centro(tab, jogador)
        elif len(canto_vazio(tab, jogador)) != 0: # estrategia 7
            return min(canto_vazio(tab, jogador))
        elif len(lateral_vazio(tab, jogador)) != 0: # estrategia 8
            return min(lateral_vazio(tab, jogador))
        else:
            raise ValueError ("escolher_posicao_auto: algum dos argumentos e invalido")
    else:
        raise ValueError ("escolher_posicao_auto: algum dos argumentos e invalido") 

# --------------------------------------------------------- #

def posicao_normal(tab, jogador): # executa todas as estrategias 'normais' por ordem
    """funcao auxiliar: retorna o resultado (posicao) da estrategia 'normal' """
    if eh_tabuleiro(tab) and str(jogador) in ('1','-1'):
        if vitoria(tab, jogador): # estrategia 1
            return min(vitoria(tab, jogador))
        elif bloqueio(tab, jogador): # estrategia 2
            return min(bloqueio(tab, jogador))
        elif centro(tab, jogador): # estrategia 5
            return centro(tab, jogador)        
        elif canto_oposto(tab, jogador): # estrategia 6
            return canto_oposto(tab, jogador)
        elif len(canto_vazio(tab, jogador)) != 0: # estrategia 7
            return min(canto_vazio(tab, jogador))
        elif len(lateral_vazio(tab, jogador)) != 0: # estrategia 8
            return min(lateral_vazio(tab, jogador))
        else:
            raise ValueError ("escolher_posicao_auto: algum dos argumentos e invalido")
    else:
        raise ValueError ("escolher_posicao_auto: algum dos argumentos e invalido")                    

# --------------------------------------------------------- #

def posicao_perfeito(tab, jogador): # executa todas as estrategias 'perfeitas' por ordem
    """funcao auxiliar: retorna o resultado (posicao) da estrategia 'perfeito'"""
    if eh_tabuleiro(tab) and str(jogador) in ('1','-1'):
        if len(vitoria(tab, jogador)) != 0: # estrategia 1
            return min(vitoria(tab, jogador))
        elif len(bloqueio(tab, jogador)) != 0: # estrategia 2
            return min(bloqueio(tab, jogador))
        elif len(bifurcacao(tab, jogador)) != 0: # estrategia 3
            return min(bifurcacao(tab, jogador))
        elif bloqueio_bifurcacao(tab, jogador): # estrategia 4
            return bloqueio_bifurcacao(tab, jogador)
        elif centro(tab, jogador): # estrategia 5
            return centro(tab, jogador)        
        elif canto_oposto(tab, jogador): # estrategia 6
            return canto_oposto(tab, jogador)
        elif len(canto_vazio(tab, jogador)) != 0: # estrategia 7
            return min(canto_vazio(tab, jogador))
        elif len(lateral_vazio(tab, jogador)) != 0: # estrategia 8
            return min(lateral_vazio(tab, jogador))
        else:
            raise ValueError ("escolher_posicao_auto: algum dos argumentos e invalido")
    else:
        raise ValueError ("escolher_posicao_auto: algum dos argumentos e invalido")

# --------------------------------------------------------- #        

def vitoria(tab, jogador): 
    """funcao auxiliar: retorna a posicao para a qual o jogador vence"""
    posicoes = ()
    if eh_tabuleiro(tab):
        diagonal_1 = obter_diagonal(tab, 1)
        diagonal_2 = obter_diagonal(tab, 2)
        for i in range(1, 4):
            coluna = obter_coluna(tab, i)
            linha = obter_linha(tab, i)           
            for e in range(1, 4):
                if sum(linha) == jogador*2 and linha[e-1] == 0:
                    # se na linha estiverem duas pecas do jogador 
                    posicoes = posicoes + ((e + ((i-1)*3)), ) # progressao aritmetica que devolve a posicao certa               
                if sum(coluna) == jogador*2 and coluna[e-1] == 0:
                    # se na coluna estiverem duas pecas do jogador
                    if i == 1:
                        if e == 1:
                            posicoes = posicoes + (1, ) #pos 1/1
                        elif e == 2:
                            posicoes = posicoes + (4, ) #pos 1/2
                        else:
                            posicoes = posicoes + (7, ) #pos1/3
                    elif i == 2:
                        if e == 1:
                            posicoes = posicoes + (2, ) #pos 2/1
                        elif e == 2:
                            posicoes = posicoes + (5, ) #pos 2/2
                        else:
                            posicoes = posicoes + (8, ) #pos 2/3
                    else:
                        posicoes = posicoes + ((e*3), ) #pos (3/1)/(3/2)/(3/3)
                if sum(diagonal_1) == jogador*2 and diagonal_1[e-1] == 0:
                    # se na diagonal_1 estiverem duas pecas do jogador
                    if (1+(e-1)*4) not in posicoes: # ve se essa pos ja foi considerada
                        posicoes = posicoes + ((1+(e-1)*4), ) # progressao aritmetica que devolve a posicao certa 
                    else:
                        posicoes = posicoes # se ja foi considerada nao adiciona nada
                if sum(diagonal_2) == jogador*2 and diagonal_2[e-1] == 0:
                    # se na diagonal_2 estiverem duas pecas do jogador
                    if (7+(e-1)*(-2)) not in posicoes: # ve se essa pos ja foi considerada
                        posicoes = posicoes + ((7+(e-1)*(-2)), ) # progressao aritmetica que devolve a posicao certa 
                    else:
                        posicoes = posicoes # se ja foi considerada nao adiciona nada
        return posicoes # devolve um tuplo com todas as posicoes de vitoria
    else:
        raise ValueError ("vitoria: algum dos argumentos e invalido")
    
# --------------------------------------------------------- #   

def bloqueio(tab, jogador):
    """funcao auxiliar: retorna a posicao para a qual se bloqueia o adversario 
    em condicoes normais"""
    posicoes = ()
    if eh_tabuleiro(tab):
        return vitoria(tab, -(jogador)) # retorna as posicoes para as quais o jogador oposto ganha
    else:
        raise ValueError ("vitoria: algum dos argumentos e invalido")
    
# --------------------------------------------------------- # 
           
def bifurcacao(tab, jogador):
    """funcao auxiliar: retorna a posicao na qual existe uma biforcacao para o 
    jogador escolhido"""
    pos = ()
    if eh_tabuleiro(tab):
        for e in range(1, 10):
            if eh_posicao_livre(tab, e):
                if len(vitoria(marcar_posicao(tab, jogador, e), jogador)) >= 2:
                    # se na posicao considerada, o jogador tiver pelo menos duas possiveis opcoes
                    # de vitoria, essa posicao e uma bifurcacao
                    pos = pos + (e, )
        return pos
    else:
        raise ValueError ("bifurcacao: algum fos argumentos e invalido")
    
# --------------------------------------------------------- # 
                
def bloqueio_bifurcacao(tab, jogador):
    """funcao auxiliar: retorna a posicao para a qual se bloqueia a bifurcacao 
    do adversario"""
    tab_check = ()
    tab_check2 = ()
    if eh_tabuleiro(tab):
        if len(bifurcacao(tab, -jogador)) == 1:
            return min(bifurcacao(tab, -jogador))
        # se o oponente so tiver uma bifurcacao o jogador joga ai para bloquear o mesmo
        
        if len(bifurcacao(tab, -jogador)) > 1:
            for e in lateral_vazio(tab, jogador) or canto_vazio(tab, jogador):
                tab_check = marcar_posicao(tab, jogador,e)
                # ve todos os tabuleiros em que o jogador cria um dois em linha
                
                for a in bifurcacao(tab_check, -jogador):
                    tab_check2 = marcar_posicao(tab_check, -jogador, a)
                    # ve os tabuleiros em que o oponente jogou numa bifurcacao do novo tab 
                    
                    for k in (bifurcacao(tab_check2, -jogador)):
                        if k not in (bifurcacao(tab, -jogador)):
                            # se ao jogar numa das posicoes se criou mais bifurcacoes para o oponente
                            # joga-se ai 
                            return min(bifurcacao(tab, -jogador))
                        else:
                            # senao retorna a posicao em que se faz um 2 em linha para obrigar o oponente a defender
                            return min(lateral_vazio(tab, jogador)) or min(canto_vazio(tab, jogador))
    else:
        raise ValueError ("bloqueio_bifurcacao: algum dos argumentos e invalido")
    
# --------------------------------------------------------- # 

def canto_oposto(tab, jogador):
    """funcao auxiliar: devolve a posicao do canto oposto ao que o adversario ocupa"""
    tab_c = tab_completo(tab)
    k = -(jogador)
    if eh_tabuleiro(tab):
        # retorna a poicao dos cantos opostos a cada canto
        if tab_c[8] == k and eh_posicao_livre(tab, 1):
            return 1
        elif tab_c[6] == k and eh_posicao_livre(tab, 3):
            return 3
        elif tab_c[2] == k and eh_posicao_livre(tab, 7):
            return 7
        elif tab_c[0] == k and eh_posicao_livre(tab, 9):
            return 9            
    else:
        raise ValueError ("canto_oposto: algum dos argumentos e invalido") 
    
# --------------------------------------------------------- # 

def centro(tab, jogador):
    """funcao auxiliar: retorna a posicao do centro do tabuleiro se esta 
    estiver disponivel"""
    if eh_tabuleiro(tab):
        if eh_posicao_livre(tab, 5): # se o centro estiver livre, retorna essa posicao
            return 5
    else:
        raise ValueError ("centro: algum dos argumentos e invalido")

# --------------------------------------------------------- # 

def canto_vazio(tab, jogador):
    """funcao auxiliar: retorna a posicao do primeiro canto vazio diponivel"""
    pos = ()
    if eh_tabuleiro(tab):
        for e in obter_posicoes_livres(tab):
            if e in (1, 3, 7, 9): 
                # ve se os cantos estao livres
                pos = pos + (e, )
        return (pos)
    else:
        raise ValueError ("canto_vazio: algum dos argumentos e invalido") 
    
# --------------------------------------------------------- # 

def lateral_vazio(tab, jogador):
    """funcao auxiliar: retorna a posicao da primeira lateral vazia diponivel"""
    pos = ()
    if eh_tabuleiro(tab):
        for e in obter_posicoes_livres(tab):
            if e in (2, 4, 6, 8):
                # ve se as laterais estao livres
                pos = pos + (e, )
        return (pos)
    else:
        raise ValueError ("lateral_vazio: algum dos argumentos e invalido")