from copy import deepcopy
from math import sqrt
from typing import Dict, Generator, List, Optional

import matplotlib.pyplot as p

DATA128 = "data128.txt"
DATA16K = "data16k.txt"
X: int = []
Y: int = []

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
        self.weights: Dict[Node, float] = {}

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
        """

        self.stack: List[Node] = []  # Holds Nodes to be traversed
        self.nodes: List[Node] = []  # Holds all Nodes left to be traversed.
        self.cycle: List[Node] = []  # Holds the current path.
        self.current_node: Optional[Node] = None
        self.distance_traversed: float = 0
        self.idx: int = -1  # index of stuff to pop yo

    def traverse(self):
        self._initialize_nodes()

        while self.nodes:
            print(len(self.cycle))
            self._make_stack()
            self._weigh_nodes()
            self._choose_shortest_dist()

        self._close_cycle()

    def _initialize_nodes(self):
        for node in self.get_data():
            self.nodes.append(node)

    def _make_stack(self):
        if self.nodes:  # Sanitycheck

            if self.idx != -1:
                self.current_node = self.nodes.pop(self.idx)
                self.idx = -1

            else:
                self.current_node = self.nodes.pop()

            self.stack = deepcopy(self.nodes)

    def _weigh_nodes(self):
        while self.stack:
            node = self.stack.pop()
            self.current_node.weights[node] = self._calc_dist(node)

    def _choose_shortest_dist(self):
        shortest_path = self._smallest(self.current_node)
        if shortest_path:

            if shortest_path in self.cycle:
                self.current_node.weights.pop(shortest_path)
                self._choose_shortest_dist()  # We need to choose a new shortest path since we don't wanna go back to the same one.

            else:
                self.distance_traversed += self.current_node.weights[shortest_path]
                # self.current_node = shortest_path -> do this in make_stack
                self.idx = self.nodes.index(
                    shortest_path
                )  # Don't catch valueerror we wanna crash here if it messes up
                self.cycle.append(self.current_node)
        else:
            print("no shortest path, cycle should be done")

    # Returns smallest key in weights
    @staticmethod
    def _smallest(node: Node) -> Optional[Node]:
        if node.weights:
            return min(node.weights, key=node.weights.get)

        else:
            return None

    def _close_cycle(self):
        self.cycle.append(self.cycle[0])

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

    @staticmethod
    def get_data() -> Generator[Node, None, None]:
        with open(DATA16K, "r") as read_h:
            for row in read_h.readlines():
                row = row.strip()
                if row:
                    yield Node(*tuple(row.split()))
