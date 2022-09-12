
# 1.2

# 1.2.1
def corrigir_palavra(word):
    """
    corrigir_palavra: cad.carateres -> cad.caratere

     - recebe uma cadeia de e carateres que representa uma palavra 
       (potencialmente modificada por um surto  de  letras) e devolve 
       a cadeia de carateres que corresponde a aplicacao da sequencia 
       de reducoes conforme descrito para obter a palavra corrigida.

    """

    length = len(word)
    i = 0

    while i < length:
        
        if length <= 2 or i+1 == length:
            break
        
        elif abs(ord(word[i]) - ord(word[i+1])) == 32:
            word = word[:i] + word[i+2:]
            i = 0
            length = len(word)

        else:
            i += 1
    
    if length == 2 and abs(ord(word[0]) - ord(word[1])) == 32:
        word = ''

    return word



# 1.2.2
def eh_anagrama(word1, word2):
    """
    ehanagrama:  cad.carateres x cad.carateres -> booleano

     - Esta funcao recebe duas cadeias de carateres correspondentes 
       a duas palavras e devolve True se e so se uma eh anagrama da outra,
       isto eh, se as palavras sao constituidas pelas mesmas letras,  
       ignorando diferencas entre maiusculas e minusculas e a  
       ordem entre carateres.

    """

    dic1 = char_counter(word1.lower())
    dic2 = char_counter(word2.lower())

    return dic1 == dic2



# 1.2.3
def corrigir_doc(doc):
    """
    corrigi_rdoc: cad.carateres -> cad.carateres

     - Esta funcao recebe uma cadeia de carateres que representa 
       o texto com erros da documentacao da BDB e devolve a cadeia 
       de carateres filtrada com as palavras corrigidas e os anagramas 
       retirados, ficando apenas a sua primeira ocorrencia. Os anagramas 
       sao avaliados apos a correcao das palavras e apenas sao retirados 
       anagramas que correspondam apalavras diferentes (sequencia de 
       carateres diferentes das palavras anteriores ignorando diferencas  
       de maiusculas e minusculas).

    """

    check = doc.split(" ")

    if type(doc) != str or len(doc) == 0:
        raise ValueError("corrigir_doc: argumento invalido")
    
    for word in check:
        if word == " ":
            raise ValueError("corrigir_doc: argumento invalido")

    filtered = corrigir_palavra(doc).split()
    length = len(filtered)
    i = 0

    while i < length:

        if length <= 2 or i+1 == length:
            break

        elif eh_anagrama(filtered[i], filtered[i+1]):
            filtered = filtered[:i+1] + filtered[i+2:]
            i = 0
            length = len(filtered)
        
        i += 1
    
    if length == 2 and  eh_anagrama(filtered[0], filtered[1]):
        filtered = filtered[0]

    res = " "
    return res.join(filtered)



#================================================================#



# 2.2

# 2.2.1
def obter_posicao(direction, pos):
    """
    obter_posicao: cad.carateres x inteiro -> inteiro

     - Esta funcao recebe uma cadeia de carateres contendo apenas um 
       caracter que representa a direcao de um unico movimento  
       ('C', 'B', 'E' ou 'D') e um inteiro representando a posicao
       atual (1, 2, 3, 4, 5, 6, 7, 8 ou 9) e devolve o inteiro que 
       corresponde a nova posicao apos do movimento.

    """

    if direction == 'C':
        if pos in [1,2,3]:
            return pos
        return pos-3

    elif direction == 'B':
        if pos in [7,8,9]:
            return pos
        return pos+3

    elif direction == 'E':
        if pos in [1,4,7]:
            return pos
        return pos-1

    elif direction == 'D':
        if pos in [3,6,9]:
            return pos
        return pos+1



# 2.2.2
def obter_digito(directions, pos):
    """
    obter_digito: cad.carateres x inteiro -> inteiro

     -  

    """

    if len(directions) == 0:
        return pos
    else:
        return obter_digito(directions[1:], obter_posicao(directions[0], pos))



# 2.2.3
def obter_pin(directions):
    """
    obter_pin: tuplo -> tuplo

     - 
     
    """

    error_flag = 0
    pin = ()
    current = 5

    if len(directions) < 4 or len(directions) > 10:
        error_flag = 1

    for mov in directions:

        if len(mov) == 0: error_flag = 1

        for letter in mov:
            if letter not in ('C','B','E','D'):
                error_flag = 1

        current = obter_digito(mov, current)
        pin += (current, )
    
    if error_flag == 1:
        raise ValueError("obter_pin: argumento invalido")

    return pin



#================================================================#


# 3.2

# 3.2.1
def eh_entrada(object):
    """
    """
    i = 0

    if (type(object)!=tuple or len(object)!=3 or type(object[0])!=str or
        type(object[1])!=str or type(object[2])!=tuple):
        return False

    cipher = object[0].split("-")

    if len(cipher) == 0:
        return False

    if len(object[1]) != 7:
        return False

    checksum = object[1][1:6]

    for i in range(0, 5):
        if (not checksum[i].isalpha) or (not checksum[i].islower()):
            return False

    if len(object[2]) == 0:
        return False
    
    for number in object[2]:
        if number <= 0:
            return False
    
    return True


# 3.2.2
def validar_cifra(cipher, checksum):
    """
    """

    cipher = cipher.split("-")

    if len(cipher) == 0:
        return False

    if len(checksum) != 7:
        return False

    cipher_dic = {}

    for word in cipher:

        for char in word:
            if (not char.isalpha()) or (not char.islower()):
                return False

            else:
                if char in cipher_dic:
                    cipher_dic[char] += 1
                else:
                    cipher_dic[char] = 1
    
    checksum = checksum[1:6]
    correct = dict_sort(cipher_dic)

    for i in range(0, 5):
        if (correct[i] != checksum[i]):
            return False
    
    return True



# 3.2.3
def filtrar_bdb(entry):
    """
    """
    if type(entry) != list or len(entry) == 0:
        raise ValueError('filtrar_bdb: argumento invalido')

    res = []

    for e in entry:

        if validar_cifra(e[0], e[1]) == False:
            res += [e, ]
    
    return res


#================================================================#


# funcoes auxiliares

# 1. (usada em eh_anagrama(word) e eh_entrada(object))
def char_counter(word):
    """
    char_counter: cad.caracteres -> dicionario

     - Esta funcao recebe uma cadeia de caracteres e retorna um
       dicionario em que cada chave eh uma letra da palavra e cada valor
       eh a quantidade de ocorrencias de cada uma dessas mesmas letras

    """
    dic = {}

    for char in word:
        if char in dic:
            dic[char] += 1
        else:
            dic[char] = 1

    return dic


def dict_sort(d):
    """
    """
    res = []

    l = list(d.items())
    l.sort(key = lambda x: x[0])
    l.sort(key = lambda x: x[1], reverse= True)

    for el in l:
        res += [el[0],]
        
    return res[:5]

        
        

            



if __name__ == '__main__':
    
    t = {'b':2, 'c':1, 'a':2, 'e':1, 'd':1, 's':3, 'z':4}
    print(dict_sort(t))
