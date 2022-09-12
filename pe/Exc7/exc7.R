
# Global vars
my_seed <- 187
sample_size <- 150
dim <- 46
a <- 47
b <- 0.47
decimal_places <- 6


set.seed(my_seed)


# Creates our samples for the binomial distribution
samples <- replicate(sample_size, rbinom(dim, a, b))

# Creates a means matrix to store the mean of all our samples
means <- matrix(data = NA, nrow = 1, ncol = sample_size)
means <- colMeans(samples)


# Calculates the mean of the means and the expected mean for our samples
sample_mean <- mean(means)
expected_mean <- a*b


final_mean = abs(sample_mean - expected_mean)
res = round(final_mean, decimal_places)


print(res)


# res: 0.018841