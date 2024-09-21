
----------------------------------------------
--             indexes.SQL                  --
----------------------------------------------
--   Grupo 20:                              --
--   - Valentim Santos ist199343            --
--   - Tiago Santos ist199333               --
--   - Yassir Yassin ist100611              --
----------------------------------------------



---------------------------------------------------
--   7.1                                         --
---------------------------------------------------
-- SELECT DISTINCT R.nome                        --
-- FROM retalhista R, responsavel_por P          --
-- WHERE R.tin = P.tin and P.nome_cat = 'Fruta'  --
---------------------------------------------------

CREATE INDEX index_tin_aux on retalhista using hash(tin);
CREATE INDEX index_tin on responsavel_por using hash(tin);



---------------------------------------------------
--   7.2                                         --
---------------------------------------------------
-- SELECT T.nome, count(T.ean)                   --
-- FROM produto P, tem_categoria T               --
-- WHERE P.cat = T.nome and P.descr like 'A%'    --
-- GROUP BY T.nome                               --
---------------------------------------------------

CREATE INDEX index_nome on tem_categoria using hash(nome);
CREATE INDEX index_descr on produto using hash(descr);


