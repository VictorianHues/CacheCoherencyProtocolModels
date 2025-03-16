import csv
import matplotlib.pyplot as plt


def plot_line_graph(x, y1, y2, x_label, y_label, title):
    plt.figure(figsize=(5, 4))
    plt.plot(x, y2, label='VALID/INVALID', marker='o')
    plt.plot(x, y1, label='MOESI', marker='o')
    plt.xlabel(x_label, fontsize=16)
    plt.ylabel(y_label, fontsize=16)
    plt.yscale('log')
    plt.xscale('log')
    #plt.title(title, fontsize=18)
    plt.legend()

    plt.tight_layout()

    plt.savefig(title + '.png')

    plt.show()

def plot_bar_graph(x, y1, y2, x_label, y_label, title):
    width = 0.35
    plt.bar([p - width/2 for p in x], y1, width=width, label='Dataset 1')
    plt.bar([p + width/2 for p in x], y2, width=width, label='Dataset 2')
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title(title)
    plt.legend()
    plt.show()

def main():
    # Initialize arrays for each column
    number_of_cpus = []
    reads = []
    rhit = []
    rmiss = []
    writes = []
    whit = []
    wmiss = []
    rhitrate = []
    whitrate = []
    hitrate = []
    bus_wait_time = []
    percent_of_total_time = []
    wait_per_request = []
    total_simulation_time = []
    memory_read_count = []
    memory_write_count = []

    number_of_cpus_2 = []
    reads_2 = []
    rhit_2 = []
    rmiss_2 = []
    writes_2 = []
    whit_2 = []
    wmiss_2 = []
    rhitrate_2 = []
    whitrate_2 = []
    hitrate_2 = []
    bus_wait_time_2 = []
    percent_of_total_time_2 = []
    wait_per_request_2 = []
    total_simulation_time_2 = []
    memory_read_count_2 = []
    memory_write_count_2 = []

    # Read the first CSV data
    csv_data_1 = """Number of CPUs,Reads,RHit,Rmiss,Writes,WHit,WMiss,RHitrate,WHitrate,Hitrate,Bus Wait Time,Percent of Total Time,Wait per request,Total Simulation Time,Memory READ Count,Memory WRITE Count
    8,6251.625,6097.5,154.125,6248.375,2059.5,4188.875,97.5325,32.95875,65.255,10066.75,6.25213E-06,0.80534,1610075,2531,13410
    7,7146.714286,6940.857143,205.8571429,7139,2480.571429,4658.428571,97.12,34.74714286,65.95,11433.85714,6.41371E-06,0.80037,1782769,3001,14650
    6,8343.166667,8027.333333,315.8333333,8323.5,3034,5289.5,96.215,36.45333333,66.36833333,13325.66667,6.54417E-06,0.79954,2036205,3813,16344
    5,9985.4,9466.6,518.8,10014.6,3868.4,6146.2,94.806,38.628,66.676,15916,6.6928E-06,0.7958,2378082,5254,18282
    4,12530.25,11530.25,1000,12469.75,5089.25,7380.5,92.02,40.8175,66.4775,19900,6.88825E-06,0.796,2888879,7965,20628
    3,16747,14501,2246,16586.33333,7313.333333,9273,86.59,44.09666667,65.44333333,26406.33333,0.000007116,0.79219,3710918,13429,23307
    2,25016.5,19012.5,6004,24983.5,11953,13030.5,75.995,47.845,61.935,39371.5,7.6435E-06,0.78743,5151071,24154,26827
    1,50067,26537,23530,49933,26515,23418,53,53.1,53.05,78126,0.000009239,0.78126,8456261,46948,31178"""

    # Read the second CSV data
    csv_data_2 = """Number of CPUs,Reads,RHit,Rmiss,Writes,WHit,WMiss,RHitrate,WHitrate,Hitrate,Bus Wait Time,Percent of Total Time,Wait per request,Total Simulation Time,Memory READ Count,Memory WRITE Count
    8,6251.625,5430.375,821.25,6248.375,1999.625,4248.75,86.87,32,59.44,18270.625,1.52688E-06,1.46165,11965069,40560,77906
    7,7146.714286,5988,1158.714286,7139,2331.857143,4807.142857,83.79714286,32.66714286,58.24142857,20387.57143,1.72243E-06,1.42713,11836395,41761,75431
    6,8343.166667,6925.833333,1417.333333,8323.5,2753.166667,5570.333333,83.015,33.075,58.07166667,23594.66667,0.000002002,1.41568,11785087,41926,74758
    5,9985.4,7943.8,2041.6,10014.6,3315.4,6699.2,79.55,33.11,56.294,27929.2,0.000002372,1.39646,11775189,43704,72882
    4,12530.25,9403.25,3127,12469.75,4275.25,8194.5,75.0475,34.2825,54.7125,34106.25,2.9135E-06,1.36425,11706913,45286,70624
    3,16747,11341.66667,5405.333333,16586.33333,5826,10760.33333,67.72333333,35.12666667,51.50333333,43493,3.76467E-06,1.30479,11552383,48497,65883
    2,25016.5,14310.5,10706,24983.5,9212.5,15771,57.205,36.875,47.045,61429,5.3905E-06,1.22858,11396237,52954,59880
    1,50067,26537,23530,49933,26515,23418,53,53.1,53.05,96993,0.000009404,0.96993,10313788,46948,50045"""

    # Parse the first CSV data
    reader = csv.reader(csv_data_1.splitlines())
    header = next(reader)  # Skip the header row

    for row in reader:
        number_of_cpus.append(int(row[0]))
        reads.append(float(row[1]))
        rhit.append(float(row[2]))
        rmiss.append(float(row[3]))
        writes.append(float(row[4]))
        whit.append(float(row[5]))
        wmiss.append(float(row[6]))
        rhitrate.append(float(row[7]))
        whitrate.append(float(row[8]))
        hitrate.append(float(row[9]))
        bus_wait_time.append(float(row[10]))
        percent_of_total_time.append(float(row[11]))
        wait_per_request.append(float(row[12]))
        total_simulation_time.append(float(row[13]))
        memory_read_count.append(int(row[14]))
        memory_write_count.append(int(row[15]))

    # Parse the second CSV data
    reader = csv.reader(csv_data_2.splitlines())
    header = next(reader)  # Skip the header row

    for row in reader:
        number_of_cpus_2.append(int(row[0]))
        reads_2.append(float(row[1]))
        rhit_2.append(float(row[2]))
        rmiss_2.append(float(row[3]))
        writes_2.append(float(row[4]))
        whit_2.append(float(row[5]))
        wmiss_2.append(float(row[6]))
        rhitrate_2.append(float(row[7]))
        whitrate_2.append(float(row[8]))
        hitrate_2.append(float(row[9]))
        bus_wait_time_2.append(float(row[10]))
        percent_of_total_time_2.append(float(row[11]))
        wait_per_request_2.append(float(row[12]))
        total_simulation_time_2.append(float(row[13]))
        memory_read_count_2.append(int(row[14]))
        memory_write_count_2.append(int(row[15]))

    # Plot the data
    plot_line_graph(number_of_cpus, reads, reads_2, "Number of CPUs", "Reads", "Number of Reads vs Number of CPUs")
    plot_line_graph(number_of_cpus, rhit, rhit_2, "Number of CPUs", "RHit", "RHit vs Number of CPUs")
    plot_line_graph(number_of_cpus, rmiss, rmiss_2, "Number of CPUs", "RMiss", "RMiss vs Number of CPUs")
    plot_line_graph(number_of_cpus, writes, writes_2, "Number of CPUs", "Writes", "Number of Writes vs Number of CPUs")
    plot_line_graph(number_of_cpus, whit, whit_2, "Number of CPUs", "WHit", "WHit vs Number of CPUs")
    plot_line_graph(number_of_cpus, wmiss, wmiss_2, "Number of CPUs", "WMiss", "WMiss vs Number of CPUs")
    plot_line_graph(number_of_cpus, rhitrate, rhitrate_2, "Number of CPUs", "RHitrate", "RHitrate vs Number of CPUs")
    plot_line_graph(number_of_cpus, whitrate, whitrate_2, "Number of CPUs", "WHitrate", "WHitrate vs Number of CPUs")
    plot_line_graph(number_of_cpus, hitrate, hitrate_2, "Number of CPUs", "Hitrate", "Hitrate vs Number of CPUs")
    plot_line_graph(number_of_cpus, bus_wait_time, bus_wait_time_2, "Number of CPUs", "Bus Wait Time", "Bus Wait Time vs Number of CPUs")
    plot_line_graph(number_of_cpus, percent_of_total_time, percent_of_total_time_2, "Number of CPUs", "Percent of Total Time", "Percent of Total Time vs Number of CPUs")
    plot_line_graph(number_of_cpus, wait_per_request, wait_per_request_2, "Number of CPUs", "Wait per Request", "Wait per Request vs Number of CPUs")
    plot_line_graph(number_of_cpus, total_simulation_time, total_simulation_time_2, "Number of CPUs", "Total Simulation Time", "Total Simulation Time vs Number of CPUs log")
    plot_line_graph(number_of_cpus, memory_read_count, memory_read_count_2, "Number of CPUs", "Memory Read Count", "Memory Read Count vs Number of CPUs")
    plot_line_graph(number_of_cpus, memory_write_count, memory_write_count_2, "Number of CPUs", "Memory Write Count", "Memory Write Count vs Number of CPUs")

main()