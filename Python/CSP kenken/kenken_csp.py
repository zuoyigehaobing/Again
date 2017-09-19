#Look for #IMPLEMENT tags in this file.

'''
Construct and return Kenken CSP model.
'''

from cspbase import *
import itertools

def kenken_csp_model(kenken_grid):
    '''Returns a CSP object representing a Kenken CSP problem along
       with an array of variables for the problem. That is return

       kenken_csp, variable_array

       where kenken_csp is a csp representing the kenken model
       and variable_array is a list of lists

       [ [  ]
         [  ]
         .
         .
         .
         [  ] ]

       such that variable_array[i][j] is the Variable (object) that
       you built to represent the value to be placed in cell i,j of
       the board (indexed from (0,0) to (N-1,N-1))


       The input grid is specified as a list of lists. The first list
	   has a single element which is the size N; it represents the
	   dimension of the square board.

	   Every other list represents a constraint a cage imposes by
	   having the indexes of the cells in the cage (each cell being an
	   integer out of 11,...,NN), followed by the target number and the
	   operator (the operator is also encoded as an integer with 0 being
	   '+', 1 being '-', 2 being '/' and 3 being '*'). If a list has two
	   elements, the first element represents a cell, and the second
	   element is the value imposed to that cell. With this representation,
	   the input will look something like this:

	   [[N],[cell_ij,...,cell_i'j',target_num,operator],...]

       This routine returns a model which consists of a variable for
       each cell of the board, with domain equal to {1-N}.

       This model will also contain BINARY CONSTRAINTS OF NOT-EQUAL between
       all relevant variables (e.g., all pairs of variables in the
       same row, etc.) and an n-ary constraint for each cage in the grid.
    '''
    grid_size = kenken_grid[0][0]
    cage_num = len(kenken_grid) - 1

    # construct domain for all variables
    i = 0
    dom = []
    for i in range(grid_size):
        dom.append(i+1) # 1~N

    # construct Variable
    vars = []
    for t in itertools.product(dom,dom):
        vars.append(Variable('V{}{}'.format(t[0],t[1]), dom))

    # construct constraints
    cons = []


    # row and col
    row_col_diff = []
    for t in itertools.product(dom,dom):
        if t[0] != t[1]:
            row_col_diff.append(t)

    for t in itertools.product(dom,dom):
        row = t[0]
        col = t[1]
        # start to add row CONSTRAINTS
        for hor in range(col+1,grid_size+1):
            scope = [get_var_at(row,col,vars,grid_size),get_var_at(row,hor,vars,grid_size)]
            con = Constraint("V{}{}-V{}{})".format(row,col,row,hor),scope)
            con.add_satisfying_tuples(row_col_diff)
            cons.append(con)
        # start to add col CONSTRAINTS
        for ver in range(row+1,grid_size+1):
            scope = [get_var_at(row,col,vars,grid_size),get_var_at(ver,col,vars,grid_size)]
            con = Constraint("V{}{}-V{}{}".format(row,col,ver,col),scope)
            con.add_satisfying_tuples(row_col_diff)
            cons.append(con)

    # start to add cage CONSTRAINTS
    for i in range(1,len(kenken_grid)):
        cage = kenken_grid[i]
        cage_size = len(cage) - 2
        scope = []


        if cage_size == 0:
            scope.append(get_var(cage[0],vars,grid_size))
            sat_tuples = [(cage[1],)]

        else:
            operator = cage[-1]
            target = cage[-2]
            involved = cage[:len(cage)-2]
            for num in involved:
                scope.append(get_var(num,vars,grid_size))

            sat_tuples = []
            sat_dict = dict()
            for candidate in itertools.product(dom,repeat=cage_size):
                if operation(candidate,operator,target):
                    for permute in itertools.permutations(list(candidate)):
                        sat_tuples.append(permute)

        con = Constraint("Cage{}".format(i),scope)
        con.add_satisfying_tuples(sat_tuples)
        cons.append(con)


    # CSP construction
    csp = CSP("{}-KenKen".format(grid_size),vars)
    for c in cons:
        csp.add_constraint(c)
    return csp,construct_vars(vars,grid_size)






####################################################
#              Shan_Helper_Function                #
####################################################
def get_var_at(i,j,vars,size):
    '''
    return the variable at row=i, column = j
    '''
    return vars[(i-1)*size + j-1]

def get_var(num,vars,size):
    '''
    return the var at the position at num
    e.g num = 11, return the var at cell(1,1)
    '''
    row = int(num / 10)
    col = int(num % 10)
    return get_var_at(row,col,vars,size)

def construct_vars(vars,grid_size):
    '''
    Convert the vars array to array of array
    '''
    rval = []
    for i in range(1,grid_size+1):
        row = []
        for j in range(1,grid_size+1):
            row.append(get_var_at(i,j,vars,grid_size))
        rval.append(row)
    return rval


def operation(candidate,operator,rval):
    '''
    check whether the candidate tuple can achieve the rval under specified operator
    '''
    check = candidate[0]
    state = False
    if operator == 0: # addition
        for i in range(1,len(candidate)):
            check += candidate[i]
        state = (rval == check)

    elif operator == 1: # subtraction
        for i in range(1,len(candidate)):
            check = check - candidate[i]
        state = (rval == check)

    elif operator == 2: # division
        for i in range(1,len(candidate)):
            check = check / candidate[i]
        state = (rval == check)

    else: # multiplication
        for i in range(1,len(candidate)):
            check = check * candidate[i]
        state = (rval == check)


    return state