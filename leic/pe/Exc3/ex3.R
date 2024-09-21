library(pacman)
pacman::p_load(pacman, rio, tidyverse, datasets, ggplot2, dplyr, data.table, ggpubr)


# Import the altered version of the excel file (The one with only the values)
file <- import("C:/Users/valen/OneDrive/Área de Trabalho/QualidadeARO3.xlsx")


# Gets the data from the seasons we need
Estarreja <- as.numeric(file[c(1:8785), 3])
Antas_Espinho <- as.numeric(file[c(1:8785), 1])

aux <- c(Estarreja, Antas_Espinho)


# Creates the data frames
df_1 <- data.frame(Estarreja)
df_2 <- data.frame(Antas_Espinho)


# Add a new column to both data frames to know where they came from later
df_1$Estações <- "Estarreja"
df_2$Estações <- "Antas Espinho"

# Renames the columns of both data frames so we can bind them
colnames(df_1) <- c("C1", "Estações")
colnames(df_2) <- c("C1", "Estações")


# Combine both data frames
df_final <- rbind(df_1, df_2)


# Creates the basic plot
plot <- ggplot(df_final, aes(aux, fill = Estações))

# Defines the histogram
hist <- geom_histogram(alpha = 0.7, binwidth = 7, col = "white", position = 'identity')

# Defines the theme of the graphs
theme <- theme_minimal() 

# Changes the labels of the graph
labs <- labs(x = "Qualidade do ar", y = "Número de observações")

plot +  hist + theme + labs
