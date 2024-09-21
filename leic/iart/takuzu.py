# takuzu.py: Template para implementação do projeto de Inteligência Artificial 2021/2022.
# Devem alterar as classes e funções neste ficheiro de acordo com as instruções do enunciado.
# Além das funções e classes já definidas, podem acrescentar outras que considerem pertinentes.

# Grupo 70:
# 99343 Valentim Santos
# 99333 Tiago Santos

from sys import stdin
import numpy as np

from search import (
    Problem,
    Node,
    astar_search,
    breadth_first_tree_search,
    depth_first_tree_search,
    greedy_search,
    recursive_best_first_search,
    compare_graph_searchers,
)




class TakuzuState:
    state_id = 0

    def __init__(self, board):
        self.board = board
        self.id = TakuzuState.state_id
        TakuzuState.state_id += 1

    def __lt__(self, other):
        return self.id < other.id

    def __str__(self):
        return self.board.__str__()




class Board:
    """Representação interna de um tabuleiro de Takuzu."""

    def __init__(self, size):
        """Cria um tabuleiro vazio (preenchido com valores arbitrarios)
        de tamanho 'size' x 'size'."""

        self.size = size
        self.board = np.empty(shape = (size, size), dtype = int)

        # Lista que posteriormente ira conter todas as posicoes vazias
        # do tabuleiro.
        self.empty_entries = []

        # Lista que posteriormente ira conter todas as possiveis jogadas
        # para cada posicao vazia do tabuleiro.
        # A primeira jogada a aparecer nesta lista corresponde ah jogada
        # possivel para a primeira posicao vazia que aparece na lista 
        # 'empty_entries', o mesmo acontecendo para as restantes.
        self.possible_plays = []

        # Cria dois arrays que guardam informacao sobre o numero de posicoes
        # vazias por linha e coluna respetivamente.
        # - (1, 2, ... -> a 1a linha/coluna tem uma posicao vazia, a 2a tem duas
        self.number_empty_entries_row = np.zeros(size, dtype = int)
        self.number_empty_entries_col = np.zeros(size, dtype = int)

        # Cria dois arrays que guardam o numero de 1's e 0's por cada linha
        # e coluna respetivamente.
        # - ((2, 3), ... -> a 1a linha/coluna tem dois 0's e tres 1's
        self.number_values_row = np.zeros(shape = (size, 2), dtype = int)
        self.number_values_col = np.zeros(shape = (size, 2), dtype = int)

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def valid_entry(self, row: int, col: int) -> bool:
        """Verifica se a entrada dada como argumento eh valida,
        retornando 'True' se o for e 'False' caso contrario."""

        return type(row) == int and type(col) == int and \
                ((row) >= 1 and (row) <= self.size) and \
                ((col) >= 1 and (col) <= self.size)

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def update_entry(self, row: int, col: int, new_val) -> None:
        """Atualiza o valor da entrada [row, col] para 'new_val',
        dado como argumento."""

        if self.valid_entry(row, col):
            self.board[row-1, col-1] = new_val

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def get_number(self, row: int, col: int) -> int:
        """Devolve o valor na respetiva posição do tabuleiro."""
        
        if self.valid_entry(row, col):
            return self.board[row-1, col-1]
        
        return None
        
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def adjacent_vertical_numbers(self, row: int, col: int) -> tuple:
        """Devolve os valores imediatamente abaixo e acima,
        respectivamente."""

        if self.valid_entry(row, col):
            down = None
            up = None

            if row == 1:
                down = self.get_number(row+1, col)

            if row == self.size:
                up = self.get_number(row-1, col)
            
            else:
                down = self.get_number(row+1, col)
                up = self.get_number(row-1, col)

            return (down, up)

        return None

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def adjacent_horizontal_numbers(self, row: int, col: int) -> tuple:
        """Devolve os valores imediatamente à esquerda e à direita,
        respectivamente."""
        
        if self.valid_entry(row, col):
            left = None
            right = None

            if col == 1:
                right = self.get_number(row, col+1)

            if col == self.size:
                left = self.get_number(row, col-1)
            
            else:
                left = self.get_number(row, col-1)
                right = self.get_number(row, col+1)

            return (left, right)

        return None

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    @staticmethod
    def parse_instance_from_stdin():
        """Lê o test do standard input (stdin) que é passado como argumento
        e retorna uma instância da classe Board.

        Por exemplo:
            $ python3 takuzu.py < input_T01

            > from sys import stdin
            > stdin.readline()
        """
        
        size = int(stdin.readline())
        # Cria o tabuleiro com o tamanho definido.
        board = Board(size)

        # Le linha a linha e preenche o tabuleiro.
        for row in range(size):
            
            line = stdin.readline()
            
            # Coloca em 'values' todos os valores da linha acabada de ler.
            values = np.array([int(n) for n in line.split()])
            
            # Preenche o tabuleiro com os valores lidos.
            for col in range(size):

                val = values[col]

                # Adiciona todas as posicoes vazias ([row, col] preenchidas com 2) ah
                # lista de posicoes vazias do tabuleiro.
                if val == 2:
                    
                    # Utilizamos 'row+1' e 'col+1' pois 'row' e 'col' pertencem
                    # ao intrvalo [0...size-1] e para a representacao escolhida
                    # devem pertencer a [1...size].
                    board.empty_entries.append([row+1, col+1])

                    # Incrementa o numero de posicoes vazias para a linha
                    # e coluna em questao. 
                    board.number_empty_entries_row[row] += 1
                    board.number_empty_entries_col[col] += 1

                else:
                    # Incrementa o numero de 1's ou 0's (dependendo do valor
                    # de 'val') da linha e coluna em questao.
                    board.number_values_row[row, val] += 1
                    board.number_values_col[col, val] += 1

                board.update_entry(row+1, col+1, val)

        # Preenche a lista 'self.possible_plays', de board, com todas as
        # jogadas posiveis para cada posicao vazia
        board.get_all_possible_plays()

        return board

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def get_col(self, col: int):
        """Retorna uma lista com todos os valores da coluna indicada."""

        # Utilizamos 1 como a linha por questao de conveniencia.
        if self.valid_entry(1, col):
            
            return self.board[:, col-1]

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def get_row(self, row: int):
        """Retorna uma lista com todos os valores da linha indicada."""

        # Utilizamos 1 como a coluna por questao de conveniencia.
        if self.valid_entry(row, 1):
            
            return self.board[row-1, :]

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def is_empty_entry(self, row: int, col: int) -> bool:
        """Retorna True caso a posicao indicada seja uma posicao vazia, e False
        caso contrario."""

        return self.valid_entry(row, col) and self.get_number(row, col) == 2

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def same_entry(self, row1: int, col1: int, row2: int, col2: int) -> bool:
        """Retorna True caso a posicao [row1, col1] e [row2, col2] sejam iguais, e
        False caso contrario."""

        return self.valid_entry(row1, col1) and self.valid_entry(row2, col2) and\
                row1 == row2 and col1 == col2

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def get_number_values_row(self, row: int) -> list:
        """Retorna a lista com o numero de 0's e 1's presentes na linha indicada."""

        # Utilizamos 1 como a coluna por questao de conveniencia.
        if self.valid_entry(row, 1):

            return [self.number_values_row[row-1, 0],
                    self.number_values_row[row-1, 1]]

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def get_number_values_col(self, col: int) -> list:
        """Retorna a lista com o numero de 0's e 1's presentes na coluna indicada."""

        # Utilizamos 1 como a linha por questao de conveniencia.
        if self.valid_entry(1, col):
            
            return [self.number_values_col[col-1, 0],
                    self.number_values_col[col-1, 1]]

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def get_number_empty_entries(self, row: int, col: int) -> list:
        """Retorna uma lista [x,y] com o numero de posicoes vazias para a linha
        'row' (x) e o numero de posicoes vazias para a coluna 'col' (y)."""

        return [self.number_empty_entries_row[row-1],
                self.number_empty_entries_col[col-1]]

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def opposite_play(self, play: int) -> int:
        """Retorna a jogada contraria ah indicada.
         - Se play == 1, o metodo retorna 0.
         - Se play == 0, o metodo retorna 1."""

        if play == 1:
            return 0
        else:
            return 1

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def get_plays_in_row(self, row: int, col: int) -> list:
        """Retorna uma lista com todos as jogadas possiveis [1,0] para uma 
        certa posicao vazia do tabuleiro, tendo em conta a linha (row) da
        posicao.""" 

        plays = []

        if self.is_empty_entry(row, col):

            # Se existirem dois vizinhos com o mesmo valor (diferente de 2), a
            # unica jogada possivel para a posicao atual sera a jogada oposta
            # a essas duas posicoes.
            # caso contrario as jogadas possiveis serao [0,1].

            left = self.get_number(row, col-1)
            far_left = self.get_number(row, col-2)
            right = self.get_number(row, col+1)
            far_right = self.get_number(row, col+2)

            # Nao verifica posicoes vazias (preenchidas com 2)
            if col == 1:

                if right != 2 and (right == far_right):
                    plays.append(self.opposite_play(right))

                else:
                    plays = [0,1]
            
            elif col == 2:

                if self.size != 3 and right != 2 and ((right == left) or (right == far_right)):
                    plays.append(self.opposite_play(right))

                # No caso em que o tabuleiro tem tamanho 3, nao verificamos
                # 'far_right' pois esse estara fora do tabuleiro.
                elif self.size == 3 and right != 2 and (right == left):
                    plays.append(self.opposite_play(right))

                else:
                    plays = [0,1]

            elif col == self.size:

                if left != 2 and (left == far_left):
                    plays.append(self.opposite_play(left))

                else:
                    plays = [0,1]

            elif col == self.size-1:

                if self.size != 3 and left != 2 and ((left == right) or (left == far_left)):
                    plays.append(self.opposite_play(left))

                # No caso em que o tabuleiro tem tamanho 3, nao verificamos
                # 'far_left' pois esse estara fora do tabuleiro.
                elif self.size == 3 and left != 2 and (left == right):
                    plays.append(self.opposite_play(left))

                else:
                    plays = [0,1]

            else:

                if left != 2 and ((left == right) or (left == far_left)):
                    plays.append(self.opposite_play(left))

                elif right != 2 and (right == far_right):
                    plays.append(self.opposite_play(right))

                else:
                    plays = [0,1]

            
            # number_of_vals[0] -> numero de 0's na linha atual
            # number_of_vals[1] -> numero de 1's na linha atual
            number_of_vals = self.get_number_values_row(row)

            # Quando o tabuleiro eh de tamanho par.
            if self.size % 2 == 0:

                # Se a linha ja tiver o numero maximo de 1's, a unica
                # jogada possivel nessa posicao sera um 0.
                if number_of_vals[1] == self.size // 2:
                    plays = [0]

                # O mesmo se aplica mas tendo em conta o numero maximo de 0's
                elif number_of_vals[0] == self.size // 2:
                    plays = [1]

            # Quando o tabuleiro eh de tamanho impar.
            else:
                # Mesma logica que foi aplicada para o tabuleiro de tamanho par.
                if number_of_vals[1] == (self.size // 2) + 1:
                    plays = [0]

                elif number_of_vals[0] == (self.size // 2) + 1:
                    plays = [1]

            return plays
        
        else:
            print(f"Err: get_plays_in_row() - posicao [{row},{col}] nao eh vazia.")

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def get_plays_in_col(self, row: int, col: int) -> list:
        """Retorna uma lista com todos as jogadas possiveis [1,0] para uma 
        certa posicao vazia do tabuleiro, tendo em conta a coluna (col) da
        posicao.""" 

        plays = []

        if self.is_empty_entry(row, col):
            
            # Se existirem dois vizinhos com o mesmo valor (diferente de 2), 
            # a unica jogada possivel para a posicao atual sera a jogada 
            # oposta a essas duas posicoes.
            # Caso contrario as jogadas possiveis serao [0,1].

            up = self.get_number(row-1, col)
            far_up = self.get_number(row-2, col)
            down = self.get_number(row+1, col)
            far_down = self.get_number(row+2, col)

            # Nao verifica posicoes vazias (preenchidas com 2)
            if row == 1:
            
                if down != 2 and (down == far_down):
                    plays.append(self.opposite_play(down))

                else:
                    plays = [0,1]
            
            elif row == 2:

                if self.size != 3 and down != 2 and ((down == up) or (down == far_down)):
                    plays.append(self.opposite_play(down))

                # No caso em que o tabuleiro tem tamanho 3, nao verificamos
                # 'far_down' pois esse estara fora do tabuleiro.
                elif self.size == 3 and down != 2 and (down == up):
                    plays.append(self.opposite_play(down))

                else:
                    plays = [0,1]

            elif row == self.size:

                if up != 2 and (up == far_up):
                    plays.append(self.opposite_play(up))

                else:
                    plays = [0,1]

            elif row == self.size-1:

                if self.size != 3 and up != 2 and ((up == down) or (up == far_up)):
                    plays.append(self.opposite_play(up))
                
                # No caso em que o tabuleiro tem tamanho 3, nao verificamos
                # 'far_up' pois esse estara fora do tabuleiro.
                elif self.size == 3 and up != 2 and (up == down):
                    plays.append(self.opposite_play(up))

                else:
                    plays = [0,1]
            
            else:

                if up != 2 and ((up == down) or (up == far_up)):
                    plays.append(self.opposite_play(up))

                elif down != 2 and (down == far_down):
                    plays.append(self.opposite_play(down))

                else:
                    plays = [0,1]


            # number_of_vals[0] -> numero de 0's na coluna atual
            # number_of_vals[1] -> numero de 1's na coluna atual
            number_of_vals = self.get_number_values_col(col)

            # Quando o tabuleiro eh de tamanho par.
            if self.size % 2 == 0:

                # Se a linha ja tiver o numero maximo de 1's, a unica
                # jogada possivel nessa posicao sera um 0.
                if number_of_vals[1] == self.size // 2:
                    plays = [0]

                # O mesmo se aplica mas tendo em conta o numero maximo de 0's
                elif number_of_vals[0] == self.size // 2:
                    plays = [1]

            # Quando o tabuleiro eh de tamanho impar.
            else:
                # Mesma logica que foi aplicada para o tabuleiro de tamanho par.
                if number_of_vals[1] == (self.size // 2) + 1:
                    plays = [0]

                elif number_of_vals[0] == (self.size // 2) + 1:
                    plays = [1]

            return plays
        
        else:
            print(f"Err: get_plays_in_col() - posicao [{row},{col}] nao eh vazia.")

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def filled_board(self):
        """Retorna True caso o tabuleiro esteja completamente preenchido, e 
        False caso contrario."""

        for empty_entry in self.empty_entries:
            # Verifica se todas as posicoes vazias do tabuleiro estao 
            # preenchidas, ou seja, estao a 'None'.

            if empty_entry != None:
                return False

        return True

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def get_possible_plays(self, row: int, col: int):
        """Retorna uma lista com todos as jogadas possiveis {1,0} para uma 
        certa posicao vazia do tabuleiro."""


        # Obtem as jogadas possiveis para a linha e coluna da posicao atual.
        row_plays = self.get_plays_in_row(row, col)
        col_plays = self.get_plays_in_col(row, col)

        # As jogadas possiveis finais para a posicao atual seram dadas 
        # pela intersecao das jogadas possiveis para a linha e coluna da
        # posicao atual.
        entry_plays = np.intersect1d(row_plays, col_plays)

        return entry_plays

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def get_all_possible_plays(self) -> None:
        """Preenche a lista 'self.possible_plays' com todas as jogadas possiveis
        para cada posicao vazia do tabuleiro."""


        for entry in self.empty_entries:

            if entry == None:
                self.possible_plays.append(None)

            if entry != None:
                self.possible_plays.append(list(self.get_possible_plays(entry[0], entry[1])))

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def make_play(self, row: int, col: int, play: int) -> None:
        """Substitui, na lista de posicoes vazias do tabuleiro ('empty_entries'),
        e na lista de jogadas possiveis ('possible_plays), a posicao recebida e a
        sua/suas possiveis jogadas, respetivamente, por None, indicando assim que
        essa posicao foi preenchida."""

        if self.is_empty_entry(row, col):

            start = 0

            # Calcula qual sera o inicio da procura pela posicao a substituir,
            # na lista de posicoes vazias.
            for i in range(row-1):
                start += self.number_empty_entries_row[i]

            for i in range(start, len(self.empty_entries)):
                
                if self.empty_entries[i] != None and \
                   self.same_entry(row, col, self.empty_entries[i][0], self.empty_entries[i][1]):

                    # Coloca a posicao dada a 'None' assim como a/as jogadas
                    # possiveis para a mesma, efetivamente marcando a posicao 
                    # como preenchida.
                    self.empty_entries[i] = None
                    self.possible_plays[i] = None

                    # Visto que a posicao atual deixou de estar vazia,
                    # o numero de entradas vazias na linha/coluna sera 
                    # decrementado.
                    self.number_empty_entries_row[row-1] -= 1
                    self.number_empty_entries_col[col-1] -= 1

                    # Atualiza tambem o numero de 0s ou 1s (consoante a jogada)
                    # que existem na linha/coluna atuais.
                    self.number_values_row[row-1, play] += 1
                    self.number_values_col[col-1, play] += 1

                    # Preenche a posicao com o valor recebido.
                    self.update_entry(row, col, play)

                    new_possible_plays = []
                    i = 0

                    # Visto que o estado do tabuleiro foi alterado (foi feita uma jogada)
                    # teremos de calcular as novas jogadas possiveis para as restantes 
                    # posicoes vazias.
                    for entry in self.empty_entries:
                        new_play = None

                        # Caso estejamos na linha/coluna da jogada que foi feita, temos de calcular
                        # a nova jogada possivel para a posicao, visto que o estado da linha/coluna
                        # foi alterado.
                        if entry != None and (entry[0] == row or entry[1] == col):
                            new_play = list(self.get_possible_plays(entry[0], entry[1]))
                        
                        # Caso estejamos numa linha/coluna que nao a que foi jogada, entao as jogadas
                        # possiveis para estas posicoes continuam a ser as mesmas que anteriormente.
                        elif entry != None and (entry[0] != row and entry[1] != col):
                            new_play = self.possible_plays[i]

                        i += 1
                        new_possible_plays.append(new_play)

                    self.possible_plays = new_possible_plays

                    return
        
        else:
            print(f"Err: fill_empty_entry() - posicao [{row},{col}] nao eh vazia.")
    
    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def copy_board(self):
        """Cria e retorna um novo tabuleiro 'Board', copia do atual."""

        new_board = Board(self.size)

        new_board.board = np.copy(self.board)
        new_board.number_empty_entries_row = np.copy(self.number_empty_entries_row)
        new_board.number_empty_entries_col = np.copy(self.number_empty_entries_col)
        new_board.number_values_row = np.copy(self.number_values_row)
        new_board.number_values_col = np.copy(self.number_values_col)
        
        new_board.empty_entries = []
        new_board.possible_plays = []
        
        n_play = 0

        # Faz uma 'deep_copy' das posicoes vazias, assim como das
        # jogadas possiveis para cada uma dessas, do tabuleiro atual
        # para o novo.
        for empty_entry in self.empty_entries:

            # Ignoramos as entradas que estao a 'None' visto que 
            # estas ja nao estao vazias.
            if empty_entry != None:
                entry = empty_entry
                play = self.possible_plays[n_play]

                new_board.empty_entries.append(entry)
                new_board.possible_plays.append(play)

            n_play += 1

        return new_board

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def pos_heuristic(self, row: int, col: int):
        """Calcula o valor heuristico da posicao dada."""
        a = 1
        b = 2

        n_adj = 0

        # Comecamos por calcular o numero de posicoes preenchidas adjacentes ah atual.
        for val in self.adjacent_horizontal_numbers(row, col) + self.adjacent_vertical_numbers(row, col):

            if val != None and val != 2:
                n_adj += 1
        
        # A heuristica de cada posicao eh calculada usando as seguintes variaveis:
        # - numero de posicoes vazias na linha/coluna (multiplicadas por uma constante 'a')
        # - numero maximo de posicoes adjacentes preenchidas (constante 4)
        # - numero de posicoes adjacentes preenchidas (multiplicadas por uma constante 'b')
        return a * (self.number_empty_entries_row[row-1] + self.number_empty_entries_col[col-1]) + (4 - b * n_adj)

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def board_heuristic(self):
        """Calcula o valor heuristico do tabuleiro."""

        h = 0

        # Calcula a heuristica do tabuleiro (soma de todas as 
        # heuristicas de todas as posicoes vazias do mesmo).
        for pos in self.empty_entries:

            if pos != None:
                h += self.pos_heuristic(pos[0], pos[1])

        return h

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def __repr__(self):
        """Retorna a representacao "oficial" do objeto 'Board'."""

        line = ""
        for row in range(self.size):
            for col in range(self.size):

                # Utilizamos 'row+1' e 'col+1' pois 'row' e 'col' pertencem
                # ao intrvalo [0...size-1] e para a representacao escolhida
                # devem pertencer a [1...size].
                line += str(self.get_number(row+1, col+1)) + '\t'
            
            # Substitui o ultimo '\t' de line por um '\n', sempre que uma linha acaba 
            line = line[:-1] + '\n'

        # Substitui o ultimo '\n', visto que estara a mais na altura de dar print
        line = line[:-1] + ''

        return "Board(board=" + repr(line) + f", size={self.size}, \
                empty_entries={self.empty_entries})"

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def __str__(self):
        """Retorna a string "oficial" que representa o objeto 'Board'.
        Metodo chamado por str(object) e as funcoes built-in format() e print() """

        line = ""
        for row in range(self.size):
            for col in range(self.size):

                # Utilizamos 'row+1' e 'col+1' pois 'row' e 'col' pertencem
                # ao intrvalo [0...size-1] e para a representacao escolhida
                # devem pertencer a [1...size].
                line += str(self.get_number(row+1, col+1)) + '\t'
            
            # Substitui o ultimo '\t' de line por um '\n', sempre que uma linha acaba 
            line = line[:-1] + '\n'

        # Substitui o ultimo '\n', visto que estara a mais na altura de dar print
        line = line[:-1] + ''

        return line




class Takuzu(Problem):


    def __init__(self, board: Board):
        """O construtor especifica o estado inicial."""

        # Estado atual do problema Takuzu.
        super().__init__(TakuzuState(board))

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def actions(self, state: TakuzuState) -> list:
        """Retorna uma lista de ações que podem ser executadas a
        partir do estado passado como argumento."""

        actions = []
        i = 0

        empty_entries = state.board.empty_entries

        for play in state.board.possible_plays:
            # Se para uma certa posicao apenas for possivel uma jogada,
            # um 0 ou um 1, adicionamos essa jogada ah lista de acoes
            # e retornamo-la.
            if play != None and len(play) == 1:
                actions.append((empty_entries[i][0],  # linha
                                empty_entries[i][1],  # coluna
                                play[0]))             # jogada

                return actions
            i += 1

        i = 0


        # Caso nao exista nenhuma jogada com apenas uma possivel alternativa
        # (um 0 ou um 1), entao adicionamos a 'actions' as primeiras alternativas
        # possiveis para a primeira posicao vazia e retornamos essas acoes.
        for play in state.board.possible_plays:

            if play != None and len(play) == 2:

                actions.append((empty_entries[i][0],
                                empty_entries[i][1], 
                                play[0]))

                actions.append((empty_entries[i][0],
                                empty_entries[i][1], 
                                play[1]))

                return actions
            i += 1

        return actions

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def result(self, state: TakuzuState, action):
        """Retorna o estado resultante de executar a 'action' sobre
        'state' passado como argumento. A ação a executar deve ser uma
        das presentes na lista obtida pela execução de
        self.actions(state)."""
        
        # Cria um novo tabuleiro (copia do atual)
        new_board = state.board.copy_board()

        # Faz a jogada especificada em 'action' no novo tabuleiro.
        new_board.make_play(action[0], action[1], action[2])

        # Retorna o novo TakuzuState (o novo estado do tabuleiro).
        return TakuzuState(new_board)

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def goal_test(self, state: TakuzuState):
        """Retorna True se e só se o estado passado como argumento é
        um estado objetivo. Deve verificar se todas as posições do tabuleiro
        estão preenchidas com uma sequência de números adjacentes."""

        if state.board.filled_board():

            board_rows = []
            board_cols = []

            board_size = state.board.size

            # Guarda em 'board_rows' e 'board_cols' todas as linhas e
            # colunas do tabuleiro atual.
            for i in range(board_size):
                row = state.board.get_row(i+1)
                col = state.board.get_col(i+1)
                
                # Fazemos uma verificacao final, para ter certeza de que
                # nao existem trios no tabuleiro.
                for j in range(board_size - 2):
                    
                    if (row[j] == row[j + 1] == row[j + 2]) or \
                       (col[j] == col[j + 1] == col[j + 2]):

                        return False 

                board_rows.append(row)
                board_cols.append(col)
            
            # Calcula o numero de linhas e colunas distintas, dentro
            # das listas.
            unique_rows = len(np.unique(board_rows, axis = 0))
            unique_cols = len(np.unique(board_cols, axis = 0))

            # Caso o numero de linhas e colunas distintas seja igual ao tamanho
            # do tabuleiro, entao o tabuleiro eh valido.
            return unique_rows == board_size and unique_cols == board_size

        # Caso o tabuleiro ainda tenha posicoes vazias, sabemos de ante mao que nao
        # nos encontramos num estado objetivo.
        return False 

    # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

    def h(self, node: Node):
        """Função heuristica utilizada para a procura A*."""
        
        return node.state.board.board_heuristic()




if __name__ == "__main__":
    board = Board.parse_instance_from_stdin()

    print("----------------------------- Initial Board -----------------------------")
    print(board)
    print("\n")

    problem = Takuzu(board)

    goal = depth_first_tree_search(problem)

    print("----------------------------- Solved Board -----------------------------")
    print(goal.state)
