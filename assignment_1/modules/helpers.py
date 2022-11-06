from copy import deepcopy
from math import sqrt
from pkgutil import get_data
import sys
from typing import Dict, Generator, List, Optional
import matplotlib.pyplot as p

DATA128 = "data128.txt"
DATA16K = "data16k.txt"
X:int = []
Y:int = []

# Precomputed these just because we are always working on the same dataset here.
MAX_X = 3937
MAX_Y = 3998

"""
TODO: I believe we only have to check which one is the furthest for each node, we don't need
to save all the neighbours. In addition we can just recurse through the nodes, always save the 
furthest away node, and once we have that, we traverse to that node and do the same thing, just remembering 
which one is the start node. However we need to check which ones we have already visited, so that we don't go back and fourth.
So we will add visited nodes in a list or something, and then we will make sure that once we have what we need, we can then
just hop on back to the first node to complete the cycle.
"""

# We could've used a tuple instead but this is just more pleasant.
class Node:
    def __init__(self, x: str, y:str) -> None:
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
        
        self.stack: List[Node] = [] # Holds Nodes to be traversed
        self.nodes: List[Node] = [] # Holds ALL Nodes.
        self.checked_nodes: List[Node] = [] # Holds checked Nodes
        self.current_node: Node
        self.cycle: List[Node] = []
        self.set_nodes()

    
    def check_weights_from_current(self,stack: List[Node]) -> None:
        while stack:
            node = stack.pop()
            self.current_node.weights[node] = self.calc_dist(node)


        
    @staticmethod
    def get_data() -> Generator[Node,None,None]:
        with open(DATA16K, "r") as read_h:
            for row in read_h.readlines():
                row = row.strip()
                if row:
                    yield Node(*tuple(row.split()))
    
    def calc_dist(self, node: Node) -> float:
        return sqrt(pow((node.x - self.current_node.x),2) + pow((node.y - self.current_node.y),2))
    
    def set_nodes(self) -> None:
        done = False

        while not done:
            if not self.nodes:
                self.nodes = [Node for Node in self.get_data()]
            
        
            if not self.stack:
                self.stack = deepcopy(self.nodes)
                
                if self.checked_nodes:
                    for node in self.checked_nodes:
                        if node in self.stack:
                            self.stack.remove(node)
                    
                    # This means we have checked everything already.
                    if not self.stack:
                        done = True
                        print(self.current_node.weights)
                        self.build_path()
                        return
                
                self.current_node = self.stack.pop()
                self.checked_nodes.append(self.current_node)
                self.check_weights_from_current(self.stack)
            
            

    # Returns largest key in weights
    @staticmethod
    def largest(node: Node) -> Optional[Node]:
        if node.weights:
            print("returning max")
            return max(node.weights, key=node.weights.get)
        
        else:
            print("returning None")
            return None

    def build_path(self):
        print("building path")
        print("current node:")
        while self.checked_nodes:
            self.current_node = self.checked_nodes.pop(0)
            largest_weight: Node = self.largest(self.current_node)
            if largest_weight:
                if largest_weight not in self.cycle:
                    if self.current_node not in self.cycle:
                        self.cycle.append(self.current_node)
                    self.cycle.append(largest_weight)

                else:
                    self.current_node.weights.pop(largest_weight)
            else:
                print("Closing cycle:",self.cycle)
                self.cycle.append(self.cycle[0]) # Close cycle.
                self.graph()
                return
            
    
    def graph(self):
        x: int = []
        y: int = []

        for node in self.cycle:
            x.append(node.x)
            y.append(node.y)

        p.plot(x,y, 'ro')
        p.plot(x,y,'k-')

        p.axis("equal")
        p.show()

def graph() -> None:
    p.plot(X,Y,color="white",marker="o", markerfacecolor="red",markersize=6)
    p.xlim(0,MAX_X)
    p.ylim(0,MAX_Y)
    p.xlabel("x-axis")
    p.ylabel("y-axis")
    p.title("Points")
    p.show()
