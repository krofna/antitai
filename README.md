# Sample implementation of a separation algorithm for Tai mapping

The entire implementation is contained in a single file, separate.cpp, which has no dependencies. Simply compile (e.g.)

> g++ separate.cpp -O2 -o separate

And run

> ./separate tree1 tree2 weight_matrix

examples/ directory contains two sample trees and two sample weight matrices, the first one corresponding to a Tai mapping of weight 4 and the second one to an anti Tai mapping of weight 4. Trees are plaintext files in the following format (0-indexed nodes, with root in 0)

<# of nodes>
<edge endpoint> <edge endpoint>
...

Matrix is a plaintext file in the following format
<# of nodes in tree 1 (n)> <# of nodes in tree 2 (m)>
<w_{0,0}> ... <w_{0, m-1}>
...
<w_{n-1,0}> ... <w_{n-1, m-1}>
