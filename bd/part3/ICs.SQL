
----------------------------------------------
--       Respostas às ICs propostas.        --
----------------------------------------------
--   Grupo 20:                              --
--   - Valentim Santos ist199343            --
--   - Tiago Santos ist199333               --
--   - Yassir Yassin ist100611              --
----------------------------------------------



--
-- 1) (RI-1) Uma categoria nao pode estar contida em si propria
--
CREATE OR REPLACE FUNCTION catNotInItself() RETURNS TRIGGER AS $$
     DECLARE
        superCat varchar(100);
     BEGIN
	superCat := NEW.super_categoria;

        -- If the super_category != category, then there is still a chance that there's
        -- a loop in the sub-categories, and so, we check if the super category
        -- is already apart of its sub-categories.
        IF superCat = NEW.categoria OR superCat IN (
            
            -- Recursively gets all the sub-categories of the super category.
            WITH RECURSIVE cte AS (
            SELECT categoria AS a
                FROM tem_outra
                WHERE super_categoria = NEW.categoria
            UNION ALL
            SELECT categoria
                FROM cte INNER JOIN tem_outra 
                ON super_categoria = a
            )
            SELECT * FROM cte
            UNION ALL
                SELECT nome 
                FROM categoria
                    WHERE nome = NEW.categoria
                
        ) THEN
            RAISE EXCEPTION 'Uma categoria nao pode estar contida em si propria.';
            RETURN OLD;

        END IF;
        RETURN NEW;
    END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER catNotInItselfTrigger
    BEFORE INSERT OR UPDATE
    ON tem_outra
    FOR EACH ROW
    EXECUTE PROCEDURE catNotInItself();




--
-- 2) (RI-4) O numero de unidades repostas num Evento de Reposicao nao pode exceder o numero de unidades especificado no Planograma.
--
CREATE OR REPLACE FUNCTION nRepoLessThanMax() RETURNS TRIGGER AS $$
    DECLARE
    un integer;
    BEGIN
        -- Stores in 'un' the maximum number of units that can be replaced,
        -- established by the Planogram.
	    un := unidades FROM planograma WHERE planograma.ean = NEW.ean AND planograma.nro = NEW.nro;
        -- If the units trying to be replaced exceed said amount, an exception
        -- is thrown, NEW is returned and the entries are not inserted into the table.
        IF NEW.unidades > un THEN
            RAISE EXCEPTION 'O numero de unidades repostas num Evento de Reposicao nao pode exceder o numero de unidades especificado no Planograma.';
            RETURN NEW;
        END IF;
        RETURN NEW;
    END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER nRepoLessThanMaxTrigger
    BEFORE INSERT OR UPDATE
    ON evento_reposicao
    FOR EACH ROW
    EXECUTE PROCEDURE nRepoLessThanMax();




-- 
-- 3) (RI-5) Um Produto so pode ser reposto numa Prateleira que apresente (pelo menos) uma das Categorias desse produto.
--
CREATE OR REPLACE FUNCTION prateCategoria() RETURNS TRIGGER AS $$
    DECLARE
        prodCat varchar(100);
        prateCat varchar(100);
    BEGIN
        -- Stores in 'prodCat' the product's category .
        SELECT cat INTO STRICT prodCat
            FROM produto WHERE produto.ean = NEW.ean;

        -- Stores in 'prateCat' the shelf's category.
        SELECT nome INTO STRICT prateCat
            FROM prateleira WHERE prateleira.nro = NEW.nro AND prateleira.num_serie = NEW.num_serie;
        
            -- If the new product's category is different from the shelf's category,
            -- then there is a chance that the shelf holds a sub-category equal to 
            -- the product's, and so, we recursively check for it.
            IF prodCat IS DISTINCT FROM prateCat AND prodCat NOT IN (
                
                -- Recursively gets all the sub-categories of the shelf's category.
                WITH RECURSIVE cte AS (
                SELECT categoria AS a
                    FROM tem_outra
                    WHERE super_categoria = prateCat
                UNION ALL
                SELECT categoria
                    FROM cte INNER JOIN tem_outra 
                    ON super_categoria = a
                )
                SELECT * FROM cte
                UNION ALL
                    SELECT nome 
                    FROM categoria
                        WHERE nome = prateCat

            ) THEN
                RAISE EXCEPTION 'Um produto apenas pode ser reposto numa prateleira que apresente (pelo menos) uma das categorias dele mesmo.';
                RETURN NEW;
            END IF;
        RETURN NEW;
    END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER prateCategoriaTrigger
    BEFORE INSERT OR UPDATE
    ON evento_reposicao
    FOR EACH ROW
    EXECUTE PROCEDURE prateCategoria();

