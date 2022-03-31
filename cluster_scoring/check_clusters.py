#!/usr/bin/env python3

import argparse
import collections
from typing import Dict, Set
import numpy as np
from scipy.misc import comb
from sklearn.metrics import adjusted_mutual_info_score, rand_score, adjusted_rand_score

import data_prep


def give_patterns_ids(clusters_map: Dict[int, Set[str]]) -> Dict[str, int]:
    all_patterns = set()
    for cluster in clusters_map.values():
        for pattern in cluster:
            all_patterns.add(pattern)
    running_pattern_id = 0
    id_map: Dict[str, int] = {}
    for pattern in all_patterns:
        id_map[pattern] = running_pattern_id
        running_pattern_id += 1
    
    return id_map

def manual_rand_index_score(clusters, classes):
    tp_plus_fp = comb(np.bincount(clusters), 2).sum()
    tp_plus_fn = comb(np.bincount(classes), 2).sum()
    A = np.c_[(clusters, classes)]
    tp = sum(comb(np.bincount(A[A[:, 0] == i, 1]), 2).sum()
             for i in set(clusters))
    fp = tp_plus_fp - tp
    fn = tp_plus_fn - tp
    tn = comb(len(A), 2) - tp - fp - fn
    score = (tp + tn) / (tp + fp + fn + tn)
    return score, (tp, fp, fn, tn)


def main(true_path: str, check_path: str):
    # Read the objects
    true_clusters = data_prep.read_clusters_objects(true_path)
    check_clusters = data_prep.read_clusters_objects(check_path)

    if len(true_clusters) == len(check_clusters):
        print("Cluster count is the same")
    else:
        print("Cluster count is different")

    all_true_patterns = set()
    for cluster_id in true_clusters:
        cluster = true_clusters[cluster_id]
        for pattern in cluster:
            all_true_patterns.add(pattern)

    all_check_patterns = set()
    for cluster_id in check_clusters:
        cluster = check_clusters[cluster_id]
        for pattern in cluster:
            all_check_patterns.add(pattern)

    if len(all_true_patterns) == len(all_check_patterns):
        print("Pattern count is the same")
    else:
        print("Pattern count is different")

    # Give all of the patterns ids
    pattern_ids = give_patterns_ids(check_clusters)

    true_cluster_vector = data_prep.create_cluster_vector(true_clusters, pattern_ids)
    check_cluster_vector = data_prep.create_cluster_vector(check_clusters, pattern_ids)

    print("True cluster vector")
    print(true_cluster_vector)
    print("Check cluster vector")
    print(check_cluster_vector)

    rscore = rand_score(true_cluster_vector, check_cluster_vector)
    arscore = adjusted_rand_score(true_cluster_vector, check_cluster_vector)
    amiscore = adjusted_mutual_info_score(true_cluster_vector, check_cluster_vector)
    manual_rscore, intermediate_values = manual_rand_index_score(true_cluster_vector, check_cluster_vector)
    print(f"\n\nRand Score: {rscore}")
    print(f"Adjusted rand score: {arscore}")
    print(f"Adjusted Mutual Information Score: {amiscore}")
    print(f"Manual rand score: {manual_rscore}")
    print(f"Intermediate manual rand score values: tp: {intermediate_values[0]}, fp: {intermediate_values[1]}, fn: {intermediate_values[2]}, tn: {intermediate_values[3]}")

    print("Vector frequency information")
    true_frequencies = collections.Counter(true_cluster_vector)
    check_frequencies = collections.Counter(check_cluster_vector)
    print("True frequencies:")
    print(true_frequencies.most_common())
    print("Check frequencies:")
    print(check_frequencies.most_common())



if __name__ == '__main__':
    parser = argparse.ArgumentParser("check_clusters.py")
    parser.add_argument("true_path", help="Path to file containing the true clusters")
    parser.add_argument("actual_path", help="Path to file containing the actual clusters")
    args = parser.parse_args()
    main(args.true_path, args.actual_path)
