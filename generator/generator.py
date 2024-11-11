import numpy as np
import os

def generate_matrices(seed):
    np.random.seed(seed)
    
    sizes = {100: "small", 300: "medium", 600: "large"}
    matrices = {}

    for size in sizes:
        matrix = np.random.randint(0, 2, (size, size))
        
        matrix = np.triu(matrix, 1)
        matrix += matrix.T
        
        matrices[sizes[size]] = matrix
    
    return matrices

seed = int(input("Enter a seed value: "))
matrices = generate_matrices(seed)

output_dir = "../in"
os.makedirs(output_dir, exist_ok=True)  

for name, matrix in matrices.items():
    filename = os.path.join(output_dir, f"{name}.txt")
    with open(filename, "w") as f:
        f.write(f"{matrix.shape[0]}\n")
        # write the matrix on the following lines
        for row in matrix:
            f.write(" ".join(map(str, row)) + "\n")
    print(f"{name.capitalize()} matrix saved to {filename}")
