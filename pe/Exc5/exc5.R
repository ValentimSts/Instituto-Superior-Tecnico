
# Global vars
my_seed <- 781
lambda <- 0.09
n <- 600
greater_than <- 7
decimal_places <- 6


set.seed(my_seed)


# Creates a sample of size n from the exponential distribution
exp <- rexp(n, lambda)


# Finds empirical cumulative distribution function for our sample "exp" and
# gets the value of that same function on the "greater_than" x point
prob1 <- 1 - ecdf(exp)(greater_than)


# Calculates the exponential cumulative distribution function for the initial
# lambda
# --------------------------------------------------
# - our bound is "greater_than" = 7
# - our rate is the same as the lambda variable
# - lower.tail = FALSE -> P(X > x) in this case P(X > 7)
prob2 <- pexp(greater_than, rate = lambda, lower.tail = FALSE)

aux <- abs(prob1 - prob2)

res <- round(aux, decimal_places)

print(res)


# res: 0.020925
