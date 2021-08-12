from statistics import geometric_mean

files = [10000001, 10000003, 1000003, 2000003, 3000003, 4000001, 4000003]

for file in files:
    with open(f'entrada/{file}.txt') as f:
        data = f.readline().split(" ")
        print(f'{file}: {geometric_mean(data)}')
