;=============================( constantes )=============================;
; terminal
TERM_READ       EQU     FFFFh	; read characters
TERM_WRITE      EQU     FFFEh	; write characters
TERM_STATUS     EQU     FFFDh	; status (0-no key pressed; 1-key pressed)
TERM_CURSOR     EQU     FFFCh	; position the cursor
TERM_COLOR      EQU     FFFBh

cor_dino        EQU     F4h
cor_cacto       EQU     18h
cor_nao_cacto   EQU     0h
cor_terreno     EQU     d8h
cor_fim         EQU     FFh

; 7 segment display
DISP7_D0        EQU     FFF0h
DISP7_D1        EQU     FFF1h
DISP7_D2        EQU     FFF2h
DISP7_D3        EQU     FFF3h
DISP7_D4        EQU     FFEEh
DISP7_D5        EQU     FFEFh

; timer
TIMER_CONTROL   EQU     FFF7h
TIMER_COUNTER   EQU     FFF6h
TIMER_SETSTART  EQU     1
TIMER_SETSTOP   EQU     0
TIMERCOUNT_INIT EQU     1000

; interrupcoes
INT_MASK        EQU     FFFAh
INT_MASK_VAL    EQU     80FFh ; (permite controlar as interrupcoes)

dim             EQU     80          ; dimensao da tabela 
alturaM         EQU     4           ; altura maxima dos cactos gerados
coluna_dino     EQU     5

;=============================( variaveis )=============================;
ORIG            3000h
X               WORD    10          ; seed inicial
contador        WORD    0           ; contador
game_over       STR     '-GAMEOVER-'
Restart         STR     '  press 0 to restart'

pos_dino        WORD    2805h
salto           WORD    0
salto_switch    WORD    0

TIMER_COUNTVAL  WORD    TIMERCOUNT_INIT ; states the current counting period
Score0          WORD    0
Score1          WORD    0
Score2          WORD    0
Score3          WORD    0

; tabela 
ORIG            1000h
Colunas         TAB     dim
    
;===============================( inicio )===============================;
ORIG            0000h
int_set:        MVI     R6, 7000h
                MVI     R1,INT_MASK     ; int mask 
                MVI     R2,INT_MASK_VAL
                STOR    M[R1],R2
                
                MOV     R1, R0          ; poe R1 a 0, valor inicial para p CMP
                ENI                     ; enable interruptions

;===========================( evento inicial )============================;
; este escerto ira testar se o botao 0 do P4 foi pressionado, caso este evento
; se confirme, o programa principal pode começar
starter_event:  MVI     R2, INT_MASK_VAL
                AND     R1, R1, R2
                CMP     R1, R0
                BR.Z      starter_event
                
                DSI                     ; disable interruptions 
                
                LOAD    R1,M[R6]        ; POP R1
                INC     R6


inicio:         MVI     R6, 7000h
                MVI     R1,INT_MASK     ; int mask 
                MVI     R2,INT_MASK_VAL
                STOR    M[R1],R2
                
                MVI     R1, TERM_CURSOR
                MVI     R2, 2900h
                STOR    M[R1], R2   ; os caracteres serao escritos a partir da 
                                    ; pos 2900h
                MVI     R4, contador; poe em R4 um contador que começa a 0
                MVI     R1, TERM_COLOR
                MVI     R2, cor_terreno
                STOR    M[R1], R2

;======================( terreno de jogo inicial )=======================;
; desenha no terminal o terreno de jogo, ainda sem nenhum cacto 
terreno_inicial:LOAD    R2, M[R4]
                INC     R2            ; aumenta o contador 
                STOR    M[R4], R2     ; atualiza o contador
                MVI     R1, 321 
                CMP     R1, R2        ; ve se ja chegou ao fim do terminal 
                
                BR.Z    comeca        ; se nao chegou ao fim continua 
 
                MVI     R1, TERM_WRITE
                MVI     R2, '▒'
                STOR    M[R1], R2
                BR      terreno_inicial
                
;-----------------------------------------------------------------------;
comeca:         MVI     R1, TERM_CURSOR
                MVI     R4, 2800h
                STOR    M[R1], R4

                MVI     R4, dim     ; dimensao da tab
                
                
                MVI     R5, X
                LOAD    R5, M[R5]   ; variavel global (seed)
                MVI     R4, dim     ; dimensao da tab 
                MVI     R1, Colunas ; poe em R1 a primeira pos da tab
                
;============================( timer event )============================;
; espera por um timer event para poder chamar as funcoes auxiliares 
; que tratam do timer
timer_event:    DEC     R6
                STOR    M[R6], R1
                DEC     R6
                STOR    M[R6], R2
                
                MVI     R1,TIMER_CONTROL
                MVI     R2,TIMER_SETSTART
                STOR    M[R1],R2          ; start timer
                ENI
                
                DSI
                LOAD    R2, M[R6]
                INC     R6
                LOAD    R1, M[R6]
                INC     R6
                
                
                JAL     atualizajogo
                
;============================( atualizajogo )=============================;
; funcao que trata da atualizacao do tabuleiro de jogo
atualizajogo:   MOV     R2, R1      ; R2 tem a pos(n) da tab
                INC     R1          ; R1 tem a pos(n+1)
                LOAD    R3, M[R1]   ; guarda a pos(n+1) 
                STOR    M[R2], R3   ; guarda na pos(n) a memoria da pos(n+1) 
                                    ; (move a tab para a esquerda)
              
;=========================( desenha no terminal )=========================;
; esta parte do codigo analisa cada posicao da tabela enquanto esta a ser
; movida para a esquerda, "desenhando" no terminal simbolos consoante a 
; posicao tenha um cacto ou nao 

                DEC     R6
                STOR    M[R6], R1  ; PUSH R1 
                DEC     R6
                STOR    M[R6], R2  ; PUSH R2
                DEC     R6
                STOR    M[R6], R3

                LOAD    R2, M[R2]  ; da load da memoria da posicao a analisar
                MOV     R3, R2     ; poe em R3 a altura do cacto
                
                DEC     R6
                STOR    M[R6], R2  ; PUSH R2 (preserva o R2 para quando 
                                   ; acabar de desenhar o dino)
                DEC     R6
                STOR    M[R6], R3  ; PUSH R3
                DEC     R6
                STOR    M[R6], R4
                DSI
                
;===============================( colisoes )===============================;
; verifica se houve alguma colisao
; R2 --> contem a coluna onde esta o dinossauro
; R3 --> contem a memoria dessa mesma coluna (altura do cacto)
                MVI     R2, Colunas
                MVI     R1, coluna_dino
                ADD     R2, R1, R2
                LOAD    R3, M[R2]
;----------------------------------------------------;           
colisoes:       MVI     R1, salto      ; altura do dino 
                LOAD    R1, M[R1]
                CMP     R1, R0         ; se essa altura for 0, ele esta no chao
                BR.P    salto_ar             ; se nao for 0, ele esta a saltar
                
                INC     R1          ; da ao dino altura de 1 (dino esta no chao)
                CMP     R1, R3         ; ve se colidio com um cacto
                JMP.P   desenha_dino
                BR      Game_over

salto_ar:       MVI     R1, salto      ; poe em R1 a altura do dino
                LOAD    R1, M[R1]
                INC     R1             ; poe a altura a 1 visto que comeca a 0
                                       ; quando o dino esta no chao
                CMP     R1, R3         ; ve se bateu num cacto
                BR.NP   Game_over
                JMP.P   desenha_dino   ; se nao, continua a desenhar o dino
                
;----------------------------------------------------;
Game_over:      MVI     R1, TERM_COLOR
                MVI     R2, cor_fim
                STOR    M[R1], R2
                MVI     R1, TERM_CURSOR 
                MVI     R2, 1224h     ; 
                STOR    M[R1], R2
                MVI     R3, 10        ; contador com o numero de letras da STR
                MVI     R2, game_over

; loop que escreve na posicao 141Eh a STR 'GAMEOVER'
escrita:        MVI     R1, TERM_WRITE
                DEC     R6
                STOR    M[R6], R3
                
                LOAD    R3, M[R2]
                STOR    M[R1], R3
                
                LOAD    R3, M[R6]
                INC     R6
                
                DEC     R3
                CMP     R3, R0
                BR.Z    restart
                INC     R2
                BR      escrita
                
;----------------------------------------------------;       
restart:        MVI     R1, TERM_CURSOR 
                MVI     R2, 141Eh
                STOR    M[R1], R2
                MVI     R3, 20        ; contador com o numero de letras da STR
                MVI     R2, Restart

; loop que escreve na posicao 141Eh a STR 'press 0 to restart'
escrita2:       MVI     R1, TERM_WRITE
                DEC     R6
                STOR    M[R6], R3
                
                LOAD    R3, M[R2]
                STOR    M[R1], R3
                
                LOAD    R3, M[R6]
                INC     R6
                
                DEC     R3
                CMP     R3, R0
                BR.Z    reiniciar
                INC     R2
                BR      escrita2
                
; da POP de todos os registos na pilha
reiniciar:      LOAD    R4, M[R6]
                INC     R6
                LOAD    R3, M[R6]  ; POP R3
                INC     R6
                LOAD    R2, M[R6]  ; POP R2
                INC     R6
                
                LOAD    R3, M[R6]  ; POP R3
                INC     R6
                LOAD    R2, M[R6]  ; POP R2
                INC     R6
                LOAD    R1, M[R6]  ; POP R1
                
                LOAD    R5, M[R6]  ; POP R5
                INC     R6 
                LOAD    R4, M[R6]  ; POP R4
                INC     R6 
                LOAD    R7, M[R6]  ; POP R7
                INC     R6
;----------------------------------------------------;
                MOV     R1, R0
                ENI
                MVI     R2, INT_MASK_VAL
                ENI
; espera que o botao 0 seja pressionado
LOOP:           AND     R1, R1, R2
                CMP     R1, R0
                BR.Z    LOOP
                DSI     
                
                ; limpa o ecra
                MVI     R1, TERM_CURSOR
                MVI     R2, FFFFh
                STOR    M[R1], R2
                
                ; reset do contador 
                MVI     R4, contador
                MVI     R1, 0
                STOR    M[R4], R1
                
                ; reset dos scores 
                MVI     R1, Score0
                MVI     R2, 0
                STOR    M[R1], R2
                MVI     R1, DISP7_D0
                STOR    M[R1], R2
                MVI     R1, Score1
                MVI     R2, 0
                STOR    M[R1], R2
                MVI     R1, DISP7_D1
                STOR    M[R1], R2
                MVI     R1, Score2
                MVI     R2, 0
                STOR    M[R1], R2
                MVI     R1, DISP7_D2
                STOR    M[R1], R2
                MVI     R1, Score3
                MVI     R2, 0
                STOR    M[R1], R2
                MVI     R1, DISP7_D3
                STOR    M[R1], R2
                
                ; limpa a tabela com as posicoes dos cactos
                MVI     R1, Colunas 
                MVI     R3, contador
                LOAD    R3, M[R3]


; coloca o valor 0 em todas as posicoes da tabela, para que assim se possa 
; reiniciar o jogo
limpa_tab:      MVI     R2, 0
                STOR    M[R1], R2
                INC     R1
                MVI     R2, dim
                INC     R3
                CMP     R2, R3
                BR.NZ   limpa_tab
                
                ; reset do contador 
                MVI     R4, contador
                MVI     R1, 0
                STOR    M[R4], R1
                
                JMP     inicio   ; volta para o inicio do programa para se poder
                                 ; volta a jogar  

;============================( desenha o dino )============================;
; coloca o cursor na posicao 2805h, e desenha nessa mesma o dinossauro

desenha_dino:   MVI     R1, TERM_COLOR
                MVI     R2, cor_dino
                STOR    M[R1], R2 
                MVI     R2, pos_dino ; variavel com a pos atual do dinossauro
                LOAD    R2, M[R2]
                MVI     R1, TERM_CURSOR
                STOR    M[R1], R2
                MVI     R1, TERM_WRITE
                MVI     R2, 'Γ'      ; simbolo que representa o dino
                STOR    M[R1], R2
                
                LOAD    R4, M[R6]  ; POP R4
                INC     R6
                LOAD    R3, M[R6]  ; POP R3
                INC     R6
                LOAD    R2, M[R6]  ; POP R2
                INC     R6
                
;=========================( desenha no terminal )=========================;
; esta parte do codigo analisa cada posicao da tabela enquanto esta a ser
; movida para a esquerda, "desenhando" no terminal simbolos consoante a 
; posicao tenha um cacto ou nao 

;===========================( desenha os cactos )===========================;
; analisa o valor da altura e se esta for maior que 0, desenha um cacto nessa
; posicao 
cactos:         CMP     R3, R0    
                BR.Z    nao_cacto  ; se a posicao tiver um 0, nao ha cacto
                
;--------------------------------------------------------------------------;
; existe cacto
desenha:        MVI     R2, 2800h  ; linha onde serao desenhados os simbolos
                MVI     R1, dim    
                SUB     R1, R1, R4 
; guarda em R1 a diferença entre a dimencao da tabela e o R4 (nosso contador), 
; ou seja, se R4 for igual a 70 quer dizer que foram movidas as primeiras 10
; posicoes da tabela, e nao as primeiras 70. Desta forma teremos em R1, o valor
; exato da posicao que queremos                
                ADD     R2, R2, R1 ; coluna onde sera desenhado o cacto
                
loop:           DEC     R3         ; percorre toda a altura do cacto 
                MVI     R1, TERM_CURSOR
                STOR    M[R1], R2  ; poe o cursor na coluna pretendida
                
                DEC     R6
                STOR    M[R6], R2  ; PUSH R2 por instantes para se recuperar
                MVI     R1, TERM_COLOR
                MVI     R2, cor_cacto
                STOR    M[R1], R2 
                MVI     R1, TERM_WRITE
                MVI     R2, '║'  
                STOR    M[R1], R2  ; escreve no terminal o simbolo
                LOAD    R2, M[R6]
                INC     R6
                
                CMP     R3, R0     ; se ja desenhou o cacto todo, sai da funcao
                BR.Z    saida
                
                MVI     R1, 100h
                SUB     R2, R2, R1 ; poe o cursor na linha acima
                BR      loop

saida:          BR      volta      ; sai da funcao
;--------------------------------------------------------------------------;
; nao existe cacto
nao_cacto:      MVI     R2, 2800h
                MVI     R1, dim
                SUB     R1, R1, R4 
                ADD     R2, R2, R1
                MVI     R3, alturaM
                
loop2:          DEC     R3
                MVI     R1, TERM_CURSOR
                STOR    M[R1], R2  ; poe o cursor na coluna pretendida
                
                DEC     R6
                STOR    M[R6], R2  ; PUSH R2 por instantes para se recuperar
                MVI     R1, TERM_COLOR
                MVI     R2, cor_nao_cacto
                MVI     R1, TERM_WRITE
                MVI     R2, ' '  
                STOR    M[R1], R2  ; desenha no terminal um ' ' ou seja, n ha cacto
                LOAD    R2, M[R6]
                INC     R6
                
                CMP     R3, R0
                BR.Z    volta      ; sai da funcao
                
                MVI     R1, 100h
                SUB     R2, R2, R1
                BR      loop2 
                
;--------------------------------------------------------------------------;
; saida deste excerto da funcao. Continua-se a mover a tabela                
volta:          DSI
                LOAD    R3, M[R6]  ; POP R3
                INC     R6
                LOAD    R2, M[R6]  ; POP R2
                INC     R6
                LOAD    R1, M[R6]  ; POP R1
                INC     R6     
;--------------------------------------------------------------------------;
                
                DEC     R4          ; decrementa a dim da tabela 
                CMP     R4, R0      ; ve se ja chegou ao fim da tab
                BR.Z    ultima_pos  
                JMP     atualizajogo; se ainda n chegou ao fim continua a mover 
                                    ; a tab 
                
;==========================( ultima pos da tab )==========================;
; guarda em R2 (ultima pos da tab), R3 (altura do cacto)
; ou seja, preenche a tabela com os valores das alturas dos cactos
                
ultima_pos:     DEC     R6
                STOR    M[R6], R7   ; PUSH R7
                DEC     R6
                STOR    M[R6], R2   ; PUSH R2
                DEC     R6
                STOR    M[R6], R1   ; PUSH R1
                
                JAL     Geracacto
                
                LOAD    R1, M[R6]   ; POP R1
                INC     R6 
                LOAD    R2, M[R6]   ; POP R2
                INC     R6
                LOAD    R7, M[R6]   ; POP R7
                INC     R6
                
                STOR    M[R2], R3   ; guarda em R2 o valor da altura do cacto
                                    ; quer haja cacto ou nao
                
                MVI     R1, Colunas ; prepara a proxima iteracao da atualizajogo
                MVI     R4, dim
                
                DEC     R6
                STOR    M[R6], R1  ; PUSH R1 
                DEC     R6
                STOR    M[R6], R2  ; PUSH R2
                
;=============================( salto check )==============================;
; esta parte da funcao verifica se existe algum salto em curso, e se sim, 
; continua o movimento do salto
                
                MVI     R2, salto_switch
                LOAD    R2, M[R2]
                CMP     R2, R0
                BR.NZ   check_DOWN
                
                MVI     R2, salto
                LOAD    R1, M[R2]
                CMP     R1, R0
                BR.Z    sem_salto
                MVI     R2, alturaM
                INC     R2
                INC     R2
                CMP     R1, R2
                BR.NZ   check_UP

;=============================( salto_event )==============================;
; ativa as interrupcoes e verifica se o botao '↑' (KEY_UP) foi pressionado, e 
; se sim, ativa um salto 
                
sem_salto:      MOV     R2, R0

salto_event:    ENI
                MVI     R1, INT_MASK_VAL
                AND     R2, R2, R1
                CMP     R2, R0
                BR.Z    sai_salto    ; se o botao nao foi pressionado, continua
                                     ; o funcionamento normal 

;===============================( check salto )=============================;
; caso exista um salto a acontecer, prepara os registos para o devido movimento
; do salto (ascendente ou descendente) --> UP ou DOWN
check_UP:       DEC     R6
                STOR    M[R6], R1  ; PUSH R1 
                DEC     R6
                STOR    M[R6], R2  ; PUSH R2
                BR      salto_UP

;-----------------------------------------------------;
check_DOWN:     DEC     R6
                STOR    M[R6], R1  ; PUSH R1 
                DEC     R6
                STOR    M[R6], R2  ; PUSH R2
                BR      salto_DOWN

;=======================( salto(movimento de subida) )======================;
salto_UP:       DSI
                MVI     R2, pos_dino
                LOAD    R2, M[R2]
                MVI     R1, TERM_CURSOR
                STOR    M[R1], R2
                MVI     R1, TERM_WRITE
                MVI     R2, ' '        ; "apaga" a pos anterior do dinossauro
                STOR    M[R1], R2
;--------------------------------------------------------------------------;
                MVI     R2, salto
                LOAD    R1, M[R2]
                INC     R1             ; inc variavel salto_up
                STOR    M[R2], R1
                
                MVI     R2, alturaM    ; ve se o salto_up atingiu a alturaM + 2
                INC     R2
                INC     R2 
                CMP     R2, R1
                BR.NZ   UP_continua    ; se nao chegou, continua a subir 
                
; caso ja tenha atingido a alturaM, repoe a variavel salto_up com o valor 0 para
; nao continuar a subir infinitamente. Para alem disso, poe a variavel salto_down
; a 0, para que seja possivel comecar o movimento de descida do dino
                
                MVI     R1, salto_switch
                MVI     R2, 1      ; na proxima iteracao vai para o check_down
                STOR    M[R1], R2 
                
;--------------------------------------------------------------------------;
UP_continua:    MVI     R2, pos_dino
                LOAD    R2, M[R2]
                MVI     R1, 100h
                SUB     R1, R2, R1     ; decrementa o valor do cursor, ou seja,
                                       ; da-nos o valor da linha acima 
                MVI     R2, pos_dino
                STOR    M[R2], R1 
                MVI     R2, TERM_CURSOR
                STOR    M[R2], R1
                MVI     R1, TERM_COLOR
                MVI     R2, cor_dino
                STOR    M[R1], R2 
                MVI     R1, TERM_WRITE
                MVI     R2, 'Γ'        ; "desenha-se" o dinossauro  
                STOR    M[R1], R2
                
                LOAD    R2, M[R6]
                INC     R6
                LOAD    R1, M[R6]
                INC     R6
                
                BR      sai_salto                      

;=====================( salto(movimento de descida) )======================;
salto_DOWN:     DSI
                MVI     R2, pos_dino
                LOAD    R2, M[R2]
                MVI     R1, TERM_CURSOR
                STOR    M[R1], R2
                MVI     R1, TERM_WRITE
                MVI     R2, ' '
                STOR    M[R1], R2
;--------------------------------------------------------------------------;
                MVI     R2, salto
                LOAD    R1, M[R2]
                DEC     R1              ; inc da variavel salto_down
                STOR    M[R2], R1
                
                CMP     R1, R0          ; ve se ja chegou ao chao 
                BR.NZ   DOWN_continua
                
                MVI     R1, salto_switch  ; se tal aconteceu, repoe a variavel 
                                          ; no seu estado inicial
                MOV     R2, R0
                STOR    M[R1], R2
;--------------------------------------------------------------------------;
DOWN_continua:  MVI     R2, pos_dino
                LOAD    R2, M[R2]
                MVI     R1, 100h
                ADD     R1, R2, R1
                MVI     R2, pos_dino    ; vai desendo de linha e dando update ao
                                        ; pos_dino
                STOR    M[R2], R1 
                MVI     R2, TERM_CURSOR
                STOR    M[R2], R1
                MVI     R1, TERM_COLOR
                MVI     R2, cor_dino
                STOR    M[R1], R2 
                MVI     R1, TERM_WRITE
                MVI     R2, 'Γ'
                STOR    M[R1], R2
                
                LOAD    R2, M[R6]
                INC     R6
                LOAD    R1, M[R6]
                INC     R6
                
                BR      sai_salto
                
;==========================================================================;
; sai da funcao que trata dos saltos e retorna o funcionamento normal
sai_salto:      DSI     
                LOAD    R2, M[R6]
                INC     R6
                LOAD    R1, M[R6]
                INC     R6

;--------------------------------------------------------------------------;
saida_final:    JMP     timer_event     

;==============================( Geracacto )===============================;
; recebe como argumentos, R5(contem a seed escolhida)
; retorna em R3, a altura do cacato, caso este seja gerado

Geracacto:      ; R5 tem a a seed (X)
                MVI     R2, 1
                AND     R1, R5, R2 ; bit = x & 1 (AND bit a bit)
                SHR     R5         ; x = x >> 1  (shift right de x)
                CMP     R1, R0     ; if bit (se a condicao se verificar)
                BR.NZ   AlteraX


;================( calcula a probabilidade de haver cacto )================;

Probabilidade:  MVI     R1, 62258d ; o endereço que nos da a probabilidade 95%
                CMP     R5, R1     
                BR.NC   Cacto      ; se nao houver carry, ha cacto
                BR      Cacto0     
           
Cacto:          MVI     R2, alturaM; if x < 62258
                DEC     R2         ; altura - 1
                AND     R3, R5, R2 ; (x & (altura - 1))
                INC     R3         ; (x & (altura - 1)) + 1
                ; R3 sai da geracacto com o valor da altura do cacto 
                ; valor entre 1 e AlturaM
                JMP     R7

;--------------------------------------------------------------------------;
Cacto0:         MVI     R3, 0       ; não gera cacto
                
                ; R3 sai da geracacto com valor 0 (nao ha cacto)
                
                JMP     R7
                
;=============( retorna um valor "aleatorio" para a seed(x) )==============;
AlteraX:        MVI     R1, b400h
                XOR     R5, R5, R1  ; XOR do x (da-nos uma seed random - x)
                BR      Probabilidade 


;============================( interrupcoes )==============================;
;\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/;
;======================( botao '0' foi pressionado )=======================;
; verifica qunaod o botao 0 e pressionado e guarda em R1 '10h' para ser
; testado, e por sua vez, comecar o programa inicial
                ORIG    7F00h
KEY_0:          DEC     R6
                STOR    M[R6],R1     ; PUSH R1
                
                MVI     R1, 10h
                RTI
                
;=======================( botao '↑' foi pressionado )=======================;
; verifica se o butao '↑' foi pressionado e se tal inicia o evento 
; correspondente
                ORIG    7F30h
KEY_UP:         MVI     R2, 3h
                RTI
;===========================( funcao timer auxiliar )=======================;
AUX_TIMER:      ; SAVE CONTEXT
                DEC     R6
                STOR    M[R6],R1
                DEC     R6
                STOR    M[R6],R2
                DEC     R6
                STOR    M[R6],R3

; se o score no primeiro display chegar a 10, comeca a incrementar o segundo
; display, e repete esta mesma logica para todos os displays

                ; UPDATE SCORE
                MVI     R1,Score0
                LOAD    R2,M[R1]
                INC     R2
                STOR    M[R1], R2
                MVI     R3, 10
                CMP     R2, R3
                BR.NZ   Disp_0
                MVI     R2, 0
                STOR    M[R1], R2
                MVI     R1, DISP7_D0
                STOR    M[R1], R2
                
                MVI     R1,Score1
                LOAD    R2,M[R1]
                INC     R2
                STOR    M[R1], R2
                MVI     R3, 10
                CMP     R2, R3
                BR.NZ   Disp_1
                MVI     R2, 0
                STOR    M[R1], R2
                MVI     R1, DISP7_D1
                STOR    M[R1], R2
                
                MVI     R1,Score2
                LOAD    R2,M[R1]
                INC     R2
                STOR    M[R1], R2
                MVI     R3, 10
                CMP     R2, R3
                BR.NZ   Disp_2
                MVI     R2, 0
                STOR    M[R1], R2
                MVI     R1, DISP7_D2
                STOR    M[R1], R2
                
                MVI     R1,Score3
                LOAD    R2,M[R1]
                INC     R2
                STOR    M[R1], R2
                MVI     R3, 10
                CMP     R2, R3
                BR.NZ   Disp_3
                MVI     R2, 0
                STOR    M[R1], R2
                MVI     R1, DISP7_D3
                STOR    M[R1], R2
                BR      saida_timer
                
                ; SHOW SCORE ON DISP7_D0
Disp_0:         MVI     R1,DISP7_D0
                STOR    M[R1],R2
                BR      saida_timer
                ; SHOW SCORE ON DISP7_D1
Disp_1:         MVI     R1,DISP7_D1
                STOR    M[R1],R2
                BR      saida_timer
                ; SHOW SCORE ON DISP7_D2
Disp_2:         MVI     R1,DISP7_D2
                STOR    M[R1],R2
                BR      saida_timer
                ; SHOW SCORE ON DISP7_D3
Disp_3:         MVI     R1,DISP7_D3
                STOR    M[R1],R2
                
saida_timer:    MVI     R1,TIMER_CONTROL
                MVI     R2,TIMER_SETSTOP
                STOR    M[R1],R2          ; stop timer
                
                ; RESTORE CONTEXT
                LOAD    R3,M[R6]
                INC     R6
                LOAD    R2,M[R6]
                INC     R6
                LOAD    R1,M[R6]
                INC     R6
                
                
                
                JMP     R7
    
;=============================( TIMER_EVENT )===============================;
; le cada interrupcao do temporizador 
                ORIG    7FF0h
TIMER_EVENT:    DEC     R6
                STOR    M[R6],R7
                
                JAL     AUX_TIMER
                
                LOAD    R7,M[R6]
                INC     R6
                RTI