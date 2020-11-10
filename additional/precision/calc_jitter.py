import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.signal import lfilter
import seaborn as sns
from pylab import *
import more_itertools as mit
from matplotlib import rc
from matplotlib.ticker import (MultipleLocator, FormatStrFormatter,
                               AutoMinorLocator)
from os import listdir
import math


def get_rising_edge(samples):
    high_samples = [idx for idx, val in enumerate(samples) if val > 1.5]

    high_groups = [list(group) for group in mit.consecutive_groups(high_samples)]

    #highs = list()
    rising_edges = list()
    for i in high_groups:
        #highs.append((min(i), max(i)))
        rising_edges.append(min(i))
    return rising_edges

def get_period(edges):
    return np.diff(edges)

def get_jitter_cycle2cycle(periods):
    return np.average(np.abs(np.diff(periods)))

def get_jitter_period(periods):
    return np.average(np.abs(np.array(periods) - np.average(periods)))

def read_file(filename, sample_rate=0):
    headers=["CH1","CH2"]#,"CH3","CH4"]

    # Read the samples in files
    df = pd.read_csv(filename, usecols=["CH1"], skiprows=[1] )

    # Read the parameters
    parameters = pd.read_csv(filename, usecols=["Start", "Increment"], nrows=2)
    if(sample_rate != 0):
        increment = 1/sample_rate  # sample per second to second per sample
    else:
        increment = parameters["Increment"][0]
    start = parameters["Start"][0]

    return df, increment, start

def write_period(periods, filename):
    df_periods = pd.DataFrame(periods, columns=["periods"])
    df_periods.to_csv(filename, index=False)

def find_csv_filenames( path_to_dir, suffix=".csv" ):
    filenames = listdir(path_to_dir)
    return [ path_to_dir+"/"+filename for filename in filenames if filename.endswith( suffix ) ]

def plot_period_box(periods_DTR, periods_USB):
    # Set Latex style font
    rc("font",family="Georgia", size="11")
    #rc('text', usetex = True)
    rc("axes", labelsize="14")

    fig = plt.figure(figsize=(5,2))
    ax = fig.add_subplot(111)
    treatments = [np.array(periods_DTR) * 1e6, np.array(periods_USB) * 1e6]
    pos = np.arange(len(treatments)) + 1
    bp = ax.boxplot(treatments, sym='k+', positions=pos,
                    notch=1, bootstrap=5000, labels=["DTR","USB"])# ,
                    # usermedians=medians,
                    # conf_intervals=conf_intervals)

    ax.set_xlabel('Trigger Type')
    ax.set_ylabel('Measured Period (us)')
    plt.setp(bp['whiskers'], color='k', linestyle='-')
    plt.setp(bp['fliers'], markersize=3.0)

    # more frequent y ticks
    y_min, y_max = ax.get_ylim()
    ax.set_yticks(np.arange(y_min, y_max, 20))

    # plt.show()
    plt.savefig('jitter_box.pdf', format="pdf", bbox_inches = 'tight')



def plot_period(periods_DTR, periods_USB):
    # Set Latex style font
    rc("font",family="Georgia", size="11")
    #rc('text', usetex = True)
    rc("axes", labelsize="14")


    y_ticks = np.arange(0, 70, 10)
    fig, axs = plt.subplots(2, 1, constrained_layout=True, figsize=(5,4))

    p_DTR_us = np.array(periods_DTR[:100])*1e6
    p_USB_us = np.array(periods_USB[:100])*1e6

    x_tick_width = 2
    x_tick_width_major = 8
    min_x = int(min(min(p_DTR_us), min(p_USB_us)) / x_tick_width) * x_tick_width
    max_x = int(max(max(p_DTR_us), max(p_USB_us)) / x_tick_width) * x_tick_width
    x_bins = range(min_x, max_x, x_tick_width)
    # x_ticks = range(min_x, max_x, 8)
    # import pdb; pdb.set_trace()
    counts, bins, patches = axs[0].hist(p_DTR_us,
                                        facecolor='dodgerblue', edgecolor='gray', bins=x_bins)
    axs[0].set_title('DTR Trigger')
    axs[0].set_xlabel('Period (\u00b5S)')
    axs[0].set_ylabel('Count')
    axs[0].set_yticks(y_ticks)
    axs[0].set_xticks(x_bins)
    plt.setp(axs[0].xaxis.get_majorticklabels(), rotation=45)
    axs[0].xaxis.set_major_locator(MultipleLocator(x_tick_width_major))
    axs[0].xaxis.set_minor_locator(MultipleLocator(x_tick_width))

    counts, bins, patches = axs[1].hist(p_USB_us,
                                        facecolor='dodgerblue', edgecolor='gray', bins=x_bins)
    axs[1].set_title('USB Trigger')
    axs[1].set_xlabel('Period (\u00b5S)')
    axs[1].set_ylabel('Count')
    axs[1].set_yticks(y_ticks)
    axs[1].set_xticks(x_bins)
    plt.setp(axs[1].xaxis.get_majorticklabels(), rotation=45)
    axs[1].xaxis.set_major_locator(MultipleLocator(x_tick_width_major))
    axs[1].xaxis.set_minor_locator(MultipleLocator(x_tick_width))
    # fig.suptitle('This is a somewhat long figure title', fontsize=16)



    #fig = plt.figure(figsize=(5,2))
    #ax = fig.add_subplot(111)
    #ax.set_xticks(bins)

    plt.savefig('jitter_hist.pdf', format="pdf", bbox_inches = 'tight')



# Read a list of files and get periods

filenames_DTR = find_csv_filenames('./oscilloscope_capture/jitter_DTR/')
filenames_USB = find_csv_filenames('./oscilloscope_capture/jitter_teensy_usb/')


# calculate jitter for DTR trigger
print("-------------------------------")
print("Processing files for DTR:")
print(filenames_DTR)

print("Processing files for USB:")
print(filenames_USB)

periods_DTR = list()
for f in filenames_DTR:
    # For the bug of RIGOL DS1000Z csv file (When capture memory, got incorrect increment)
    df, increment, start = read_file(f, 500000000) # From the txt file (Sampling Rate)

    rising_edges = get_rising_edge(df["CH1"])
    periods_DTR.extend(np.array(get_period(rising_edges)) * increment)

# Calculate jitter - DTR
periods_DTR = periods_DTR[:100]
jitter_DTR_c2c = get_jitter_cycle2cycle(periods_DTR)
jitter_DTR_p = get_jitter_period(periods_DTR)


print("[DTR Trigger Result] \n"
      "%d cycles, average period: %0.10fus, \n"
      "cycle 2 cycle jitter: %0.10fus \n"
      "period jitter %0.10fus\n"% (len(periods_DTR), np.average(periods_DTR)*1e6, jitter_DTR_c2c*1e6, jitter_DTR_p*1e6))
write_period(periods_DTR, "periods_dtr.csv")

# Calculate jitter for USB trigger
periods_USB = list()
for f in filenames_USB:
    # For the bug of RIGOL DS1000Z csv file (When capture memory, got incorrect increment)
    df, increment, start = read_file(f, 500000000) # From the txt file (Sampling Rate)

    rising_edges = get_rising_edge(df["CH1"])
    periods_USB.extend(np.array(get_period(rising_edges)) * increment)

periods_USB = periods_USB[:100]
jitter_USB_c2c = get_jitter_cycle2cycle(periods_USB)
jitter_USB_p = get_jitter_period(periods_USB)


print("[USB Trigger Result] \n"
      "%d cycles, average period: %0.10fus, \n"
      "cycle 2 cycle jitter: %0.10fus \n"
      "period jitter %0.10fus\n"% (len(periods_DTR), np.average(periods_DTR)*1e6, jitter_USB_c2c*1e6, jitter_USB_p*1e6))

write_period(periods_USB, "periods_usb.csv")
# import pdb; pdb.set_trace()
plot_period(periods_DTR, periods_USB)
#plot_period_box(periods_DTR, periods_USB)

