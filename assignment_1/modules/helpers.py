from copy import deepcopy
from math import sqrt
from typing import Dict, Generator, List, Optional, Tuple
import json
import ndjson
import matplotlib.pyplot as p

DATA128 = "data128.txt"
DATA16K = "data16k.txt"
CYCLE_128 = "cycles.ndjson"
CYCLE_16K = "cycles_16k.ndjson"
X: int = []
Y: int = []
MAX = 2_147_483_647 # To avoid a jump

# Precomputed these just because we are always working on the same dataset here.
MAX_X = 3937
MAX_Y = 3998

"""
I believe we only have to check which one is the furthest for each node, we don't need
to save all the neighbours. In addition we can just recurse through the nodes, always save the 
furthest away node, and once we have that, we traverse to that node and do the same thing, just remembering 
which one is the start node. However we need to check which ones we have already visited, so that we don't go back and fourth.
So we will add visited nodes in a list or something, and then we will make sure that once we have what we need, we can then
just hop on back to the first node to complete the cycle.
"""

"""
Okey so obviously the above comment is depricated. 
The code needs to look for the SHORTEST cycle, not the longest.
So instead of looking at which node is the furthest away, let's look at which one is the closest instead. 

TODO: Save weights in order to present the total travelled distance.
TODO: Modify member function to find clusters of short possible paths of traversal.

"""

# We could've used a tuple instead but this is just more pleasant.
class Node:
    def __init__(self, x: str, y: str) -> None:
        self.x = int(x)
        self.y = int(y)

    def __hash__(self) -> int:
        return hash(
            (
                self.x,
                self.y,
            )
        )

    def __eq__(self, __o: object) -> bool:
        if not isinstance(__o, Node):
            raise TypeError("invalid argument type")

        return self.__hash__() == __o.__hash__()


class Traverser:
    def __init__(self) -> None:

        """
        Traverser will utilize Dijkstras algorithm for shortest path by
        initially by checking all weights to the nodes from the current node.
        Then it will move to the closest node, and continue doing this until it has
        found the shortest path from the starting node.

        To improve upon this algorithm, one can use evolutionary computation to
        attempt to find the shortest path, by constantly searching for a shorter path.
        And just attempting to find a path shorter than that one, and for each shorter path
        we could stop and see how much shorter we have gotten, since it's unfeasable to calculate
        all permutations via bruteforce. At least for the data16k problem.


        TODO: Don't save all possible places to traverse, don't weigh all and save the weight,
        instead, just always save the smallest weight to reduce memory usage. I.e change
        how we save the weights. We only need to save 1 weight, which is the shortest distance
        that is possible to traverse to.
        """

        self.nodes: List[Node] = []  # Holds all Nodes left to be traversed.
        self.possible_starting_nodes: List[Node] = [] # Holds all Nodes we can use as staring nodes.
        self.cycle: List[Node] = []  # Holds the current path.
        self.current_node: Optional[Node] = None
        self.distance_traversed: float = 0
        self.starting_nodes_set = False

    def traverse(self):
        self._initialize_nodes()

        if self.starting_nodes_set and self.possible_starting_nodes:
            while self.nodes:
                self._init_playground()
                self._choose_shortest_dist()

            self._close_cycle()

    def _initialize_nodes(self):
        print(f"possible starting nodes: {len(self.possible_starting_nodes)}")

        self.nodes.clear() # Make sure it is empty.
        self.cycle.clear() # Make sure cycle is empty too
        self.distance_traversed = 0 # reset this

        for node in self.get_data():
            self.nodes.append(node)

        if not self.starting_nodes_set:
            self.possible_starting_nodes = deepcopy(self.nodes)
            self.starting_nodes_set = True

    def _init_playground(self):
        if self.nodes:  # Sanitycheck

            # This should always happen the first time, so we
            # Can make sure that the node hasn't already been
            # used as the starting node by choosing anything that hasn't
            # already been used
            if self.possible_starting_nodes and not self.cycle: # Sanity check
                self.current_node = self.possible_starting_nodes.pop()
                self.nodes.pop(self.nodes.index(self.current_node)) # Remove it from self.nodes.
            
            
                


    # Returns closest node
    def _closest(self) -> Tuple[Optional[Node],float]:
        shortest_dist: float = MAX
        closest_node: Optional[Node] = None

        for node in self.nodes:
            if node not in self.cycle and node != self.current_node:
                cmp_distance = self._calc_dist(node)
                
                if cmp_distance < shortest_dist:
                    shortest_dist = cmp_distance
                    closest_node = node

        return closest_node, shortest_dist

    def _choose_shortest_dist(self):
        closest_node, shortest_dist = self._closest()
    
        if closest_node:
            self.distance_traversed += shortest_dist
            self.current_node = self.nodes.pop(self.nodes.index(closest_node))
            self.cycle.append(self.current_node)

        else:
            print("no shortest path, cycle should be done")


    def _close_cycle(self):
        self.cycle.append(self.cycle[0])
        self.distance_traversed += self._calc_dist(self.cycle[0])
        print(f"cycle_len: {len(self.cycle)}")
        print(f"distance_traversed: {self.distance_traversed}")
        self._dump_cycle()

    def _reset_traversal(self):
        self.traverse()

    def graph(self):
        x: int = []
        y: int = []

        for node in self.cycle:
            x.append(node.x)
            y.append(node.y)

        p.plot(x, y, "ro")
        p.plot(x, y, "k-")

        p.axis("equal")
        p.show()

    def _calc_dist(self, node: Node) -> float:
        return sqrt(
            pow((node.x - self.current_node.x), 2)
            + pow((node.y - self.current_node.y), 2)
        )

    def _dump_cycle(self):
        data: Dict[List[str],Dict[float,List[str]]] = {}
        coordinates: List[str] = []
        
        for node in self.cycle:
            coordinates.append(f"({str(node.x)},{str(node.y)})")
        
        data[f"({self.cycle[0].x},{self.cycle[0].y})"] = {self.distance_traversed : coordinates}

        with open(CYCLE_16K, "a") as write_h:
            json.dump(data, write_h)
            write_h.write("\n")
        
        # self._reset_traversal()
            


    @staticmethod
    def get_data() -> Generator[Node, None, None]:
        with open(DATA16K, "r") as read_h:
            for row in read_h.readlines():
                row = row.strip()
                if row:
                    yield Node(*tuple(row.split()))
    
