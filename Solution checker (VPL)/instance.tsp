from pychoco import Model

def get_magic_square(n):
    # calculate magic constant
    magic = n * (n**2 + 1) // 2
    
    model = Model("magic_square_" + str(n))

    # create the grid
    square = []
    for i in range(n):
        square.append(model.intvars(n, 1, n**2, "row_" + str(i)))

    # all numbers must be unique
    flat_list = []
    for row in square:
        for val in row:
            flat_list.append(val)
    model.all_different(flat_list).post()

    # check rows and columns
    for i in range(n):
        model.sum(square[i], "=", magic).post()
        
        col = [square[j][i] for j in range(n)]
        model.sum(col, "=", magic).post()

    # diagonals
    d1 = [square[i][i] for i in range(n)]
    d2 = [square[i][n - 1 - i] for i in range(n)]
    model.sum(d1, "=", magic).post()
    model.sum(d2, "=", magic).post()

    # standard form conditions to avoid symmetric duplicates
    if n > 1:
        # top left is smallest corner
        model.arithm(square[0][0], "<", square[0][n - 1]).post()
        model.arithm(square[0][0], "<", square[n - 1][0]).post()
        model.arithm(square[0][0], "<", square[n - 1][n - 1]).post()
        
        # second element of first row < first element of second row
        model.arithm(square[0][1], "<", square[1][0]).post()

    # solve it
    solver = model.get_solver()
    sol = solver.find_solution()

    if sol:
        print("Magic square of order", n)
        for row in square:
            print([sol.get_int_val(x) for x in row])
    else:
        print("No solution")


# test it out
get_magic_square(3)
print("---")
get_magic_square(4)