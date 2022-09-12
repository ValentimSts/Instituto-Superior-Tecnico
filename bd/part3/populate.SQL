
----------------------------------------------
--              populate.SQL                --
----------------------------------------------
--   Grupo 20:                              --
--   - Valentim Santos ist199343            --
--   - Tiago Santos ist199333               --
--   - Yassir Yassin ist100611              --
----------------------------------------------



-- Table: categoria(nome)
INSERT INTO categoria VALUES ('Comida');
INSERT INTO categoria VALUES ('Bebidas');

INSERT INTO categoria VALUES ('Fruta');
INSERT INTO categoria VALUES ('Vegetais');
INSERT INTO categoria VALUES ('Snacks');

INSERT INTO categoria VALUES ('Frutos secos');

INSERT INTO categoria VALUES ('Banana');
INSERT INTO categoria VALUES ('Pera');
INSERT INTO categoria VALUES ('Meloa');
INSERT INTO categoria VALUES ('Laranja');
INSERT INTO categoria VALUES ('Cereja');

INSERT INTO categoria VALUES ('Alface');
INSERT INTO categoria VALUES ('Tomate');
INSERT INTO categoria VALUES ('Cebola');
INSERT INTO categoria VALUES ('Pepino');
INSERT INTO categoria VALUES ('Cenoura');

INSERT INTO categoria VALUES ('Batatas');
INSERT INTO categoria VALUES ('Bolachas');
INSERT INTO categoria VALUES ('Gomas');
INSERT INTO categoria VALUES ('Chocolates');
INSERT INTO categoria VALUES ('Cereais');

INSERT INTO categoria VALUES ('Noz');
INSERT INTO categoria VALUES ('Tamara');
INSERT INTO categoria VALUES ('Amendoins');
INSERT INTO categoria VALUES ('Granola');
INSERT INTO categoria VALUES ('Passas');

INSERT INTO categoria VALUES ('Agua');
INSERT INTO categoria VALUES ('Refrigerantes');
INSERT INTO categoria VALUES ('Cerveja');
INSERT INTO categoria VALUES ('Sidra');
INSERT INTO categoria VALUES ('Vinho');



-- Table: categoria_simples(nome)
INSERT INTO categoria_simples VALUES ('Banana');
INSERT INTO categoria_simples VALUES ('Pera');
INSERT INTO categoria_simples VALUES ('Meloa');
INSERT INTO categoria_simples VALUES ('Laranja');
INSERT INTO categoria_simples VALUES ('Cereja');

INSERT INTO categoria_simples VALUES ('Alface');
INSERT INTO categoria_simples VALUES ('Tomate');
INSERT INTO categoria_simples VALUES ('Cebola');
INSERT INTO categoria_simples VALUES ('Pepino');
INSERT INTO categoria_simples VALUES ('Cenoura');

INSERT INTO categoria_simples VALUES ('Batatas');
INSERT INTO categoria_simples VALUES ('Bolachas');
INSERT INTO categoria_simples VALUES ('Gomas');
INSERT INTO categoria_simples VALUES ('Chocolates');
INSERT INTO categoria_simples VALUES ('Cereais');

INSERT INTO categoria_simples VALUES ('Noz');
INSERT INTO categoria_simples VALUES ('Tamara');
INSERT INTO categoria_simples VALUES ('Amendoins');
INSERT INTO categoria_simples VALUES ('Granola');
INSERT INTO categoria_simples VALUES ('Passas');

INSERT INTO categoria_simples VALUES ('Agua');
INSERT INTO categoria_simples VALUES ('Refrigerantes');
INSERT INTO categoria_simples VALUES ('Cerveja');
INSERT INTO categoria_simples VALUES ('Sidra');
INSERT INTO categoria_simples VALUES ('Vinho');



-- Table: super_categoria(nome)
INSERT INTO super_categoria VALUES ('Comida');
INSERT INTO super_categoria VALUES ('Bebidas');

INSERT INTO super_categoria VALUES ('Fruta');
INSERT INTO super_categoria VALUES ('Vegetais');
INSERT INTO super_categoria VALUES ('Snacks');

INSERT INTO super_categoria VALUES ('Frutos secos');


-- Table: tem_outra(super_categoria, categoria)
INSERT INTO tem_outra VALUES ('Fruta', 'Banana');
INSERT INTO tem_outra VALUES ('Fruta', 'Pera');
INSERT INTO tem_outra VALUES ('Fruta', 'Meloa');
INSERT INTO tem_outra VALUES ('Fruta', 'Laranja');
INSERT INTO tem_outra VALUES ('Fruta', 'Cereja');

INSERT INTO tem_outra VALUES ('Vegetais', 'Alface');
INSERT INTO tem_outra VALUES ('Vegetais', 'Tomate');
INSERT INTO tem_outra VALUES ('Vegetais', 'Cebola');
INSERT INTO tem_outra VALUES ('Vegetais', 'Pepino');
INSERT INTO tem_outra VALUES ('Vegetais', 'Cenoura');

INSERT INTO tem_outra VALUES ('Snacks', 'Batatas');
INSERT INTO tem_outra VALUES ('Snacks', 'Bolachas');
INSERT INTO tem_outra VALUES ('Snacks', 'Gomas');
INSERT INTO tem_outra VALUES ('Snacks', 'Chocolates');
INSERT INTO tem_outra VALUES ('Snacks', 'Cereais');

INSERT INTO tem_outra VALUES ('Frutos secos', 'Noz');
INSERT INTO tem_outra VALUES ('Frutos secos', 'Tamara');
INSERT INTO tem_outra VALUES ('Frutos secos', 'Amendoins');
INSERT INTO tem_outra VALUES ('Frutos secos', 'Granola');
INSERT INTO tem_outra VALUES ('Frutos secos', 'Passas');

INSERT INTO tem_outra VALUES ('Bebidas', 'Agua');
INSERT INTO tem_outra VALUES ('Bebidas', 'Refrigerantes');
INSERT INTO tem_outra VALUES ('Bebidas', 'Cerveja');
INSERT INTO tem_outra VALUES ('Bebidas', 'Sidra');
INSERT INTO tem_outra VALUES ('Bebidas', 'Vinho');

INSERT INTO tem_outra VALUES ('Snacks', 'Frutos secos');

INSERT INTO tem_outra VALUES ('Comida', 'Fruta');
INSERT INTO tem_outra VALUES ('Comida', 'Vegetais');
INSERT INTO tem_outra VALUES ('Comida', 'Snacks');



-- Table: produto(ean, cat, descr)
INSERT INTO produto VALUES ('0000000000001', 'Banana', 'Banana da Madeira');
INSERT INTO produto VALUES ('0000000000002', 'Pera', 'Pera portuguesa');
INSERT INTO produto VALUES ('0000000000003', 'Meloa', 'Meloa amarela');
INSERT INTO produto VALUES ('0000000000004', 'Laranja', 'Laranja do Algarve');
INSERT INTO produto VALUES ('0000000000005', 'Cereja', 'Cereja silvestre');

INSERT INTO produto VALUES ('0000000000006', 'Alface', 'Alface do Lidl');
INSERT INTO produto VALUES ('0000000000007', 'Tomate', 'Tomate cherry');
INSERT INTO produto VALUES ('0000000000008', 'Cebola', 'Cebola roxa');
INSERT INTO produto VALUES ('0000000000009', 'Pepino', 'Pepino comum');
INSERT INTO produto VALUES ('0000000000010', 'Cenoura', 'Cenoura ralada');

INSERT INTO produto VALUES ('0000000000011', 'Batatas', 'Batatas Lays');
INSERT INTO produto VALUES ('0000000000012', 'Bolachas', 'Bolachas Maria');
INSERT INTO produto VALUES ('0000000000013', 'Gomas', 'Marshmallows');
INSERT INTO produto VALUES ('0000000000014', 'Chocolates', 'Chocolate Daim');
INSERT INTO produto VALUES ('0000000000015', 'Cereais', 'Corn Flakes');

INSERT INTO produto VALUES ('0000000000016', 'Noz', 'Noz peca');
INSERT INTO produto VALUES ('0000000000017', 'Tamara', 'Tamara nacional');
INSERT INTO produto VALUES ('0000000000018', 'Amendoins', 'Amendoins espanhois');
INSERT INTO produto VALUES ('0000000000019', 'Granola', 'Granola de chocolate');
INSERT INTO produto VALUES ('0000000000020', 'Passas', 'Goji berry');

INSERT INTO produto VALUES ('0000000000021', 'Agua', 'Agua mineral');
INSERT INTO produto VALUES ('0000000000022', 'Refrigerantes', '7up');
INSERT INTO produto VALUES ('0000000000023', 'Cerveja', 'Super Bock');
INSERT INTO produto VALUES ('0000000000024', 'Sidra', 'Somersby');
INSERT INTO produto VALUES ('0000000000025', 'Vinho', 'Vinho branco');


-- Table: tem_categoria(ean, nome)
INSERT INTO tem_categoria VALUES ('0000000000001', 'Banana');
INSERT INTO tem_categoria VALUES ('0000000000002', 'Pera');
INSERT INTO tem_categoria VALUES ('0000000000003', 'Meloa');
INSERT INTO tem_categoria VALUES ('0000000000004', 'Laranja');
INSERT INTO tem_categoria VALUES ('0000000000005', 'Cereja');

INSERT INTO tem_categoria VALUES ('0000000000006', 'Alface');
INSERT INTO tem_categoria VALUES ('0000000000007', 'Tomate');
INSERT INTO tem_categoria VALUES ('0000000000008', 'Cebola');
INSERT INTO tem_categoria VALUES ('0000000000009', 'Pepino');
INSERT INTO tem_categoria VALUES ('0000000000010', 'Cenoura');

INSERT INTO tem_categoria VALUES ('0000000000011', 'Batatas');
INSERT INTO tem_categoria VALUES ('0000000000012', 'Bolachas');
INSERT INTO tem_categoria VALUES ('0000000000013', 'Gomas');
INSERT INTO tem_categoria VALUES ('0000000000014', 'Chocolates');
INSERT INTO tem_categoria VALUES ('0000000000015', 'Cereais');

INSERT INTO tem_categoria VALUES ('0000000000016', 'Noz');
INSERT INTO tem_categoria VALUES ('0000000000017', 'Tamara');
INSERT INTO tem_categoria VALUES ('0000000000018', 'Amendoins');
INSERT INTO tem_categoria VALUES ('0000000000019', 'Granola');
INSERT INTO tem_categoria VALUES ('0000000000020', 'Passas');

INSERT INTO tem_categoria VALUES ('0000000000021', 'Agua');
INSERT INTO tem_categoria VALUES ('0000000000022', 'Refrigerantes');
INSERT INTO tem_categoria VALUES ('0000000000023', 'Cerveja');
INSERT INTO tem_categoria VALUES ('0000000000024', 'Sidra');
INSERT INTO tem_categoria VALUES ('0000000000025', 'Vinho');


-- Table: IVM(num_serie, fabricante)
INSERT INTO IVM VALUES ('200300001', 'Valentim');
INSERT INTO IVM VALUES ('200300002', 'Tiago');
INSERT INTO IVM VALUES ('200300003', 'Yassir');
INSERT INTO IVM VALUES ('200300004', 'Obama');
INSERT INTO IVM VALUES ('200300005', 'Antonio Costa');


-- Table: ponto_de_retalho(nome, distrito, concelho)
INSERT INTO ponto_de_retalho VALUES ('Trap-house', 'Faro', 'Loule');
INSERT INTO ponto_de_retalho VALUES ('Bando', 'Faro', 'Lagos');
INSERT INTO ponto_de_retalho VALUES ('Casinhas mortas', 'Lisboa', 'Amadora');
INSERT INTO ponto_de_retalho VALUES ('White house', 'Columbia', 'Washington DC');
INSERT INTO ponto_de_retalho VALUES ('Primeira feira', 'Alentejo', 'Vila nova de mil fontes');


-- Table: instalada_em(num_serie, fabricante, local)
INSERT INTO instalada_em VALUES ('200300001', 'Valentim', 'Bando');
INSERT INTO instalada_em VALUES ('200300002', 'Tiago', 'Trap-house');
INSERT INTO instalada_em VALUES ('200300003', 'Yassir', 'Casinhas mortas');
INSERT INTO instalada_em VALUES ('200300004', 'Obama', 'White house');
INSERT INTO instalada_em VALUES ('200300005', 'Antonio Costa', 'Primeira feira');


-- Table: prateleira(nro, num_serie, fabricante, altura, nome)
INSERT INTO prateleira VALUES ('1', '200300001', 'Valentim', '10', 'Banana');
INSERT INTO prateleira VALUES ('2', '200300001', 'Valentim', '15', 'Pera');
INSERT INTO prateleira VALUES ('3', '200300001', 'Valentim', '5', 'Meloa');
INSERT INTO prateleira VALUES ('4', '200300001', 'Valentim', '10', 'Laranja');
INSERT INTO prateleira VALUES ('5', '200300001', 'Valentim', '10', 'Cereja');

INSERT INTO prateleira VALUES ('6', '200300002', 'Tiago', '10', 'Alface');
INSERT INTO prateleira VALUES ('7', '200300002', 'Tiago', '5', 'Tomate');
INSERT INTO prateleira VALUES ('8', '200300002', 'Tiago', '5', 'Cebola');
INSERT INTO prateleira VALUES ('9', '200300002', 'Tiago', '10', 'Pepino');
INSERT INTO prateleira VALUES ('10', '200300002', 'Tiago', '10', 'Cenoura');

INSERT INTO prateleira VALUES ('11', '200300003', 'Yassir', '10', 'Batatas');
INSERT INTO prateleira VALUES ('12', '200300003', 'Yassir', '10', 'Bolachas');
INSERT INTO prateleira VALUES ('13', '200300003', 'Yassir', '20', 'Gomas');
INSERT INTO prateleira VALUES ('14', '200300003', 'Yassir', '5', 'Chocolates');
INSERT INTO prateleira VALUES ('15', '200300003', 'Yassir', '15', 'Cereais');

INSERT INTO prateleira VALUES ('16', '200300004', 'Obama', '11', 'Noz');
INSERT INTO prateleira VALUES ('17', '200300004', 'Obama', '4', 'Tamara');
INSERT INTO prateleira VALUES ('18', '200300004', 'Obama', '10', 'Amendoins');
INSERT INTO prateleira VALUES ('19', '200300004', 'Obama', '7', 'Granola');
INSERT INTO prateleira VALUES ('20', '200300004', 'Obama', '12', 'Passas');

INSERT INTO prateleira VALUES ('21', '200300005', 'Antonio Costa', '20', 'Agua');
INSERT INTO prateleira VALUES ('22', '200300005', 'Antonio Costa', '17', 'Refrigerantes');
INSERT INTO prateleira VALUES ('23', '200300005', 'Antonio Costa', '25', 'Cerveja');
INSERT INTO prateleira VALUES ('24', '200300005', 'Antonio Costa', '10', 'Sidra');
INSERT INTO prateleira VALUES ('25', '200300005', 'Antonio Costa', '30', 'Vinho');



-- Table: planograma(ean, nro, num_serie, fabricante, faces, unidades, loc)
INSERT INTO planograma VALUES ('0000000000001', '1', '200300001', 'Valentim', '1', '3', 'Faro');
INSERT INTO planograma VALUES ('0000000000002', '2', '200300001', 'Valentim', '1', '12', 'Faro');
INSERT INTO planograma VALUES ('0000000000003', '3', '200300001', 'Valentim', '2', '5', 'Faro');
INSERT INTO planograma VALUES ('0000000000004', '4', '200300001', 'Valentim', '3', '7', 'Faro');
INSERT INTO planograma VALUES ('0000000000005', '5', '200300001', 'Valentim', '1', '9', 'Faro');

INSERT INTO planograma VALUES ('0000000000006', '6', '200300002', 'Tiago', '2', '2', 'Faro');
INSERT INTO planograma VALUES ('0000000000007', '7', '200300002', 'Tiago', '2', '2', 'Faro');
INSERT INTO planograma VALUES ('0000000000008', '8', '200300002', 'Tiago', '1', '4', 'Faro');
INSERT INTO planograma VALUES ('0000000000009', '9', '200300002', 'Tiago', '3', '13', 'Faro');
INSERT INTO planograma VALUES ('0000000000010', '10', '200300002', 'Tiago', '1', '9', 'Faro');

INSERT INTO planograma VALUES ('0000000000011', '11', '200300003', 'Yassir', '3', '1', 'Lisboa');
INSERT INTO planograma VALUES ('0000000000012', '12', '200300003', 'Yassir', '3', '11', 'Lisboa');
INSERT INTO planograma VALUES ('0000000000013', '13', '200300003', 'Yassir', '3', '15', 'Lisboa');
INSERT INTO planograma VALUES ('0000000000014', '14', '200300003', 'Yassir', '1', '4', 'Lisboa');
INSERT INTO planograma VALUES ('0000000000015', '15', '200300003', 'Yassir', '2', '3', 'Lisboa');

INSERT INTO planograma VALUES ('0000000000016', '16', '200300004', 'Obama', '4', '13', 'Columbia');
INSERT INTO planograma VALUES ('0000000000017', '17', '200300004', 'Obama', '2', '23', 'Columbia');
INSERT INTO planograma VALUES ('0000000000018', '18', '200300004', 'Obama', '4', '6', 'Columbia');
INSERT INTO planograma VALUES ('0000000000019', '19', '200300004', 'Obama', '6', '7', 'Columbia');
INSERT INTO planograma VALUES ('0000000000020', '20', '200300004', 'Obama', '1', '10', 'Columbia');

INSERT INTO planograma VALUES ('0000000000021', '21', '200300005', 'Antonio Costa', '2', '10', 'Alentejo');
INSERT INTO planograma VALUES ('0000000000022', '22', '200300005', 'Antonio Costa', '2', '20', 'Alentejo');
INSERT INTO planograma VALUES ('0000000000023', '23', '200300005', 'Antonio Costa', '1', '23', 'Alentejo');
INSERT INTO planograma VALUES ('0000000000024', '24', '200300005', 'Antonio Costa', '4', '19', 'Alentejo');
INSERT INTO planograma VALUES ('0000000000025', '25', '200300005', 'Antonio Costa', '2', '11', 'Alentejo');


-- Table: retalhista(tin, name)
INSERT INTO retalhista VALUES ('1', 'Diogo');
INSERT INTO retalhista VALUES ('2', 'John');
INSERT INTO retalhista VALUES ('3', 'Xina');
INSERT INTO retalhista VALUES ('4', 'Barba');
INSERT INTO retalhista VALUES ('5', 'DM');
INSERT INTO retalhista VALUES ('6', 'Mugube');
INSERT INTO retalhista VALUES ('7', 'Joao');
INSERT INTO retalhista VALUES ('8', 'Gilberto');
INSERT INTO retalhista VALUES ('9', 'Florinda');
INSERT INTO retalhista VALUES ('10', 'Ahmed');
INSERT INTO retalhista VALUES ('11', 'Afonso Henriques');
INSERT INTO retalhista VALUES ('12', 'Gaston');
INSERT INTO retalhista VALUES ('13', 'Guts');
INSERT INTO retalhista VALUES ('14', 'Griffith');
INSERT INTO retalhista VALUES ('15', 'Spike');
INSERT INTO retalhista VALUES ('16', 'Joe Biden');
INSERT INTO retalhista VALUES ('17', 'Piruca');
INSERT INTO retalhista VALUES ('18', 'Savesta');
INSERT INTO retalhista VALUES ('19', 'Tyler1');
INSERT INTO retalhista VALUES ('20', 'Bozo');
INSERT INTO retalhista VALUES ('21', 'Bilu');
INSERT INTO retalhista VALUES ('22', 'J Cole');
INSERT INTO retalhista VALUES ('23', 'Luis Xu');
INSERT INTO retalhista VALUES ('24', 'Zara G');
INSERT INTO retalhista VALUES ('25', 'Jesus');



-- Table: responsavel_por(nome_cat, tin, num_serie, fabricante)
INSERT INTO responsavel_por VALUES ('Banana', '1', '200300001', 'Valentim');
INSERT INTO responsavel_por VALUES ('Alface', '6', '200300002', 'Tiago');
INSERT INTO responsavel_por VALUES ('Batatas', '11', '200300003', 'Yassir');
INSERT INTO responsavel_por VALUES ('Granola', '19', '200300004', 'Obama');
INSERT INTO responsavel_por VALUES ('Vinho', '25', '200300005', 'Antonio Costa');


-- Table: evento_reposicao(ean, nro, num_serie, fabricante, instante, unidades, tin)
INSERT INTO evento_reposicao VALUES ('0000000000001', '1', '200300001', 'Valentim', '2022-07-11 23:59:00', '2', '1');
INSERT INTO evento_reposicao VALUES ('0000000000001', '1', '200300001', 'Valentim', '2022-08-11 23:55:00', '2', '1');
INSERT INTO evento_reposicao VALUES ('0000000000003', '3', '200300001', 'Valentim', '2022-07-09 12:30:30', '4', '3');
INSERT INTO evento_reposicao VALUES ('0000000000005', '5', '200300001', 'Valentim', '2022-08-17 18:03:10', '8', '5');
INSERT INTO evento_reposicao VALUES ('0000000000005', '5', '200300001', 'Valentim', '2022-08-22 11:03:10', '8', '5');

INSERT INTO evento_reposicao VALUES ('0000000000006', '6', '200300002', 'Tiago', '2022-07-15 09:11:11', '1', '6');
INSERT INTO evento_reposicao VALUES ('0000000000007', '7', '200300002', 'Tiago', '2022-09-13 17:23:45', '2', '7');
INSERT INTO evento_reposicao VALUES ('0000000000008', '8', '200300002', 'Tiago', '2022-11-17 15:30:00', '3', '8');

INSERT INTO evento_reposicao VALUES ('0000000000011', '11', '200300003', 'Yassir', '2022-08-09 08:45:00', '1', '11');
INSERT INTO evento_reposicao VALUES ('0000000000013', '13', '200300003', 'Yassir', '2022-06-21 14:39:32', '12', '13');

INSERT INTO evento_reposicao VALUES ('0000000000018', '18', '200300004', 'Obama', '2022-12-24 11:59:59', '5', '18');

INSERT INTO evento_reposicao VALUES ('0000000000022', '22', '200300005', 'Antonio Costa', '2022-05-13 18:22:30', '4', '22');
INSERT INTO evento_reposicao VALUES ('0000000000025', '25', '200300005', 'Antonio Costa', '2022-10-18 05:03:11', '8', '25');

