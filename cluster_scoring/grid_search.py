#!/usr/bin/env python3

from pydoc import describe
from tabnanny import check
import tempfile
from typing import Dict, List, Tuple
import subprocess
import re
import json
from argparse import ArgumentParser


class GridSpec:
    cluster_tool_path: str = ""
    check_clusters_path: str = ""
    truth_data_path: str = ""
    similarity_graph_path: str = ""
    clustering_spec_file: str = ""


def read_spec_file(file_path: str) -> GridSpec:
    with open(file_path, 'r') as spec_file:
        obj = json.load(spec_file)
        spec = GridSpec()
        spec.cluster_tool_path = obj["cluster_tool"]
        spec.check_clusters_path = obj["cluster_checker"]
        spec.truth_data_path = obj["truth_data"]
        spec.similarity_graph_path = obj["similarity_graph"]
        spec.clustering_spec_file = obj["clustering_spec"]
    return spec


def main(spec: GridSpec):
    # Do some grid searching
    inflation_params = [x/100.0 for x in range(100, 600, 25)]
    pruning_params   = [x/100.0 for x in range(600, 900, 25)]
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
        with tempfile.NamedTemporaryFile('rw') as clusters_output_file:
            invocation = [
                spec.cluster_tool_path,
                "--existing-graph", spec.similarity_graph_path,
                "-f", spec.clustering_spec_file,
                "-i", str(inflation_val),
                "-p", str(pruning_val),
                "-k", str(k_val),
                "-P", clusters_output_file.name,
                spec.truth_data_path
            ]

            # Execute the configuration
            cluster_complete = subprocess.run(invocation, check=True, capture_output=True, encoding='utf-8')
            cluster_complete.check_returncode()

            # Check the cluster results
            check_complete = subprocess.run([spec.check_clusters_path, spec.truth_data_path, clusters_output_file.name], check=True, capture_output=True, encoding='utf-8')
            check_complete.check_returncode()

            rand_index = rand_finder.search(check_complete.stdout).group(1)
            adj_rand_index = adj_rand_finder.search(check_complete.stdout).group(1)

            score_matrix[(inflation_val, pruning_val, k_val)] = (rand_index, adj_rand_index)

    # We have a map of all of the scores. Save them to a file
    with open('grid_search_results.json', 'w') as results_file:
        json.dump(score_matrix, results_file)
        

if __name__ == '__main__':
    parser = ArgumentParser('grid_search.py', description='Try to optimize clustering parameters')
    parser.add_argument('spec_file', help="Spec file that defines the grid search parameters")

    args = parser.parse_args()
    spec = read_spec_file(args.spec_file)
    main(spec)
