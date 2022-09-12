library(pacman)
pacman::p_load(pacman, rio, tidyverse, datasets, ggplot2, dplyr, data.table, ggpubr)


# Import the altered version of the excel file (The one with only the values)
file <- import("C:/Users/valen/OneDrive/Área de Trabalho/Altered.xlsx")


# Get from the file only the lines we need, and create a matrix with them
#
# DK (Denmark) -> line 10
# IS (Iceland) -> line 31
# PL (Poland) -> line 26

#     1   2   3
# DK ... ... ...
# IS ... ... ...
# PL ... ... ...

table <- file[c(10,31,26),c(1,2,3)]


# Creates a new array with the year values repeated 3 times (one for each country)
Ano <- rep(c("2004", "2018"), 3)

# Creates a new array with each country value repeated 2 times (one for each year)
País <- rep(c("Dinamarca", "Islândia", "Polónia"), each = 2)


# Stores the values for each country/year pair
DK_1 <- as.double(table[1,2])
DK_2 <- as.double(table[1,3])

IS_1 <- as.double(table[2,2])
IS_2 <- as.double(table[2,3])

PL_1 <- as.double(table[3,2])
PL_2 <- as.double(table[3,3])


# Creates an array containing all the country/year pairs
ResiduosPerCapita <- array(c(DK_1, DK_2, IS_1, IS_2, PL_1, PL_2))


# Creates the data frame
df <- data.frame(Ano, País, ResiduosPerCapita)


# Creates the default plot with the variables created previously
plot <- ggplot(df, aes(x = País, y = ResiduosPerCapita, fill = Ano))


# Defines the columns of the graph
bars <- geom_col(position = position_dodge())

# Defines the text of the graph
text <- geom_text(aes(label = ResiduosPerCapita),
                  vjust = 1.7,
                  color = "white",
                  position = position_dodge(0.9),
                  size = 4)

# Defines the scale of the graph
scale <- scale_fill_brewer(palette="Paired")

# Defines the theme of the graph
theme <- theme_minimal() 


# Joins Final result
plot + bars + text + scale + theme
