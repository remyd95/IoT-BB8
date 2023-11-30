import matplotlib.pyplot as plt
import numpy as np

data = [
    (64529, 64860, 20),
    (64860, 65318, 27),
    (54635, 55212, 50),
    (57145, 57778, 70),
    (53821, 54619, 80),
    (59036, 60104, 92),
    (60138, 61701, 146),
]

differences = []
centimeters = []

# Calculate differences and collect centimeters
for start, end, cm in data:
    diff = end - start
    differences.append(diff)
    centimeters.append(cm)

# Perform linear regression
slope, intercept = np.polyfit(centimeters, differences, 1)

# Generate the fitted line
fit_line = np.polyval([slope, intercept], centimeters)

# Plotting
plt.plot(centimeters, differences, marker='o', linestyle='-', label='Data')
plt.plot(centimeters, fit_line, linestyle='--', color='red', label=f'Fitted Line: {slope:.2f}x + {intercept:.2f}')
plt.title('Difference vs Centimeters with Fitted Line')
plt.xlabel('Centimeters')
plt.ylabel('Difference')
plt.grid(True)
plt.legend()

# Save plot to a file
plt.savefig('difference_plot_with_fit.png')

# Show the plot
plt.show()

# Formula for calculating cm based on the difference
def calculate_cm(difference):
    return (difference - intercept) / slope

print(f"Formula: cm = (measurement - {intercept:.2f}) / {slope:.2f}")