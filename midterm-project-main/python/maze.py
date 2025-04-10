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
        print(self.raw_data)
        self.path = [] # store the node on the shortest path in order
        self.actions = [] #store the action in number type
        self.node_dict = dict()  # key: index, value: the correspond node
        for i in range(self.rows):
            cur = Node(i + 1)
            for j in range(4):
               if(self.raw_data[i][j] != -1): 
                   cur.set_successor(self.raw_data[i][j], j + 1, self.raw_data[i][j + 4])
            self.node_dict.update({i + 1:cur})


        

    def get_start_point(self):
        if len(self.node_dict) < 2:
            log.error("Error: the start point is not included.")
            return 0
        return self.node_dict[1] ## setting node 1 is always the start point

    def get_node_dict(self):
        return self.node_dict

    def BFS(self, start_node: Node, end_node: Node):
        # TODO : design your data structure here for your algorithm
        self.qq = []
        self.qq.append(start_node.index) # first is the node index the second is where it comes from
        self.node_dict[start_node.index].prev = -1 #-1 means the start point 

        head = 0
        while (len(self.qq)>= 0):
            idx = self.qq[head]
            if(idx == end_node.index):
                #print('found', idx)
                break
            for cur in self.node_dict[idx].get_successors():
                if(self.node_dict[cur[0]].prev == -2): ## -2 means the point hasn't used
                    self.qq.append(cur[0])
                    self.node_dict[cur[0]].prev = idx
            head+=1
        # Tips : return a sequence of nodes from the node to the nearest unexplored deadend
        # # trace
        self.traceback(idx)
    
    def traceback(self, idx):
        self.path.insert(0, self.node_dict[idx])
        previdx = self.node_dict[idx].prev
        if(previdx != -1): ##previdx == -1 means find the point is the start point
            self.traceback(previdx)
        else:
            for node in self.path:
                print(node.index)
            self.getActions(self.path)

    def getAction(self, car_dir, node_from: Node, node_to: Node):
        # TODO : get the car action
        # Tips : return an action and the next direction of the car if the node_to is the Successor of node_to
        # If not, print error message and return 0
        turnface = node_from.get_direction_to(node_to)
        match car_dir:
            case Direction.NORTH:
                match turnface:
                    case Direction.NORTH:
                        self.actions.append(Action.ADVANCE)
                    case Direction.EAST:
                        self.actions.append(Action.TURN_RIGHT)
                    case Direction.WEST:
                        self.actions.append(Action.TURN_LEFT)
                    case Direction.SOUTH:
                        self.actions.append(Action.U_TURN)
            case Direction.SOUTH:
                match turnface:
                    case Direction.NORTH:
                        self.actions.append(Action.U_TURN)
                    case Direction.EAST:
                        self.actions.append(Action.TURN_LEFT)
                    case Direction.WEST:
                        self.actions.append(Action.TURN_RIGHT)
                    case Direction.SOUTH:
                        self.actions.append(Action.ADVANCE)
            case Direction.EAST:
                match turnface:
                    case Direction.NORTH:
                        self.actions.append(Action.TURN_LEFT)
                    case Direction.EAST:
                        self.actions.append(Action.ADVANCE)
                    case Direction.WEST:
                        self.actions.append(Action.U_TURN)
                    case Direction.SOUTH:
                        self.actions.append(Action.TURN_RIGHT)
            case Direction.WEST:
                match turnface:
                    case Direction.NORTH:
                        self.actions.append(Action.TURN_RIGHT)
                    case Direction.EAST:
                        self.actions.append(Action.U_TURN)
                    case Direction.WEST:
                        self.actions.append(Action.ADVANCE)
                    case Direction.SOUTH:
                        self.actions.append(Action.TURN_LEFT)
        return None

    def getActions(self, nodes: List[Node]):
        # TODO : given a sequence of nodes, return the corresponding action sequence
        car_dir = self.path[0].get_direction_to(self.path[1])
        for i in range(len(nodes) - 1):
            self.getAction(car_dir, self.path[i], self.path[i + 1])
            car_dir = self.path[i].get_direction_to(self.path[i + 1])
        self.actions.append(Action.HALT)
        self.actions_to_str(self.actions)
        # Tips : iterate through the nodes and use getAction() in each iteration
        return None

    def actions_to_str(self, actions):
        # cmds should be a string sequence like "fbrl....", use it as the input of BFS checklist #1
        cmd = "fbrls" # forward, backward, turn right, turn left, stop
        cmds = ""
        for action in actions:
            cmds += cmd[action - 1]
        log.info(cmds)
        return cmds

'''
    def BFS_2(self, node_from: Node, node_to: Node):
        # TODO : similar to BFS but with fixed start point and end point
        # Tips : return a sequence of nodes of the shortest path
        return None
'''
