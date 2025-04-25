import csv
import logging
import math
from time import process_time
from enum import IntEnum
from typing import List

import numpy as np
import pandas as pd


from node import Direction, Node

log = logging.getLogger(__name__)
start_time = process_time()

class Action(IntEnum):
    ADVANCE = 1
    TURN_RIGHT = 2
    TURN_LEFT = 3
    U_TURN_R = 4
    U_TURN_L = 5
    HALT = 6
    

class Duration(IntEnum):
    ADVANCE = 800 #unit ms
    U_TURN_L = 1600
    U_TURN_R = 1600
    TURN_RIGHT = 900
    TURN_LEFT = 900


class Maze:

    def __init__(self, filepath: str = r"D:\Document\code\Car_project\midterm-project-main\big_maze_113.csv"):
        self.raw_data = pd.read_csv(filepath, index_col="index").fillna(-1)
        self.num_nodes = len(self.raw_data)
        self.raw_data = self.raw_data.values
        self.rows = 6
        #from csv to node
        self.nodes = dict()  # key: index, value: the correspond node
        self.start_node = Node()
        for i in range(self.num_nodes):
            cur = Node(i + 1)
            for j in range(4):
               if(self.raw_data[i][j] != -1): 
                   cur.set_successor(self.raw_data[i][j], j + 1, self.raw_data[i][j + 4])
                   self.nodes.update({i+1:cur})
        #for BFS
        self.path = [] # store the node on the shortest path in order
        self.actions = [] #store the action in number type
        #for setting treasure info
        self.treasures = [] # store the treasure node
        #for DFS
        self.shortest_node_path = []
        self.max_scores = -1
        #final cmd string
        self.cmds = 'f'

    # set start
    def set_start_node(self, start):
        self.start_node = self.nodes[start]
        self.set_treasure_info()
        return None
    
    # clear path[] and actions[] before next bfs
    def clear(self):
        self.path.clear()
        self.actions.clear()
        for i in range(1, len(self.nodes) + 1):
            self.nodes[i].prev = -2
    
    # find treasure point and set up neighboring treasures' distance and path
    def set_treasure_info(self):
        # find all treasure points
        for node in self.nodes.values():
            if(len(node.get_successors()) == 1):
                self.treasures.append(node)
        #do bfs and store the distance and the cmd
        num = len(self.nodes)
        self.treasure_info = [[0, ''] * (num + 1) for i in range(num + 1)]
        for node1  in self.treasures:
            for node2 in self.treasures:
                if(node1.index != node2.index):
                    self.BFS(node1, node2)
                    self.treasure_info[node1.index][node2.index] = [self.count_time(), self.count_score(node2), self.actions_to_str()]
                    self.clear()
                else:
                    self.treasure_info[node1.index][node2.index] = ['']
    
    def count_time(self):
        time = 0
        for num in self.actions:
            match num:
                case Action.ADVANCE:
                    time += Duration.ADVANCE
                case Action.TURN_LEFT:
                    time += Duration.TURN_LEFT
                case Action.TURN_RIGHT:
                    time += Duration.TURN_RIGHT
                case Action.U_TURN_L:
                    time += Duration.U_TURN_L
                case Action.U_TURN_R:
                    time += Duration.U_TURN_R
        return time

    def count_score(self, node: Node):
        nodex = (node.index - 1)//self.rows
        nodey = (node.index - 1)%self.rows
        start_nodex = (self.start_node.index - 1)//self.rows
        start_nodey = (self.start_node.index - 1)%self.rows
        return 30*(abs(nodex - start_nodex) + abs(nodey - start_nodey))

    # do the node queue by bfs
    def BFS(self, start: Node, end: Node):
        # TODO : design your data structure here for your algorithm
        qq = []
        qq.append(start.index) # first is the node index the second is where it comes from
        self.nodes[start.index].prev = -1 #-1 means the start point 

        head = 0
        while (len(qq)>= 0):
            idx = qq[head]
            if(idx == end.index):
                #print('found', idx)
                break
            for cur in self.nodes[idx].get_successors():
                if(self.nodes[cur[0]].prev == -2): ## -2 means the point hasn't used
                    qq.append(cur[0])
                    self.nodes[cur[0]].prev = idx
            head+=1
        # # trace
        self.traceback(idx)
        self.getActions()

        #for node in self.path:
        #    print(node.index, end = ' ')
        #print("\n--------")

    # based on end_node to traceback and fill in path[]
    def traceback(self, idx):
        self.path.insert(0, self.nodes[idx])
        previdx = self.nodes[idx].prev
        if(previdx != -1): ##previdx == -1 means find the point is the start point
            self.traceback(previdx)

    # determine the action of the car
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
                        if(node_from.index >= 1 and node_from.index <= self.rows):
                            self.actions.append(Action.U_TURN_L)
                        elif(node_from.index > self.num_nodes - self.rows):
                            self.actions.append(Action.U_TURN_R)
                        else:
                            self.actions.append(Action.U_TURN_R)
            case Direction.SOUTH:
                match turnface:
                    case Direction.NORTH:
                        if(node_from.index >= 1 and node_from.index <= self.rows):
                            self.actions.append(Action.U_TURN_R)
                        elif(node_from.index > self.num_nodes - self.rows):
                            self.actions.append(Action.U_TURN_L)
                        else:
                            self.actions.append(Action.U_TURN_R)
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
                        if(node_from.index % self.rows == 1):
                            self.actions.append(Action.U_TURN_R)
                        elif(node_from.index % self.rows == 0):
                            self.actions.append(Action.U_TURN_L)
                        else:
                            self.actions.append(Action.U_TURN_R)
                    case Direction.SOUTH:
                        self.actions.append(Action.TURN_RIGHT)
            case Direction.WEST:
                match turnface:
                    case Direction.NORTH:
                        self.actions.append(Action.TURN_RIGHT)
                    case Direction.EAST:
                        if(node_from.index % self.rows == 1):
                            self.actions.append(Action.U_TURN_L)
                        elif(node_from.index % self.rows == 0):
                            self.actions.append(Action.U_TURN_R)
                        else:
                            self.actions.append(Action.U_TURN_R)
                    case Direction.WEST:
                        self.actions.append(Action.ADVANCE)
                    case Direction.SOUTH:
                        self.actions.append(Action.TURN_LEFT)
        return None
    
    #do getAction according to path and call actions_to_str
    def getActions(self):
        # TODO : given a sequence of nodes, return the corresponding action sequence
        car_dir = self.path[0].get_direction_to(self.path[1])
        for i in range(1, len(self.path) - 1):
            self.getAction(car_dir, self.path[i], self.path[i + 1])
            car_dir = self.path[i].get_direction_to(self.path[i + 1])
        self.getAction(car_dir, self.path[-1], self.path[-2])
        # Tips : iterate through the nodes and use getAction() in each iteration
        return None
    
    # turn actions list to cmds string
    def actions_to_str(self):
        # cmds should be a string sequence like "fbrl....", use it as the input of BFS checklist #1
        cmd = "frlqps" # forward, turn right, turn left, uturn  stop
        cmds = ""
        for action in self.actions:
            cmds += cmd[action - 1]
        return cmds
    
    # translate nodepath_to_cmds
    def nodepath_to_cmds(self):
        for i in range(len(self.shortest_node_path) - 1):
            self.cmds += self.treasure_info[self.shortest_node_path[i].index][self.shortest_node_path[i + 1].index][2]
        print("node sequence:")
        for node in self.shortest_node_path:
            print(node.index, end = ' ')
        print()
        print("message command:")
        print(self.cmds)

    #maze initializ
    #maze = Maze()
    #maze setting
def solve_maze(maze: Maze):
    maze.set_start_node(24)

    #container for DFS
    for node in maze.nodes.values():
        node.used = False
    node_path = []

    #show maze.treasures
    for node1 in maze.treasures:
        for node2 in maze.treasures:
            print(node1.index, ' ', node2.index, ' ', maze.treasure_info[node1.index][node2.index][0], end = '\t')
        print()

    def DFS(maze:Maze, at_node:Node, node_path: Node = []):

        found = False
        at_node.used = True

        for node in maze.treasures:
            if (node.used == False):
                node.used = True
                node_path.append(node)
                DFS(maze, node, node_path)
                node.used = False
                node_path.pop()
                found = True

        if(found == False):
            total_time, score = 0, 0
            for i in range(len(node_path) - 1):  
                total_time += maze.treasure_info[node_path[i].index][node_path[i + 1].index][0]
                if(total_time >= 69000):
                    break
                score += maze.treasure_info[node_path[i].index][node_path[i + 1].index][1]
            if(score > maze.max_scores):
                #print(score)
                maze.shortest_node_path.clear()
                maze.max_scores = score
                for node in node_path:
                    maze.shortest_node_path.append(node)


    node_path.append(maze.start_node)
    maze.start_node.used = True       

    for node in maze.treasures:
        if(node.used == False):
            node.used = True
            node_path.append(node)
            DFS(maze, node, node_path)
            node_path.pop()
            node.used = False

    # print
    maze.nodepath_to_cmds()
    maze.cmds += 's'
    print(maze.cmds)
    
    print("estimated score:")
    print(maze.max_scores)

    end_time = process_time()
    #print('total time consumed:')
    #print(end_time - start_time)


"""
# store any two point bfs distance and actions string
1. pick any two point
2. count the len(actions) to be the distance of any two point ## the criteria can be altered / time/ distance/ number of turns
3. clear path and actions for the next bfs

# dfs to find the shortest path
1. DFS(maze, at_node, count) at node record the current node, count to record the total distance
    if it is an end point, 
        compare count with the shortest_distance
    if it isn't
        search the rest unused
        add the distance, record it in the node_path
        call for the next level DFS
        delete the distance and pop the node_path
        
"""