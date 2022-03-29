
import json
from optparse import Option
import re
from time import perf_counter
from typing import Dict, List, Set, Tuple


def read_clusters_objects(path: str) -> Dict[int, Set[str]]:
    all_clusters: Dict[int, set[str]] = {}
    running_cluster_id = 0
    with open(path, 'r') as clusters_file:
        cluster_obj = json.load(clusters_file)
        for cluster in cluster_obj:
            cluster_patterns = set(cluster)
            all_clusters[running_cluster_id] = cluster_patterns
            running_cluster_id += 1
    
    return all_clusters


def create_cluster_vector(cluster_map: Dict[int, Set[str]], pattern_ids: Dict[str, int]) -> List[int]:
    cluster_vector = [0] * len(pattern_ids)
    for cluster_idx in cluster_map:
        cluster = cluster_map[cluster_idx]
        for pattern in cluster:
            if pattern in pattern_ids:
                pattern_id = pattern_ids[pattern]
                cluster_vector[pattern_id] = cluster_idx
    return cluster_vector


def cluster_map_to_list(cluster_map: Dict[int, Set[int]], patterns: List[int]) -> List[int]:
    patterns_to_clusters = [0] * len(patterns)
    for idx, pattern in enumerate(patterns):
        for cluster_id, cluster_patterns in cluster_map.items():
            if pattern in cluster_patterns:
                patterns_to_clusters[idx] = cluster_id
                continue

    return patterns_to_clusters


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
