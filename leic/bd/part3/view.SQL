
----------------------------------------------
--                view.SQL                  --
----------------------------------------------
--   Grupo 20:                              --
--   - Valentim Santos ist199343            --
--   - Tiago Santos ist199333               --
--   - Yassir Yassin ist100611              --
----------------------------------------------



------------------------------------------------------------------------------------------------------
--  vendas(ean , cat, ano , trimestre, dia_mes, mes , dia_semana , distrito, concelho, unidades)    --
--                                                                                                  --
--     dia_mes: [0, 31]                                                                             --
--     mes: [0, 12]                                                                                 --                     
--     dia_semana: [0, 7]                                                                           --
------------------------------------------------------------------------------------------------------

DROP VIEW vendas;

CREATE VIEW vendas(ean, cat, ano, trimestre, dia_mes, mes, dia_semana, distrito, concelho, unidades) AS
    SELECT evento.ean,
           prod.cat,
           EXTRACT(YEAR from evento.instante) ano,
           EXTRACT(QUARTER from evento.instante) trimestre,
           EXTRACT(MONTH from evento.instante) mes,
           EXTRACT(DAY from evento.instante) dia_mes,
           EXTRACT(DOW from evento.instante) dia_semana,
           ponto_ret.distrito,
           ponto_ret.concelho,
           evento.unidades
        FROM evento_reposicao evento 
        INNER JOIN produto prod
            ON evento.ean = prod.ean
        INNER JOIN instalada_em inst 
            ON evento.num_serie = inst.num_serie AND evento.fabricante = inst.fabricante 
        INNER JOIN ponto_de_retalho ponto_ret 
            ON inst.loc = ponto_ret.nome
