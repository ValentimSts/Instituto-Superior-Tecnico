
# Global vars
my_seed <- 793
m <- 900
n <- seq(100, 2500, by = 100)
lambda <- 1.78

epsilon <- 30/100
lambda_c <- 1.58

trust_level <- 0.94
alpha <- 1 - trust_level


set.seed(my_seed)


# Creates the lists used to store all the samples 
buffer <- list()
uncontaminated_vals <- list()
contaminated_vals <- list()


# Generates 'm' samples for each value of 'n' and stores all of them in the buffer
for(num in n) {
  
  for(i in 1:m) {
    # Generates a random sample of size "x" with the "lambda" as its rate
    exp <- rexp(num, rate = lambda)
    # Stores the sample in our buffer
    buffer <- append(buffer, list(exp))
  }
  
  # Fills both lists of values with the samples we got for later use
  uncontaminated_vals <- append(uncontaminated_vals, list(buffer))
  contaminated_vals <- append(contaminated_vals, list(buffer))
  
  # Resets the buffer
  buffer <- list()
}


# Substitutes any epsilon values for new ones with a new 'lambda_c'
for(num in n) {
  
  for(i in 1:m) {
    # Stores the iteration we're in for the 'n' sequence:
    # n = 100 -> 1, n = 200 -> 2, ..., n = 2500 -> 25
    iter <- num/100
    
    # Variable used to iterate over the random values 
    x <- 1
    
    # Generates a new random sample of size 'num*epsilon' with a new rate 'lambda_c' 
    new_exp_vals <- rexp(num*epsilon, rate = lambda_c)
    
    # Gets random values from a random sample of size 'num*epsilon' to replace
    # the older values of one of our vectors 
    # Creating the contaminated vector to be used for "MAc" later  
    for(random_val in sample.int(num, num*epsilon)) {
      contaminated_vals[[iter]][[i]][[random_val]] <- new_exp_vals[[x]]
      x <- x+1
    }
    
  }
}


total <- 0

# Vector for the uncontaminated samples
MA <- c()

for(num in n) {
  
  for(i in 1:m) {
    
    # Stores the iteration we're in for the 'n' sequence:
    # n = 100 -> 1, n = 200 -> 2, ..., n = 2500 -> 25
    iter <- num/100
    
    # Calculates the upper and lower bounds for our confidence interval
    upper_bound <- (mean(uncontaminated_vals[[iter]][[i]])) * 
                   (1 + qnorm(1 - (alpha/2)) / sqrt(num))
    
    lower_bound <- (mean(uncontaminated_vals[[iter]][[i]])) *
                   (1 - qnorm(1 - (alpha/2)) / sqrt(num))
    
    # Calculates the amplitude of the interval
    amp <- abs(upper_bound - lower_bound)
    
    total = total + amp
  }
  # Calculates the probability 
  prob = total/m
  # Appends said probability to our "MA" vector
  MA <- append(MA, prob)
  total = 0
}


total = 0


# Vector for the contaminated samples
MAc <- c()

for(num in n){
  
  for(i in 1:m){
    
    # Stores the iteration we're in for the 'n' sequence:
    # n = 100 -> 1, n = 200 -> 2, ..., n = 2500 -> 25
    iter <- num/100
    
    # Calculates the upper and lower bounds for our confidence interval
    upper_bound <- (mean(contaminated_vals[[iter]][[i]]) -
      qnorm(1 - (alpha/2), mean = 0, sd = 1) * sqrt(var(contaminated_vals[[iter]][[i]])/num)) ** -1
    
    lower_bound <- (mean(contaminated_vals[[iter]][[i]]) -
      qnorm((alpha/2), mean = 0, sd = 1) * sqrt(var(contaminated_vals[[iter]][[i]])/num)) ** -1
    
    # Calculates the amplitude of the interval
    amp <- abs(upper_bound - lower_bound)
    
    total = total + amp
  }
  # Calculates the probability 
  prob = total/m
  # Appends said probability to our "MA" vector
  MAc <- append(MAc, prob)
  total = 0
}


# Creates our data frames
df_1 <- data.frame(n, MA)
df_2 <- data.frame(n, MAc)

# Add a new column to both data frames
df_1$amplitudes <- "MA"
df_2$amplitudes <- "MAc"

# Renames the columns of both data frames so we can bind them
colnames(df_1) <- c("n", "means", "amplitudes")
colnames(df_2) <- c("n", "means", "amplitudes")

# Combine both data frames
df_final <- rbind(df_1, df_2)


library(ggplot2)
# Plots the graph
ggplot(df_final, aes(x = n, y = means, color = amplitudes)) +
  geom_line(size = 0.7)+
  labs(x = "Valor de n", y = "Média de Amplitudes", color = "Amplitudes")+
  theme_minimal()

