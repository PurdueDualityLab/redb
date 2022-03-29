
import json
import re
from typing import Dict, List, Set, Tuple


def read_clusters_objects(path: str) -> Tuple[Dict[str, int], Dict[int, Set[str]]]:
    """Read an array of clusters into a map that assigns every regex an id

    Args:
        path (str): Path to the file to read

    Returns:
        dict[int, str]: map of regexes and their ids
    """
    all_clusters: Dict[int, set[str]] = {}
    pattern_ids: Dict[str, int] = {}
    running_pattern_id = 0
    running_cluster_id = 0
    with open(path, 'r') as clusters_file:
        for cluster in clusters_file:
            cluster_patterns = set()
            for pattern in cluster:
                cluster_patterns.add(pattern)
                pattern_ids[pattern] = running_pattern_id
                running_pattern_id += 1
            all_clusters[running_cluster_id] = cluster_patterns
            running_cluster_id += 1
    
    return (pattern_ids, all_clusters)


def read_cluster_map(path: str) -> Dict[int, Set[int]]:
    cluster_map: Dict[int, set[int]] = {}
    cluster_parser = re.compile(r"(\d+)\s*")
    cluster_id = 0
    with open(path, 'r') as cluster_file:
        for line in cluster_file:
            id_strings = [match.group(1) for match in cluster_parser.finditer(line)]
            ids = set(map(int, id_strings))
            cluster_map[cluster_id] = ids
            cluster_id += 1

    return cluster_map


def read_all_pattern_ids(path: str) -> List[int]:
    id_set: Set[int] = set()
    cluster_parser = re.compile(r"(\d+)\s*")
    with open(path, 'r') as cluster_file:
        for line in cluster_file:
            id_strings = [match.group(1) for match in cluster_parser.finditer(line)]
            ids = set(map(int, id_strings))
            id_set.update(ids)

    return list(id_set)
