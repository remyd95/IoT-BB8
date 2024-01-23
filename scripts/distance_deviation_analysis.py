import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit

data = [
    (60223.82, 60974.54, 136),
    (60974.54, 61785.91, 139),
    (61785.91, 62415.82, 45),
    (62415.82, 63185.69, 86),
    (63185.69, 64059.26, 118),
    (64059.26, 65393.66, 190),
    (65393.66, 66099.21, 58),
    (66099.21, 66985.61, 96),
    (66985.61, 67882.57, 101),
    (69037.62, 69900.67, 88),
    (69900.67, 70871.25, 96),
    (70871.25, 71786.35, 73),
    (71786.35, 72323.87, 67),
    (72323.87, 73535.55, 195),
    (73535.55, 74644.11, 192),
    (76485.80, 77613.51, 154),
    (77613.51, 78465.55, 145),
    (78465.55, 79564.47, 172),
    (79564.47, 80519.15, 137),
    (80519.15, 81391.53, 98),
    (81391.53, 81983.14, 73),
    (81983.14, 82622.45, 65),
    (82622.45, 83223.79, 57),
    (83223.79, 83968.87, 64),
    (83968.87, 84483.35, 57),
    (84483.35, 84882.64, 47),
    (84882.64, 85318.09, 48),
    (85318.09, 86416.79, 134),
    (86416.79, 87509.19, 146),
    (87509.19, 88506.15, 144),
    (88506.15, 89869.69, 147),
    (89869.69, 91122.36, 190),
    (91122.36, 92272.18, 157),
    (92272.18, 93625.09, 185),
    (93625.09, 94901.94, 164),
    (96089.94, 97048.15, 129),
    (97048.15, 98222.89, 150),
    (98222.89, 99432.45, 189),
    (99432.45, 100619.30, 152),
    (100619.30, 101693.77, 169),
    (101693.77, 103094, 179),
    (103094, 103493, 24),
    (103493, 103862.12, 32),
    (103862.12, 104110.87, 24),
    (104110.87, 104516.75, 47),
    (104516.75, 104960.38, 44),
    (104960.38, 105271.28, 23),
]

data = sorted(data, key=lambda x: x[2])

differences = []
centimeters = []

# Calculate differences and collect centimeters
for start, end, cm in data:
    diff = end - start
    differences.append(diff)
    centimeters.append(cm)


def sqrt_func(x, a, b):
    return a * np.sqrt(x) + b


params, covs = curve_fit(sqrt_func, centimeters, differences)

print("params: ", params)
print("covariance: ", covs)

a, b = params[0], params[1]
fit_line = a * np.sqrt(centimeters) + b

print(f"Equation = {a} * sqrt(x) + {b}")

# Plotting
plt.plot(centimeters, differences, marker='o', linestyle='-', label='Data')
plt.plot(centimeters, fit_line, linestyle='--', color='red')
plt.title('Difference vs Centimeters with Fitted Line')
plt.xlabel('Centimeters')
plt.ylabel('Difference')
plt.grid(True)
plt.legend()

# Save plot to a file
plt.savefig('difference_plot_with_fit.png')

# Show the plot
plt.show()


def distance_estimation(x):
    return ((x + 224.41) / 108.44) ** 2


def distance_estimation_inv(x):
    return 108.44262769035421 * np.sqrt(x) + -224.41739519624443


print(distance_estimation(distance_estimation_inv(500)))


def distance_estimate_small(x):
    return (((20 * x + 224.41) / 108.44) ** 2) / 20


print(distance_estimate_small(20) * 20 * 4)
