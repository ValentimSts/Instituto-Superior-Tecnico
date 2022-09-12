
# Global vars
my_seed <- 386
m <- 850
n <- 1133
lambda <- 4.1
trust_level <- 0.9

alpha <- 1 - trust_level
decimal_places <- 6


set.seed(my_seed)

x <- 0

for (i in 1:m) {
  
  # Generates a random sample of size n with the "lambda" as its rate
  dist_sample <- rexp(n, rate = lambda)
  # Calculates the new lambda for that same sample
  new_lambda <- 1/mean(dist_sample)
  
  # Calculates the upper and lower bounds for our confidence interval
  upper_bound <- new_lambda * (1 + (qnorm(1 - (alpha/2)) / sqrt(n)))
  lower_bound <- new_lambda * (1 - (qnorm(1 - (alpha/2)) / sqrt(n)))
  # Calculates the amplitude of the interval
  amp <- abs(upper_bound - lower_bound)
  
  # Adds up the amplitude
  x = x + amp
}

prob <- x/m
res <- round(prob, decimal_places)

print(res)


# res: 0.400688
