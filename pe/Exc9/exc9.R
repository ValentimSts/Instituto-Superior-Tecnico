
# Global vars
my_seed <- 317
m <- 1500
n <- seq(100, 5000, by = 100)
lambda <- 0.76

trust_level <- 0.94
alpha <- 1 - trust_level


set.seed(my_seed)


total <- 0
res <- c()


for (x in n) {
  
  for (i in 1:m) {
    # Generates a random sample of size "x" with the "lambda" as its rate
    exp <- rexp(x, rate = lambda)
    new_lambda <- 1/mean(exp)

    # Calculates the upper and lower bounds for our confidence interval
    upper_bound <- new_lambda * (1 + (qnorm(1 - (alpha/2)) / sqrt(x)))
    lower_bound <- new_lambda * (1 - (qnorm(1 - (alpha/2)) / sqrt(x)))
    # Calculates the amplitude of the interval
    amp <- abs(upper_bound - lower_bound)
    
    total = total + amp
  }
  # Calculates the probability 
  prob <- total/m
  # Appends said probability to our "res" vector
  res <- append(res, prob)
  total = 0
}


# Creates the data frame
df <- data.frame(res, n)


library(ggplot2)
# Plots the graph
ggplot(df, aes(x = n, y = res)) +
  geom_point(colour = 'darkorange') +
  theme_minimal() +
  labs(x = "Dimensão da amostra", y = "Média da amplitude")

