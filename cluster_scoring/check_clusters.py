#!/usr/bin/env python3

import argparse
from typing import Dict, List, Set

from sklearn.metrics import rand_score, adjusted_rand_score

import data_prep


def cluster_map_to_list(cluster_map: Dict[int, Set[int]], patterns: List[int]) -> List[int]:
    patterns_to_clusters = [0] * len(patterns)
    for idx, pattern in enumerate(patterns):
        for cluster_id, cluster_patterns in cluster_map.items():
            if pattern in cluster_patterns:
                patterns_to_clusters[idx] = cluster_id
                continue

    return patterns_to_clusters


def main(true_path: str, check_path: str):
    # Read the objects
    true_ids, true_clusters = data_prep.read_clusters_objects(true_path)
    check_ids, check_clusters = data_prep.read_clusters_objects(check_path)
    # cehck if they match up
    if true_ids == check_ids:
        print("ids match up")
    else:
        print("ids don't match up")


if __name__ == '__main__':
    parser = argparse.ArgumentParser("check_clusters.py")
    parser.add_argument("true_path", help="Path to file containing the true clusters")
    parser.add_argument("actual_path", help="Path to file containing the actual clusters")
    args = parser.parse_args()
    main(args.true_path, args.actual_path)
