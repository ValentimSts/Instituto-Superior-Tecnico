library(ggplot2)

# Global vars
n1 <- 2
n2 <- 27
n3 <- 60
my_seed <- 566
sample_size <- 830
lower_bound <- 6
upper_bound <- 10

n_values <- c(n1, n2, n3)

# Sets our seed
set.seed(my_seed)


means <- matrix(data = NA, nrow = 3, ncol = sample_size)


for (i in 1:3) {
  # Creates the samples for each value of n
  samples <- replicate(n = sample_size, runif(n_values[i], min = lower_bound,
                                                           max = upper_bound))
  
  # Calculate the mean of each column
  means[i,] <- colMeans(samples)
}


# Creates all the data frames we need
df_1 <- data.frame(means[1,])
df_2 <- data.frame(means[2,])
df_3 <- data.frame(means[3,])


hist <- geom_histogram(aes(y = ..density..),
                       bins = 30, col = "white", fill = "darkorange")
theme <- theme_minimal()

# Creates the normal distribution curves to be fitted into the histograms
fun1 <- stat_function(fun = dnorm,
                      args = list(mean = mean(means[1,]),
                                  sd = sqrt(((upper_bound - lower_bound )^2)/12/n1)),
                      colour = 'darkblue', size = 0.7)
fun2 <- stat_function(fun = dnorm,
                      args = list(mean = mean(means[2,]),
                                  sd = sqrt(((upper_bound - lower_bound )^2)/12/n2)),
                      colour = 'darkblue', size = 0.7)
fun3 <- stat_function(fun = dnorm,
                      args = list(mean = mean(means[3,]),
                                  sd = sqrt(((upper_bound - lower_bound )^2)/12/n3)),
                      colour = 'darkblue', size = 0.7)


# Creates the three plots
plot_1 <- ggplot(df_1, aes(means[1,], fill = means[1,])) + hist + theme + fun1 +
  labs(x = "Distribuição da média para n1", y = "Ocorrências")

plot_2 <- ggplot(df_2, aes(means[2,], fill = means[2,])) + hist + theme + fun2 +
  labs(x = "Distribuição da média para n2", y = "Ocorrências")

plot_3 <- ggplot(df_3, aes(means[3,], fill = means[3,])) + hist + theme + fun3 +
  labs(x = "Distribuição da média para n3", y = "Ocorrências")


library(gridExtra)
# Arranges all the plots on the same page
grid.arrange(plot_1, plot_2, plot_3, ncol = 3)
