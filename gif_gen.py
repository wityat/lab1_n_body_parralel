import matplotlib.pyplot as plt
from celluloid import Camera
import csv

camera = Camera(plt.figure())
with open('output.csv', 'r') as csvfile:
    reader = csv.reader(csvfile)
    next(reader)  # Skip header row
    for row in reader:
        row = row[1:-1]
        plt.scatter([float(val) for val in row[::2]],[float(val) for val in row[1::2]], color='black', s=3)
        camera.snap()
camera.animate().save('picture.gif')
