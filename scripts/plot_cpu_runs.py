import numpy as np
import matplotlib.pyplot as plt

# Hit rates from each CPU group
RHitrate_values_MOESI = [
    [79.97, 80.93, 81.37, 80.57, 80.1, 81.03, 78.95, 79.94],
    [81.1, 79.97, 82.41, 84.03, 81.85, 78.47, 79.63, 81.56],
    [79.73, 80.52, 81.85, 81.08, 79.1, 80.61, 81.41, 79.55],
    [80.54, 82.06, 80.67, 79.57, 81.07, 82.86, 81.76, 79.84],
    [78.39, 81.68, 81.25, 80.93, 81.79, 79.25, 80.32, 79.73],
    [80.44, 81.59, 79.28, 82.57, 80.72, 83.41, 81.16, 82.57],
    [83.83, 79.53, 79.22, 79.83, 81.71, 81.54, 81.14, 81.6],
    [80.5, 78.89, 81.74, 81.67, 80.85, 79.49, 80.69, 81.51],
]

WHitrate_values_MOESI = [
    [23.24, 21.64, 20.06, 22.85, 21.16, 24.81, 26.64, 25.16],
    [23.09, 22.96, 21.59, 20.33, 22.43, 22.95, 24.96, 21.24],
    [24.07, 20.67, 19.29, 20.61, 21.51, 23.16, 21.41, 22.27],
    [23.95, 22.53, 21.63, 20.86, 21.01, 20.48, 21.66, 19.19],
    [25.16, 20.96, 22.27, 23.49, 19.37, 22.96, 23.87, 21.93],
    [22.89, 23.06, 22.76, 21.04, 22.4, 22.85, 23.41, 20.52],
    [21.74, 22.6, 20.49, 20.78, 21.04, 22.26, 25.53, 24.32],
    [23.18, 21.65, 19.00, 24.52, 22.49, 22.86, 22.7, 18.95],
]

# Bus wait times
BusWaitTimes_MOESI = [
    [993, 1028, 1035, 1022, 1017, 1014, 1002, 984],
    [1004, 1006, 1020, 1012, 1023, 1005, 996, 1039],
    [1011, 1018, 1015, 996, 1009, 1001, 999, 1010],
    [986, 1004, 1020, 1005, 1003, 1002, 1040, 1045],
    [1002, 1037, 1016, 990, 1039, 1031, 1014, 1028],
    [1003, 996, 1004, 992, 1003, 1006, 999, 1044],
    [1006, 1005, 1037, 1026, 1021, 993, 989, 1001],
    [1002, 1000, 1053, 995, 1008, 1003, 1023, 1029],
]

BusWaitTimes_validinvalid = [
    [2315, 2225, 2117, 1885, 1799, 1946, 1819, 1754],
    [2438, 2125, 1963, 1972, 1908, 1692, 1778, 1751],
    [2376, 2020, 2184, 1969, 1942, 1803, 1809, 1768],
    [2296, 2299, 1969, 1816, 1908, 1704, 1841, 1811],
    [2405, 2175, 1992, 1977, 1717, 1883, 1724, 1745],
    [2389, 2136, 2179, 1947, 1925, 1867, 1753, 1740],
    [2361, 2197, 2137, 1960, 1864, 1681, 1834, 1715],
    [2424, 1958, 1997, 2055, 1848, 1780, 1854, 1755],
    [2315, 2231, 1999, 2012, 1949, 1863, 1797, 1686],
    [2174, 2224, 2055, 2023, 1901, 1731, 1788, 1781]
]

# Read hit rates
RHitrate_values_validinvalid = [
    [79.69, 74.02, 71.70, 83.28, 84.20, 65.59, 77.44, 80.75],
    [63.56, 77.06, 77.29, 68.98, 69.17, 89.56, 75.47, 85.83],
    [75.70, 81.44, 67.43, 76.53, 74.84, 76.58, 80.37, 76.21],
    [77.72, 62.05, 80.94, 83.31, 79.42, 86.44, 75.28, 69.92],
    [64.98, 73.02, 81.40, 71.11, 86.49, 74.25, 81.82, 85.26],
    [72.08, 81.52, 62.17, 76.95, 71.38, 72.33, 84.44, 87.18],
    [77.53, 74.25, 66.93, 74.77, 77.25, 86.19, 75.16, 83.97],
    [74.39, 86.96, 83.92, 66.77, 80.19, 81.73, 74.36, 72.20],
    [74.80, 73.96, 81.17, 75.81, 70.50, 72.41, 80.25, 85.45],
    [81.12, 71.64, 76.96, 68.72, 72.68, 87.35, 76.97, 78.16]
]

# Write hit rates
WHitrate_values_validinvalid = [
    [20.99, 19.48, 17.79, 21.06, 18.78, 22.93, 19.56, 15.86],
    [20.85, 21.08, 20.99, 18.86, 17.85, 18.91, 21.68, 21.62],
    [21.69, 19.59, 18.54, 21.02, 21.36, 18.35, 20.56, 16.88],
    [20.03, 23.25, 18.52, 19.53, 18.69, 19.28, 21.49, 21.76],
    [23.54, 21.45, 19.94, 19.58, 19.91, 21.07, 22.33, 20.68],
    [21.59, 19.97, 22.27, 19.47, 20.31, 21.01, 21.45, 19.86],
    [20.06, 20.22, 20.67, 20.70, 18.99, 16.94, 20.03, 18.23],
    [22.39, 17.97, 17.85, 19.81, 18.56, 19.17, 19.49, 20.65],
    [19.84, 23.13, 18.77, 18.18, 21.48, 20.55, 22.45, 20.18],
    [19.97, 21.33, 18.03, 22.04, 20.09, 22.65, 24.30, 22.31]
]






average_RHitrate_array = []
average_WHitrate_array = []
average_overall_hitrate_array = []
average_bus_wait_time_array = []

validinvalid_average_RHitrate_array = []
validinvalid_average_WHitrate_array = []
validinvalid_average_overall_hitrate_array = []
validinvalid_average_bus_wait_time_array = []


all_cpu_average_overall_hitrate = 0
all_cpu_average_read_hitrate = 0
all_cpu_average_write_hitrate = 0
all_cpu_average_bus_wait_time = 0

all_cpu_average_overall_hitrate_validinvalid = 0
all_cpu_average_read_hitrate_validinvalid = 0
all_cpu_average_write_hitrate_validinvalid = 0
all_cpu_average_bus_wait_time_validinvalid = 0

for i in range(8):
    average_RHitrate = np.mean(RHitrate_values_MOESI[i])
    average_WHitrate = np.mean(WHitrate_values_MOESI[i])
    average_overall_hitrate = (average_RHitrate + average_WHitrate) / 2
    average_bus_wait_time = np.mean(BusWaitTimes_MOESI[i])

    average_RHitrate_array.append(average_RHitrate)
    average_WHitrate_array.append(average_WHitrate)
    average_overall_hitrate_array.append(average_overall_hitrate)
    average_bus_wait_time_array.append(average_bus_wait_time)

    validinvalid_average_RHitrate = np.mean(RHitrate_values_validinvalid[i])
    validinvalid_average_WHitrate = np.mean(WHitrate_values_validinvalid[i])
    validinvalid_average_overall_hitrate = (validinvalid_average_RHitrate + validinvalid_average_WHitrate) / 2
    validinvalid_average_bus_wait_time = np.mean(BusWaitTimes_validinvalid[i])

    validinvalid_average_RHitrate_array.append(validinvalid_average_RHitrate)
    validinvalid_average_WHitrate_array.append(validinvalid_average_WHitrate)
    validinvalid_average_overall_hitrate_array.append(validinvalid_average_overall_hitrate)
    validinvalid_average_bus_wait_time_array.append(validinvalid_average_bus_wait_time)

    all_cpu_average_overall_hitrate += average_overall_hitrate
    all_cpu_average_read_hitrate += average_RHitrate
    all_cpu_average_write_hitrate += average_WHitrate
    all_cpu_average_bus_wait_time += average_bus_wait_time

    all_cpu_average_overall_hitrate_validinvalid += validinvalid_average_overall_hitrate
    all_cpu_average_read_hitrate_validinvalid += validinvalid_average_RHitrate
    all_cpu_average_write_hitrate_validinvalid += validinvalid_average_WHitrate
    all_cpu_average_bus_wait_time_validinvalid += validinvalid_average_bus_wait_time
                                       
    print(f"CPU Group {i + 1}")
    print(f"Average Read Hit Rate: {average_RHitrate:.2f}")
    print(f"Average Write Hit Rate: {average_WHitrate:.2f}")
    print(f"Average Overall Hit Rate: {average_overall_hitrate:.2f}")
    print(f"Average Bus Wait Time: {average_bus_wait_time:.2f}")
    print()
    print(f"Average Read Hit Rate (Valid/Invalid): {validinvalid_average_RHitrate:.2f}")
    print(f"Average Write Hit Rate (Valid/Invalid): {validinvalid_average_WHitrate:.2f}")
    print(f"Average Overall Hit Rate (Valid/Invalid): {validinvalid_average_overall_hitrate:.2f}")
    print(f"Average Bus Wait Time (Valid/Invalid): {validinvalid_average_bus_wait_time:.2f}")


all_cpu_average_overall_hitrate /= 8
all_cpu_average_read_hitrate /= 8
all_cpu_average_write_hitrate /= 8
all_cpu_average_bus_wait_time /= 8

all_cpu_average_overall_hitrate_validinvalid /= 8
all_cpu_average_read_hitrate_validinvalid /= 8
all_cpu_average_write_hitrate_validinvalid /= 8
all_cpu_average_bus_wait_time_validinvalid /= 8

print(f"Average Overall Hit Rate for all CPU Groups: {all_cpu_average_overall_hitrate:.2f}")
print(f"Average Read Hit Rate for all CPU Groups: {all_cpu_average_read_hitrate:.2f}")
print(f"Average Write Hit Rate for all CPU Groups: {all_cpu_average_write_hitrate:.2f}")
print(f"Average Bus Wait Time for all CPU Groups: {all_cpu_average_bus_wait_time:.2f}")

print(f"Average Overall Hit Rate for all CPU Groups (Valid/Invalid): {all_cpu_average_overall_hitrate_validinvalid:.2f}")
print(f"Average Read Hit Rate for all CPU Groups (Valid/Invalid): {all_cpu_average_read_hitrate_validinvalid:.2f}")
print(f"Average Write Hit Rate for all CPU Groups (Valid/Invalid): {all_cpu_average_write_hitrate_validinvalid:.2f}")
print(f"Average Bus Wait Time for all CPU Groups (Valid/Invalid): {all_cpu_average_bus_wait_time_validinvalid:.2f}")

plt.figure(figsize=(10, 5))

bar_width = 0.25
index = np.arange(8)

plt.subplot(1, 2, 1)

plt.bar(index, average_RHitrate_array, label="Read Hit Rate", alpha=0.5)
plt.bar(index, average_overall_hitrate_array, label="Overall Hit Rate", alpha=0.5)
plt.bar(index, average_WHitrate_array, label="Write Hit Rate", alpha=0.5)

#plt.bar(index, average_RHitrate_array, bar_width, label="Read Hit Rate", alpha=0.7)
#plt.bar(index + 2 * bar_width, average_overall_hitrate_array, bar_width, label="Overall Hit Rate", alpha=0.7)
#plt.bar(index + bar_width, average_WHitrate_array, bar_width, label="Write Hit Rate", alpha=0.7)
#plt.ylim(0, 100)
#plt.axhline(y=np.mean(average_RHitrate_array), color='r', linestyle='--', label="Average Read Hit Rate")
#plt.axhline(y=np.mean(average_overall_hitrate_array), color='g', linestyle='--', label="Average Overall Hit Rate")
#plt.axhline(y=np.mean(average_WHitrate_array), color='b', linestyle='--', label="Average Write Hit Rate")
plt.ylim(0, 100)
plt.xlabel("CPU Num", fontsize=16)
plt.ylabel("Average Hit Rate (%)", fontsize=16)
plt.title("Average Hit Rates", fontsize=16)
plt.legend()

plt.subplot(1, 2, 2)
plt.bar(range(0, 8), average_bus_wait_time_array, label="Bus Wait Time", alpha=0.7)
plt.xlabel("CPU Number", fontsize=16)
plt.ylabel("Time (ns)", fontsize=16)
plt.title("Average Bus Wait Times", fontsize=16)

plt.tight_layout()
plt.show()


plt.figure(figsize=(10, 5))

bar_width = 0.25
index = np.arange(8)

plt.subplot(1, 2, 1)

plt.bar(index, validinvalid_average_RHitrate_array, label="Read Hit Rate", alpha=0.5)
plt.bar(index, validinvalid_average_overall_hitrate_array, label="Overall Hit Rate", alpha=0.5)
plt.bar(index, validinvalid_average_WHitrate_array, label="Write Hit Rate", alpha=0.5)


#plt.bar(index, average_RHitrate_array, bar_width, label="Read Hit Rate", alpha=0.7)
#plt.bar(index + 2 * bar_width, average_overall_hitrate_array, bar_width, label="Overall Hit Rate", alpha=0.7)
#plt.bar(index + bar_width, average_WHitrate_array, bar_width, label="Write Hit Rate", alpha=0.7)
#plt.ylim(0, 100)
#plt.axhline(y=np.mean(average_RHitrate_array), color='r', linestyle='--', label="Average Read Hit Rate")
#plt.axhline(y=np.mean(average_overall_hitrate_array), color='g', linestyle='--', label="Average Overall Hit Rate")
#plt.axhline(y=np.mean(average_WHitrate_array), color='b', linestyle='--', label="Average Write Hit Rate")
plt.ylim(0, 100)
plt.xlabel("CPU Number", fontsize=16)
plt.ylabel("Average Hit Rate (%)", fontsize=16)
plt.title("Average Hit Rates", fontsize=16)
plt.legend()

plt.subplot(1, 2, 2)
plt.bar(range(0, 8), validinvalid_average_bus_wait_time, label="Bus Wait Time", alpha=0.7)
plt.xlabel("CPU Num", fontsize=16)
plt.ylabel("Time (ns)", fontsize=16)
plt.title("Average Bus Wait Times", fontsize=16)

plt.tight_layout()
plt.show()