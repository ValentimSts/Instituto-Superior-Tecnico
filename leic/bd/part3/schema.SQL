
----------------------------------------------
--               schema.SQL                 --
----------------------------------------------
--   Grupo 20:                              --
--   - Valentim Santos ist199343            --
--   - Tiago Santos ist199333               --
--   - Yassir Yassin ist100611              --
----------------------------------------------



DROP TABLE IF EXISTS categoria CASCADE;
DROP TABLE IF EXISTS categoria_simples CASCADE;
DROP TABLE IF EXISTS super_categoria CASCADE;
DROP TABLE IF EXISTS tem_outra CASCADE;
DROP TABLE IF EXISTS produto CASCADE;
DROP TABLE IF EXISTS tem_categoria CASCADE;
DROP TABLE IF EXISTS IVM CASCADE;
DROP TABLE IF EXISTS ponto_de_retalho CASCADE;
DROP TABLE IF EXISTS instalada_em CASCADE;
DROP TABLE IF EXISTS prateleira CASCADE;
DROP TABLE IF EXISTS planograma CASCADE;
DROP TABLE IF EXISTS retalhista CASCADE;
DROP TABLE IF EXISTS responsavel_por CASCADE;
DROP TABLE IF EXISTS evento_reposicao CASCADE;



--
-- categoria(nome)
--    - RI-RE1: O valor do atributo nome de qualquer registo da relação categoria tem
--      de existir na relacao categoria_simples ou na relacao super_categoria.
--
CREATE TABLE categoria (
    nome VARCHAR(100),

    PRIMARY KEY (nome)
);


--
-- categoria_simples(nome)
--    - nome: FK(categoria)
--    - RI-RE2: O valor do atributo nome de qualquer registo de categoria_simples
--      nao pode existir em super_categoria.
--
CREATE TABLE categoria_simples (
    nome VARCHAR(100),

    PRIMARY KEY(nome),
    FOREIGN KEY(nome) references categoria(nome)
);


--
-- super_categoria(nome)
--    - nome: FK(categoria)
--    - RI-RE3: O valor do atributo nome de qualquer registo tem de existir no 
--      atributo super_categoria da relacao constituida.
--
CREATE TABLE super_categoria (
    nome VARCHAR(100),

    PRIMARY KEY(nome), 
    FOREIGN KEY(nome) references categoria(nome)
);


--
-- tem_outra(super_categoria, categoria)
--    - super_categoria: FK(super_categoria)
--    - categoria: FK(categoria)
--    - RI-RE4 nao podem existir valores repetidos dos atributos super_categoria e 
--      categoria numa sequencia de registos relacionados pela FK categoria.
--    - RI-RE5: Para qualquer registo desta relacao, verifica-se que os atributos 
--      super_categoria e categoria sao distintos.
--
CREATE TABLE tem_outra (
    super_categoria VARCHAR(100) NOT NULL,
    categoria VARCHAR(100),

    PRIMARY KEY(categoria),
    FOREIGN KEY(super_categoria) references super_categoria(nome),
    FOREIGN KEY(categoria) references categoria(nome)
);


--
-- produto(ean, cat, descr)
--    - cat: FK(categoria)
--    - RI-RE6: O valor do atributo ean existente em qualquer registo da relacao 
--      produto tem de existir tambem no atributo ean da relacao tem_categoria.
--
CREATE TABLE produto (
    ean CHAR(13),
    cat VARCHAR(100) NOT NULL,
    descr VARCHAR(100),

    PRIMARY KEY(ean),
    FOREIGN KEY(cat) references categoria(nome)
);


--
-- tem_categoria(ean, nome)
--    - ean: FK(produto)
--    - nome: FK(categoria)
--
CREATE TABLE tem_categoria (
    ean CHAR(13),
    nome VARCHAR(100),

    PRIMARY KEY(ean, nome),
    FOREIGN KEY(ean) references produto(ean),
    FOREIGN Key(nome) references categoria(nome)
);


--
-- IVM(num_serie, fabricante)
--
CREATE TABLE IVM (
    num_serie INTEGER,
    fabricante VARCHAR(100),

    PRIMARY KEY(num_serie, fabricante)
);


--
-- ponto_de_retalho(nome, distrito, concelho)
--
CREATE TABLE ponto_de_retalho (
    nome VARCHAR(100),
    distrito VARCHAR(100) NOT NULL,
    concelho VARCHAR(100) NOT NULL,

    PRIMARY KEY(nome)
);


--
-- instalada_em(num_serie, fabricante, local)
--    - num_serie, fabricante: FK(IVM)
--    - local: FK(ponto_de_retalho)
--
CREATE TABLE instalada_em (
    num_serie INTEGER,
    fabricante VARCHAR(100),
    loc VARCHAR(100) NOT NULL,

    PRIMARY KEY(num_serie, fabricante),
    FOREIGN KEY(num_serie, fabricante) references IVM(num_serie, fabricante),
    FOREIGN KEY(loc) references ponto_de_retalho(nome)
);


--
-- prateleira(nro, num_serie, fabricante, altura, nome)
--    - num_serie, fabricante: FK(IVM)
--    - nome: FK(categoria)
--
CREATE TABLE prateleira (
    nro INTEGER,
    num_serie INTEGER,
    fabricante VARCHAR(100),
    altura INTEGER NOT NULL,
    nome VARCHAR(100) NOT NULL,

    PRIMARY KEY(nro, num_serie, fabricante),
    FOREIGN KEY(num_serie, fabricante) references IVM(num_serie, fabricante),
    FOREIGN KEY(nome) references categoria(nome) 
);


--
-- planograma(ean, nro, num_serie, fabricante, faces, unidades, loc)
--    - ean: FK(produto)
--    - nro, num_serie, fabricante: FK(prateleira)
--
CREATE TABLE planograma (
    ean CHAR(13),
    nro INTEGER,
    num_serie INTEGER,
    fabricante VARCHAR(100),
    faces INTEGER NOT NULL,
    unidades INTEGER,
    loc VARCHAR(100) NOT NULL,

    PRIMARY KEY(ean, nro, num_serie, fabricante),
    FOREIGN KEY(ean) references produto(ean),
    FOREIGN KEY(nro, num_serie, fabricante) references prateleira(nro, num_serie, fabricante) 
);


--
-- retalhista(tin, name)
--    - RI-RE7: unique(name)
--
CREATE TABLE retalhista (
    tin INTEGER,
    nome VARCHAR(100) NOT NULL UNIQUE,

    PRIMARY KEY(tin)
);


--
-- responsavel_por(nome_cat, tin, num_serie, fabricante)
--    - num_serie, fabricante: FK(IVM)
--    - tin: FK(retalhista)
--    - nome_cat: FK(categoria)
--
CREATE TABLE responsavel_por (
    nome_cat VARCHAR(100) NOT NULL,
    tin INTEGER NOT NULL,
    num_serie INTEGER,
    fabricante VARCHAR(100),

    PRIMARY KEY(num_serie, fabricante),
    FOREIGN KEY(num_serie, fabricante) references IVM(num_serie, fabricante),
    FOREIGN KEY(tin) references retalhista(tin),
    FOREIGN KEY(nome_cat) references categoria(nome)
);


--
-- evento_reposicao(ean, nro, num_serie, fabricante, instante, unidades, tin)
--    - ean, nro, num_serie, fabricante: FK(planograma)
--    - tin: FK(retalhista)
--    - RI-RE78: o valor do atributo unidades de um registo X desta relacao nao 
--      pode exceder o valor do atributo unidades do registo da relacao planograma
--      referido pelo registo X.
--
CREATE TABLE evento_reposicao (
    ean CHAR(13),
    nro INTEGER,
    num_serie INTEGER,
    fabricante VARCHAR(100),
    instante timestamp,
    unidades INTEGER,
    tin INTEGER NOT NULL,

    PRIMARY KEY(ean, nro, num_serie, fabricante, instante),
    FOREIGN KEY(ean, nro, num_serie, fabricante) references planograma(ean, nro, num_serie, fabricante),
    FOREIGN KEY(tin) references retalhista(tin)
);





-----------------------------------------------------------------------------------
--                    Triggers for retailer/category removal                     --
-----------------------------------------------------------------------------------
--  All the triggers below are used for deleting entries from both 'retalhista'  --
--  and 'categoria' tables, essentially working as a 'ON DELETE CASCADE without' --
--  actually using it.                                                           --
-----------------------------------------------------------------------------------


-- Table: prateleira (Trigger)
CREATE OR REPLACE FUNCTION remove_shelf() 
RETURNS TRIGGER AS
$$
BEGIN
    DELETE FROM planograma WHERE nro = OLD.nro AND num_serie = OLD.num_serie AND fabricante = OLD.fabricante;
    RETURN OLD;
END;
$$
LANGUAGE plpgsql;

CREATE TRIGGER remove_shelf_trigger
    BEFORE DELETE OR UPDATE
    ON prateleira
    FOR EACH ROW
    EXECUTE PROCEDURE remove_shelf();




-- Table: planograma (Trigger)
CREATE OR REPLACE FUNCTION remove_planogram() 
RETURNS TRIGGER AS
$$
BEGIN
    DELETE FROM evento_reposicao WHERE nro = OLD.nro AND num_serie = OLD.num_serie AND fabricante = OLD.fabricante;
    RETURN OLD;
END;
$$
LANGUAGE plpgsql;

CREATE TRIGGER remove_planogram_trigger
    BEFORE DELETE OR UPDATE
    ON planograma
    FOR EACH ROW
    EXECUTE PROCEDURE remove_planogram();




-- Table: produto (Trigger)
CREATE OR REPLACE FUNCTION remove_product() 
RETURNS TRIGGER AS
$$
BEGIN
    DELETE FROM planograma WHERE ean = OLD.ean;
    RETURN OLD;
END;
$$
LANGUAGE plpgsql;

CREATE TRIGGER remove_product_trigger
    BEFORE DELETE OR UPDATE
    ON produto
    FOR EACH ROW
    EXECUTE PROCEDURE remove_product();




-- Table: categoria (Trigger)
CREATE OR REPLACE FUNCTION remove_category() 
RETURNS TRIGGER AS
$$
DECLARE
    nome_categoria VARCHAR(100);
BEGIN
    nome_categoria := OLD.nome;

    DELETE FROM tem_outra WHERE categoria = nome_categoria;

    -- Checks if the category is simple or super and deletes it
    -- from its respective table.
    IF nome_categoria IN (SELECT nome FROM categoria_simples) THEN
        DELETE FROM categoria_simples WHERE nome = nome_categoria;
    
    ELSIF nome_categoria IN (SELECT nome FROM super_categoria) THEN
        DELETE FROM super_categoria WHERE nome = nome_categoria;

    END IF;

    DELETE FROM tem_categoria WHERE nome = nome_categoria;
    DELETE FROM produto WHERE cat = nome_categoria;
    DELETE FROM prateleira WHERE nome = nome_categoria;
    DELETE FROM responsavel_por WHERE nome_cat = nome_categoria;

    RETURN OLD;
END;
$$
LANGUAGE plpgsql;

CREATE TRIGGER remove_category_trigger
    BEFORE DELETE OR UPDATE
    ON categoria
    FOR EACH ROW
    EXECUTE PROCEDURE remove_category();




-- Table: super_categoria (Trigger)
CREATE OR REPLACE FUNCTION remove_super_category() 
RETURNS TRIGGER AS
$$
DECLARE
    num_sub INTEGER;
    nome_sub VARCHAR(100);
    nome_categoria VARCHAR(100);
BEGIN
    nome_categoria := OLD.nome;

    -- Stores in 'num_sub' the number of sub_categories of a 
    -- super category.
    SELECT COUNT(categoria) INTO num_sub
    FROM tem_outra
        WHERE super_categoria = nome_categoria;

    -- Loops over all the sub categories and deletes them.
    WHILE num_sub != 0
    LOOP
        SELECT categoria INTO nome_sub
        FROM tem_outra
            WHERE super_categoria = nome_categoria
        LIMIT 1;

        DELETE FROM categoria WHERE nome = nome_sub;

        -- updates the 'num_sub' variable.
        SELECT COUNT(categoria) INTO num_sub
        FROM tem_outra
            WHERE super_categoria = nome_categoria;
    END LOOP;
    
    RETURN OLD;
END;
$$
LANGUAGE plpgsql;

CREATE TRIGGER remove_super_category_trigger
    BEFORE DELETE OR UPDATE
    ON super_categoria
    FOR EACH ROW
    EXECUTE PROCEDURE remove_super_category();




-- Table: retalhista (Trigger)
CREATE OR REPLACE FUNCTION remove_retailer() 
RETURNS TRIGGER AS
$$
DECLARE
    tin_retalhista INTEGER;
    num_cat_responsavel INTEGER;
    nome_categoria VARCHAR(100);
BEGIN
    tin_retalhista := OLD.tin;

    DELETE FROM evento_reposicao WHERE tin = tin_retalhista;

    -- Stores in 'num_cat_responsavel' the number of categories
    -- the retailer is responsible for.
    SELECT COUNT(nome_cat) INTO num_cat_responsavel
    FROM responsavel_por
        WHERE tin = tin_retalhista;

    -- Loops over all the categories and deletes them.
    WHILE num_cat_responsavel != 0
    LOOP
        SELECT nome_cat INTO nome_categoria
        FROM responsavel_por
            WHERE tin = tin_retalhista
        LIMIT 1;

        DELETE FROM categoria WHERE nome = nome_categoria;

        SELECT COUNT(nome_cat) INTO num_cat_responsavel
        FROM responsavel_por
            WHERE tin = tin_retalhista;
    END LOOP;
    
    RETURN OLD;
END;
$$
LANGUAGE plpgsql;

CREATE TRIGGER remove_retailer_trigger
    BEFORE DELETE OR UPDATE
    ON retalhista
    FOR EACH ROW
    EXECUTE PROCEDURE remove_retailer();

