from statistics import geometric_mean

files = [1000003,  2000003,  3000003,  4000003,
         10000001, 10000003, 10000005, 10000011, 10000021, 10000055,
         12000123, 12000155, 13000155, 13010155, 15000121]

for file in files:
    with open(f'entrada/{file}.txt') as f:
        data = f.readline().split(" ")
        print(f'{file}: {geometric_mean(data)}')
