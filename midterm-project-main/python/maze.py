import csv
import logging
import math
from enum import IntEnum
from typing import List

import numpy as np
import pandas as pd

from node import Direction, Node

log = logging.getLogger(__name__)


class Action(IntEnum):
    ADVANCE = 1
    U_TURN = 2
    TURN_RIGHT = 3
    TURN_LEFT = 4
    HALT = 5


class Maze:
    def __init__(self, filepath: str = r"D:\Document\code\Car_project\midterm-project-main\maze.csv"):
        # TODO : read file and implement a data structure you like
        # For example, when parsing raw_data, you may create several Node objects.
        # Then you can store these objects into self.nodes.
        # Finally, add to nd_dict by {key(index): value(corresponding node)}
        self.raw_data = pd.read_csv(filepath, index_col="index").fillna(-1)
        self.rows = len(self.raw_data)
        self.raw_data = self.raw_data.values
        self.path = []
        self.node_dict = dict()  # key: index, value: the correspond node
        for i in range(self.rows):
            cur = Node(i)
            for j in range(4):
               if(self.raw_data[i][j] != -1): 
                   cur.set_successor(self.raw_data[i][j], j + 1, self.raw_data[i][j + 4])
            self.node_dict.update({i:cur})


        

    def get_start_point(self):
        if len(self.node_dict) < 2:
            log.error("Error: the start point is not included.")
            return 0
        return self.node_dict[1] ## setting node 1 is always the start point

    def get_node_dict(self):
        return self.node_dict

    def BFS(self, end_node: Node):
        # TODO : design your data structure here for your algorithm
        self.qq = []
        used = []
        for i in range(self.rows):
            used[i] = False
        self.qq[0] = [1, -1] # first is the node index the second is where it comes from
        used[1] = True

        head = 0, tail = 1
        hidx = 0
        while (len(self.qq)>= 0):
            idx = self.qq[head][0]
            if(idx == end_node.index):
                break
            for cur in self.node_dict[idx].get_successors():
                if(used[cur.index] == False):
                    self.qq[tail][0] = cur.index
                    self.qq[tail][1] = idx
                    used[cur.index] = True
                    tail+=1
            head+=1
        # trace
        self.traceback(head)

        # Tips : return a sequence of nodes from the node to the nearest unexplored deadend
        return None
    
    def traceback(self, qidx):
        #qidx.from == -1 end, else add to the front of the path
        self.path.insert(0, self.node_dict[self.qq[qidx][0]])
        if(self.qq[qidx][1] == -1):
            


    def BFS_2(self, node_from: Node, node_to: Node):
        # TODO : similar to BFS but with fixed start point and end point
        # Tips : return a sequence of nodes of the shortest path
        return None

    def getAction(self, car_dir, node_from: Node, node_to: Node):
        # TODO : get the car action
        # Tips : return an action and the next direction of the car if the node_to is the Successor of node_to
        # If not, print error message and return 0
        return None

    def getActions(self, nodes: List[Node]):
        # TODO : given a sequence of nodes, return the corresponding action sequence
        for i in range(nodes):
            self.getAction()
        # Tips : iterate through the nodes and use getAction() in each iteration
        return None

    def actions_to_str(self, actions):
        # cmds should be a string sequence like "fbrl....", use it as the input of BFS checklist #1
        cmd = "fbrls"
        cmds = ""
        for action in actions:
            cmds += cmd[action - 1]
        log.info(cmds)
        return cmds

    def strategy(self, node: Node):
        return self.BFS(node)

    def strategy_2(self, node_from: Node, node_to: Node):
        return self.BFS_2(node_from, node_to)
