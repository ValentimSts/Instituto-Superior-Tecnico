
----------------------------------------------
--            Consultas OLAP                --
----------------------------------------------
--   Grupo 20:                              --
--   - Valentim Santos ist199343            --
--   - Tiago Santos ist199333               --
--   - Yassir Yassin ist100611              --
----------------------------------------------


-- Usando a view "Vendas" escrever as consultas que permitam analisar o numero
-- total de artigos vendidos:


--
-- 1. Num dado periodo (i.e. entre duas datas), por dia da semana, por concelho e no total.
--
SELECT dia_semana, concelho, SUM(unidades) AS TotalSales
FROM vendas v
    WHERE make_date(ano::int, dia_mes::int, mes::int)
        BETWEEN '2020-01-01'::date AND '2024-02-23'::date
GROUP BY 
    CUBE(dia_semana, concelho)


--
-- 2. Num dado distrito (i.e.'Lisboa'), por concelho, categoria, dia da semana e no total.
--
SELECT concelho, cat, dia_semana, SUM(unidades) AS TotalSales
FROM vendas v
    WHERE distrito = 'Lisboa'
GROUP BY 
    CUBE(concelho, cat, dia_semana)


