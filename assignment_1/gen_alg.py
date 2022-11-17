import numpy as np
import random
from typing import List
from datetime import datetime
DATA16K = "data16k.txt"
DATA128 = "data128.txt"
POPULATION = 100
MUTATION_RATE = 0.3

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


nodes: List[List[int]] = []

with open(DATA128, "r") as read_h:
    for row in read_h.readlines():
        row = row.strip()
        x,y = tuple(row.split())
        tmp = [int(x),int(y)]
        if tmp not in nodes:
            nodes.append(tmp)

N_NODES = len(nodes)

# Generating a list of coordenades representing each city
coordinates_list = nodes

# Function to compute the distance between two points
def compute_city_distance_coordinates(a,b):
    return ((a[0]-b[0])**2+(a[1]-b[1])**2)**0.5

print(N_NODES)
# First step: Create the first population set
def genesis(city_list):

    population_set = []
    for i in range(POPULATION):
        #Randomly generating a new solution
        
        sol_i = np.array(city_list)[np.random.choice(list(range(N_NODES)), N_NODES, replace=False)]
        population_set.append(sol_i)
        
    return np.array(population_set)

population_set = genesis(nodes)

print(f"{len(population_set)}")
def fitness_eval(city_list):
    total = 0
    for i in range(POPULATION):
        a = city_list[i]
        b = city_list[i+1]
        total += compute_city_distance_coordinates(a,b)
    return total

def get_all_fitnes(population_set):
    fitnes_list = np.zeros(len(population_set))

    #Looping over all solutions computing the fitness for each solution
    for i in range(len(population_set)):
        fitnes_list[i] = fitness_eval(population_set[i])
    return fitnes_list

fitnes_list = get_all_fitnes(population_set)



def progenitor_selection(population_set,fitnes_list):
    total_fit = fitnes_list.sum()
    prob_list = fitnes_list/total_fit
    
    #Notice there is the chance that a progenitor. mates with oneself
    progenitor_list_a = np.random.choice(list(range(len(prob_list))), len(prob_list),p=prob_list, replace=True)
    progenitor_list_b = np.random.choice(list(range(len(prob_list))), len(prob_list),p=prob_list, replace=True)
    
    progenitor_list_a = population_set[progenitor_list_a]
    progenitor_list_b = population_set[progenitor_list_b]

    return np.array([progenitor_list_a,progenitor_list_b])


progenitor_list = progenitor_selection(population_set,fitnes_list)

def mate_progenitors(prog_a, prog_b):
    offspring = prog_a[0:5]

    for city in prog_b:

        if not city in offspring:
            offspring = np.concatenate((offspring,[city]))

    return offspring
            
    
    
def mate_population(progenitor_list):
    new_population_set = []
    for i in range(progenitor_list.shape[1]):
        prog_a, prog_b = progenitor_list[0][i], progenitor_list[1][i]
        offspring = mate_progenitors(prog_a, prog_b)
        new_population_set.append(offspring)
        
    return new_population_set

new_population_set = mate_population(progenitor_list)

def mutate_offspring(offspring):
    for _ in range(int(len(offspring)*MUTATION_RATE)):
        a = np.random.randint(0,len(offspring))
        b = np.random.randint(0,len(offspring))

        offspring[a], offspring[b] = offspring[b], offspring[a]

    return offspring
    
    
def mutate_population(new_population_set):
    mutated_pop = []
    for offspring in new_population_set:
        mutated_pop.append(mutate_offspring(offspring))
    return mutated_pop

mutated_pop = mutate_population(new_population_set)



best_solution = [-1,np.inf,np.array([])]
for i in range(100_000_000):
    fitnes_list = get_all_fitnes(mutated_pop)
    
    #Saving the best solution
    if fitnes_list.min() < best_solution[1]:
        best_solution[0] = i
        best_solution[1] = fitnes_list.min()
        best_solution[2] = np.array(mutated_pop)[fitnes_list.min() == fitnes_list]
        print(i, best_solution[1], fitnes_list.min(), datetime.now().strftime("%d/%m/%y %H:%M"))
    
    progenitor_list = progenitor_selection(population_set,fitnes_list)
    new_population_set = mate_population(progenitor_list)
    
    mutated_pop = mutate_population(new_population_set)

