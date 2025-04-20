import argparse
import logging
import os
import sys
import time

import numpy as np
import pandas
from BTinterface import BTInterface
from maze import Action, Maze, Duration, solve_maze
from score import ScoreboardServer, ScoreboardFake

logging.basicConfig(
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s", level=logging.INFO
)

log = logging.getLogger(__name__)

# TODO : Fill in the following information
TEAM_NAME = "ThursdayTeam3"
SERVER_URL = "http://140.112.175.18:5000/"
MAZE_FILE = "python/data/medium_maze.csv"
BT_PORT = "COM9"


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--mode", default= "0", help="0: treasure-hunting, 1: self-testing", type=str)
    parser.add_argument("--maze-file", default=MAZE_FILE, help="Maze file", type=str)
    parser.add_argument("--bt-port", default=BT_PORT, help="Bluetooth port", type=str)
    parser.add_argument(
        "--team-name", default=TEAM_NAME, help="Your team name", type=str
    )
    parser.add_argument("--server-url", default=SERVER_URL, help="Server URL", type=str)
    return parser.parse_args()


def main(mode: str, bt_port: str, team_name: str, server_url: str, maze_file: str):
    maze = Maze(maze_file)
    #point = ScoreboardFake("your team name", "data/fakeUID.csv") # for local testing
    solve_maze(maze)
    interface = BTInterface(port=bt_port)
    point = ScoreboardServer(team_name, server_url)
    # TODO : Initialize necessary variables

    if mode == "0":
        log.info("Mode 0: For treasure-hunting")
        interface.start()
        interface.send_action(maze.cmds[0])
        maze.cmds = maze.cmds = maze.cmds[1:]
        interface.send_action(maze.cmds[0])
        maze.cmds = maze.cmds = maze.cmds[1:]
        while(True):
            Get = interface.bt.serial_read_string().strip()
            print(repr(Get))
            if(Get == 'msg'):
                print("received msg")
                GetMsg = interface.bt.serial_read_string().strip()
                if(GetMsg == 'z' and len(maze.cmds) > 0):
                    print("received z\n")
                    interface.send_action(maze.cmds[0])
                    maze.cmds = maze.cmds[1:]
                if(GetMsg == 'Finish'):
                    interface.end_process()
                    break
            elif(Get == 'UID'):
                print("received UID")
                GetUID = interface.bt.serial_read_string().strip()
                print(GetUID)
                if(GetUID):
                    point.add_UID(GetUID)


    elif mode == "1":
        log.info("Mode 1: Self-testing mode.")
        # TODO: You can write your code to test specific function.
        '''
        maze.BFS(maze.node_dict[1], maze.node_dict[48])
        #to execute this
        #you needto comment def of "point" and "interface"
        '''

        '''
        cmd = ["f", "r", "b", "f", "b", "f", "s"]
        idx = 0
        
        while(True):
            print(interface.bt.serial_read_string())
            print("k\n")
            if(interface.bt.serial_read_string() == 'N'):
                interface.send_action(cmd[idx])
                print("cmd:")
                print(cmd[idx])
                print("idx:")
                print(idx)
                idx += 1
        '''
        
        #while(True):
        #    cmd = input()
        #    interface.send_action(cmd)

    else:
        log.error("Invalid mode")
        sys.exit(1)


if __name__ == "__main__":
    args = parse_args()
    main(**vars(args))
