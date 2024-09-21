library(ggplot2)

# Import the data from the excel file
file <- import("C:/Users/valen/OneDrive/Área de Trabalho/Utentes.xlsx")


# Gets the data we need from the file
dados <- file[, c(1, 3)]

Idade <- dados[, 1]
IMC <- dados[, 2]


# Create the basic plot to be used
plot <- ggplot(dados, main = "Dispersão entre Idade e IMC dos utentes", 
               aes(x = Idade, y = IMC))


# Defines the scatter point graph
points <- geom_point(shape = 22, fill = "orange")
# Defines the regression line
smooth <- geom_smooth(method = "lm", formula = y ~ x, color = "brown")
# Defines the theme 
theme <- theme_minimal()
# Defines the labels
labs <- labs(x = "Idade", y = "IMC")


plot + points + smooth + labs + theme