import numpy as np
import matplotlib.pyplot as plt

# Загружаем данные из файла spectrum.txt
data = np.loadtxt('spectrum.txt')
frequencies = data[:, 0]  # первый столбец - частоты
amplitudes = data[:, 1]   # второй столбец - амплитуды

# Находим название аккорда
with open('chord_name.txt', 'r') as file:
    first_line = file.readline().strip()

# Строим график
plt.figure(figsize=(12, 5))

plt.plot(frequencies, amplitudes, 'b-', linewidth=1)
plt.xlabel('Частота (Гц)')
plt.ylabel('Амплитуда')
plt.title('Спектр сигнала')
plt.grid(True, alpha=0.3)

# Ограничиваем диапазон для наглядности (до 2000 Гц)
plt.xlim(0, min(1000, max(frequencies)))

# Находим все локальные максимумы (пики)
peaks_idx = []
for i in range(1, len(amplitudes) - 1):
    if amplitudes[i] > amplitudes[i-1] and amplitudes[i] > amplitudes[i+1]:
        peaks_idx.append(i)

# Фильтруем по высоте (только пики выше 20% от максимума)
max_amp = max(amplitudes)
threshold = max_amp * 0.2

strong_peaks_idx = [i for i in peaks_idx if amplitudes[i] > threshold]

peak_freqs = frequencies[strong_peaks_idx]
peak_amps = amplitudes[strong_peaks_idx]

# Отмечаем все пики на графике
plt.plot(peak_freqs, peak_amps, 'ro', markersize=8, label=f'Найдено {len(peak_freqs)} пиков \n {first_line}')

# Подписываем каждый пик (если их не слишком много)
if len(peak_freqs) <= 10:
    for freq, amp in zip(peak_freqs, peak_amps):
        plt.annotate(f'{freq:.1f} Гц', 
                     xy=(freq, amp), 
                     xytext=(5, 5),
                     textcoords='offset points',
                     fontsize=8,
                     alpha=0.7)

plt.legend()
plt.tight_layout()
plt.show()
