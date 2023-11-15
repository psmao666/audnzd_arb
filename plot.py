import matplotlib.pyplot as plt
from datetime import datetime

# Read data from the file
with open("portfolio.txt", "r") as file:
    lines = file.readlines()

# Extract Date and Value for plotting
dates = []
values = []

for line in lines:
    date_str, value_str = line.split(' ')
    # Convert the date string to a datetime object
    date = datetime.strptime(date_str, "%Y.%m.%d,%H:%M")
    dates.append(date)
    values.append(float(value_str))

# Create a line plot with dates on the X-axis
plt.plot(dates, values, marker='o')
plt.xlabel("Date")
plt.ylabel("Value")
plt.title("Date vs Value Plot")
plt.xticks(rotation=45)  # Rotate x-axis labels for better visibility
plt.tight_layout()  # Adjust layout to prevent clipping of labels
plt.show()
