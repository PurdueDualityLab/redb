
import re


def read_cluster_map(path: str) -> dict[int, set[int]]:
    cluster_map: dict[int, set[int]] = {}
    cluster_parser = re.compile(r"(\d+)\s*")
    cluster_id = 0
    with open(path, 'r') as cluster_file:
        for line in cluster_file:
            id_strings = [match.group(1) for match in cluster_parser.finditer(line)]
            ids = set(map(int, id_strings))
            cluster_map[cluster_id] = ids
            cluster_id += 1

    return cluster_map


def read_all_pattern_ids(path: str) -> list[int]:
    id_set: set[int] = set()
    cluster_parser = re.compile(r"(\d+)\s*")
    with open(path, 'r') as cluster_file:
        for line in cluster_file:
            id_strings = [match.group(1) for match in cluster_parser.finditer(line)]
            ids = set(map(int, id_strings))
            id_set.update(ids)

    return list(id_set)
