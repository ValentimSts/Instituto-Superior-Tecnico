#!/usr/bin/python3

# ---------------------------------- #
#  Aplicacao web                     #
#  Grupo 20:                         #
#  - Valentim Santos ist199343       #
#  - Tiago Santos ist199333          #
#  - Yassir Yassin ist100611         #
# ---------------------------------- #

import psycopg2
import psycopg2.extras

from wsgiref.handlers import CGIHandler
from flask import Flask
from flask import render_template, redirect, url_for, request

# Sets up our application.
app = Flask(__name__)

# SGBD configs
DB_HOST = "db.tecnico.ulisboa.pt"
DB_USER = "ist199343"
DB_DATABASE = DB_USER
DB_PASSWORD = "tecnicodb199343"

DB_CONNECTION_STRING = "host=%s dbname=%s user=%s password=%s" % (DB_HOST, DB_DATABASE, DB_USER, DB_PASSWORD)



# - - - - - - #
#  MAIN MENUS #
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

# Root page route.
@app.route('/')
def index():
  try:
    return render_template("index.html", params = request.args)

  except Exception as e:
    return str(e)



# Menu page route.
@app.route("/menu")
def main_menu():
  try:
    return render_template("index.html", params = request.args)

  except Exception as e:
    return str(e)



# Error page route
@app.route("/erro")
def error():
  try:
    return render_template("erro.html", params = request.args)

  except Exception as e:
    return str(e)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #



# - - - - - - #
#  CATEGORIES #
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

# Category page route.
@app.route("/categorias")
def edit_categories():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)
    query = "SELECT nome FROM categoria;"
    cursor.execute(query)

    return render_template("categorias.html", cursor = cursor, params = request.args)

  except Exception as e:
    return str(e)

  finally:
    cursor.close()
    dbConn.close()



# Simple category page route.
@app.route("/categorias/simples")
def edit_simple_categories():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)
    query = "SELECT nome FROM categoria_simples;"
    cursor.execute(query)

    return render_template("simples.html", cursor = cursor, params = request.args)

  except Exception as e:
    return str(e)

  finally:
    cursor.close()
    dbConn.close()



# Super category page route.
@app.route("/categorias/super")
def edit_super_categories():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)
    query = "SELECT nome FROM super_categoria;"
    cursor.execute(query)

    return render_template("super.html", cursor = cursor, params = request.args)

  except Exception as e:
    return str(e)

  finally:
    cursor.close()
    dbConn.close()



# Insert a new category into the system.
@app.route("/categorias/inserir_categoria")
def insert_category():

  try:
    return render_template("inserir_categoria.html", params = request.args)
  
  except Exception as e:
    return str(e)



# Performs the actual insert.
@app.route("/categorias/insert", methods = ["POST"])
def execute_insert_category():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)

    query1 = "INSERT INTO categoria VALUES (%s);"
    
    # Gets the data we need from the URL query string
    data = (request.form["nome"], )
    category_type = request.form["tipo"]

    # Inserts the new category into its respective table, specified
    # by the user.
    if category_type == "simples":
      query2 = "INSERT INTO categoria_simples VALUES (%s);"

    elif category_type == "super":
      query2 = "INSERT INTO super_categoria VALUES (%s);"

    cursor.execute(query1, data)
    cursor.execute(query2, data)

    return redirect(url_for("edit_categories"))

  except Exception as e:
    return str(e)

  finally:
    dbConn.commit()
    cursor.close()
    dbConn.close()



# Remove a category from the system.
@app.route("/categorias/remover_categoria")
def remove_category():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)

    category_type = request.args.get("tipo")

    if category_type == "simples":
      query = "SELECT nome FROM categoria_simples;"
      html = "remover_categoria_simples.html"
    
    elif category_type == "super":
      query = "SELECT nome FROM super_categoria;"
      html = "remover_categoria_super.html"

    cursor.execute(query)

    return render_template(html, cursor = cursor, params = request.args)

  except Exception as e:
    return str(e)



@app.route("/categorias/remove", methods = ['POST'])
def execute_category():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)

    query = "DELETE FROM categoria WHERE nome = %s;"
    cat_name = (request.form["nome"], )

    cursor.execute(query, cat_name)

    return redirect(url_for("edit_categories"))
    
  except Exception as e:
    return str(e)
    # return redirect(url_for("error"))

  finally:
    dbConn.commit()
    cursor.close()
    dbConn.close()



# Show all the sub-categories of a catgory.
@app.route("/categorias/super/sub_categorias")
def show_sub_categories():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)

    # Gets all the sub-categories of the super category given.
    query = "WITH RECURSIVE cte AS ( \
	              SELECT categoria AS a \
		              FROM tem_outra \
		              WHERE super_categoria=%s \
	              UNION ALL \
	              SELECT categoria \
		              FROM cte INNER JOIN tem_outra \
		              ON super_categoria = a ) \
             SELECT * FROM cte"

    # Gets the data we need from the URL query string.
    data = (request.args.get("nome"), )

    cursor.execute(query, data)

    return render_template("sub_categorias.html", cursor = cursor, params = request.args)

  except Exception as e:
    return str(e)

  finally:
    cursor.close()
    dbConn.close()



# Insert a sub-category from the system.
@app.route("/categorias/super/sub_categorias/inserir_sub_categoria")
def insert_sub_category():
  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)

    nome = (request.args.get("nome"), request.args.get("nome"))

    query = "SELECT nome FROM categoria \
                WHERE nome != %s \
             EXCEPT \
             SELECT categoria FROM tem_outra \
	              WHERE super_categoria = %s;"

    cursor.execute(query, nome)

    return render_template("inserir_sub_categoria.html", cursor = cursor, params = request.args)

  except Exception as e:
    return str(e)



# Performs the actual insert.
@app.route("/categorias/super/sub_categorias/insert_sub", methods = ["POST"])
def execute_insert_sub_category():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)

    category_query = "INSERT INTO categoria VALUES (%s);"
    simple_query = "INSERT INTO categoria_simples VALUES (%s);"
    has_other_query = "INSERT INTO tem_outra VALUES (%s, %s);"

    sub = (request.form["sub"], )
    both = (request.form["super"], request.form["sub"])

    cursor.execute(category_query, sub)
    cursor.execute(simple_query, sub)
    cursor.execute(has_other_query, both)

    return redirect(url_for("edit_super_categories"))

  except Exception as e:
    return str(e)

  finally:
    dbConn.commit()
    cursor.close()
    dbConn.close()

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #



# - - - - - - #
#  RETAILERS  #
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

# Retailer page route.
@app.route("/retalhistas")
def edit_retailers():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)
    query = "SELECT tin, nome FROM retalhista;"
    cursor.execute(query)

    return render_template("retalhistas.html", cursor = cursor, params = request.args)

  except Exception as e:
    return str(e)

  finally:
    cursor.close()
    dbConn.close()



# Insert a new retailer into the system.
@app.route("/retalhistas/inserir_retalhista")
def insert_retailer():

  try:
    return render_template("inserir_retalhista.html", params = request.args)
  
  except Exception as e:
    return str(e)



# Performs the actual insert.
@app.route("/retalhistas/insert", methods = ["POST"])
def execute_insert_retailer():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)

    query = "INSERT INTO retalhista VALUES (%s,%s);"

    # Gets the data we need from the URL query string
    data = (request.form["tin"], request.form["nome"])

    # Inserts into the 'retalhista' table the newly added retailer.
    cursor.execute(query, data)

    return redirect(url_for("edit_retailers"))

  except Exception as e:
    return redirect(url_for("error"))

  finally:
    dbConn.commit()
    cursor.close()
    dbConn.close()



# Remove a retailer from the system.
@app.route("/retalhistas/remover_retalhista")
def remove_retailer():
  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)

    query = "SELECT tin, nome FROM retalhista;"

    cursor.execute(query)

    return render_template("remover_retalhista.html", cursor = cursor, params = request.args)

  except Exception as e:
    return str(e)



# Performs the actual removal.
@app.route("/retalhistas/remove", methods = ["POST"])
def execute_remove_retailer():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)

    query = "DELETE FROM retalhista WHERE tin=%s;"
    data = (request.form["tin"], )

    cursor.execute(query, data)

    return redirect(url_for("edit_retailers"))

  except Exception as e:
    return redirect(url_for("error"))

  finally:
    dbConn.commit()
    cursor.close()
    dbConn.close()

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #



# - - - - - - - - - - - - #
#   REPLENISHMENT EVENTS  #
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

# IVM page route.
@app.route("/ivm")
def edit_ivm():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)

    query = "SELECT * FROM ivm;"

    cursor.execute(query)

    return render_template("ivm.html", cursor = cursor, params = request.args)

  except Exception as e:
    return str(e)

  finally:
    cursor.close()
    dbConn.close()





# Replenishment event page route.
@app.route("/ivm/eventos")
def edit_events():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)

    data = (request.args.get("num_serie"), )
    query = "SELECT * FROM evento_reposicao \
	                NATURAL JOIN produto \
	                NATURAL JOIN retalhista \
                    WHERE num_serie = %s;"

    cursor.execute(query, data)

    return render_template("eventos.html", cursor = cursor, params = request.args)

  except Exception as e:
    return str(e)

  finally:
    cursor.close()
    dbConn.close()



@app.route("/ivm/eventos/unidades_repostas")
def replenished_products():
  dbConn = None
  cursor = None

  try:
    dbConn = psycopg2.connect(DB_CONNECTION_STRING)
    cursor = dbConn.cursor(cursor_factory = psycopg2.extras.DictCursor)

    data = (request.args.get("num_serie"), )
    query = "SELECT nome, SUM(unidades) \
	           FROM tem_categoria NATURAL JOIN evento_reposicao \
                 WHERE num_serie = %s \
	  	       GROUP BY nome;"

    cursor.execute(query, data)

    return render_template("unidades_repostas.html", cursor = cursor, params = request.args)

  except Exception as e:
    return str(e)

  finally:
    cursor.close()
    dbConn.close()


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #


CGIHandler().run(app) 
