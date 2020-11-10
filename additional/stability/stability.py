import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.signal import lfilter
import seaborn as sns
from pylab import *
import more_itertools as mit
from matplotlib import rc
import math

from os import listdir

filename_z170 = "./SVID experiments - Stability Test Z170 (simple).csv"
df_z170 = pd.read_csv(filename_z170, usecols=["Frequency","First Fault Undervolt", "Freeze Undervolt"] )

filename_z370 = "./SVID experiments - Stability Test Z370(simple).csv"
df_z370 = pd.read_csv(filename_z370, usecols=["Frequency","First Fault Undervolt", "Freeze Undervolt"] )

# Set Latex style font
rc("font",family="Georgia", size="11")
rc("axes", labelsize="14")

fig = plt.figure(figsize=(5,2))
ax = fig.add_subplot(111)

min_freq = min(min(df_z170["Frequency"]), min(df_z370["Frequency"]))
max_freq = max(max(df_z170["Frequency"]), max(df_z370["Frequency"]))
# i3-9100-MZ370
#i3-7100-AZ170
ax.plot(df_z170["Frequency"], np.array(df_z170["First Fault Undervolt"])*1e3, "-", label="i3-7100 First Fault ")
ax.plot(df_z170["Frequency"], np.array(df_z170["Freeze Undervolt"])*1e3, ":", label="i3-7100 Crash")

ax.plot(df_z370["Frequency"], np.array(df_z370["First Fault Undervolt"])*1e3, "--", label="i3-9100 First Fault")
ax.plot(df_z370["Frequency"], np.array(df_z370["Freeze Undervolt"])*1e3, "-.", label="i3-9100 Crash")
# Place a legend above this subplot, expanding itself to
# fully use the given bounding box.
ax.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc='lower left',
           ncol=2, mode="expand", borderaxespad=0.)

# ax.set_title('DTR Trigger')
ax.set_xlabel('Frequency (GHz)')
ax.set_ylabel('Undervolting (mV)')

#y_min, y_max = ax.get_ylim()
y_min = min(min(df_z170["Freeze Undervolt"]), min(df_z370["Freeze Undervolt"])) *1e3
y_max = max(max(df_z170["First Fault Undervolt"]), max(df_z370["First Fault Undervolt"])) * 1e3

ax.set_yticks(np.arange(math.floor(y_min), math.ceil(y_max), 20))
ax.set_xlim(min_freq, max_freq)
plt.subplots_adjust(bottom=0.3, top=0.85, left=0.15)
# plt.show()
plt.savefig('stability.pdf', format="pdf", bbox_inches = 'tight')