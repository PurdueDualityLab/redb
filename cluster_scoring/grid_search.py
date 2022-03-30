#!/usr/bin/env python3

from pydoc import describe
from typing import Dict, List, Tuple
import subprocess
import re
import json
from argparse import ArgumentParser


def main(path_to_cluster_tool: str, path_to_truth_dataset: str, path_to_similarity_graph: str, path_to_spec_file: str):
    # Do some grid searching
    inflation_params = [x for x in range(1, 6, .25)]
    pruning_params   = [x for x in range(.6, .9, .25)]
    top_k_edges      = [x for x in range(70, 90, 2)]

    # Make every input parameter
    input_space: List[Tuple(float, float, int)] = []
    for inflation_value in inflation_params:
        for pruning_value in pruning_params:
            for k_edge in top_k_edges:
                input_space.append((inflation_value, pruning_value, k_edge))
    
    # For each input parameter, make a cluster tool invocation
    rand_finder = re.compile(r"Rand Score: ([0-9.]+)")
    adj_rand_finder = re.compile(r"Adjusted rand score: ([0-9.]+)")
    score_matrix: Dict[Tuple[float, float, int], Tuple[float, float]] = {}
    for inflation_val, pruning_val, k_val in input_space:
        invocation = [
            path_to_cluster_tool,
            "--existing-graph", path_to_similarity_graph,
            "-f", path_to_spec_file,
            "-i", str(inflation_val),
            "-p", str(pruning_val),
            "-k", str(k_val),
            path_to_truth_dataset
        ]

        # Execute the configuration
        completed_process = subprocess.run(invocation, check=True, capture_output=True, encoding='utf-8')
        completed_process.check_returncode()
        rand_index = rand_finder.search(completed_process.stdout).group(1)
        adj_rand_index = adj_rand_finder.search(completed_process.stdout).group(1)

        score_matrix[(inflation_val, pruning_val, k_val)] = (rand_index, adj_rand_index)

    # We have a map of all of the scores. Save them to a file
    with open('grid_search_results.json', 'w') as results_file:
        json.dump(score_matrix, results_file)
        

if __name__ == '__main__':
    parser = ArgumentParser('grid_search.py', description='Try to optimize clustering parameters')
    parser.add_argument('cluster-tool', help='Path to cluster tool')
    parser.add_argument('existing-graph', help="Path to existing similarity graph")
    parser.add_argument('truth-dataset', help="Path to truth dataset")
    parser.add_argument('spec-file', help="Path to spec file")

    args = parser.parse_args()
    main(args.cluster_tool, args.truth_dataset, args.existing_graph, args.spec_file)
