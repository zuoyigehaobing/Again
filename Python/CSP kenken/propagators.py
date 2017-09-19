#Look for #IMPLEMENT tags in this file. These tags indicate what has
#to be implemented to complete problem solution.

'''This file will contain different constraint propagators to be used within
   bt_search.

   propagator == a function with the following template
      propagator(csp, newVar=None)
           ==> returns (True/False, [(Variable, Value), (Variable, Value) ...]

      csp is a CSP object---the propagator can use this to get access
      to the variables and constraints of the problem. The assigned variables
      can be accessed via methods, the values assigned can also be accessed.

      newVar (newly instaniated variable) is an optional argument.
      if newVar is not None:
          then newVar is the most
           recently assigned variable of the search.
      else:
          progator is called before any assignments are made
          in which case it must decide what processing to do
           prior to any variables being assigned. SEE BELOW

       The propagator returns True/False and a list of (Variable, Value) pairs.
       Return is False if a deadend has been detected by the propagator.
       in this case bt_search will backtrack
       return is true if we can continue.

      The list of variable values pairs are all of the values
      the propagator pruned (using the variable's prune_value method).
      bt_search NEEDS to know this in order to correctly restore these
      values when it undoes a variable assignment.

      NOTE propagator SHOULD NOT prune a value that has already been
      pruned! Nor should it prune a value twice

      PROPAGATOR called with newVar = None
      PROCESSING REQUIRED:
        for plain backtracking (where we only check fully instantiated
        constraints)
        we do nothing...return true, []

        for forward checking (where we only check constraints with one
        remaining variable)
        we look for unary constraints of the csp (constraints whose scope
        contains only one variable) and we forward_check these constraints.

        for gac we establish initial GAC by initializing the GAC queue
        with all constaints of the csp


      PROPAGATOR called with newVar = a variable V
      PROCESSING REQUIRED:
         for plain backtracking we check all constraints with V (see csp method
         get_cons_with_var) that are fully assigned.

         for forward checking we forward check all constraints with V
         that have one unassigned variable left

         for gac we initialize the GAC queue with all constraints containing V.
   '''

def prop_BT(csp, newVar=None):
    '''Do plain backtracking propagation. That is, do no
    propagation at all. Just check fully instantiated constraints'''

    if not newVar:
        return True, []
    for c in csp.get_cons_with_var(newVar):
        if c.get_n_unasgn() == 0:
            vals = []
            vars = c.get_scope()
            for var in vars:
                vals.append(var.get_assigned_value())
            if not c.check(vals):
                return False, []
    return True, []


def prop_FC(csp, newVar=None):
    '''Do forward checking. That is check constraints with
       only one uninstantiated variable. Remember to keep
       track of all pruned variable,value pairs and return '''
    rval = []
    DWO = False
    check_list = None
    if not newVar:
        check_list = csp.get_all_cons()
    else:
        check_list = csp.get_cons_with_var(newVar)
    for c in check_list:
        # can use get_unasgn_vars directly, try whether save time
        if c.get_n_unasgn() == 1:
            unasgn_var = c.get_unasgn_vars()[0]
            state,prun = FC_check(c,unasgn_var)
            rval += prun
            if not state: # DWO occurs
                DWO = True
                break
    if DWO:
        return False,rval
    return True,rval


####################################################
#              Shan_Helper_Function                #
####################################################
def FC_check(c,x):
    '''Do forward checking on a single variable and a single
       constraint, return True if there is some possible value
       return False if DWO occurs
    '''
    curdom = x.cur_domain()
    rval = []
    state = False
    for d in curdom:
        x.assign(d)
        check = []
        for item in c.get_scope():
            check.append(item.get_assigned_value())
        if c.check(check):
            state = True
        else:
            rval.append((x,d))
            x.prune_value(d)
        x.unassign()
    return state,rval





def prop_GAC(csp, newVar=None):
    '''Do GAC propagation. If newVar is None we do initial GAC enforce
       processing all constraints. Otherwise we do GAC enforce with
       constraints containing newVar on GAC Queue'''
    rval = []
    q = [] # iterator queue
    # if newVar is None,do initial GAC_Enforce of all constaints
    if not newVar:
        for c in csp.get_all_cons():
            q.append(c)

    else:
        # prune all value other than the assigned one of newVar
        curdom = newVar.cur_domain()
        for val in curdom:
            if val == newVar.get_assigned_value():
                continue
            else:
                rval.append((newVar,val))
                newVar.prune_value(val)
        # add constraints containing newVar in its scope
        q = csp.get_cons_with_var(newVar)

    # gac Enforce
    state,prun = GAC_Enforce(csp,q)
    rval += prun
    return state,rval





####################################################
#              Shan_Helper_Function                #
####################################################
def GAC_Enforce(csp,q):
    '''GAC-Queue contains all constraints one of whose variables has
       had its domain reduced. At the root of the search tree first we
       run GAC_Enforce with all constraints on GAC-Queue
    '''
    rval = []
    while len(q) != 0:
        cur_con = q.pop(0) # pop(0) --> 189 || pop() -->197
        scope = cur_con.get_scope()
        for var in scope:
            curdom = var.cur_domain()
            for val in curdom:
                if not cur_con.has_support(var,val):
                    rval.append((var,val))
                    var.prune_value(val)
                    if var.cur_domain_size() == 0:
                        return False,rval #DWO occurs
                    else:
                        for new_con in csp.get_cons_with_var(var):
                            if new_con in q:
                                continue
                            else:
                                q.append(new_con)
    return True,rval


# end