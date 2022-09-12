library(ggplot2, tidyverse)


# Import the altered version of the excel file (The one with only the values)
file <- import("C:/Users/valen/OneDrive/Área de Trabalho/Altered.xlsx")


# Gets the lines and columns we need to plot, Men and Women from the following
# countries:
#
# BG (Bulgaria)
# PL (Poland)
# CY (Cyprus)
#
# 2002 - 2019 -> lines 43 to 61
#
# H: BG -> col 42 / PL -> col 61 / CY -> col 43
# M: BG -> col 76 / PL -> col 95 / CY -> col 77

# Creates an array to store all the values contiguously 
Homem_aux <- file[c(42:59), c(42, 61, 43)]
Homem <- array(c(Homem_aux[c(1:18), 1], Homem_aux[c(1:18), 2], Homem_aux[c(1:18), 3]))

Mulher_aux <- file[c(42:59), c(76, 95, 77)]
Mulher <- array(c(Mulher_aux[c(1:18), 1], Mulher_aux[c(1:18), 2], Mulher_aux[c(1:18), 3]))


# Creates a new vector with the year values repeated 3 times (once for each country)
# "2002 2003 2004 ... 2002 2003 2004 ... 2002 2003 2004 ..."
Ano <- rep(c(file[c(42:59),1]), 3)


# Creates a new vector with the country names, each repeated 18 times (one for each year)
País_M <- rep(c("BG - Bulgária (M)", "PL - Polónia (M)", "CY - Chipre (M)"), each = 18)
País_H <- rep(c("BG - Bulgária (H)", "PL - Polónia (H)", "CY - Chipre (H)"), each = 18)


# Creates the data frames for both men and women
df_M <- data.frame(Ano, País_M, Mulher)
df_H <- data.frame(Ano, País_H, Homem)


# Renames the columns of both data frames so we can bind them
colnames(df_M) <- c("C1", "C2", "C3")
colnames(df_H) <- c("C1", "C2", "C3")


# Add a new column to both data frames (Duplicate from column 2)
df_M$Países = df_M$C2
df_H$Países = df_H$C2


# Finally joins both data frames
df <- rbind(df_M, df_H)


# Creates the default plot with the variables created previously
plot <- ggplot(df, aes(x = (df %>% pull("C1")) , y = (df %>% pull("C3")), col = Países))

# Creates a line for our graph
line <- geom_line()

# Creates a scatter point representation for our data
points <- geom_point()

# Defines the theme of the graph
theme <- theme_minimal() 

# Changes the labels of the graph
labs <- labs(x = "Anos", y = "Esperança de vida", color = "Países")


# Creates the final result
plot + line + points + theme + labs


