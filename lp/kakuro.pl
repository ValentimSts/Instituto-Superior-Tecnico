% --------------------------- %
% Valentim Santos - ist199343 %
% --------------------------- %

:- [codigo_comum].

% =====================( Predicados Auxiliares Globais )===================== %
% - Usadas em mais do que uma funcao


% - estrutura espaco e operacoes associadas ah mesma
% ---------------------------------------------------- 
% faz_espaco/2 - vars_espaco/1 - soma_espaco/1
% - Construtor para o tipo espaco. Retorna o espaco com as variaveis Vars de soma Soma.
%   ao tipo espaco estam associadas os predicados que retornam as variaveis Vars ou a soma
%   Soma desse mesmo espaco.
faz_espaco(Soma, Vars, espaco(Soma, Vars)).
vars_espaco(espaco(_, Vars), Vars).
soma_espaco(espaco(Soma, _), Soma).

% - estrutura numero comum e operacoes associadas ah mesma
% ----------------------------------------------------------
% faz_comum/2 - pos_faz_comum/1 - num_faz_comum/1 
%  - Construtor para o tipo numero_comum. Retorna uma lista no tipo (Indice, Num),
%    em que Indice eh o indice do numero comum, e Num eh o numero. pos_faz_comum e 
%    num_faz_comum retornam a Pos (Indice) e o Num da lista, respetivamente.
faz_comum(Indice, Num, (Indice, Num)).
pos_faz_comum((Indice, _), Indice).
num_faz_comum((_, Num), Num).


% =========================================================================== %   

% combinacoes_soma/4
% - Devolve a lista ordenada (Combs) cujos elementos sao as
%   combinacoes N a N, dos elementos de Els cuja soma eh Soma.
combinacoes_soma(N, Els, Soma, Combs):-
    findall(Comb, (combinacao(N, Els, Comb), sum_list(Comb, Soma)), Combs).


% =========================================================================== %   

% permutacoes_soma/4
% - Devolve a lista ordenada (Perms) cujos elementos sao as permutacoes 
%   das combinacoes N a N, dos elementos Els, cuja soma eh Soma.
permutacoes_soma(N, Els, Soma, Perms):-
    combinacoes_soma(N, Els, Soma, Combs),
    findall(Perm, (member(X, Combs), permutation(X, Perm)), Perms_Aux),
    sort(Perms_Aux,  Perms).


% =========================================================================== %   

% first/1 - second/1 (predicado auxiliar)
% - Recebem uma lista, e retornam o primeiro ou o segundo elemento dessa
%   mesma lista, respectivamente
first([Frst|_], Frst).
second([_, Sec|_], Sec).

% get_soma/3 (predicado auxiliar)
% - Retorna a Soma associada a uma posicao dependendo da variavel H_V, que especifica
%   qual a soma para o espaco se quer, horizontal ou vertical.
get_soma(Pos, H_V, Soma):-
    get_soma_aux(Pos, H_V, Soma).
get_soma_aux([Col|_], v, Col).
get_soma_aux([_,Linha|_], h, Linha).


% espaco_fila/2
% - Retorna o espaco de fila (Esp), em que Fila eh uma fila
%   (linha ou coluna) de um puzzle e H_V eh um dos atomos h ou v, 
%   conforme se trate de uma fila horizontal ou vertical,respectivamente
espaco_fila(Fila, Esp, H_V):-
    espaco_fila_aux(Fila, H_V, 0, Esp).

espaco_fila_aux([P|R], H_V, Soma, Esp):-
    % se encontrar uma lista comeca a contar as vriaveis ah frente da mesma
    is_list(P),
    conta_vars(R, [], Vars), length(Vars, N),
    check(Vars, N, P, H_V, Esp);
    espaco_fila_aux(R, H_V, Soma, Esp).

% conta_vars/3 (predicado auxiliar)
% - Retorna uma lista (Vars), com todas as ocorrencias de variaveis anonimas
%   numa fila.
conta_vars([P|_], Vars, Vars):-
    % encontrou uma lista --> para de contar vars
    is_list(P).
conta_vars([], Vars, Vars). % chegou ao fim da lista
conta_vars([P|R], Aux, Vars):-
    % enquanto P nao for uma lista, ou seja, for uma var 
    % adiciona a Aux.
    \+is_list(P),
    append(Aux, [P], New_Aux),
    conta_vars(R, New_Aux, Vars).

% check/4 (predicado auxiliar)
% - Recebe a lista de variaveis (Vars) da Fila, o comprimento dessa mesma 
%   lista (N), a posicao que foi analizada (P) e o atomo H_V. Se N > 0
%   entao existem variaveis e eh criado o espaco, senao a funcao falha.
check(_, 0, _, _, _):- fail.
check(Vars, N, P, H_V, Esp):-
    N>0, get_soma(P, H_V, New_Soma),
    faz_espaco(New_Soma, Vars, Esp).


% =========================================================================== %   

% espacos_fila/2
% - Recebe uma Fila, uma fila (linha ou coluna) de uma grelha e H_V, um dos 
%   atomos h ou v, E retorna Espacos, a lista de todos os espacos de Fila, 
%   da esquerda para a direita.
espacos_fila(H_V, Fila, Espacos):-
    espacos_fila_aux(H_V, Fila, Espacos).

espacos_fila_aux(H_V, Fila, []):-
    % se o predicado espaco_fila tiver falhado, ou seja, se nao existirem
    % espacos, entao espacos_fila retorna a lista vazia [].
    \+espaco_fila(Fila, _, H_V), !.
espacos_fila_aux(H_V, Fila, Espacos):-
    bagof(Esp, espaco_fila(Fila, Esp, H_V), Espacos).


% =========================================================================== %    

% espacos_puzzle/2
% - Retorna a lista de espacos (Espacos) do puzzle Puzzle, tal como descrito
%   na seccao 2.1, no passo 1.
espacos_puzzle(Puzzle, Espacos):-
    mat_transposta(Puzzle, Transpose),
    % aplicamos espacos_fila h, a todos os elementos da matriz Puzzle inicial,
    % para obter cada linha, e espacos_fila v ah transposta, para obter as colunas
    aplica_espacos_fila(h, Puzzle, [], Esps),
    aplica_espacos_fila(v, Transpose, [], EspsT),
    append(Esps, EspsT, Espacos_Aux),
    flatten(Espacos_Aux, Espacos), !.

% aplica_espacos_fila/3 (funcao auxiliar)
% - Aplica o predicado espacos_fila a todos os elementos do Puzzle que recebe como
%   argumento, conservando as variaveis nele presentes.
aplica_espacos_fila(_, [], Res, Res).
aplica_espacos_fila(H_V, [P|R], Aux, Res):-
    espacos_fila(H_V, P, Esp), append(Aux, Esp, New_Aux),
    aplica_espacos_fila(H_V, R, New_Aux, Res).
    
% =========================================================================== %   

% mesma_var/2 (predicado auxiliar)
% - Verifica se a variavel Var esta presente na lista de variaveis Vars,
%   sem que esta seja unificada.
mesma_var(Var, Vars_Lst):-
    member(VarY, Vars_Lst), Var == VarY.

% espacos_com_posicoes_comuns/3
% - Recebe uma lista de espacos (Espacos), um espaco (Esp) e retorna 
%   uma lista (Esps_com) com todos os espacos com posicoes em comum com Esp
espacos_com_posicoes_comuns(Espacos, Esp, Esps_com):-
    vars_espaco(Esp, Vars),
    encontra_comuns(Vars, Esp, Espacos, Esps_com).

encontra_comuns(Vars, Esp, Espacos, Esps_com):-
    bagof(Esp_Aux, (member(Esp_Aux, Espacos), goal_com(Esp_Aux, Vars)), Esps_com_Aux),
    % Retiramos o proprio espaco da lista dos seus espacos comuns.
    subtract(Esps_com_Aux, [Esp], Esps_com).

% goal usado para o bagof
goal_com(Esp, Vars):- vars_espaco(Esp, V_Esp), verifica_comum(Vars, V_Esp).

% verifica_comum/2
% - Verifica se algum dos elemntos da primeira lista esta presente na segunda.
%   Funciona como um member, mas sem unificar variaveis.
verifica_comum([], _):- fail.
verifica_comum([P|_], V_Esp):- mesma_var(P, V_Esp), !.
verifica_comum([P|R], V_Esp):-
    \+mesma_var(P, V_Esp), verifica_comum(R, V_Esp).


% =========================================================================== % 

% permutacoes_soma_espacos/2
% - Retorna Perms_soma, a lista de listas de 2 elementos, em que o primeiro 
%   elemento eh um  espaco de espacos e o segundo eh a lista ordenada de  
%   permutacoes cuja soma eh igual ah soma do espaco.
permutacoes_soma_espacos(Espacos, Perms_soma):-
    aux_perm_soma(Espacos, [], Perms_soma).

aux_perm_soma([], Perms_soma, Perms_soma).
aux_perm_soma([Esp|R_Esp], Aux, Perms_soma):-
    perms_soma_espaco(Esp, Perm), append(Aux, [Perm], New_Aux),
    aux_perm_soma(R_Esp, New_Aux, Perms_soma).


% emcontra as permutacoes de cada espaco e junta-as ao mesmo
perms_soma_espaco(Esp, Perm):-
    soma_espaco(Esp, Soma),
    vars_espaco(Esp, Vars),
    length(Vars, N),
    % encontramos todas as permutacoes possiveis de N a N e soma Soma
    permutacoes_soma(N, [1,2,3,4,5,6,7,8,9], Soma, Perms),
    % juntamos as permutacoes ao espaco
    append([Esp], [Perms], Perm).


% =========================================================================== %

% permutacao_possivel_espaco/4
% - recebe um espaco Esp, uma lista de espacos Espacos e uma lista de Perms_soma,
%   obtida pelo predicado anterior, e retorna Perm, sendo Perm uma permutacao possivel 
%   para o espaco Esp, tal como descrito na Seccao 2.1, no passo 2.
permutacao_possivel_espaco(Perm, Esp, Espacos, Perms_soma):-
    % Esp_com tem os espacos com as mesmas posicoes que Esp.
    espacos_com_posicoes_comuns(Espacos, Esp, Esp_Com), 
    % Esp_afetados eh uma lista com todos os espacos com posicoes comuns
    % a Esp, incluindo o proprio Esp.
    append([Esp], Esp_Com, Esp_afetados),
    % P_afetados contem todas os espacos, e permutacoes de soma correspondentes, 
    % que contem posicoes em comum.
    % -------------------------------------------------------------------------
    % forca uma unificacao, encontrando assim todas as listas de Perms_soma
    % que tenham como primeiro elemento Esp1, um espaco de Esp_afetados.
    bagof([Esp1, Var], (member(Esp1, Esp_afetados), member([Esp1, Var], Perms_soma)), P_afetados),
    % Perms eh a lista que contem apenas as permutacoes possiveis para
    % os espacos afetados.
    findall(P, (member(Var, P_afetados),nth1(2, Var, P)), Perms),
    nth1(1, Perms, Perm_Espaco, New_Perms),
    % Perm_Espaco contem as permutacoes para o espaco Esp
    % New_Perms contem o resto das permutacoes.
    find_perm_possivel(Perm_Espaco, New_Perms, Perm).


% find_perm_possivel/2 (predicado auxiliar)
% - Recebe a uma lista (Perm_Espaco) com as permutacoes possiveis para o espaco
%   Esp, e uma lista (New_Perms) com as permutacoes possiveis para os espacos
%   com posicoes comuns a Esp, retornando entao a permutacao desejada, Perm.
% --------------------------------------------------------------------------- %
% Exemplo: Perm_Espaco = [[1,2], [2,1]] / Perm_Esp = [1,2]
%          New_Perms = [[<perms para espaco_A>], [<perms para espaco_B>]]
find_perm_possivel([], _, _):- fail.
find_perm_possivel([Perm_Esp|R_PermEsp], New_Perms, Perm):-
    % [1,2] eh uma perm possivel se 1 for comum as perms do espaco_A, e 2 for
    % comum as perms do espaco_B.
    check_possivel(Perm_Esp, New_Perms), Perm = Perm_Esp;
    % se [1,2] nao for possivel, tentamos com [2,1] (o resto da lista).
    find_perm_possivel(R_PermEsp, New_Perms, Perm).


% check_possiveis/2 (predicado auxiliar)
% - Recebe as listas Perm_Esp da funcao anterior (ex: [1,2]) e a lista New_Perms
%   verificando se 1 pertence ah primeira lista de New_Perms e o 2 ah segunda. 
check_possivel([], _):- !.
check_possivel(_, []):- !.
check_possivel([P_El|R_El], [Perms|R_Perms]):-
    flatten(Perms, Comp), 
    member(P_El, Comp)-> check_possivel(R_El, R_Perms);
    fail.


% =========================================================================== %

% permutacoes_possiveis_espaco/4
% - Devolve Perms_poss, uma lista de 2 elementos em que o primeiro eh a lista de 
%   variaveis de Esp e o segundo eh a lista ordenada de permutacoes possiveis para 
%   o espao Esp, tal como descrito na Seccao 2.1, no passo 2.
permutacoes_possiveis_espaco(Espacos, Perms_soma, Esp, Perms_poss):-
    bagof(Perm, permutacao_possivel_espaco(Perm, Esp, Espacos, Perms_soma), Perms),
    vars_espaco(Esp, Vars), append([Vars], [Perms], Perms_poss). 


% =========================================================================== %

% permutacoes_possiveis_espacos/2
% - Perms_poss eh uma lista de 2 elementos em que o primeiro eh a lista de variaveis 
%   de Esp e o segundo eh a lista ordenada de permutacoes possiveis para oespaco Esp, 
%   tal como descrito na Seccao 2.1, no passo 2.
permutacoes_possiveis_espacos(Espacos, Perms_poss_esps):-
    permutacoes_soma_espacos(Espacos, Perms_soma),
    bagof(Perm, goal_perms_pe(Espacos, Perms_soma, Perm), Perms_poss_esps).

% goal usado para o bagof
goal_perms_pe(Espacos, Perms_soma, Perm):-
    member(Esp, Espacos), permutacoes_possiveis_espaco(Espacos, Perms_soma, Esp, Perm).


% =========================================================================== %

% numeros_comuns/2
% -  Retorna (Numeros_comuns), uma lista de pares(pos, numero), significando    
%    que todas as listas de Lst_Perms contem o numero numero na posicao pos
numeros_comuns(Lst_Perms, Numeros_comuns):-
    first(Lst_Perms, L_Aux),
    delete(Lst_Perms, L_Aux, New_P),
    nums_comuns_aux(L_Aux, New_P, 1, [], Numeros_comuns).

nums_comuns_aux([], _, _, Numeros_comuns, Numeros_comuns):- !.
nums_comuns_aux([P|R], Perms, N, Aux, Numeros_comuns):-
    % se P eh comum entao criamos uma lista (N, P)
    comum(P, N, Perms)-> 
    faz_comum(N, P, Com), 
    append(Aux, [Com], New_Aux),
    % aumenta-se o indice
    New_N is N+1,
    nums_comuns_aux(R, Perms, New_N, New_Aux, Numeros_comuns);
    New_N is N+1,
    nums_comuns_aux(R, Perms, New_N, Aux, Numeros_comuns).

% comum/3 (predicado auxiliar)
% - Recebe um numero N e o seu indice Index, na lista em que se encontra e verifica
%   se para todas as restantes listas esse mesmo numero encontra-se no mesmo indice.
comum(_, _, []):- !.
comum(N, Index, [P|R]):-
    nth1(Index, P, N),
    comum(N, Index, R).


% =========================================================================== %

% atribui_comuns/1
% - Recebe Perms_Possiveis, uma lista de permutacoes possiveis, actualiza  esta  
%   lista atribuindo a cada espaco numeros comuns a todas as permutacoes 
%   possiveis para esse espaco, tal como descrito na Seccao 2.1, no passo 3a.
atribui_comuns([]):- !.
atribui_comuns([Perms|R_Perms]):-
    nth1(2, Perms, Perm),
    numeros_comuns(Perm, Com),
    nth1(1, Perms, Posicoes), 
    atribui_comuns_aux(Posicoes, Com),
    atribui_comuns(R_Perms).

% unifica cada variavel comum com o respetivo numero em comum
atribui_comuns_aux(_, []):- !.
atribui_comuns_aux(Posicoes, [Com|R_Com]):-
    pos_faz_comum(Com, Pos), num_faz_comum(Com, Num),
    nth1(Pos, Posicoes, Var), Var = Num,
    atribui_comuns_aux(Posicoes, R_Com).


% =========================================================================== %

% retira_impossiveis/2
% - Retorna  Novas_Perms_Possiveis, o resultado de tirar permutacoes impossiveis
%   de Perms_Possiveis, tal como descrito na Seccao 2.1, no passo 3b.
retira_impossiveis(Perms_Possiveis, Novas_Perms_Possiveis):-
    retira_impossiveis(Perms_Possiveis, [], Novas_Perms_Possiveis).

retira_impossiveis([], Novas_Perms_Possiveis, Novas_Perms_Possiveis):- !.
retira_impossiveis([Perms|R_Perms], Aux, Novas_Perms_Possiveis):-
    nth1(1, Perms, Posicoes),
    nth1(2, Perms, Perm_poss),
    % Exemplo:
    % - Posicoes = [1, 2, _1234, _5678, 4],
    % - Perms_poss = [[1,2,3,5,4],[3,2,1,7,4], [1,2,9,8,4]],
    %  bagof(Posicoes, member(Posicoes,Perms_poss), P).
    % - P = [[1, 2, 3, 5, 4], [1, 2, 9, 8, 4]].
    % forca uma unificacao com todas as perms que tenham 1, 2 e 4 nas
    % mesmas posicoes que a lista inicial.
    bagof(Posicoes, member(Posicoes,Perm_poss), Perms_poss),
    % cria a nova Perm possivel e adiciona ah lista final
    append([Posicoes], [Perms_poss], New_Perms_Poss),
    append(Aux, [New_Perms_Poss], New_Aux),
    retira_impossiveis(R_Perms, New_Aux, Novas_Perms_Possiveis).


% =========================================================================== %

% simplifica/2
% - Retorna Novas_Perms_Possiveis, o resultado de simplificar Perms_Possiveis,   
%   talcomo descrito na Seccao 2.1, no passo 3.
simplifica(Perms_Possiveis, Novas_Perms_Possiveis):-
    atribui_comuns(Perms_Possiveis),
    % retiramos os impossiveis a Perms_Possiveis, utilizando New_Perms_Poss
    % como a iteracao de comparacao
    retira_impossiveis(Perms_Possiveis, New_Perms_Poss),
    % utiliza-se Perms_Possiveis sem qualquer alteracao para a iteracao anterior
    simplifica_aux(Perms_Possiveis, New_Perms_Poss, Novas_Perms_Possiveis).

simplifica_aux(Anterior, Perms_Possiveis, Res_Perms_Possiveis):-
    % caso a interecao do predicado simplifica_aux ja nao causar quaiqueres
    % mudancas em Perms_Possiveis, entao chegamos ao caso de paragem
    Perms_Possiveis == Anterior,
    % Aplica-se entao atribui_comuns e retira_impossiveis uma ultima vez.
    atribui_comuns(Perms_Possiveis), !,
    retira_impossiveis(Perms_Possiveis, Res_Perms_Possiveis).

simplifica_aux(_, Perms_Possiveis, Novas_Perms_Possiveis):-
    % se chegou aqui entao Anterior foi diferente de Perms_Possiveis
    atribui_comuns(Perms_Possiveis),
    retira_impossiveis(Perms_Possiveis, New_Perms_Poss),
    simplifica_aux(Perms_Possiveis, New_Perms_Poss, Novas_Perms_Possiveis).


% =========================================================================== %

% inicializa/2
% - Retorna Perms_Possiveis, a lista de permutacoes possiveis simplificada para
%   Puzzle
inicializa(Puzzle, Perms_Possiveis):-
    espacos_puzzle(Puzzle, Espacos),
    permutacoes_possiveis_espacos(Espacos, Perms_Poss),
    simplifica(Perms_Poss, Perms_Possiveis).


% =========================================================================== %

% goals usados para o findall e o bagof, respectivamente.
goal_fdall(Esc_Poss, Perms):- member(Esc, Esc_Poss), nth1(2, Esc, Perms).
goal_esc(Esc):- nth1(2, Esc, Perms), length(Perms, N), N>1.

% escolhe_menos_alternativas/2
% - Retorna Escolha, o elemento de Perms_Possiveis escolhido segundo o criterio 
%   indicado na Seccao 2.2, no passo 1. Se todos os espacos em Perms_Possiveis tiverem  
%   associadas listas de permutacoes unitarias, o predicado devolve false.
escolhe_menos_alternativas(Perms_Possiveis, Escolha):-
    bagof(Esc, (member(Esc, Perms_Possiveis), goal_esc(Esc)), Esc_Poss),
    % a lista lens contem o numero de permutacoes de cada escolha possivel
    findall(N, (goal_fdall(Esc_Poss, Perms), length(Perms, N)), Lens),
    % vamos buscar o minimo dessa lista e descobrimos o indice do mesmo
    % vamos buscar a escolha de mesmo indice que Min ah lista Esc_Poss
    min_list(Lens, Min), nth1(Ind, Lens, Min), nth1(Ind, Esc_Poss, Escolha), !;
    fail.


% =========================================================================== %

% subtitui/4 (predicado auxiliar)
% - Recebe dois valores, Old e New e uma lista, e retorna uma nova lista em 
%   que todas as ocorrencias de Old foram subtituidas por New.
substitui(_, _, [], []):- !.
substitui(Old, New, [Old|R_Lst], [New|R_Res]):- 
    substitui(Old, New, R_Lst, R_Res).
substitui(Old, New, [P|R_Lst], [P|R_Res]):-
    P \= Old,
    substitui(Old, New, R_Lst, R_Res).

% experimenta_perm/3
% - Perms_Possiveis eh uma lista de permutacoes possiveis, e Escolha eh um dos seus 
%   elementos (escolhido pelo predicado anterior), segue os seguintes passos:
% -----------------------------------------------------------------------------------
% 1. Sendo Esp e Lst_Perms o espaco e a lista de permutacoes de Escolha, respectivamente,  
%    escolhe uma permutacao de Lst_Perms, Perm. 
% 2. Unifica Esp com Perm.
% 3. Novas_Perms_Possiveis eh o resultado de substituir, em Perms_Possiveis, o 
%    elemento Escolha pelo elemento [Esp, [Perm]].
experimenta_perm(Escolha, Perms_Possiveis, Novas_Perms_Possiveis):-
    nth1(1, Escolha, Esp), nth1(2, Escolha, Lst_Perms),
    member(Perm, Lst_Perms),
    Esp = Perm, append([Esp], [[Perm]], Subst),
    substitui(Escolha, Subst, Perms_Possiveis, Novas_Perms_Possiveis).


% =========================================================================== %

%  Predicadoresolve_aux/2
% - Retorna Novas_Perms_Possiveis, o resultado de aplicar o algoritmo descrito na 
%   Seccao 2.2 a Perms_Possiveis.
resolve_aux(Perms_Possiveis, Perms_Possiveis):-
    % quando escolhe_menos_alternativas retornar false, ou seja, ja nao houverem 
    % alternativas a escolher, o predicado acaba.
    \+escolhe_menos_alternativas(Perms_Possiveis, _).

resolve_aux(Perms_Possiveis, Novas_Perms_Possiveis):-
    escolhe_menos_alternativas(Perms_Possiveis, Escolha),
    experimenta_perm(Escolha, Perms_Possiveis, Experimenta),
    simplifica(Experimenta, New_Perms_Poss),
    resolve_aux(New_Perms_Poss, Novas_Perms_Possiveis).


% =========================================================================== %

% resolve/1
% - Resolve o puzzle Puz, isto eh, apos a invocacao deste predicado a grelha de Puz  
%   tem todas as variaveis substituidas por numeros que respeitam as restricoes Puz.
resolve(Puz):-
    inicializa(Puz, Perms_Possiveis),
    resolve_aux(Perms_Possiveis, _), !.