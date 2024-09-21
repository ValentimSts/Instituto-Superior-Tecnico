
----------------------------------------------
--   Respostas às consultas SQL propostas   --
----------------------------------------------
--   Grupo 20:                              --
--   - Valentim Santos ist199343            --
--   - Tiago Santos ist199333               --
--   - Yassir Yassin ist100611              --
----------------------------------------------



--
-- 1) Qual o nome do retalhista (ou retalhistas) responsaveis pela reposição do maior numero de categorias?
--
SELECT nome
FROM retalhista AS r          
	NATURAL JOIN ( 
		SELECT tin, COUNT(cat) AS num_cat
		FROM evento_reposicao
        	NATURAL JOIN produto
        		GROUP BY tin) c
GROUP BY nome, tin, num_cat
    HAVING num_cat >= ALL (        
        SELECT COUNT(cat)
		FROM evento_reposicao
        	NATURAL JOIN produto
        		GROUP BY tin
    )



--
-- 2) Qual o nome do ou dos retalhistas que sao responsaveis por todas as categorias simples?
--
SELECT nome
FROM retalhista AS r
	WHERE NOT EXISTS (
		SELECT nome
		FROM categoria_simples
			EXCEPT
		SELECT nome_cat
		FROM (responsavel_por NATURAL JOIN retalhista) AS resp
			WHERE resp.nome = r.nome
	)



--
-- 3) Quais os produtos (ean) que nunca foram repostos?
--
SELECT ean FROM produto 
    WHERE ean NOT IN (  
        SELECT DISTINCT ean FROM evento_reposicao
    )



--
-- 4) Quais os produtos (ean) que foram repostos sempre pelo mesmo retalhista?
--
SELECT c.ean
FROM evento_reposicao as c
    GROUP BY c.ean, c.tin
    HAVING c.tin = ALL (
        SELECT tin
		FROM evento_reposicao 
        	WHERE ean = c.ean
    )
    ORDER BY c.ean
