import random
import math
import matplotlib.pyplot as plt

def calculate_difference(real_positions, estimated_positions):
    differences = [abs(real - estimated) for real, estimated in zip(real_positions, estimated_positions)]
    total_difference = sum(differences)
    return total_differencez

def calculate_angle(x1, y1, x2, y2):
    angle = math.atan2(y2 - y1, x2 - x1)
    return math.degrees(angle)

def move_towards_target(current_x, current_y, target_x, target_y, step_size, angle):
    new_x = current_x + step_size * math.cos(math.radians(angle))
    new_y = current_y + step_size * math.sin(math.radians(angle))
    return new_x, new_y

def move_towards_target_estim(current_x, current_y, target_x, target_y, step_size):
    angle_offset = random.uniform(-10, 10)
    angle = calculate_angle(current_x, current_y, target_x, target_y)
    estimation_error_x = random.uniform(5, 10)
    estimation_error_y = random.uniform(5, 10)
    new_x = current_x + (step_size+estimation_error_x) * math.cos(math.radians(angle+angle_offset))
    new_y = current_y + (step_size+estimation_error_y) * math.sin(math.radians(angle+angle_offset))
    return new_x, new_y, angle

def simulate_navigation(num_steps, start_x, start_y, target_x, target_y):
    real_positions = [(start_x, start_y)]  # Starting at point A
    estimated_positions = [(start_x, start_y)]  # Initial estimation

    for _ in range(num_steps):
        # Move towards the target using real positions
        step_size = 20
        new_x_estim, new_y_estim,angle_estim = move_towards_target_estim(estimated_positions[-1][0], estimated_positions[-1][1], target_x, target_y, step_size)
         # Simulate estimation with a small error
        estimated_positions.append(
            (new_x_estim,new_y_estim)
        )
        
        new_x, new_y = move_towards_target(real_positions[-1][0], real_positions[-1][1], target_x, target_y, step_size, angle_estim)
        real_positions.append((new_x, new_y))
       

    return real_positions, estimated_positions

def plot_navigation(real_positions, estimated_positions, target_x, target_y):
    real_x, real_y = zip(*real_positions)
    estimated_x, estimated_y = zip(*estimated_positions)

    plt.plot(real_x, real_y, label='Real Positions', marker='o')
    plt.plot(estimated_x, estimated_y, label='Estimated Positions', marker='x')
    plt.scatter(target_x, target_y, color='red', label='Target Point B', marker='D')
    plt.legend()
    plt.xlabel('X-axis')
    plt.ylabel('Y-axis')
    plt.title('Navigation towards Target Point B')
    plt.show()

def calculate_angle_from_origin(x, y):
    angle = math.atan2(y, x)
    return math.degrees(angle)

if __name__ == "__main__":
    num_steps = 6  # You can adjust the number of steps as needed
    target_x, target_y = 1000, 1000  # Target point B coordinates
    old_x, old_y = 0, 0  # Starting point A coordinates

    real_positions, estimated_positions = simulate_navigation(num_steps, old_x, old_y, target_x, target_y)
    # real_positions_2, estimated_positions_2 = simulate_navigation(num_steps, old_x, old_y, target_x, target_y)

    real = real_positions#+ real_positions_2
    estimate = estimated_positions# + estimated_positions_2


    # difference = calculate_difference(real, estimate)

    # print("Real Positions:", real)
    # print("Estimated Positions:", estimate)
    # print("Total Difference:", difference)
    print("Estimated_angle_from_origin:", calculate_angle_from_origin(estimate[-1][0], estimate[-1][1]))
    print("Real_angle_from_origin:", calculate_angle_from_origin(real[-1][0], real[-1][1]))
    plot_navigation(real, estimate, target_x, target_y)

