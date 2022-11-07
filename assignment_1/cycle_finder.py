import sys
import json
from modules import helpers as h


def main() -> None:
    # t = h.Traverser()
    # t.traverse()
    
    with open(h.DATA128, "r") as read_h:
        pass
    smallest_dist = 0
    with open("cycles.ndjson", "r") as read_h:
        for line in read_h:
            data = json.loads(line)
            for value in data.values():
                for key in value.keys():
                    if smallest_dist == 0:
                        smallest_dist = float(key)

                    val = float(key)
                    if val < smallest_dist:
                        smallest_dist = val

    print(smallest_dist)


if __name__ == "__main__":
    sys.exit(main())
