#Look for #IMPLEMENT tags in this file. These tags indicate what has
#to be implemented to complete the Snowman Puzzle domain.

#   You may add only standard python imports---i.e., ones that are automatically
#   available on TEACH.CS
#   You may not remove any imports.
#   You may not import or otherwise source any of your own files

# import os for time functions


from search import * #for search engines
from snowman import SnowmanState, Direction, snowman_goal_state #for snowball specific classes and problems
from test_problems import PROBLEMS #20 test problems
import os

#snowball HEURISTICS
def heur_simple(state):
  '''trivial admissible snowball heuristic'''
  '''INPUT: a snowball state'''
  '''OUTPUT: a numeric value that serves as an estimate of the distance of the state (# of moves required to get) to the goal.'''
  return len(state.snowballs)

def heur_zero(state):
  return 0

def heur_manhattan_distance(state):
#IMPLEMENT
    '''admissible snowball puzzle heuristic: manhattan distance'''
    '''INPUT: a snowball state'''
    '''OUTPUT: a numeric value that serves as an estimate of the distance of the state to the goal.'''
    #We want an admissible heuristic, which is an optimistic heuristic.
    #It must always underestimate the cost to get from the current state to the goal.
    #The sum of the Manhattan distances between the snowballs and the destination for the Snowman is such a heuristic.
    #When calculating distances, assume there are no obstacles on the grid.
    #You should implement this heuristic function exactly, even if it is tempting to improve it.
    #Your function should return a numeric value; this is the estimate of the distance to the goal.
    rval = 0
    for snowball,value in state.snowballs.items():
        # only one snowball
        if value < 3:
            rval += abs(snowball[0] - state.destination[0])
            rval += abs(snowball[1] - state.destination[1])
        # two snowballs stack together
        elif value < 6:
            rval += abs(snowball[0] - state.destination[0]) * 2
            rval += abs(snowball[1] - state.destination[1]) * 2
        # three snowballs stack together
        else:
            rval += abs(snowball[0] - state.destination[0]) * 3
            rval += abs(snowball[1] - state.destination[1]) * 3
    return rval

def heur_alternate(state):
#IMPLEMENT
    '''a better heuristic'''
    '''INPUT: a snowball state'''
    '''OUTPUT: a numeric value that serves as an estimate of the distance of the state to the goal.'''
    #heur_manhattan_distance has flaws.
    #Write a heuristic function that improves upon heur_manhattan_distance to estimate distance between the current state and the goal.
    #Your function should return a numeric value for the estimate of the distance to the goal.


    rval = 0
    for snowball in state.snowballs.keys():
        val = state.snowballs[snowball]
        factor = 7
        if val == 6:
            if snowball == state.destination:
                return 0
        elif val == 5 or val == 4:
            if snowball == state.destination:
                rval += 12
        elif val == 3:
            if snowball == state.destination:
                continue
        elif val == 2 or val == 1:
            factor = 3 - val
            if snowball == state.destination:
                rval += 12
        elif val == 0:
            factor = 3
            if snowball == state.destination:
                continue
            rval += horizontal_preference_Big(snowball,state)
            rval += vertical_preference_Big(snowball,state)

        # detect dead cases
        if dead(snowball,state) == True:
            return float('inf')
        rval += manhattan(snowball,state.robot) * factor
        rval += manhattan(snowball,state.destination) * factor
    return rval/1.4

def manhattan(a,b):
    """
    return the manhattan distance between a and b
    """
    return abs(a[0] - b[0]) + abs(a[1]-b[1])


def horizontal_preference_Big(snowball,state):
    """
	Guide the robot to push Big ball to another line if it touches another ball

	"""

    rval = 0

    if snowball[0] < state.destination[0]:
        right = getright(snowball)
        if right not in state.snowballs:
            rval -= 4

    elif snowball[0] > state.destination[0]:
        left = getleft(snowball)
        if left not in state.snowballs:
            rval -= 4

    if snowball[0] < state.robot[0] and snowball[1] < state.robot[1]:
        


    return rval

def vertical_preference_Big(snowball,state):
    """
	Guide the ball to avoid the obstacles on its way to the destination
	"""
    rval = 0
    if snowball[1] > state.destination[1]:
        top = gettop(snowball)
        if isObstacle(top,state):
            rval += 3

    elif snowball[1] < state.destination[1]:
        bot = getbot(snowball)
        if isObstacle(bot,state):
            rval += 3

    else:
        if snowball[0] > state.destination[0]:
            left = getleft(snowball)
            if isObstacle(left,state):
                rval += 3
        elif snowball[0] < state.destination[0]:
            right = getright(snowball)
            if isObstacle(right,state):
                rval += 3
    return rval

def dead(snowball,state):
    """
    Check whether it is impossible for the current snowball to be pushed to the
    destination:

    (1) if the snowball is at corner and the destination is not that corner
    (2) if the snowball is against the boudary or against a wall of obstacles
        while the destination is not.
    (3) if two boxes touch each other and cause a deadlock there

    Return True
    """

    # corner stuck
    left = getleft(snowball)
    right = getright(snowball)
    top = gettop(snowball)
    bot = getbot(snowball)
    if isObstacle(left,state) or isObstacle(right,state):
        if isObstacle(bot,state) or isObstacle(top,state):
            return True

    # wall stuck , the ball is against the wall but the destination is
    # on other direction
    if snowball[0] == 0 or snowball[0] == state.width -1:
        if snowball[0] != state.destination[0]:
            return True
    elif snowball[1] == 0 or snowball[1] == state.height -1:
        if snowball[1] != state.destination[1]:
            return True
    elif horizontal_obstacle_extension(snowball,state):
        return True
    elif vertical_obstacle_extension(snowball,state):
        return True


    # if a medium and a C ball touches other, check whether it causes deadlock
    if state.snowballs[snowball] == 5:
        # horizontal checking :
        if left in state.snowballs:
            left_top = gettop(left)
            left_bot = getbot(left)
            if isObstacle(left_top,state) or isObstacle(left_bot,state):
                if isObstacle(bot,state) or isObstacle(top,state):
                    return True
        elif right in state.snowballs:
            right_top = gettop(right)
            right_bot = getbot(right)
            if isObstacle(right_top,state) or isObstacle(right_bot,state):
                if isObstacle(top,state) or isObstacle(bot,state):
                    return True

        # vertical checking :
        elif bot in state.snowballs:
            bot_left = getleft(bot)
            bot_right = getright(bot)
            if isObstacle(bot_left,state) or isObstacle(bot_right,state):
                if isObstacle(left,state) or isObstacle(right,state):
                    return True
        elif top in state.snowballs:
            top_left = getleft(top)
            top_right = getright(top)
            if isObstacle(top_left,state) or isObstacle(top_right,state):
                if isObstacle(left,state) or isObstacle(right,state):
                    return True

    return False


def gettop(box):
    """
    return the position of the grid to the top of t
    """
    return (box[0],box[1]-1)

def getbot(box):
    """
    return the position of the grid to the bottom of t
    """
    return (box[0],box[1]+1)

def getleft(box):
    """
    return the position of the grid to the left of t
    """
    return (box[0]-1,box[1])

def getright(box):
    """
    return the position of the grid to the right of t
    """
    return (box[0]+1,box[1])

def isObstacle(grid,state):
    """
    return False if grid is a bounday point or obstacle
    """
    if(grid[0] < 0 or grid[1] < 0 or grid[0] >= state.width or grid[1] >= state.height):
        return True
    return grid in state.obstacles


def horizontal_obstacle_extension(snowball,state):
    """
	check whether the ball is unable to move vertically anyway
	"""
    #check left
    cur = snowball
    if isObstacle(gettop(cur),state)==False and isObstacle(getbot(cur),state)==False:
        return False
    left = getleft(cur)
    right = getright(cur)
    box = False
    while not isObstacle(left,state):
        if isObstacle(gettop(left),state)==False and isObstacle(getbot(left),state)==False:
            return False
        left = getleft(left)
    box = False
    while not isObstacle(right,state):
        if isObstacle(gettop(right),state)==False and isObstacle(getbot(right),state)==False:
            return False
        right = getright(right)

    if snowball[1] == state.destination[1]:
        return False
    return True


def vertical_obstacle_extension(snowball,state):
    """
	check whether the ball is unable to move horizontally anyway
	"""
    #check top
    cur = snowball
    if isObstacle(getleft(cur),state)==False and isObstacle(getright(cur),state)==False:
        return False
    top = gettop(cur)
    bot = getbot(cur)
    while not isObstacle(top,state):
        if isObstacle(getleft(top),state)==False and isObstacle(getright(cur),state)==False:
            return False
        top = gettop(top)
    while not isObstacle(bot,state):
        if isObstacle(getleft(bot),state)==False and isObstacle(getright(bot),state)==False:
            return False
        bot = getbot(bot)
    if snowball[0] == state.destination[0]:
        return False
    return True

## =============================== Helper Functions ===========================


def fval_function(sN, weight):
#IMPLEMENT
    """
    Provide a custom formula for f-value computation for Anytime Weighted A star.
    Returns the fval of the state contained in the sNode.

    @param sNode sN: A search node (containing a SnowballState)
    @param float weight: Weight given by Anytime Weighted A star
    @rtype: float
    """
    #Many searches will explore nodes (or states) that are ordered by their f-value.
    #For UCS, the fvalue is the same as the gval of the state. For best-first search, the fvalue is the hval of the state.
    #You can use this function to create an alternate f-value for states; this must be a function of the state and the weight.
    #The function must return a numeric f-value.
    #The value will determine your state's position on the Frontier list during a 'custom' search.
    #You must initialize your search engine object as a 'custom' search engine if you supply a custom fval function.
    return sN.gval + sN.hval * weight

def anytime_gbfs(initial_state, heur_fn, timebound = 10):
#IMPLEMENT
    '''Provides an implementation of anytime greedy best-first search, as described in the HW1 handout'''
    '''INPUT: a snowball state that represents the start state and a timebound (number of seconds)'''
    '''OUTPUT: A goal state (if a goal is found), else False'''
    se = SearchEngine('best_first', 'full')
    se.init_search(initial_state, goal_fn=snowman_goal_state, heur_fn=heur_fn)
    first_ans = se.search(timebound)

    # first round
    if not first_ans:
        return False
    rval = first_ans

    # keep searching
    new_time_bound = se.search_stop_time - os.times()[0]
    new_cost_bound = (rval.gval,float('inf'),float('inf'))
    while new_time_bound > 0:
        next_ans = se.search(new_time_bound,new_cost_bound)
        if not next_ans:
            break
        rval = next_ans
        new_time_bound = se.search_stop_time - os.times()[0]
        new_cost_bound = (rval.gval,float('inf'),float('inf'))
    return rval

def anytime_weighted_astar(initial_state, heur_fn, weight=1., timebound = 10):
#IMPLEMENT
    '''Provides an implementation of anytime weighted a-star, as described in the HW1 handout'''
    '''INPUT: a snowball state that represents the start state and a timebound (number of seconds)'''
    '''OUTPUT: A goal state (if a goal is found), else False'''
    wrapped_fval_function = (lambda sN : fval_function(sN,weight))
    se = SearchEngine('custom', 'full')
    se.init_search(initial_state, goal_fn=snowman_goal_state,
                       heur_fn=heur_fn,
                       fval_function = wrapped_fval_function)


    # first round searching
    first_ans = se.search(timebound)
    if not first_ans:
        return False
    rval = first_ans


    # if still have time or state to explore go on
    new_time_bound = se.search_stop_time - os.times()[0]
    new_cost_bound = (float('inf'),float('inf'),rval.gval+se.heur_fn(rval))
    while new_time_bound > 0:
        next_ans = se.search(new_time_bound,new_cost_bound)
        if not next_ans:
            break
        rval = next_ans
        new_time_bound = se.search_stop_time - os.times()[0]
        new_cost_bound = (float('inf'),float('inf'),rval.gval+se.heur_fn(rval))

    return rval

if __name__ == "__main__":
  #TEST CODE
  solved = 0; unsolved = []; counter = 0; percent = 0; timebound = 2; #2 second time limit for each problem
  print("*************************************")
  print("Running A-star")

  for i in range(0, 10): #note that there are 20 problems in the set that has been provided.  We just run through 10 here for illustration.

    print("*************************************")
    print("PROBLEM {}".format(i))

    s0 = PROBLEMS[i] #Problems will get harder as i gets bigger

    se = SearchEngine('astar', 'full')
    se.init_search(s0, goal_fn=snowman_goal_state, heur_fn=heur_simple)
    final = se.search(timebound)

    if final:
      final.print_path()
      solved += 1
    else:
      unsolved.append(i)
    counter += 1

  if counter > 0:
    percent = (solved/counter)*100

  print("*************************************")
  print("{} of {} problems ({} %) solved in less than {} seconds.".format(solved, counter, percent, timebound))
  print("Problems that remain unsolved in the set are Problems: {}".format(unsolved))
  print("*************************************")

  solved = 0; unsolved = []; counter = 0; percent = 0; timebound = 8; #8 second time limit
  print("Running Anytime Weighted A-star")

  for i in range(0, 10):
    print("*************************************")
    print("PROBLEM {}".format(i))

    s0 = PROBLEMS[i] #Problems get harder as i gets bigger
    weight = 10
    final = anytime_weighted_astar(s0, heur_fn=heur_simple, weight=weight, timebound=timebound)

    if final:
      final.print_path()
      solved += 1
    else:
      unsolved.append(i)
    counter += 1

  if counter > 0:
    percent = (solved/counter)*100

  print("*************************************")
  print("{} of {} problems ({} %) solved in less than {} seconds.".format(solved, counter, percent, timebound))
  print("Problems that remain unsolved in the set are Problems: {}".format(unsolved))
  print("*************************************")


