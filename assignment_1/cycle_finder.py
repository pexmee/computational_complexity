import sys

from modules import helpersv2 as h


def main() -> None:
    t = h.Traverser()
    t.traverse()
    # t.graph()
    print(f"Distance traversed:", t.distance_traversed)


if __name__ == "__main__":
    sys.exit(main())
