#!/usr/bin/env python3

import argparse

from sklearn.metrics import rand_score, adjusted_rand_score

import data_prep


def cluster_map_to_list(cluster_map: dict[int, set[int]], patterns: list[int]) -> list[int]:
    patterns_to_clusters = [0] * len(patterns)
    for idx, pattern in enumerate(patterns):
        for cluster_id, cluster_patterns in cluster_map.items():
            if pattern in cluster_patterns:
                patterns_to_clusters[idx] = cluster_id
                continue

    return patterns_to_clusters


def main(true_path: str, check_path: str):
    # load in everything
    true_clusters = data_prep.read_cluster_map(true_path)
    true_patterns = data_prep.read_all_pattern_ids(true_path)
    actual_clusters = data_prep.read_cluster_map(check_path)
    actual_patterns = data_prep.read_all_pattern_ids(check_path)

    # see how many patterns are not in alignment
    print(f"actual_patterns size - true patterns size = {len(actual_patterns) - len(true_patterns)}")


if __name__ == '__main__':
    parser = argparse.ArgumentParser("check_clusters.py")
    parser.add_argument("true_path", help="Path to file containing the true clusters")
    parser.add_argument("actual_path", help="Path to file containing the actual clusters")
    args = parser.parse_args()
    main(args.true_path, args.actual_path)
