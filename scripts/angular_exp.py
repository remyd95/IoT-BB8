import matplotlib.pyplot as plt
import numpy as np

def read_data(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()[1:]  # Skip the first line
    
    data = [list(map(int, line.strip().split())) for line in lines if line.strip()]  # Exclude empty lines
    return np.array(data)

# def plot_imu_error(imu_data, real_data):
#     imu_diff = (imu_data[:, 0] - imu_data[:, 1])
#     real_diff = (real_data[:, 0] - real_data[:, 1])
#     print(imu_diff)
#     print(real_diff)

#     plt.figure(figsize=(10, 6))
    
#     plt.subplot(2, 1, 1)
#     plt.plot(imu_diff, label='IMU Start-Stop Difference')
#     plt.plot(real_diff, label='Real Start-Stop Difference')
#     plt.title('IMU and Real Start-Stop Differences')
#     plt.legend()

#     plt.subplot(2, 1, 2)
#     plt.plot(imu_data[:, 0], label='IMU Start')
#     plt.plot(imu_data[:, 1], label='IMU Stop')
#     plt.plot(real_data[:, 0], label='Real Start')
#     plt.plot(real_data[:, 1], label='Real Stop')
#     plt.title('IMU and Real Positions')
#     plt.legend()

#     plt.tight_layout()
#     plt.show()

def plot_curved_polar_lines(imu_data, real_data):
    num_rows = imu_data.shape[0]
    angles = np.linspace(0, 2*np.pi, num_rows, endpoint=False)

    plt.figure(figsize=(10, 10))

    for i, (imu_row, real_row) in enumerate(zip(imu_data, real_data)):
        imu_start, imu_stop = np.radians(imu_row[:2])
        real_start, real_stop = np.radians(real_row[:2])

        # Calculate curved lines using parametric equations for a circle
        # Ensure linspace goes over 0
        if imu_stop < imu_start:
            imu_line = np.linspace(imu_start, imu_stop+ 2 * np.pi , 1000)
        else:
            imu_line = np.linspace(imu_start, imu_stop, 1000)

        if real_stop < real_start:
            real_line = np.linspace(real_start, real_stop+ 2 * np.pi, 1000)
        else:
            real_line = np.linspace(real_start, real_stop, 1000)

        # Plot the shape formed by connecting start and stop positions
        ax = plt.subplot(2, num_rows/2, i+1, projection='polar')
        ax.plot(imu_line, np.ones_like(imu_line), label='IMU', linewidth=2)
        ax.plot(real_line, np.ones_like(real_line), label='Real', linewidth=2)

        # Plot lines from the centerpoint (0, 0) to the endpoints in the same color
        ax.plot([0, imu_line[0]], [0, imu_line[-1]], color='blue', linestyle='--')
        ax.plot([0, imu_line[-1]], [0, imu_line[0]], color='blue', linestyle='--')

        ax.plot([0, real_line[0]], [0, real_line[-1]], color='orange', linestyle='--')
        ax.plot([0, real_line[-1]], [0, real_line[0]], color='orange', linestyle='--')

        ax.set_rlabel_position(90)
        ax.set_rmax(1.2)
        ax.set_rticks([])  # Hide radial ticks
        # ax.set_title(f'Data Row {i+1}')

    plt.legend()
    plt.suptitle('Polar Plot of IMU and Real Positions with Curved Lines', y=1.02)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    file_path = "scripts/angular_experiment.txt"  # Replace with your file path
    data = read_data(file_path)

    imu_data = data[:, :2]
    real_data = data[:, 2:]
    real_data = real_data + [141, 141]

    diff = real_data - imu_data
    diff_abs = np.abs(diff)
    print(diff_abs)

    # Apply the condition to each column separately
    diff_abs[:, 0] = np.where(diff_abs[:, 0] > 180, np.abs(diff[:, 0] - 360), diff_abs[:, 0])
    diff_abs[:, 1] = np.where(diff_abs[:, 1] > 180, np.abs(diff[:, 1] - 360), diff_abs[:, 1])

    print(max(max(diff_abs[:, 0]),max(diff_abs[:, 1])))

    
    plot_curved_polar_lines(imu_data, real_data)
