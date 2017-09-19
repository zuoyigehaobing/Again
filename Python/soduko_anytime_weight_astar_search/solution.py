#Look for #IMPLEMENT tags in this file. These tags indicate what has
#to be implemented to complete the Sokoban warehouse domain.

#   You may add only standard python imports---i.e., ones that are automatically
#   available on TEACH.CS
#   You may not remove any imports.
#   You may not import or otherwise source any of your own files

# import os for time functions
import os
from search import * #for search engines
from sokoban import SokobanState, Direction, PROBLEMS, sokoban_goal_state #for Sokoban specific classes and problems

#SOKOBAN HEURISTICS
def heur_displaced(state):
  '''trivial admissible sokoban heuristic'''
  '''INPUT: a sokoban state'''
  '''OUTPUT: a numeric value that serves as an estimate of the distance of the state to the goal.'''
  count = 0
  for box in state.boxes:
    if box not in state.storage:
      count += 1
  return count

def heur_manhattan_distance(state):
#IMPLEMENT
    '''admissible sokoban heuristic: manhattan distance'''
    '''INPUT: a sokoban state'''
    '''OUTPUT: a numeric value that serves as an estimate of the distance of the state to the goal.'''
    #We want an admissible heuristic, which is an optimistic heuristic.
    #It must always underestimate the cost to get from the current state to the goal.
    #The sum Manhattan distance of the boxes to their closest storage spaces is such a heuristic.
    #When calculating distances, assume there are no obstacles on the grid and that several boxes can fit in one storage bin.
    #You should implement this heuristic function exactly, even if it is tempting to improve it.
    #Your function should return a numeric value; this is the estimate of the distance to the goal.
    # ==================  My function ====================
    # no restrictions : sum of min of all the boxes to the goal , allow overlap since optimistic


    rval = 0
    if state.restrictions is None:
        for box in state.boxes:
            cur_man = -1
            for storage in state.storage:
                this_man = abs(storage[0] - box[0]) + abs(storage[1] - box[1])
                if this_man <= cur_man or cur_man == -1:
                    cur_man = this_man
            rval += cur_man

    # restrictions
    else:
            # some of the boxes sharing restrictions : remain to be done
            for box in state.boxes:
                cur_man = -1
                for storage in state.storage:
                    if state.boxes[box] != state.storage[storage]:
                        continue
                    this_man = abs(storage[0] - box[0]) + abs(storage[1] - box[1])
                    if this_man <= cur_man or cur_man == -1:
                        cur_man = this_man
                rval += cur_man

    return rval

def heur_alternate(state):
#IMPLEMENT
    '''a better sokoban heuristic'''
    '''INPUT: a sokoban state'''
    '''OUTPUT: a numeric value that serves as an estimate of the distance of the state to the goal.'''
    #heur_manhattan_distance has flaws.
    #Write a heuristic function that improves upon heur_manhattan_distance to estimate distance between the current state and the goal.
    #Your function should return a numeric value for the estimate of the distance to the goal.
    # ========================= MY FILTER FUNCTION ===========================
    # STEP 1 : get all the information we need

    # =========================  SELECT H VAL  ===========================
    rval = 0
    hook = detecthook(state)
    blacklist = dict()

    for box in state.boxes:
        for storage in state.storage:
            if onlychoice(box,storage,state):
                blacklist[storage] = box

    num_instorage = 0
    if state.restrictions is None:
        for box in state.boxes:

            if len(hook) >0 and box in hook:
                return float('inf')
            if cornerstuck(box,state) == 1:
                return float('inf')
            if touchboxes(box,state):
                return float('inf')
            if wallstuck(box,state):
                return float('inf')


            cur_man = -1
            for storage in state.storage:

                x = abs(storage[0] - box[0])
                y = abs(storage[1] - box[1])
                this_man = x + y
                if storage != box and stableoccupied(storage,state):
                    this_man = float('inf')
                if storage == box and stableoccupied(storage,state):
                    rval = rval  - min(state.width,state.height)/2
                if storage in blacklist:
                    if box != blacklist[storage]:
                        this_man = float('inf')
                    else:
                        cur_man = 2 * this_man
                        break
                avoid = have_to_aviod(box,storage,state)
                #avoid = 0
                if avoid == 1:
                    this_man = box[1] * 2 + this_man*1.1
                if avoid == 2:
                    this_man = (state.height-1-box[1]) * 2 + this_man*1.1
                if avoid == 3:
                    this_man = box[0] * 2 + this_man*1.1
                if avoid == 4:
                    this_man = (state.width-1-box[0]) * 2 + this_man*1.1
                if this_man <= cur_man or cur_man == - 1:
                    cur_man = this_man
            rval += cur_man

    # restrictions
    else:
            # some of the boxes sharing restrictions : remain to be done
            for box in state.boxes:
                if len(hook) >0 and box in hook:
                    return float('inf')
                if cornerstuck(box,state) == 1:
                    return float('inf')
                if touchboxes(box,state):
                    return float('inf')
                if wallstuck(box,state):
                    return float('inf')


                cur_man = -1
                for storage in state.storage:
                    if state.boxes[box] != state.storage[storage]:
                        continue
                    x = abs(storage[0] - box[0])
                    y = abs(storage[1] - box[1])
                    this_man = x + y
                    if storage != box and stableoccupied(storage,state):
                        this_man = float('inf')
                    if storage == box and stableoccupied(storage,state):
                        rval = rval - min(state.width,state.height)/2
                    if storage in blacklist:
                        if box != blacklist[storage]:
                            this_man = float('inf')
                        else:
                            cur_man = 2 * this_man
                            break
                rval += cur_man


    return rval + man_to_box_min(state)


def fval_function(sN, weight):
#IMPLEMENT
    """
    Provide a custom formula for f-value computation for Anytime Weighted A star.
    Returns the fval of the state contained in the sNode.

    @param sNode sN: A search node (containing a SokobanState)
    @param float weight: Weight given by Anytime Weighted A star
    @rtype: float
    """

    #Many searches will explore nodes (or states) that are ordered by their f-value.
    #For UCS, the fvalue is the same as the gval of the state. For best-first search, the fvalue is the hval of the state.
    #You can use this function to create an alternate f-value for states; this must be a function of the state and the weight.
    #The function must return a numeric f-value.
    #The value will determine your state's position on the Frontier list during a 'custom' search.
    #You must initialize your search engine object as a 'custom' search engine if you supply a custom fval function.
    return sN.gval + weight * sN.hval

def anytime_gbfs(initial_state, heur_fn, timebound = 10):
#IMPLEMENT
    '''Provides an implementation of anytime greedy best-first search, as described in the HW1 handout'''
    '''INPUT: a sokoban state that represents the start state and a timebound (number of seconds)'''
    '''OUTPUT: A goal state (if a goal is found), else False'''

    se = SearchEngine('best_first', 'full')
    se.init_search(initial_state, goal_fn=sokoban_goal_state, heur_fn=heur_fn)


    # first round searching
    first_ans = se.search(timebound)
    if not first_ans:
        return False
    rval = first_ans


    # if still have time or state to explore go on
    better = True
    while better:
        new_timebound = se.search_stop_time - os.times()[0]
        if new_timebound <= 0:
            break
        new_costbound = (rval.gval,float('inf'),float('inf'))
        better = se.search(new_timebound,new_costbound)
        if better and better.gval < rval.gval:
            rval = better
    return rval

def anytime_weighted_astar(initial_state, heur_fn, weight=1., timebound = 10):
#IMPLEMENT
    '''Provides an implementation of anytime weighted a-star, as described in the HW1 handout'''
    '''INPUT: a sokoban state that represents the start state and a timebound (number of seconds)'''
    '''OUTPUT: A goal state (if a goal is found), else False'''

    wrapped_fval_function = (lambda sN : fval_function(sN,weight))
    se = SearchEngine('custom', 'full')
    se.init_search(initial_state, goal_fn=sokoban_goal_state,
                       heur_fn=heur_fn,
                       fval_function = wrapped_fval_function)
    # first round searching

    first_ans = se.search(timebound)
    if not first_ans:
        return False
    rval = first_ans


    # if still have time or state to explore go on
    better = True
    while better:
        new_timebound = se.search_stop_time - os.times()[0]
        if new_timebound <= 0:
            break
        new_costbound = (float('inf'),float('inf'),rval.gval+se.heur_fn(rval))
        better = se.search(new_timebound,new_costbound)
        if better and better.gval < rval.gval:
            rval = better
    return rval


# ======================== Helper Functions ================================
def man_to_box_min(state):
    robot = state.robot
    rval = -1
    for box in state.boxes:
        distance = abs(box[0]-robot[0]) + abs(box[1]-robot[1])
        if rval == -1:
            rval = distance
        elif distance < rval:
            rval = distance
    return rval - 1


def instorage(box,state):
    if state.restrictions is None:
        if box not in state.storage:
            return False
        return True
    if box not in state.restrictions[state.boxes[box]]:
        return False
    return True

def gettop(box):
    return (box[0],box[1]-1)
def getbot(box):
    return (box[0],box[1]+1)
def getleft(box):
    return (box[0]-1,box[1])
def getright(box):
    return (box[0]+1,box[1])


def IsObstacle(t,state):
    """
    return False if this is a bounday point or obstacle
    """
    if(t[0] == -1 or t[1] == -1 or t[0] == state.width or t[1] == state.height):
        return True
    elif t in state.obstacles:
        return True
    return False


def cornerstuck(box,state):
    """
    check whether a box stuck in the corner , only check stuck by boundry and obs
    return 0 if not
    return 1 if yes and that corner is not it's storage -> results inf in h
    return 2 if yes but that corner is it's storage
    """
    # first check the top direction
    top = (box[1] == 0 or (box[0],box[1]-1) in state.obstacles)
    bot = (box[1] == state.height - 1 or (box[0],box[1]+1) in state.obstacles)
    if top or bot:
        left = (box[0] == 0 or (box[0]-1,box[1]) in state.obstacles)
        right = (box[0] == state.width-1 or (box[0]+1,box[1]) in state.obstacles)
        if left or right:
            # check whether it is in the storage
            if instorage(box,state):
                return 2
            else:
                return 1
    return 0



def TouchBox_horizontal(box,state):
    left = getleft(box)
    right = getright(box)
    if left in state.boxes:
        return left
    if right in state.boxes:
        return right
    else:
        return None

def TouchBox_vertical(box,state):
    top = gettop(box)
    bot = getbot(box)
    if top in state.boxes:
        return top
    if bot in state.boxes:
        return bot
    return None

def vertical_moveable(box,state):
    top = gettop(box)
    bot = getbot(box)
    if IsObstacle(top,state) or IsObstacle(bot,state):
        return False
    if top in state.storage:
        if stableoccupied(top,state):
            return False
    if bot in state.storage:
        if stableoccupied(bot,state):
            return False
    return True

def horizontal_moveable(box,state):
    left = getleft(box)
    right = getright(box)
    if IsObstacle(left,state) or IsObstacle(right,state):
        return False
    if left in state.storage:
        if stableoccupied(left,state):
            return False
    if right in state.storage:
        if stableoccupied(right,state):
            return False
    return True

def touchboxes(box,state):
    hor_toucher = TouchBox_horizontal(box,state)
    if hor_toucher is not None:
        if instorage(box,state) and instorage(hor_toucher,state):
            return False
        check1 = vertical_moveable(box,state)
        check2 = vertical_moveable(hor_toucher,state)
        if check1 == False and check2 == False:
            return True
    ver_toucher = TouchBox_vertical(box,state)
    if ver_toucher is not None:
        if instorage(box,state) and instorage(ver_toucher,state):
            return False
        check1 = horizontal_moveable(box,state)
        check2 = horizontal_moveable(ver_toucher,state)
        if check1 == False and check2 == False:
            return True
        # if touch both sides
        if hor_toucher is not None:
            check3 = vertical_moveable(hor_toucher,state)
            if check3 == False and check2 == False:
                return True

    return False



def lineconnection(t1,t2,state):
    # t1 storage t2 box
    # assume that t1 and t2 are in the same line
    # if vertical line
    if t1[0] == t2[0]:
        start = min(t1[1],t2[1])
        end = max(t1[1],t2[1])
        while start != end:
            my_next = (t1[0],start+1)
            if IsObstacle(my_next,state):
                return False
            # if other box
            if my_next in state.boxes and instorage(my_next,state):
                return False
            start = start + 1
    # if horizontal line
    elif t1[1] == t2[1]:
        start = min(t1[0],t2[0])
        end = max(t1[0],t2[0])
        while start != end:
            my_next = (start+1,t1[1])
            if IsObstacle(my_next,state):
                return False
            # if other box
            if my_next in state.boxes and instorage(my_next,state):
                return False
            start = start + 1
    return True


def stableoccupied(storage,state):
    for box in state.boxes:
        if storage == box and cornerstuck(box,state)==2:
            return True
    return False



def not_enough_storage_hor_line(box,storage,state):
    bcount = 0
    scount = 0
    for b in state.boxes:
        if b[0] == box[0]:
            bcount = bcount + 1
    for s in state.storage:
        if s[0] == storage[0]:
            scount = scount + 1
    if scount < bcount:
        return True
    return False


def not_enough_storage_ver_line(box,storage,state):
    bcount = 0
    scount = 0
    for b in state.boxes:
        if b[1] == box[1]:
            bcount = bcount + 1
    for s in state.storage:
        if s[1] == storage[1]:
            scount = scount + 1
    if scount < bcount:
        return True
    return False


def obstacleoneside(box,state):
    if box[0] == 0:
        return 'left'
    elif box[0] == state.width-1:
        return 'right'
    elif box[1] == 0:
        return 'top'
    elif box[1] == state.height - 1:
        return 'bot'
    return 'no'

def wallstuck(box,state):
    """
    check whether a box stuck in oneway
    return 0 if not
    return 1 if yes and that position is not it's storage -> results inf in h
    return 2 if yes but that position is it's storage
    """
    # horizontal stuck wall on left or right


    if instorage(box,state):
        return False
    check = obstacleoneside(box,state)
    if check.__eq__('no'):
        return False
    if state.restrictions is None:
        if check in {'left','right'}:
            for storage in state.storage:
                if storage[0] == box[0]:
                    if lineconnection(storage,box,state):
                        if not_enough_storage_hor_line(box,storage,state):
                            return True
                        return False
            return True
        # vertical stuck wall on left or right
        elif check in {'top','bot'}:
            for storage in state.storage:
                if storage[1] == box[1]:
                    if lineconnection(storage,box,state):
                        if not_enough_storage_ver_line(box,storage,state):
                            return True
                        return False
            return True
    else:
        if check in {'left','right'}:
            for storage in state.storage:
                if state.boxes[box] != state.storage[storage]:
                    continue
                if storage[0] == box[0]:
                    if lineconnection(storage,box,state):
                        if not_enough_storage_hor_line(box,storage,state):
                            return True
                        return False
            return True
        # vertical stuck wall on left or right
        elif check in {'top','bot'}:
            for storage in state.storage:
                if state.boxes[box] != state.storage[storage]:
                    continue
                if storage[1] == box[1]:
                    # check whether obstacle between
                    if lineconnection(storage,box,state):
                        if not_enough_storage_ver_line(box,storage,state):
                            return True
                        return False
            return True
    return False



def getneighbours(t):
    top = (t[0],t[1]-1)
    bot = (t[0],t[1]+1)
    left = (t[0]-1,t[1])
    right = (t[0]+1,t[1])
    return [top,bot,right,left]


def detecthook(state):
    hor_start = -1
    hor_end = -1
    ver_start = -1
    ver_end = -1
    rval = dict()
    for obstacle in state.obstacles:
        [top,bot,right,left] = getneighbours(obstacle)
        if top in state.obstacles:
            if right in state.obstacles:
                hor_start = obstacle[0]
                ver_end = obstacle[1]
                # expand
                hor_end = obstacle[0]
                ver_start = obstacle[1]
                my_next = obstacle
                while(getright(my_next) in state.obstacles):
                    hor_end += 1
                    my_next = getright(my_next)

                if hor_end == state.width-1:
                    collect = True
                    for i in  range(top[0]+1,state.width):
                        to_add = (i,top[1])
                        if to_add in state.storage:
                            collect = False
                    if collect:
                        for i in  range(top[0]+1,state.width):
                            to_add = (i,top[1])
                            rval[to_add] = 0
                my_next = obstacle
                while(gettop(my_next) in state.obstacles):
                    ver_start -= 1
                    my_next = gettop(my_next)

                if ver_start == 0:
                    collect = True
                    for i in range(0,right[1]):
                        to_add = (top[0]+1,i)
                        if to_add in state.storage:
                            collect = False
                    if collect :
                        for i in range(0,right[1]):
                            to_add = (top[0]+1,i)
                            rval[to_add] = 0




            elif left in state.obstacles:
                hor_end = obstacle[0]
                ver_end = obstacle[1]
                # expand
                hor_start = obstacle[0]
                ver_start = obstacle[1]
                my_next = obstacle
                while(getleft(my_next) in state.obstacles):
                    hor_start -= 1
                    my_next = getleft(my_next)

                if hor_start == 0:
                    collect = True
                    y = top[1]
                    for x in range(0,top[0]):
                        if (x,y) in state.storage:
                            collect = False
                    if collect:
                        for x in range(0,top[0]):
                            rval[(x,y)] = 0


                my_next = obstacle
                while(gettop(my_next) in state.obstacles):
                    ver_start -= 1
                    my_next = gettop(my_next)

                if ver_start == 0:
                    collect = True
                    x = left[0]
                    for y in range(0,left[1]):
                        if (x,y) in state.storage:
                            collect = False
                    if collect:
                        for y in range(0,left[1]):
                            rval[(x,y)] = 0
                #return [range(hor_start,hor_end),range(ver_start,ver_end)]


        elif  bot in state.obstacles:
            if right in state.obstacles:
                hor_start = obstacle[0]
                ver_start = obstacle[1]
                # expand
                hor_end = obstacle[0]
                ver_end = obstacle[1]
                my_next = obstacle
                while(getright(my_next) in state.obstacles):
                    hor_end += 1
                    my_next = getright(my_next)

                if hor_end == state.width - 1:
                    y = bot[1]
                    collect = True
                    for x in range(bot[0],state.width):
                        if (x,y) in state.storage:
                            collect = False
                    if collect:
                        for x in range(bot[0],state.width):
                            rval[(x,y)] = 0

                my_next = obstacle
                while(getbot(my_next) in state.obstacles):
                    ver_end += 1
                    my_next = getbot(my_next)

                if ver_end == state.height - 1:
                    x = right[0]
                    collect = True
                    for y in range(right[1],state.height):
                        if (x,y) in state.storage:
                            collect = False
                    if collect:
                        for y in range(right[1],state.height):
                            rval[(x,y)] = 0
                #return [range(hor_start,hor_end),range(ver_start,ver_end)]



            elif left in state.obstacles:
                hor_end = obstacle[0]
                ver_start = obstacle[1]
                # expand
                hor_start = obstacle[0]
                ver_end = obstacle[1]
                my_next = obstacle
                while(getleft(my_next) in state.obstacles):
                    hor_end -= 1
                    my_next = getleft(my_next)

                if hor_start == 0:
                    collect = True
                    y = bot[1]
                    for x in range(0,bot[0]):
                        if((x,y) in state.storage):
                            collect = False
                    if collect:
                        for x in range(0,bot[0]):
                            rval[(x,y)] = 0

                my_next = obstacle
                while(getbot(my_next) in state.obstacles):
                    ver_end += 1
                    my_next = getbot(my_next)

                if ver_end == state.height-1:
                    x = left[0]
                    collect = True
                    for y in range(left[1],state.height):
                        if (x,y) in storage:
                            collect = False

                    if collect:
                        for y in range(left[1],state.height):
                            rval[(x,y)] = 0
                #return [range(hor_start,hor_end),range(ver_start,ver_end)]
    return rval

def onlychoice(box,storage,state):
    if box[0] == storage[0] or box[1] == storage[1]:
        # on the left boundary
        if box[0] == 0 or box[0] == state.width-1:
            for s in state.storage:
                if s[0] == box[0] and s != storage:
                    return False
            return True
        # on the top
        elif box[1] == 0 or box[1] == state.height-1:
            for s in state.storage:
                if s[1] == box[1] and s!= storage:
                    return False
            return True
    return False


def have_to_aviod(box,storage,state):
    # box on the right of the storage
    if box[0] >= storage[0]:
        start = min(box[1],storage[1])
        end = max(box[1],storage[1])
        while start != end:
            my_next = (box[0],start+1)
            if IsObstacle(my_next,state):
                if obstacle_extension_left(my_next,state):
                    right = getright(box)
                    if IsObstacle(right,state):
                        if box[1] >= storage[1]:
                            return 2
                        else:
                            return 1
                    # direction to go right : cur = (state.width-1 - box[0]) * 2
                    return 4
            start = start + 1

    if storage[0] >= box[0]:
        start = min(box[1],storage[1])
        end = max(box[1],storage[1])
        while start != end:
            my_next = (storage[0],start+1)
            if IsObstacle(my_next,state):
                if obstacle_extension_left(my_next,state):
                    right = getright(box)
                    if IsObstacle(right,state):
                        if box[1] >= storage[1]:
                            return 2
                        else:
                            return 1
                    # direction to go right : cur = (state.width-1 - box[0]) * 2
                    return 4
            start = start + 1

    if box[0] <= storage[0]:
        start = min(box[1],storage[1])
        end = max(box[1],storage[1])
        while start != end:
            my_next = (box[0],start+1)
            if IsObstacle(my_next,state):
                if obstacle_extension_right(my_next,state):
                    left = getleft(box)
                    if IsObstacle(left,state):
                        # direction to away : cur = ()
                        if box[1] >= storage[1]:
                            return 2
                        else:
                            return 1
                    # direction to go left
                    return 3
            start = start + 1

    if storage[0] <= box[0]:
        start = min(box[1],storage[1])
        end = max(box[1],storage[1])
        while start != end:
            my_next = (storage[0],start+1)
            if IsObstacle(my_next,state):
                if obstacle_extension_right(my_next,state):
                    left = getleft(box)
                    if IsObstacle(left,state):
                        # direction to away : cur = ()
                        if box[1] >= storage[1]:
                            return 2
                        else:
                            return 1
                    # direction to go left
                    return 3
            start = start + 1

    if box[1] >= storage[1]:
        start = min(box[0],storage[0])
        end = max(box[0],storage[0])
        while start != end:
            my_next = (start+1,box[1])
            if IsObstacle(my_next,state):
                if obstacle_extension_top(my_next,state):
                    bot = getbot(box)
                    if IsObstacle(bot,state):
                        # direction to away
                        if box[0] >= storage[0]:
                            return 4
                        else:
                            return 3
                    # direction to go bot
                    return 2
            start = start + 1

    if storage[1] >= box[1]:
        start = min(box[0],storage[0])
        end = max(box[0],storage[0])
        while start != end:
            my_next = (start+1,storage[1])
            if IsObstacle(my_next,state):
                if obstacle_extension_top(my_next,state):
                    bot = getbot(box)
                    if IsObstacle(bot,state):
                        # direction to away
                        if box[0] >= storage[0]:
                            return 4
                        else:
                            return 3
                    # direction to go bot
                    return 2
            start = start + 1

    if box[1] <= storage[1]:
        start = min(box[0],storage[0])
        end = max(box[0],storage[0])
        while start != end:
            my_next = (start+1,box[1])
            if IsObstacle(my_next,state):
                if obstacle_extension_bot(my_next,state):
                    top = gettop(box)
                    if IsObstacle(top,state):
                        # direction to away
                        if box[0] >= storage[0]:
                            return 4
                        else:
                            return 3
                    # direction to go bot
                    return 1
            start = start + 1

    if storage[1] <= box[1]:
        start = min(box[0],storage[0])
        end = max(box[0],storage[0])
        while start != end:
            my_next = (start+1,storage[1])
            if IsObstacle(my_next,state):
                if obstacle_extension_bot(my_next,state):
                    top = gettop(box)
                    if IsObstacle(top,state):
                        # direction to away
                        if box[0] >= storage[0]:
                            return 4
                        else:
                            return 3
                    # direction to go bot
                    return 1
            start = start + 1


    return 0



def obstacle_extension_left(obs,state):
    for i in range(0,obs[0]+1):
        check = (i,obs[1])
        if not check in state.obstacles:
            return False
    return True


def obstacle_extension_right(obs,state):
    for i in range(obs[0],state.width):
        check = (i,obs[1])
        if not check in state.obstacles:
            return False
    return True


def obstacle_extension_top(obs,state):
    for i in range(0,obs[1]+1):
        check = (obs[0],i)
        if not check in state.obstacles:
            return False
    return True


def obstacle_extension_bot(obs,state):
    for i in range(obs[1],state.height):
        check = (obs[0],i)
        if not check in state.obstacles:
            return False
    return True

if __name__ == "__main__":
  #TEST CODE
  solved = 0; unsolved = []; counter = 0; percent = 0; timebound = 2; #2 second time limit for each problem
  print("*************************************")
  print("Running A-star")

  for i in range(0, 10): #note that there are 40 problems in the set that has been provided.  We just run through 10 here for illustration.

    print("*************************************")
    print("PROBLEM {}".format(i))

    s0 = PROBLEMS[i] #Problems will get harder as i gets bigger

    se = SearchEngine('astar', 'full')
    se.init_search(s0, goal_fn=sokoban_goal_state, heur_fn=heur_displaced)
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
    final = anytime_weighted_astar(s0, heur_fn=heur_displaced, weight=weight, timebound=timebound)

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
