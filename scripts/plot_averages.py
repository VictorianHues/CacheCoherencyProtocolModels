import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# Data for plotting
experiments = ["Assignment 1", "FFT 1024", "Matrix Mult 50x50", "Matrix Vector 5000x8"]
sim_ids = [1, 4, 7, 9]
sim_times = [5727, 1913830, 27803495, 12265648]
protocols = ["N/A", "N/A", "VALID/INVALID", "VALID/INVALID"]

# Plot 1: Total Simulation Time for Experiments
plt.figure(figsize=(10, 5))
sns.barplot(x=experiments, y=sim_times, palette="viridis")
plt.xlabel("Experiment")
plt.ylabel("Total Simulation Time (ns)")
plt.title("Total Simulation Time Across Experiments")
plt.xticks(rotation=45)
plt.show()

# Data for hit rates
sim_ids_hit = [2, 3, 5, 6, 7, 8, 9, 10]
hit_rates = [45, 45, 99.38, 99.38, 96.26, 99.86, 97.82, 98.53]

# Plot 2: Cache Hit Rates
plt.figure(figsize=(10, 5))
sns.barplot(x=sim_ids_hit, y=hit_rates, palette="magma")
plt.xlabel("Simulation ID")
plt.ylabel("Cache Hit Rate (%)")
plt.title("Cache Hit Rate by Simulation")
plt.show()

# Data for bus wait times
bus_wait_times = [102, 55, 159831, 4686, 49335, 404, 16252, 1456]

# Plot 3: Bus Wait Times
plt.figure(figsize=(10, 5))
sns.barplot(x=sim_ids_hit, y=bus_wait_times, palette="coolwarm")
plt.xlabel("Simulation ID")
plt.ylabel("Bus Wait Time (ns)")
plt.title("Bus Wait Times Across Simulations")
plt.yscale("log")  # Log scale for better visualization
plt.show()
