import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('spectrum.txt')
frequencies = data[:, 0]
amplitudes = data[:, 1]

with open('chord_name.txt', 'r') as file:
    chord_name = file.readline().strip()


peaks_data = np.loadtxt('frequencies.txt', dtype=[('freq', float), ('note', 'U10')])
peak_freqs = peaks_data['freq']
peak_notes = peaks_data['note']


plt.figure(figsize=(12, 5))
plt.plot(frequencies, amplitudes, 'b-', linewidth=1, alpha=0.7)
plt.xlabel('Частота (Гц)')
plt.ylabel('Амплитуда')
plt.title('Спектр сигнала')
plt.grid(True, alpha=0.3)
plt.xlim(0, min(1000, max(frequencies)))






peak_amps = []
for pf in peak_freqs:
    idx = np.argmin(np.abs(frequencies - pf))
    peak_amps.append(amplitudes[idx])
    
plt.scatter(peak_freqs, peak_amps, color='red', s=80, zorder=5, label=f'{chord_name}')
    

if len(peak_freqs) <= 15:
    for freq, amp in zip(peak_freqs, peak_amps):
        plt.annotate(f'{freq:.1f} Гц', xy=(freq, amp), xytext=(5, 5), textcoords='offset points', fontsize=8, alpha=0.7)

plt.legend()
plt.tight_layout()
plt.show()
