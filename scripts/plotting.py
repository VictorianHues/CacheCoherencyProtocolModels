import numpy as np
import matplotlib.pyplot as plt

cpu_ids = np.arange(8)
random_rw_hitrate = [0.448, 0.328, 0.32, 0.4, 0.432, 0.344, 0.368, 0.328]
all_reads_same_address = [99.17, 98.36, 97.55, 96.74, 95.94, 95.13, 94.32, 93.51]
all_writes_same_address = [99.17, 98.36, 97.55, 96.74, 95.94, 95.13, 94.32, 93.51]
all_reads_random_addresses = [0.416, 0.336, 0.416, 0.424, 0.4, 0.384, 0.336, 0.4]
all_writes_random_addresses = [0.368, 0.448, 0.464, 0.4, 0.432, 0.392, 0.408, 0.384]
alternating_rw_same_address = [99.17, 98.36, 97.55, 96.74, 95.94, 95.13, 94.32, 93.51]
random_rw_random_addresses = [0.448, 0.328, 0.32, 0.4, 0.432, 0.344, 0.368, 0.328]

plt.figure(figsize=(8, 5))
plt.bar(cpu_ids, random_rw_hitrate, color='skyblue', alpha=0.75)

plt.xlabel("CPU ID")
plt.ylabel("Hit Rate (%)")
plt.title("Cache Hit Rate for Random Read/Writes on Random Addresses")
plt.xticks(cpu_ids)
plt.ylim(0, max(random_rw_hitrate)+0.1)
plt.grid(axis='y', linestyle='--', alpha=0.7)

plt.show()


# Data for each run type
hit_rates = {
    "All Reads on Same Address": {
        "RHitrate": [99.17, 98.36, 97.55, 96.74, 95.94, 95.13, 94.32, 93.51],
        "WHitrate": [None] * 8,  # No writes in this test
        "Hitrate": [99.17, 98.36, 97.55, 96.74, 95.94, 95.13, 94.32, 93.51],
    },
    "All Writes on Same Address": {
        "RHitrate": [None] * 8,  # No reads in this test
        "WHitrate": [99.17, 98.36, 97.55, 96.74, 95.94, 95.13, 94.32, 93.51],
        "Hitrate": [99.17, 98.36, 97.55, 96.74, 95.94, 95.13, 94.32, 93.51],
    },
    "All Reads on Random Addresses": {
        "RHitrate": [0.416, 0.336, 0.416, 0.424, 0.4, 0.384, 0.336, 0.4],
        "WHitrate": [None] * 8,  # No writes in this test
        "Hitrate": [0.416, 0.336, 0.416, 0.424, 0.4, 0.384, 0.336, 0.4],
    },
    "All Writes on Random Addresses": {
        "RHitrate": [None] * 8,  # No reads in this test
        "WHitrate": [0.368, 0.448, 0.464, 0.4, 0.432, 0.392, 0.408, 0.384],
        "Hitrate": [0.368, 0.448, 0.464, 0.4, 0.432, 0.392, 0.408, 0.384],
    },
    "Alternating Read/Writes on Same Address": {
        "RHitrate": [99.17, 98.36, 97.55, 96.74, 95.94, 95.13, 94.32, 93.51],
        "WHitrate": [None] * 8,  # No writes in this test
        "Hitrate": [99.17, 98.36, 97.55, 96.74, 95.94, 95.13, 94.32, 93.51],
    },
    "Random Read/Writes on Random Addresses": {
        "RHitrate": [0.4465, 0.4025, 0.3033, 0.3055, 0.4488, 0.3375, 0.3963, 0.2708],
        "WHitrate": [0.4495, 0.2544, 0.3368, 0.4936, 0.4153, 0.3505, 0.3391, 0.3857],
        "Hitrate": [0.448, 0.328, 0.32, 0.4, 0.432, 0.344, 0.368, 0.328],
    },
}

# Compute averages while handling None values
averages = {
    run_type: {
        rate_type: np.mean([v for v in values if v is not None])
        for rate_type, values in data.items()
    }
    for run_type, data in hit_rates.items()
}