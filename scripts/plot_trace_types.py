import numpy as np
import matplotlib.pyplot as plt

# Data for VALID/INVALID CACHE COHERENCY PROTOCOL
valid_invalid_data = np.array([
    [6271, 5477, 794, 6229, 2170, 4059, 87.34, 34.84, 61.18],
    [6211, 5421, 790, 6289, 2117, 4172, 87.28, 33.66, 60.3],
    [6264, 5507, 757, 6236, 2080, 4156, 87.92, 33.35, 60.7],
    [6220, 5533, 687, 6280, 1992, 4288, 88.95, 31.72, 60.2],
    [6239, 5500, 739, 6261, 1944, 4317, 88.16, 31.05, 59.55],
    [6223, 5396, 827, 6277, 1959, 4318, 86.71, 31.21, 58.84],
    [6308, 5289, 1019, 6192, 1866, 4326, 83.85, 30.14, 57.24],
    [6277, 5320, 957, 6223, 1869, 4354, 84.75, 30.03, 57.51]
])

# Data for MOESI CACHE COHERENCY PROTOCOL
moesi_data = np.array([
    [6271, 6122, 149, 6229, 2149, 4080, 97.62, 34.5, 66.17],
    [6211, 6062, 149, 6289, 2100, 4189, 97.6, 33.39, 65.3],
    [6264, 6105, 159, 6236, 2075, 4161, 97.46, 33.27, 65.44],
    [6220, 6061, 159, 6280, 2057, 4223, 97.44, 32.75, 64.94],
    [6239, 6083, 156, 6261, 1976, 4285, 97.5, 31.56, 64.47],
    [6223, 6068, 155, 6277, 2095, 4182, 97.51, 33.38, 65.3],
    [6308, 6160, 148, 6192, 2015, 4177, 97.65, 32.54, 65.4],
    [6277, 6119, 158, 6223, 2009, 4214, 97.48, 32.28, 65.02]
])

# Calculate averages
valid_invalid_avg = np.mean(valid_invalid_data, axis=0)
moesi_avg = np.mean(moesi_data, axis=0)

print("Random Read/Writes on Random Addresses Trace File")
print("VALID/INVALID CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {valid_invalid_avg[0]:.2f}, RHit: {valid_invalid_avg[1]:.2f}, Rmiss: {valid_invalid_avg[2]:.2f}, Writes: {valid_invalid_avg[3]:.2f}, WHit: {valid_invalid_avg[4]:.2f}, WMiss: {valid_invalid_avg[5]:.2f}, RHitrate: {valid_invalid_avg[6]:.2f}, WHitrate: {valid_invalid_avg[7]:.2f}, Hitrate: {valid_invalid_avg[8]:.2f}")

print("\nMOESI CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {moesi_avg[0]:.2f}, RHit: {moesi_avg[1]:.2f}, Rmiss: {moesi_avg[2]:.2f}, Writes: {moesi_avg[3]:.2f}, WHit: {moesi_avg[4]:.2f}, WMiss: {moesi_avg[5]:.2f}, RHitrate: {moesi_avg[6]:.2f}, WHitrate: {moesi_avg[7]:.2f}, Hitrate: {moesi_avg[8]:.2f}")

print()
print()


# Data for Alternating Read/Writes on same address - VALID/INVALID CACHE COHERENCY PROTOCOL
valid_invalid_alternating_data = np.array([
    [6250, 6249, 1, 6250, 6250, 0, 99.98, 100, 99.99],
    [6250, 6249, 1, 6250, 6218, 32, 99.98, 99.49, 99.74],
    [6250, 6249, 1, 6250, 6182, 68, 99.98, 98.91, 99.45],
    [6250, 6249, 1, 6250, 6146, 104, 99.98, 98.34, 99.16],
    [6250, 6249, 1, 6250, 6106, 144, 99.98, 97.7, 98.84],
    [6250, 6249, 1, 6250, 6058, 192, 99.98, 96.93, 98.46],
    [6250, 6249, 1, 6250, 6002, 248, 99.98, 96.03, 98.01],
    [6250, 6249, 1, 6250, 5922, 328, 99.98, 94.75, 97.37]
])

# Data for Alternating Read/Writes on same address - MOESI CACHE COHERENCY PROTOCOL
moesi_alternating_data = np.array([
    [6250, 5980, 270, 6250, 5685, 565, 95.68, 90.96, 93.32],
    [6250, 5980, 270, 6250, 4993, 1257, 95.68, 79.89, 87.78],
    [6250, 6036, 214, 6250, 4658, 1592, 96.58, 74.53, 85.55],
    [6250, 6249, 1, 6250, 3132, 3118, 99.98, 50.11, 75.05],
    [6250, 6040, 210, 6250, 2940, 3310, 96.64, 47.04, 71.84],
    [6250, 5990, 260, 6250, 4941, 1309, 95.84, 79.06, 87.45],
    [6250, 6003, 247, 6250, 3965, 2285, 96.05, 63.44, 79.74],
    [6250, 6248, 2, 6250, 3134, 3116, 99.97, 50.14, 75.06]
])

# Calculate averages
valid_invalid_alternating_avg = np.mean(valid_invalid_alternating_data, axis=0)
moesi_alternating_avg = np.mean(moesi_alternating_data, axis=0)

print("Alternating Read/Writes on same address Trace File")
print("VALID/INVALID CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {valid_invalid_alternating_avg[0]:.2f}, RHit: {valid_invalid_alternating_avg[1]:.2f}, Rmiss: {valid_invalid_alternating_avg[2]:.2f}, Writes: {valid_invalid_alternating_avg[3]:.2f}, WHit: {valid_invalid_alternating_avg[4]:.2f}, WMiss: {valid_invalid_alternating_avg[5]:.2f}, RHitrate: {valid_invalid_alternating_avg[6]:.2f}, WHitrate: {valid_invalid_alternating_avg[7]:.2f}, Hitrate: {valid_invalid_alternating_avg[8]:.2f}")

print("\nMOESI CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {moesi_alternating_avg[0]:.2f}, RHit: {moesi_alternating_avg[1]:.2f}, Rmiss: {moesi_alternating_avg[2]:.2f}, Writes: {moesi_alternating_avg[3]:.2f}, WHit: {moesi_alternating_avg[4]:.2f}, WMiss: {moesi_alternating_avg[5]:.2f}, RHitrate: {moesi_alternating_avg[6]:.2f}, WHitrate: {moesi_alternating_avg[7]:.2f}, Hitrate: {moesi_alternating_avg[8]:.2f}")

print()
print()




# Data for All Writes on Random Addresses - VALID/INVALID CACHE COHERENCY PROTOCOL
valid_invalid_writes_data = np.array([
    [0, 0, 0, 12500, 3440, 9060, np.nan, 27.52, 27.52],
    [0, 0, 0, 12500, 3441, 9059, np.nan, 27.53, 27.53],
    [0, 0, 0, 12500, 3445, 9055, np.nan, 27.56, 27.56],
    [0, 0, 0, 12500, 3433, 9067, np.nan, 27.46, 27.46],
    [0, 0, 0, 12500, 3429, 9071, np.nan, 27.43, 27.43],
    [0, 0, 0, 12500, 3357, 9143, np.nan, 26.86, 26.86],
    [0, 0, 0, 12500, 3411, 9089, np.nan, 27.29, 27.29],
    [0, 0, 0, 12500, 3507, 8993, np.nan, 28.06, 28.06]
])

# Data for All Writes on Random Addresses - MOESI CACHE COHERENCY PROTOCOL
moesi_writes_data = np.array([
    [0, 0, 0, 12500, 3544, 8956, np.nan, 28.35, 28.35],
    [0, 0, 0, 12500, 3528, 8972, np.nan, 28.22, 28.22],
    [0, 0, 0, 12500, 3502, 8998, np.nan, 28.02, 28.02],
    [0, 0, 0, 12500, 3555, 8945, np.nan, 28.44, 28.44],
    [0, 0, 0, 12500, 3506, 8994, np.nan, 28.05, 28.05],
    [0, 0, 0, 12500, 3452, 9048, np.nan, 27.62, 27.62],
    [0, 0, 0, 12500, 3505, 8995, np.nan, 28.04, 28.04],
    [0, 0, 0, 12500, 3551, 8949, np.nan, 28.41, 28.41]
])

# Calculate averages
valid_invalid_writes_avg = np.nanmean(valid_invalid_writes_data, axis=0)
moesi_writes_avg = np.nanmean(moesi_writes_data, axis=0)

print("All Writes on Random Addresses Trace File")
print("VALID/INVALID CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {valid_invalid_writes_avg[0]:.2f}, RHit: {valid_invalid_writes_avg[1]:.2f}, Rmiss: {valid_invalid_writes_avg[2]:.2f}, Writes: {valid_invalid_writes_avg[3]:.2f}, WHit: {valid_invalid_writes_avg[4]:.2f}, WMiss: {valid_invalid_writes_avg[5]:.2f}, RHitrate: {valid_invalid_writes_avg[6]:.2f}, WHitrate: {valid_invalid_writes_avg[7]:.2f}, Hitrate: {valid_invalid_writes_avg[8]:.2f}")

print("\nMOESI CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {moesi_writes_avg[0]:.2f}, RHit: {moesi_writes_avg[1]:.2f}, Rmiss: {moesi_writes_avg[2]:.2f}, Writes: {moesi_writes_avg[3]:.2f}, WHit: {moesi_writes_avg[4]:.2f}, WMiss: {moesi_writes_avg[5]:.2f}, RHitrate: {moesi_writes_avg[6]:.2f}, WHitrate: {moesi_writes_avg[7]:.2f}, Hitrate: {moesi_writes_avg[8]:.2f}")

print()
print()


# Data for All Reads on Random Addresses - VALID/INVALID CACHE COHERENCY PROTOCOL
valid_invalid_reads_data = np.array([
    [12500, 10887, 1613, 0, 0, 0, 87.1, np.nan, 87.1],
    [12500, 10941, 1559, 0, 0, 0, 87.53, np.nan, 87.53],
    [12500, 10931, 1569, 0, 0, 0, 87.45, np.nan, 87.45],
    [12500, 10913, 1587, 0, 0, 0, 87.3, np.nan, 87.3],
    [12500, 10864, 1636, 0, 0, 0, 86.91, np.nan, 86.91],
    [12500, 10697, 1803, 0, 0, 0, 85.58, np.nan, 85.58],
    [12500, 11118, 1382, 0, 0, 0, 88.94, np.nan, 88.94],
    [12500, 11451, 1049, 0, 0, 0, 91.61, np.nan, 91.61]
])

# Data for All Reads on Random Addresses - MOESI CACHE COHERENCY PROTOCOL
moesi_reads_data = np.array([
    [12500, 12223, 277, 0, 0, 0, 97.78, np.nan, 97.78],
    [12500, 12229, 271, 0, 0, 0, 97.83, np.nan, 97.83],
    [12500, 12218, 282, 0, 0, 0, 97.74, np.nan, 97.74],
    [12500, 12237, 263, 0, 0, 0, 97.9, np.nan, 97.9],
    [12500, 12222, 278, 0, 0, 0, 97.78, np.nan, 97.78],
    [12500, 12216, 284, 0, 0, 0, 97.73, np.nan, 97.73],
    [12500, 12229, 271, 0, 0, 0, 97.83, np.nan, 97.83],
    [12500, 12228, 272, 0, 0, 0, 97.82, np.nan, 97.82]
])

# Calculate averages
valid_invalid_reads_avg = np.nanmean(valid_invalid_reads_data, axis=0)
moesi_reads_avg = np.nanmean(moesi_reads_data, axis=0)

print("All Reads on Random Addresses Trace File")
print("VALID/INVALID CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {valid_invalid_reads_avg[0]:.2f}, RHit: {valid_invalid_reads_avg[1]:.2f}, Rmiss: {valid_invalid_reads_avg[2]:.2f}, Writes: {valid_invalid_reads_avg[3]:.2f}, WHit: {valid_invalid_reads_avg[4]:.2f}, WMiss: {valid_invalid_reads_avg[5]:.2f}, RHitrate: {valid_invalid_reads_avg[6]:.2f}, WHitrate: {valid_invalid_reads_avg[7]:.2f}, Hitrate: {valid_invalid_reads_avg[8]:.2f}")

print("\nMOESI CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {moesi_reads_avg[0]:.2f}, RHit: {moesi_reads_avg[1]:.2f}, Rmiss: {moesi_reads_avg[2]:.2f}, Writes: {moesi_reads_avg[3]:.2f}, WHit: {moesi_reads_avg[4]:.2f}, WMiss: {moesi_reads_avg[5]:.2f}, RHitrate: {moesi_reads_avg[6]:.2f}, WHitrate: {moesi_reads_avg[7]:.2f}, Hitrate: {moesi_reads_avg[8]:.2f}")

print()
print()



# Data for All Writes on the same address - VALID/INVALID CACHE COHERENCY PROTOCOL
valid_invalid_same_address_writes_data = np.array([
    [0, 0, 0, 12500, 2641, 9859, np.nan, 21.13, 21.13],
    [0, 0, 0, 12500, 2641, 9859, np.nan, 21.13, 21.13],
    [0, 0, 0, 12500, 2641, 9859, np.nan, 21.13, 21.13],
    [0, 0, 0, 12500, 2641, 9859, np.nan, 21.13, 21.13],
    [0, 0, 0, 12500, 2641, 9859, np.nan, 21.13, 21.13],
    [0, 0, 0, 12500, 2641, 9859, np.nan, 21.13, 21.13],
    [0, 0, 0, 12500, 2641, 9859, np.nan, 21.13, 21.13],
    [0, 0, 0, 12500, 2643, 9857, np.nan, 21.14, 21.14]
])

# Data for All Writes on the same address - MOESI CACHE COHERENCY PROTOCOL
moesi_same_address_writes_data = np.array([
    [0, 0, 0, 12500, 9529, 2971, np.nan, 76.23, 76.23],
    [0, 0, 0, 12500, 8434, 4066, np.nan, 67.47, 67.47],
    [0, 0, 0, 12500, 8371, 4129, np.nan, 66.97, 66.97],
    [0, 0, 0, 12500, 9530, 2970, np.nan, 76.24, 76.24],
    [0, 0, 0, 12500, 9543, 2957, np.nan, 76.34, 76.34],
    [0, 0, 0, 12500, 10443, 2057, np.nan, 83.54, 83.54],
    [0, 0, 0, 12500, 10459, 2041, np.nan, 83.67, 83.67],
    [0, 0, 0, 12500, 10443, 2057, np.nan, 83.54, 83.54]
])

# Calculate averages
valid_invalid_same_address_writes_avg = np.nanmean(valid_invalid_same_address_writes_data, axis=0)
moesi_same_address_writes_avg = np.nanmean(moesi_same_address_writes_data, axis=0)

print("All Writes on the same address Trace File")
print("VALID/INVALID CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {valid_invalid_same_address_writes_avg[0]:.2f}, RHit: {valid_invalid_same_address_writes_avg[1]:.2f}, Rmiss: {valid_invalid_same_address_writes_avg[2]:.2f}, Writes: {valid_invalid_same_address_writes_avg[3]:.2f}, WHit: {valid_invalid_same_address_writes_avg[4]:.2f}, WMiss: {valid_invalid_same_address_writes_avg[5]:.2f}, RHitrate: {valid_invalid_same_address_writes_avg[6]:.2f}, WHitrate: {valid_invalid_same_address_writes_avg[7]:.2f}, Hitrate: {valid_invalid_same_address_writes_avg[8]:.2f}")

print("\nMOESI CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {moesi_same_address_writes_avg[0]:.2f}, RHit: {moesi_same_address_writes_avg[1]:.2f}, Rmiss: {moesi_same_address_writes_avg[2]:.2f}, Writes: {moesi_same_address_writes_avg[3]:.2f}, WHit: {moesi_same_address_writes_avg[4]:.2f}, WMiss: {moesi_same_address_writes_avg[5]:.2f}, RHitrate: {moesi_same_address_writes_avg[6]:.2f}, WHitrate: {moesi_same_address_writes_avg[7]:.2f}, Hitrate: {moesi_same_address_writes_avg[8]:.2f}")



print()
print()


# Data for All Reads on the same address - VALID/INVALID CACHE COHERENCY PROTOCOL
valid_invalid_same_address_reads_data = np.array([
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99]
])

# Data for All Reads on the same address - MOESI CACHE COHERENCY PROTOCOL
moesi_same_address_reads_data = np.array([
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99],
    [12500, 12499, 1, 0, 0, 0, 99.99, np.nan, 99.99]
])

# Calculate averages
valid_invalid_same_address_reads_avg = np.nanmean(valid_invalid_same_address_reads_data, axis=0)
moesi_same_address_reads_avg = np.nanmean(moesi_same_address_reads_data, axis=0)

print("All Reads on the same address Trace File")
print("VALID/INVALID CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {valid_invalid_same_address_reads_avg[0]:.2f}, RHit: {valid_invalid_same_address_reads_avg[1]:.2f}, Rmiss: {valid_invalid_same_address_reads_avg[2]:.2f}, Writes: {valid_invalid_same_address_reads_avg[3]:.2f}, WHit: {valid_invalid_same_address_reads_avg[4]:.2f}, WMiss: {valid_invalid_same_address_reads_avg[5]:.2f}, RHitrate: {valid_invalid_same_address_reads_avg[6]:.2f}, WHitrate: {valid_invalid_same_address_reads_avg[7]:.2f}, Hitrate: {valid_invalid_same_address_reads_avg[8]:.2f}")

print("\nMOESI CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {moesi_same_address_reads_avg[0]:.2f}, RHit: {moesi_same_address_reads_avg[1]:.2f}, Rmiss: {moesi_same_address_reads_avg[2]:.2f}, Writes: {moesi_same_address_reads_avg[3]:.2f}, WHit: {moesi_same_address_reads_avg[4]:.2f}, WMiss: {moesi_same_address_reads_avg[5]:.2f}, RHitrate: {moesi_same_address_reads_avg[6]:.2f}, WHitrate: {moesi_same_address_reads_avg[7]:.2f}, Hitrate: {moesi_same_address_reads_avg[8]:.2f}")



print()
print()


# Data for matrix_mult_50_50_p8.trf - VALID/INVALID CACHE COHERENCY PROTOCOL
valid_invalid_matrix_mult_data = np.array([
    [228571, 222945, 5626, 35721, 31464, 4257, 97.54, 88.08, 96.26],
    [228571, 227136, 1435, 35721, 34611, 1110, 99.37, 96.89, 99.04],
    [195918, 195773, 145, 30618, 30545, 73, 99.93, 99.76, 99.9],
    [195918, 195841, 77, 30618, 30616, 2, 99.96, 99.99, 99.97],
    [195918, 195839, 79, 30618, 30610, 8, 99.96, 99.97, 99.96],
    [195918, 195839, 79, 30618, 30616, 2, 99.96, 99.99, 99.96],
    [195918, 195839, 79, 30618, 30610, 8, 99.96, 99.97, 99.96],
    [195918, 195841, 77, 30618, 30616, 2, 99.96, 99.99, 99.97]
])

# Data for matrix_mult_50_50_p8.trf - MOESI CACHE COHERENCY PROTOCOL
moesi_matrix_mult_data = np.array([
    [228571, 228214, 357, 35721, 35719, 2, 99.84, 99.99, 99.86],
    [228571, 228229, 342, 35721, 35719, 2, 99.85, 99.99, 99.87],
    [195918, 195596, 322, 30618, 30616, 2, 99.84, 99.99, 99.86],
    [195918, 195594, 324, 30618, 30616, 2, 99.83, 99.99, 99.86],
    [195918, 195581, 337, 30618, 30616, 2, 99.83, 99.99, 99.85],
    [195918, 195588, 330, 30618, 30616, 2, 99.83, 99.99, 99.85],
    [195918, 195586, 332, 30618, 30616, 2, 99.83, 99.99, 99.85],
    [195918, 195601, 317, 30618, 30616, 2, 99.84, 99.99, 99.86]
])

# Calculate averages
valid_invalid_matrix_mult_avg = np.mean(valid_invalid_matrix_mult_data, axis=0)
moesi_matrix_mult_avg = np.mean(moesi_matrix_mult_data, axis=0)

print("matrix_mult_50_50_p8.trf Trace File")
print("VALID/INVALID CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {valid_invalid_matrix_mult_avg[0]:.2f}, RHit: {valid_invalid_matrix_mult_avg[1]:.2f}, Rmiss: {valid_invalid_matrix_mult_avg[2]:.2f}, Writes: {valid_invalid_matrix_mult_avg[3]:.2f}, WHit: {valid_invalid_matrix_mult_avg[4]:.2f}, WMiss: {valid_invalid_matrix_mult_avg[5]:.2f}, RHitrate: {valid_invalid_matrix_mult_avg[6]:.2f}, WHitrate: {valid_invalid_matrix_mult_avg[7]:.2f}, Hitrate: {valid_invalid_matrix_mult_avg[8]:.2f}")

print("\nMOESI CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {moesi_matrix_mult_avg[0]:.2f}, RHit: {moesi_matrix_mult_avg[1]:.2f}, Rmiss: {moesi_matrix_mult_avg[2]:.2f}, Writes: {moesi_matrix_mult_avg[3]:.2f}, WHit: {moesi_matrix_mult_avg[4]:.2f}, WMiss: {moesi_matrix_mult_avg[5]:.2f}, RHitrate: {moesi_matrix_mult_avg[6]:.2f}, WHitrate: {moesi_matrix_mult_avg[7]:.2f}, Hitrate: {moesi_matrix_mult_avg[8]:.2f}")

print()
print()


# Data for matrix_vector_5000_8_p8-O2.trf - VALID/INVALID CACHE COHERENCY PROTOCOL
valid_invalid_matrix_vector_data = np.array([
    [83750, 81883, 1867, 12500, 12273, 227, 97.77, 98.18, 97.82],
    [83750, 82448, 1302, 12500, 12342, 158, 98.45, 98.74, 98.48],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53]
])

# Data for matrix_vector_5000_8_p8-O2.trf - MOESI CACHE COHERENCY PROTOCOL
moesi_matrix_vector_data = np.array([
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53],
    [83750, 82494, 1256, 12500, 12342, 158, 98.50, 98.74, 98.53]
])

# Calculate averages
valid_invalid_matrix_vector_avg = np.mean(valid_invalid_matrix_vector_data, axis=0)
moesi_matrix_vector_avg = np.mean(moesi_matrix_vector_data, axis=0)

print("matrix_vector_5000_8_p8-O2.trf Trace File")
print("VALID/INVALID CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {valid_invalid_matrix_vector_avg[0]:.2f}, RHit: {valid_invalid_matrix_vector_avg[1]:.2f}, Rmiss: {valid_invalid_matrix_vector_avg[2]:.2f}, Writes: {valid_invalid_matrix_vector_avg[3]:.2f}, WHit: {valid_invalid_matrix_vector_avg[4]:.2f}, WMiss: {valid_invalid_matrix_vector_avg[5]:.2f}, RHitrate: {valid_invalid_matrix_vector_avg[6]:.2f}, WHitrate: {valid_invalid_matrix_vector_avg[7]:.2f}, Hitrate: {valid_invalid_matrix_vector_avg[8]:.2f}")

print("\nMOESI CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {moesi_matrix_vector_avg[0]:.2f}, RHit: {moesi_matrix_vector_avg[1]:.2f}, Rmiss: {moesi_matrix_vector_avg[2]:.2f}, Writes: {moesi_matrix_vector_avg[3]:.2f}, WHit: {moesi_matrix_vector_avg[4]:.2f}, WMiss: {moesi_matrix_vector_avg[5]:.2f}, RHitrate: {moesi_matrix_vector_avg[6]:.2f}, WHitrate: {moesi_matrix_vector_avg[7]:.2f}, Hitrate: {moesi_matrix_vector_avg[8]:.2f}")


print()
print()

# Data for Fast Fourier Transform - fft_1024_p8.trf - VALID/INVALID CACHE COHERENCY PROTOCOL
valid_invalid_fft_data = np.array([
    [66238, 64493, 1745, 20569, 20112, 457, 97.37, 97.78, 97.46],
    [66381, 64781, 1600, 20969, 20330, 639, 97.59, 96.95, 97.44],
    [66328, 64833, 1495, 20929, 20300, 629, 97.75, 96.99, 97.57],
    [66301, 64965, 1336, 20914, 20281, 633, 97.98, 96.97, 97.74],
    [66280, 64861, 1419, 20897, 20254, 643, 97.86, 96.92, 97.63],
    [66380, 65201, 1179, 20963, 20377, 586, 98.22, 97.20, 97.98],
    [66381, 65632, 749, 20966, 20464, 502, 98.87, 97.61, 98.57],
    [66368, 65014, 1354, 20955, 20331, 624, 97.96, 97.02, 97.73]
])

# Data for Fast Fourier Transform - fft_1024_p8.trf - MOESI CACHE COHERENCY PROTOCOL
moesi_fft_data = np.array([
    [66238, 65948, 290, 20569, 20501, 68, 99.56, 99.67, 99.59],
    [66381, 66116, 265, 20969, 20796, 173, 99.60, 99.17, 99.50],
    [66328, 66071, 257, 20929, 20757, 172, 99.61, 99.18, 99.51],
    [66301, 66050, 251, 20914, 20740, 174, 99.62, 99.17, 99.51],
    [66280, 66021, 259, 20897, 20717, 180, 99.61, 99.14, 99.50],
    [66380, 66119, 261, 20963, 20790, 173, 99.61, 99.17, 99.50],
    [66381, 66132, 249, 20966, 20791, 175, 99.62, 99.17, 99.51],
    [66368, 66115, 253, 20955, 20783, 172, 99.62, 99.18, 99.51]
])

# Calculate averages
valid_invalid_fft_avg = np.mean(valid_invalid_fft_data, axis=0)
moesi_fft_avg = np.mean(moesi_fft_data, axis=0)

print("Fast Fourier Transform - fft_1024_p8.trf Trace File")
print("VALID/INVALID CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {valid_invalid_fft_avg[0]:.2f}, RHit: {valid_invalid_fft_avg[1]:.2f}, Rmiss: {valid_invalid_fft_avg[2]:.2f}, Writes: {valid_invalid_fft_avg[3]:.2f}, WHit: {valid_invalid_fft_avg[4]:.2f}, WMiss: {valid_invalid_fft_avg[5]:.2f}, RHitrate: {valid_invalid_fft_avg[6]:.2f}, WHitrate: {valid_invalid_fft_avg[7]:.2f}, Hitrate: {valid_invalid_fft_avg[8]:.2f}")

print("\nMOESI CACHE COHERENCY PROTOCOL Averages:")
print(f"Reads: {moesi_fft_avg[0]:.2f}, RHit: {moesi_fft_avg[1]:.2f}, Rmiss: {moesi_fft_avg[2]:.2f}, Writes: {moesi_fft_avg[3]:.2f}, WHit: {moesi_fft_avg[4]:.2f}, WMiss: {moesi_fft_avg[5]:.2f}, RHitrate: {moesi_fft_avg[6]:.2f}, WHitrate: {moesi_fft_avg[7]:.2f}, Hitrate: {moesi_fft_avg[8]:.2f}")




all_validinvalid_hitrates = np.array([
    valid_invalid_avg[8],
    valid_invalid_alternating_avg[8],
    valid_invalid_writes_avg[8],
    valid_invalid_reads_avg[8],
    valid_invalid_same_address_writes_avg[8],
    valid_invalid_same_address_reads_avg[8],
    valid_invalid_matrix_mult_avg[8],
    valid_invalid_matrix_vector_avg[8],
    valid_invalid_fft_avg[8]
])

all_moesi_hitrates = np.array([
    moesi_avg[8],
    moesi_alternating_avg[8],
    moesi_writes_avg[8],
    moesi_reads_avg[8],
    moesi_same_address_writes_avg[8],
    moesi_same_address_reads_avg[8],
    moesi_matrix_mult_avg[8],
    moesi_matrix_vector_avg[8],
    moesi_fft_avg[8]
])
    



plt.figure(figsize=(10, 6))

ind = np.arange(9)
bar_width = 0.35

plt.bar(ind, all_validinvalid_hitrates, bar_width, label='VALID/INVALID', alpha=0.8)
plt.bar(ind + bar_width, all_moesi_hitrates, bar_width, label='MOESI', alpha=0.8)

#plt.xlabel('Trace Files', fontsize=16)
plt.ylabel('Hitrate (%)', fontsize=16)
#plt.title('Hitrate Comparison between VALID/INVALID and MOESI Cache Coherency Protocols')
plt.xticks(ind + bar_width / 2, ('All Random', 
                                 'Alternating', 
                                 'All Writes', 
                                 'All Reads', 
                                 'All Writes on \nSame Address', 
                                 'All Reads on \nSame Address', 
                                 'Matrix Mult', 
                                 'Matrix Vector Mult',
                                 'Fast Fourier Transform'), rotation=25, fontsize=12)
plt.legend()

plt.tight_layout()
plt.show()








# Data for All Reads on the same address
valid_invalid_reads_same_address = np.array([
    [25810, 8, 0]
])

moesi_reads_same_address = np.array([
    [25813, 8, 0]
])

# Data for All Writes on the same address
valid_invalid_writes_same_address = np.array([
    [10100006, 78870, 21130]
])

moesi_writes_same_address = np.array([
    [409867, 3860, 0]
])

# Data for All Reads on Random Addresses
valid_invalid_reads_random = np.array([
    [11699944, 12198, 103643]
])

moesi_reads_random = np.array([
    [252571, 2198, 0]
])

# Data for All Writes on Random Addresses
valid_invalid_writes_random = np.array([
    [10100006, 72537, 27463]
])

moesi_writes_random = np.array([
    [1578482, 2656, 12971]
])

# Data for Alternating Read/Writes on same address
valid_invalid_alternating_rw = np.array([
    [5324446, 1124, 51124]
])

moesi_alternating_rw = np.array([
    [233523, 2020, 0]
])

# Data for Random Read/Writes on Random Addresses
valid_invalid_random_rw = np.array([
    [11965069, 40560, 77906]
])

moesi_random_rw = np.array([
    [1610075, 2531, 13410]
])

# Data for Matrix Multiplication 8 CPUs - matrix_mult_50_50_p8.trf
valid_invalid_matrix_mult = np.array([
    [27803495, 13059, 262223]
])

moesi_matrix_mult = np.array([
    [788379, 2677, 0]
])

# Data for matrix_vector_5000_8_p8-O2.trf
valid_invalid_matrix_vector = np.array([
    [12265648, 12038, 109404]
])

moesi_matrix_vector = np.array([
    [1172902, 11312, 296]
])

# Data for Fast Fourier Transform - fft_1024_p8.trf
valid_invalid_fft = np.array([
    [19387256, 15590, 176363]
])

moesi_fft = np.array([
    [532133, 3341, 235]
])


moesi_bus_arbitration_wait_time = np.array([
    moesi_random_rw[0][0],
    moesi_alternating_rw[0][0],
    moesi_writes_random[0][0],
    moesi_reads_random[0][0],
    moesi_writes_same_address[0][0],
    moesi_reads_same_address[0][0],
    moesi_matrix_mult[0][0],
    moesi_matrix_vector[0][0],
    moesi_fft[0][0]
])

valid_invalid_bus_arbitration_wait_time = np.array([
    valid_invalid_random_rw[0][0],
    valid_invalid_alternating_rw[0][0],
    valid_invalid_writes_random[0][0],
    valid_invalid_reads_random[0][0],
    valid_invalid_writes_same_address[0][0],
    valid_invalid_reads_same_address[0][0],
    valid_invalid_matrix_mult[0][0],
    valid_invalid_matrix_vector[0][0],
    valid_invalid_fft[0][0]
])

plt.figure(figsize=(10, 6))

ind = np.arange(9)
bar_width = 0.35

plt.bar(ind, valid_invalid_bus_arbitration_wait_time, bar_width, label='VALID/INVALID', alpha=0.8)
plt.bar(ind + bar_width, moesi_bus_arbitration_wait_time, bar_width, label='MOESI', alpha=0.8)

plt.ylabel('Bus Arbitration Wait Time (cycles)', fontsize=16)
plt.xticks(ind + bar_width / 2, ('All Random', 
                                 'Alternating RW', 
                                 'All Writes Random', 
                                 'All Reads Random', 
                                 'All Writes on \nSame Address', 
                                 'All Reads on \nSame Address', 
                                 'Matrix Mult', 
                                 'Matrix Vector Mult',
                                 'Fast Fourier Transform'), rotation=25, fontsize=12)
plt.legend()

plt.tight_layout()
plt.show()


moesit_read_counts = np.array([
    moesi_random_rw[0][0],
    moesi_alternating_rw[0][0],
    moesi_writes_random[0][0],
    moesi_reads_random[0][0],
    moesi_writes_same_address[0][0],
    moesi_reads_same_address[0][0],
    moesi_matrix_mult[0][0],
    moesi_matrix_vector[0][0],
    moesi_fft[0][0]
])

valid_invalid_read_counts = np.array([
    valid_invalid_random_rw[0][0],
    valid_invalid_alternating_rw[0][0],
    valid_invalid_writes_random[0][0],
    valid_invalid_reads_random[0][0],
    valid_invalid_writes_same_address[0][0],
    valid_invalid_reads_same_address[0][0],
    valid_invalid_matrix_mult[0][0],
    valid_invalid_matrix_vector[0][0],
    valid_invalid_fft[0][0]
])

plt.figure(figsize=(10, 6))

ind = np.arange(9)
bar_width = 0.35

plt.bar(ind, valid_invalid_read_counts, bar_width, label='VALID/INVALID', alpha=0.8)
plt.bar(ind + bar_width, moesit_read_counts, bar_width, label='MOESI', alpha=0.8)

plt.ylabel('Read Counts', fontsize=16)
plt.xticks(ind + bar_width / 2, ('All Random', 
                                 'Alternating RW', 
                                 'All Writes Random', 
                                 'All Reads Random', 
                                 'All Writes on \nSame Address', 
                                 'All Reads on \nSame Address', 
                                 'Matrix Mult', 
                                 'Matrix Vector Mult',
                                 'Fast Fourier Transform'), rotation=25, fontsize=12)
plt.legend()

plt.tight_layout()
plt.show()

moesi_write_counts = np.array([
    moesi_random_rw[0][1],
    moesi_alternating_rw[0][1],
    moesi_writes_random[0][1],
    moesi_reads_random[0][1],
    moesi_writes_same_address[0][1],
    moesi_reads_same_address[0][1],
    moesi_matrix_mult[0][1],
    moesi_matrix_vector[0][1],
    moesi_fft[0][1]
])

valid_invalid_write_counts = np.array([
    valid_invalid_random_rw[0][1],
    valid_invalid_alternating_rw[0][1],
    valid_invalid_writes_random[0][1],
    valid_invalid_reads_random[0][1],
    valid_invalid_writes_same_address[0][1],
    valid_invalid_reads_same_address[0][1],
    valid_invalid_matrix_mult[0][1],
    valid_invalid_matrix_vector[0][1],
    valid_invalid_fft[0][1]
])

plt.figure(figsize=(10, 6))

ind = np.arange(9)
bar_width = 0.35

plt.bar(ind, valid_invalid_write_counts, bar_width, label='VALID/INVALID', alpha=0.8)
plt.bar(ind + bar_width, moesi_write_counts, bar_width, label='MOESI', alpha=0.8)

plt.ylabel('Write Counts', fontsize=16)
plt.xticks(ind + bar_width / 2, ('All Random', 
                                 'Alternating RW', 
                                 'All Writes Random', 
                                 'All Reads Random', 
                                 'All Writes on \nSame Address', 
                                 'All Reads on \nSame Address', 
                                 'Matrix Mult', 
                                 'Matrix Vector Mult',
                                 'Fast Fourier Transform'), rotation=25, fontsize=12)
plt.legend()

plt.tight_layout()
plt.show()
